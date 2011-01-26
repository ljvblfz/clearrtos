/******************************************************************************
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE. This file and program are licensed    *
 * under the GNU Lesser General Public License Version 3, 29 June 2007.       *
 * The complete license can be accessed from the following location:          *
 * http://opensource.org/licenses/lgpl-3.0.html                               *
 *                                                                            *
 * Author: Yun Li (yunli.open@gmail.com)                                      *
 *   Date: 08/22/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "config.h"
#include "queue.h"
#include "errsync.h"
#include "interrupt.h"
#include "clib.h"
#include "console.h"
#include "task.h"

#define QUEUE_LAST_INDEX        (CONFIG_MAX_QUEUE - 1)
#define MAGIC_NUMBER_QUEUE      0x51554555L

#define is_invalid_handler(_handler) \
    ((_handler == null) || ((_handler)->magic_number_ != MAGIC_NUMBER_QUEUE))


typedef struct {
    statistic_t no_queue_;
    statistic_t message_lost_;
} queue_statistic_t;

static queue_t g_queue_pool [CONFIG_MAX_QUEUE];
queue_statistic_t g_statistics;
static dll_t g_free_queue;
static dll_t g_used_queue;

static bool queue_check_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    queue_handler_t handler = (queue_handler_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    if (!queue_is_empty (handler)) {
        console_print ("Warning: queue \"%s\" isn't empty\n", handler->name_);
    }
    console_print ("Error: queue \"%s\" isn't deleted\n", handler->name_);
    return true;
}

error_t module_queue (system_state_t _state)
{
    if (STATE_DESTROYING == _state) {
        // check whether all queues created have been deleted or not, if not
        // take them as error
        (void) dll_traverse (&g_used_queue, queue_check_for_each, 0);
    }
    return 0;
}

static void queue_init ()
{
    usize_t idx;
    
    for (idx = 0; idx <= QUEUE_LAST_INDEX; ++ idx) {
        dll_push_tail (&g_free_queue, &g_queue_pool [idx].node_);
    }
}

error_t queue_create (const char _name [], queue_handler_t *_p_handler, 
    void *_buffer, usize_t _element_size, usize_t _capacity)
{
    static bool initialized = false;
    interrupt_level_t level;
    queue_handler_t handler;
    char sem_name [NAME_MAX_LENGTH + 1] = "Queue:";
    error_t ecode;

    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_QUEUE_CREATE_INVCONTEXT);
    }

    level = global_interrupt_disable ();
    if (!initialized) {
        queue_init ();
        initialized = true;
    }
    handler = (queue_handler_t)dll_pop_head (&g_free_queue);
    if (0 == handler) {
        g_statistics.no_queue_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_QUEUE_CREATE_NOQUE);
    }
    memset (handler, 0, sizeof (*handler));
    handler->magic_number_ = MAGIC_NUMBER_QUEUE;
    global_interrupt_enable (level);
    strncpy (&sem_name [6], _name, sizeof (sem_name) - 7);
    sem_name [sizeof (sem_name) - 1] = 0;
    ecode = semaphore_create (&handler->semaphore_, _name, 0);
    if (ecode != 0) {
        goto error;
    }
    //lint -e{774}
    if (null == _name) {
        handler->name_ [0] = 0;
    }
    else {
        strncpy (handler->name_, _name, sizeof (handler->name_) - 1);
        handler->name_ [sizeof (handler->name_) - 1] = 0;
    }
    fifo_init (&handler->fifo_, _buffer, _element_size, _capacity);
    dll_push_tail (&g_used_queue, &handler->node_);
    *_p_handler = handler;
    return 0;
error:
    dll_push_head (&g_free_queue, &handler->node_);
    *_p_handler = null;
    return ecode;
}

error_t queue_delete (queue_handler_t _handler)
{
    interrupt_level_t level;

    if (is_in_interrupt () && STATE_UP == system_state ()) {
        return ERROR_T (ERROR_QUEUE_DELETE_INVCONTEXT);
    }

    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_QUEUE_DELETE_INVHANDLER);
    }

    level = global_interrupt_disable ();
    _handler->magic_number_ = 0;
    dll_remove (&g_used_queue, &_handler->node_);
    dll_push_tail (&g_free_queue, &_handler->node_);
    global_interrupt_enable (level);
    
    (void) semaphore_delete (_handler->semaphore_);
    return 0;
}

error_t queue_message_send (queue_handler_t _handler, const void *_p_element)
{
    interrupt_level_t level;

    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_QUEUE_SEND_INVHANDLER);
    }
    
    level = global_interrupt_disable ();
    if (fifo_is_full (&_handler->fifo_)) {
        g_statistics.message_lost_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_QUEUE_SEND_FULL);
    }
    fifo_element_put (&_handler->fifo_, _p_element);
    global_interrupt_enable (level);
    return semaphore_give (_handler->semaphore_);
}

error_t queue_message_receive (queue_handler_t _handler, msecond_t _timeout, 
    void *_p_element)
{
    error_t ecode;
    interrupt_level_t level;

    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_QUEUE_RECV_INVHANDLER);
    }
    
    ecode = semaphore_take (_handler->semaphore_, _timeout);
    if (ecode != 0) {
        return ecode;
    }
    level = global_interrupt_disable ();
    fifo_element_get (&_handler->fifo_, _p_element);
    global_interrupt_enable (level);
    return 0;
}

bool queue_is_empty (const queue_handler_t _handler)
{
    interrupt_level_t level;
    bool is_empty;

    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_QUEUE_EMPTY_INVHANDLER);
    }

    level = global_interrupt_disable ();
    is_empty = fifo_is_empty (&_handler->fifo_);
    global_interrupt_enable (level);
    return is_empty;
}

bool queue_is_full (const queue_handler_t _handler)
{
    interrupt_level_t level;
    bool is_full;

    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_QUEUE_FULL_INVHANDLER);
    }

    level = global_interrupt_disable ();
    is_full = fifo_is_full (&_handler->fifo_);
    global_interrupt_enable (level);
    return is_full;
}

static bool queue_dump_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    queue_handler_t handler = (queue_handler_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);
    
    console_print ("  Name: %s\n", handler->name_);
    console_print ("    Message Size: %u\n", fifo_element_size (&handler->fifo_));
    console_print ("        Capacity: %u\n", fifo_capacity (&handler->fifo_));
    console_print ("          Filled: %u\n", fifo_count (&handler->fifo_));
    console_print ("\n");
    return true;
}

void queue_dump ()
{
    if (is_in_interrupt ()) {
        return;
    }

    scheduler_lock ();
    console_print ("\n\n");
    console_print ("Summary\n");
    console_print ("-------\n");
    console_print ("  Supported: %u\n", CONFIG_MAX_QUEUE);
    console_print ("  Allocated: %u\n", dll_size (&g_used_queue));
    console_print ("  .BSS Used: %u\n", ((address_t)&g_used_queue 
        - (address_t)g_queue_pool) + sizeof (g_used_queue));
    console_print ("\n");
    console_print ("Statistics\n");
    console_print ("----------\n");
    console_print ("      No Object: %u\n", g_statistics.no_queue_);
    console_print ("  Messsage Lost: %u\n", g_statistics.message_lost_);
    console_print ("\n");
    console_print ("Queue Details\n");
    console_print ("-------------\n");
    (void) dll_traverse (&g_used_queue, queue_dump_for_each, 0);
    console_print ("\n");
    scheduler_unlock ();
}

