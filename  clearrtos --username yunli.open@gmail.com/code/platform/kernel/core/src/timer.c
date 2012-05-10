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
 *   Date: 12/05/2010                                                         *
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
#include "config.h"
#include "timer.h"
#include "interrupt.h"
#include "task.h"
#include "errtmr.h"
#include "console.h"
#include "clib.h"
#include "queue.h"

//lint -esym(728, g_statistics)

#define BUCKET_LAST_INDEX   (CONFIG_MAX_BUCKET - 1)
#define TIMER_LAST_INDEX    (CONFIG_MAX_TIMER - 1)
#define MAGIC_NUMBER_TIMER  0x54494D45L

#define is_invalid_handle(_handle) \
    ((_handle == null) || ((_handle)->magic_number_ != MAGIC_NUMBER_TIMER))

typedef struct {
    dll_t dll_;
    statistic_t hit_;
    statistic_t redo_;
    usize_t reentrance_;
    usize_t level_;
} bucket_t;

typedef struct {
    statistic_t notimer_;
    statistic_t traversed_;
    statistic_t abnormal_;
    statistic_t queue_full_;
} timer_statistic_t;

typedef struct {
    timer_handle_t handle_;
    msecond_t remained_round_;
} timer_remained_t;

static timer_instance_t g_timer_pool [CONFIG_MAX_TIMER];
static dll_t g_free_timer;
static dll_t g_inactive_timer;

static usize_t g_cursor;
static bucket_t g_buckets [CONFIG_MAX_BUCKET];
static timer_statistic_t g_statistics;
static timer_handle_t g_timer_next;
static bucket_t *g_bucket_firing;
static queue_handle_t g_timer_queue;

//lint -e{818}
static void timer_message_send (const timer_handle_t _handle)
{
    if (0 != queue_message_send (g_timer_queue, &_handle)) {
        interrupt_level_t level;

        level = global_interrupt_disable ();
        g_statistics.queue_full_ ++;
        global_interrupt_enable (level);
    }
}

void timer_fire ()
{
    interrupt_level_t level;
    timer_handle_t handle;
    
    level = global_interrupt_disable ();
    g_bucket_firing = &g_buckets [g_cursor];
    if (0 == dll_size (&g_bucket_firing->dll_)) {
        // no timer is expired
        goto out;
    }
    
    handle  = (timer_handle_t) dll_head (&g_bucket_firing->dll_);
    while (0 != handle) {
        g_statistics.traversed_ ++;
        g_timer_next = (timer_handle_t) dll_next (&g_bucket_firing->dll_, 
            &handle->node_);
        if (handle->round_ > 0) {
            // in this case the timer is still not expired
            handle->round_ --;
            break;
        }
        else {
            // hooray, the timer is expired
            dll_remove (&g_bucket_firing->dll_, &handle->node_);
            dll_push_tail (&g_inactive_timer, &handle->node_);
            handle->state_ = TIMER_STOPPED;
            if (g_bucket_firing->reentrance_ > 0) {
                g_bucket_firing->level_ ++;
            }
            global_interrupt_enable (level);
            if (TIMER_TYPE_INTERRUPT == handle->type_) {
                handle->callback_(handle, handle->arg_);
            }
            else if (TIMER_TYPE_TASK == handle->type_) {
                timer_message_send (handle);
            }
            level = global_interrupt_disable ();
        }
        handle = g_timer_next;
    }
    
out:
    g_cursor ++;
    if (g_cursor > BUCKET_LAST_INDEX) {
        g_cursor = 0;
    }
    g_timer_next = 0;
    g_bucket_firing = 0;
    global_interrupt_enable (level);;
}

static bool timer_check_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    timer_handle_t handle = (timer_handle_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    console_print ("Error: timer \"%s\" isn't deleted\n", handle->name_);
    return true;
}

