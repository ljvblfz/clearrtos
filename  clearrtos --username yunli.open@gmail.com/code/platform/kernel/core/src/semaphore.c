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
 *   Date: 08/10/2010                                                         *
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
#include "semaphore.h"
#include "errsync.h"
#include "interrupt.h"
#include "task.h"
#include "console.h"

static semaphore_t g_semaphore_pool [CONFIG_MAX_SEMAPHORE];
static sync_container_t g_semaphore_container;

static bool semaphore_callback_take (sync_object_handle_t _handle)
{
    semaphore_handle_t p_semaphore = (semaphore_handle_t) _handle;
    if (0 != p_semaphore->count_) {
        // semaphore is available, grab it
        p_semaphore->count_ --;
        return true;
    }
    return false;
}

static void semaphore_callback_wait (sync_object_handle_t _handle)
{
    UNUSED (_handle);
}

static bool semaphore_callback_give (sync_object_handle_t _handle,
    error_t *_p_ecode)
{
    semaphore_handle_t p_semaphore = (semaphore_handle_t) _handle;

    UNUSED (_p_ecode);
    
    if (task_bitmap_is_empty (&p_semaphore->object_.pending_bitmap_)) {
        // no task is pending on this semaphore
        p_semaphore->count_ ++;
        return true;
    }
    return false;
}

static void semaphore_callback_wake (sync_object_handle_t _handle)
{
    task_handle_t p_task;
    bit_t bit;
    
    UNUSED (_handle);

    bit = task_bitmap_lowest_bit_get (&_handle->pending_bitmap_);
    task_bitmap_bit_clear (&_handle->pending_bitmap_, bit);
    p_task = task_from_priority ((task_priority_t)bit);
    (void) task_state_change (p_task, TASK_STATE_READY);
}

static bool semaphore_check_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    //lint -e{740, 826}
    semaphore_handle_t handle = (semaphore_handle_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    console_print ("Error: semaphore \"%s\" isn't deleted\n", 
        handle->object_.name_);
    return true;
}

error_t module_semaphore (system_state_t _state)
{
    if (STATE_DESTROYING == _state) {
        // check whether all semaphores created have been deleted or not, if not
        // take them as error
        (void) dll_traverse (&g_semaphore_container.used_, 
            semaphore_check_for_each, 0);
    }
    return 0;
}

static void semaphore_init ()
{
    sync_operation_t opt = {semaphore_callback_take, semaphore_callback_wait, 
        semaphore_callback_give, semaphore_callback_wake};
    //lint -e{545}
    (void) sync_container_init (&g_semaphore_container, &g_semaphore_pool, 
        CONFIG_MAX_SEMAPHORE, sizeof (semaphore_t), &opt);
}

error_t semaphore_create (semaphore_handle_t *_p_handle, const char _name [], 
    usize_t _count)
{
    static bool initialized = false;
    interrupt_level_t level;
    error_t ecode;

    level = global_interrupt_disable ();
    if (!initialized) {
        semaphore_init ();
        initialized = true;
    }
    global_interrupt_enable (level);
    ecode = sync_object_alloc (&g_semaphore_container, 
        (sync_object_handle_t *)_p_handle, _name);
    if (0 == ecode) {
        (*_p_handle)->count_ = _count;
    }
    return ecode;
}

error_t semaphore_delete (semaphore_handle_t _handle)
{
    return sync_object_free (&_handle->object_);
}

error_t semaphore_try_to_take (semaphore_handle_t _handle)
{
    return sync_point_try_to_enter (&_handle->object_);
}

error_t semaphore_take (semaphore_handle_t _handle, msecond_t _timeout)
{
    return sync_point_enter (&_handle->object_, _timeout);
}

error_t semaphore_give (semaphore_handle_t _handle)
{
    return sync_point_exit (&_handle->object_);
}

//lint -e{818}
usize_t semaphore_count_get (const semaphore_handle_t _handle)
{
    interrupt_level_t level;
    usize_t count;

    level = global_interrupt_disable ();
    count = _handle->count_;
    global_interrupt_enable (level);
    return count;
}

static bool semaphore_dump_for_each (dll_t *_p_dll, dll_node_t *_p_node, 
    void *_p_arg)
{
    //lint -e{740, 826}
    semaphore_handle_t handle = (semaphore_handle_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);
    
    console_print ("  Name: %s\n", handle->object_.name_);
    console_print ("    Count: %u\n", handle->count_);
    console_print ("\n");
    return true;
}

void semaphore_dump ()
{
    if (is_in_interrupt ()) {
        return;
    }

    scheduler_lock ();
    console_print ("\n\n");
    console_print ("Summary\n");
    console_print ("-------\n");
    console_print ("  Supported: %u\n", CONFIG_MAX_SEMAPHORE);
    console_print ("  Allocated: %u\n", dll_size (&g_semaphore_container.used_));
    console_print ("  .BSS Used: %u\n", ((address_t)&g_semaphore_container 
        - (address_t)g_semaphore_pool) + sizeof (g_semaphore_container));
    console_print ("\n");
    console_print ("Statistics\n");
    console_print ("----------\n");
    console_print ("  No Object: %u\n", g_semaphore_container.stats_noobj_);
    console_print ("\n");
    console_print ("Semaphore Details\n");
    console_print ("-----------------\n");
    (void) dll_traverse (&g_semaphore_container.used_, semaphore_dump_for_each, 0);
    console_print ("\n");
    scheduler_unlock ();
}