static void task_timer (const char _name [], void *_p_arg)
{
    queue_handle_t queue = (queue_handle_t)_p_arg;
    timer_handle_t handle;
    
    for (;;) {
        if (0 != queue_message_receive (queue, 0, &handle)) {
            console_print ("Error: task \"%s\" cannot recieve message", _name);
            (void) task_suspend (task_self ());
        }
        handle->callback_(handle, handle->arg_);
    }
}

error_t module_timer (system_state_t _state)
{
    usize_t idx;
    error_t ecode;
    static task_handle_t handle;

    if (STATE_UP == _state) {
        STACK_DECLARE (stack, CONFIG_TIMER_TASK_STACK_SIZE);
        QUEUE_BUFFER_DECLARE (buffer, sizeof (timer_handle_t), 
            CONFIG_TIMER_QUEUE_SIZE);
        
        ecode = queue_create ("Timer", &g_timer_queue, buffer, 
            sizeof (timer_handle_t), CONFIG_TIMER_QUEUE_SIZE);
        if (0 != ecode) {
            return ecode;
        }
        ecode = task_create (&handle, "Timer", CONFIG_TIMER_TASK_PRIORITY, 
            stack, sizeof (stack));
        if (0 != ecode) {
            return ecode;
        }
        ecode = task_start (handle, task_timer, g_timer_queue);
        if (0 != ecode) {
            return ecode;
        }
    }
    else if (STATE_DOWN == _state) {
        if (0 != task_delete (handle)) {
            console_print ("Error: cannot delete task \"Timer\"");
        }
        if (0 != queue_delete (g_timer_queue)) {
            console_print ("Error: cannot delete queue \"Timer\"");
        }
    }
    else if (STATE_DESTROYING == _state) {
        for (idx = 0; idx <= BUCKET_LAST_INDEX; ++ idx) {
            (void) dll_traverse (&g_buckets [idx].dll_, timer_check_for_each, 0);
        }
        (void) dll_traverse (&g_inactive_timer, timer_check_for_each, 0);
    }
    return 0;
}

static void timer_init ()
{
    usize_t index;
    
    for (index = 0; index <= TIMER_LAST_INDEX; ++ index) {
        dll_push_tail (&g_free_timer, &g_timer_pool [index].node_);
    }
}

error_t timer_alloc (timer_handle_t *_p_handle, const char *_name,
    timer_type_t _type)
{
    interrupt_level_t level;
    static bool initialized = false;
    timer_handle_t handle;
    
    if (0 == _p_handle) {
        return ERROR_T (ERROR_TIMER_ALLOC_INVHANDLE);
    }

    *_p_handle = null;
    level = global_interrupt_disable ();
    if (!initialized) {
        timer_init ();
        initialized = true;
    }
    
    handle = (timer_handle_t)dll_pop_head (&g_free_timer);
    if (0 == handle) {
        g_statistics.notimer_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TIMER_ALLOC_NOTIMER);
    }
    global_interrupt_enable (level);

    handle->type_ = _type;
    handle->magic_number_ = MAGIC_NUMBER_TIMER;
    handle->state_ = TIMER_CREATED;
    dll_node_init (&handle->node_);
    if (0 == _name) {
        handle->name_ [0] = 0;
    }
    else {
        strncpy (handle->name_, _name, (usize_t)sizeof (handle->name_));
        handle->name_ [sizeof (handle->name_) - 1] = 0;
    }

    level = global_interrupt_disable ();
    dll_push_tail (&g_inactive_timer, &handle->node_);
    global_interrupt_enable (level);
    
    *_p_handle = handle;
    return 0;
}

error_t timer_free (timer_handle_t _handle)
{
    interrupt_level_t level;
    
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TIMER_FREE_INVHANDLE);
    }
    if (TIMER_STARTED == _handle->state_) {
        timer_handle_t next;
        bucket_t *p_bucket = &g_buckets [_handle->bucket_index_];
        
        if (g_timer_next == _handle) {
            g_timer_next = (timer_handle_t) dll_next (&g_bucket_firing->dll_, 
                &_handle->node_);
        }
        next = (timer_handle_t)dll_next (&p_bucket->dll_, &_handle->node_);
        if (0 != next) {
            next->round_ += _handle->round_;
        }
        dll_remove (&p_bucket->dll_, &_handle->node_);
        if (p_bucket->reentrance_ > 0) {
            g_bucket_firing->level_ ++;
        }
    }
    else {
        dll_remove (&g_inactive_timer, &_handle->node_);
    }
    _handle->magic_number_ = 0;
    dll_push_tail (&g_free_timer, &_handle->node_);
    global_interrupt_enable (level);
    
    return 0;
}

static void timer_insert (timer_handle_t _handle)
{
    interrupt_level_t interrupt_level;
    bucket_t *p_bucket;
    usize_t count = 0, round, level;
    timer_handle_t iterator;
    
    interrupt_level = global_interrupt_disable ();
    _handle->bucket_index_ = (g_cursor + _handle->ticks_) % CONFIG_MAX_BUCKET;
    dll_remove (&g_inactive_timer, &_handle->node_);

    p_bucket = &g_buckets [_handle->bucket_index_];
    round = _handle->ticks_ / CONFIG_MAX_BUCKET;
    level = ++ p_bucket->level_;
    p_bucket->reentrance_ ++;

redo:
    iterator = (timer_handle_t) dll_head (&p_bucket->dll_);
    _handle->round_ = round;
    for (;;) {
        if (0 == iterator) {
            dll_push_tail (&p_bucket->dll_, &_handle->node_);
            break;
        }
        if (_handle->round_ <= iterator->round_) {
            iterator->round_ -= _handle->round_;
            dll_insert_before (&p_bucket->dll_, &iterator->node_, &_handle->node_);
            break;
        }
        _handle->round_ -= iterator->round_;
        iterator = (timer_handle_t) dll_next (&p_bucket->dll_, &iterator->node_);
            
        count ++;
        if (count < CONFIG_INTERRUPT_FLASH_FREQUENCY) {
            continue;
        }
        
        count = 0;
        global_interrupt_enable (interrupt_level);
        // at this moment we give a chance to the higher pirority interrupt
        // (or task) for being served (or running)
        interrupt_level = global_interrupt_disable ();
        if (p_bucket->level_ != level) {
            level = ++ p_bucket->level_;
            p_bucket->redo_ ++;
            goto redo;
        }
    }
    if (g_bucket_firing == p_bucket) {
        if(0 == g_timer_next || _handle->round_ <= g_timer_next->round_) {
            g_timer_next = _handle;
        }
    }
    p_bucket->hit_ ++;
    if (0 == -- p_bucket->reentrance_) {
        p_bucket->level_ = 0;
    }
    _handle->state_ = TIMER_STARTED;
    global_interrupt_enable (interrupt_level);
}

error_t timer_start (timer_handle_t _handle, msecond_t _duration, 
    expiry_callback_t _cb, void *_arg)
{
    interrupt_level_t level;
    
    if (null == _cb) {
        return ERROR_T (ERROR_TIMER_ALLOC_INVCB);
    }
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TIMER_START_INVHANDLE);
    }
    if (TIMER_STARTED == _handle->state_) {
        g_statistics.abnormal_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TIMER_START_INVSTATE);
    }

    _handle->ticks_ = _duration / CONFIG_TICK_DURATION_IN_MSEC;
    if (0 == _handle->ticks_) {
        _handle->ticks_ ++;
    }
    _handle->callback_ = _cb;
    _handle->arg_ = _arg;
    global_interrupt_enable (level);
    timer_insert (_handle);
    return 0;
}

error_t timer_restart (timer_handle_t _handle)
{
    interrupt_level_t level;
    
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TIMER_RESTART_INVHANDLE);
    }
    if (TIMER_STOPPED != _handle->state_) {
        g_statistics.abnormal_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TIMER_RESTART_INVSTATE);
    }
    global_interrupt_enable (level);

    timer_insert (_handle);
    return 0;
}

static bool timer_calculate_remained (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    timer_handle_t handle = (timer_handle_t)_p_node;
    timer_remained_t *p_remained = (timer_remained_t *)_p_arg;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    if (p_remained->handle_ == handle) {
        p_remained->remained_round_ += handle->round_;
        return false;
    }
    p_remained->remained_round_ += handle->round_;
    return true;
}

error_t timer_stop (timer_handle_t _handle, msecond_t *_p_remained)
{
    interrupt_level_t level;
    timer_handle_t next;
    bucket_t *p_bucket;
    
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TIMER_STOP_INVHANDLE);
    }
    if (_handle->state_ != TIMER_STARTED) {
        g_statistics.abnormal_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TIMER_STOP_INVSTATE);
    }
    
    if (g_timer_next == _handle) {
        g_timer_next = (timer_handle_t) dll_next (&g_bucket_firing->dll_, 
            &_handle->node_);
    }
    p_bucket = &g_buckets [_handle->bucket_index_];
    if (_p_remained != null) {
        timer_remained_t remained = {_handle, 0};
        
        (void) dll_traverse (&p_bucket->dll_, timer_calculate_remained, &remained);
        *_p_remained = remained.remained_round_ * CONFIG_MAX_BUCKET;
        if (g_cursor > _handle->bucket_index_) {
            *_p_remained = (*_p_remained) * CONFIG_MAX_BUCKET - 
                (g_cursor - _handle->bucket_index_);
        }
        else {
            *_p_remained = (*_p_remained - 1) * CONFIG_MAX_BUCKET +
                (g_cursor - _handle->bucket_index_) + 1;
        }
        *_p_remained *= CONFIG_TICK_DURATION_IN_MSEC;
    }
    _handle->state_ = TIMER_STOPPED;
    next = (timer_handle_t)dll_next (&p_bucket->dll_, &_handle->node_);
    if (0 != next) {
        next->round_ += _handle->round_;
    }
    dll_remove (&p_bucket->dll_, &_handle->node_);
    dll_push_tail (&g_inactive_timer, &_handle->node_);
    if (p_bucket->reentrance_ > 0) {
        p_bucket->level_ ++;
    }
    global_interrupt_enable (level);

    return 0;
}

//lint -e{818}
bool timer_is_started (const timer_handle_t _handle)
{
    interrupt_level_t level;
    bool started;
    
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        started = false;
    }
    else {
        started = (bool)(_handle->state_ == TIMER_STARTED);
    }
    global_interrupt_enable (level);
    return started;
}

void timer_dump ()
{
    usize_t index;
    
    scheduler_lock ();
    console_print ("\n");
    console_print ("Summary\n");
    console_print ("-------\n");
    console_print ("  Supported: %u\n", CONFIG_MAX_TIMER);
    console_print ("  Allocated: %u\n", CONFIG_MAX_TIMER - 
        dll_size (&g_free_timer));
    console_print ("  .BSS Used: %u\n", ((address_t)&g_bucket_firing 
        - (address_t)g_timer_pool) + sizeof (g_bucket_firing));
    console_print ("\n");
    console_print ("Statistics\n");
    console_print ("----------\n");
    console_print ("    No Timer: %u\n", g_statistics.notimer_);
    console_print ("    Abnormal: %u\n", g_statistics.abnormal_);
    console_print ("   Traversed: %u\n", g_statistics.traversed_);
    console_print ("  Queue Full: %u\n", g_statistics.queue_full_);
    console_print ("\n");
    console_print ("Bucket Details\n");
    console_print ("--------------\n");
    for (index = 0; index <= BUCKET_LAST_INDEX; ++ index) {
        console_print ("  [%u]: hit (%u), redo (%u), timer held (%u)\n", 
            index, g_buckets [index].hit_, g_buckets [index].redo_, 
            dll_size (&g_buckets [index].dll_));
    }
    console_print ("\n");
    scheduler_unlock ();
}

