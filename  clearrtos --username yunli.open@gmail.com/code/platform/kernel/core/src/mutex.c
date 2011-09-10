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
#include "mutex.h"
#include "errsync.h"
#include "task.h"
#include "console.h"

static mutex_t g_mutex_pool [CONFIG_MAX_MUTEX];
static sync_container_t g_mutex_container;

static bool mutex_callback_lock (sync_object_handle_t _handle)
{
    mutex_handle_t p_mutex = (mutex_handle_t) _handle;
    task_handle_t p_task = task_self ();
    
    if (null == p_mutex->owner_) {
        // grab the mutex
        p_mutex->owner_ = p_task;
        p_mutex->inherited_ = false;
        p_mutex->reference_ = 0;
        return true;
    }
    else if (p_mutex->is_recursive_ && p_task == p_mutex->owner_) {
        p_mutex->reference_ ++;
        return true;
    }
    return false;
}

static void mutex_callback_wait (sync_object_handle_t _handle)
{
    mutex_handle_t p_mutex = (mutex_handle_t) _handle;
    task_handle_t p_task = task_self ();
    
    if (p_mutex->owner_->priority_ > p_task->priority_) {
        if (!p_mutex->inherited_) {
            p_mutex->original_ = p_mutex->owner_->priority_;
            p_mutex->inherited_ = true;
        }
        task_priority_change (p_mutex->owner_, p_task->priority_);
    }
}

static bool mutex_callback_unlock (sync_object_handle_t _handle,
    error_t *_p_ecode)
{
    mutex_handle_t p_mutex = (mutex_handle_t) _handle;
    task_handle_t p_task = task_self ();

    // grab the mutex
    if (is_in_interrupt () || is_invalid_task (p_task)) {
        return ERROR_T (ERROR_MUTEX_INVCONTEXT);
    }
    if (p_mutex->owner_ != p_task) {
        *_p_ecode = ERROR_T (ERROR_MUTEX_NOTOWNER);
        return false;
    }
    if (p_mutex->reference_ > 0) {
        p_mutex->reference_ --;
        return true;
    }
    // restore owner's original priority
    if (p_mutex->inherited_) {
        p_mutex->inherited_ = false;
        task_priority_change (p_mutex->owner_, p_mutex->original_);
    }
    if (task_bitmap_is_empty (&p_mutex->object_.pending_bitmap_)) {
        // no task is pending on this mutex
        p_mutex->owner_ = null;
        return true;
    }
    return false;
}

static void mutex_callback_wake (sync_object_handle_t _handle)
{
    mutex_handle_t p_mutex = (mutex_handle_t) _handle;
    task_handle_t p_task;
    bit_t bit;

    bit = task_bitmap_lowest_bit_get (&_handle->pending_bitmap_);
    task_bitmap_bit_clear (&_handle->pending_bitmap_, bit);
    p_task = task_from_priority ((task_priority_t)bit);
    (void) task_state_change (p_task, TASK_STATE_READY);
    p_mutex->owner_ = p_task;
}

static bool mutex_check_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    //lint -e{740, 826}
    mutex_handle_t handle = (mutex_handle_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    console_print ("Error: mutex \"%s\" isn't deleted\n", handle->object_.name_);
    return true;
}

error_t module_mutex (system_state_t _state)
{
    if (STATE_DESTROYING == _state) {
        // check whether all mutics created have been deleted or not, if not take
        // them as error
        (void) dll_traverse (&g_mutex_container.used_, mutex_check_for_each, 0);
    }
    return 0;
}

static void mutex_init ()
{
    sync_operation_t opt = {mutex_callback_lock, mutex_callback_wait, 
        mutex_callback_unlock, mutex_callback_wake};
    //lint -e{545}
    (void) sync_container_init (&g_mutex_container, &g_mutex_pool, 
        CONFIG_MAX_MUTEX, sizeof (mutex_t), &opt);
}

error_t mutex_create (mutex_handle_t *_p_handle, const char _name [], 
    bool _recursive)
{
    static bool initialized = false;
    interrupt_level_t level;
    error_t ecode;

    level = global_interrupt_disable ();
    if (!initialized) {
        mutex_init ();
        initialized = true;
    }
    global_interrupt_enable (level);
    ecode = sync_object_alloc (&g_mutex_container, 
        (sync_object_handle_t *) _p_handle, _name);
    if (0 == ecode) {
        (*_p_handle)->owner_ = null;
        (*_p_handle)->inherited_ = false;
        (*_p_handle)->is_recursive_ = _recursive;
    }
    return ecode;
}

error_t mutex_delete (mutex_handle_t _handle)
{
    return sync_object_free (&_handle->object_);
}

error_t mutex_try_to_lock (mutex_handle_t _handle)
{
    return sync_point_try_to_enter (&_handle->object_);
}

error_t mutex_lock (mutex_handle_t _handle, msecond_t _timeout)
{
    return sync_point_enter (&_handle->object_, _timeout);
}

error_t mutex_unlock (mutex_handle_t _handle)
{
    return sync_point_exit (&_handle->object_);
}

static bool mutex_dump_for_each (dll_t *_p_dll, dll_node_t *_p_node, 
    void *_p_arg)
{
    //lint -e{740, 826}
    mutex_handle_t handle = (mutex_handle_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);
    
    console_print ("  Name: %s\n", handle->object_.name_);
    if (0 == handle->owner_) {
        console_print ("    Owner: null\n");
    }
    else {
        console_print ("    Owner: %s\n", handle->owner_->name_);
    }
    console_print ("\n");
    return true;
}

void mutex_dump ()
{
    if (is_in_interrupt ()) {
        return;
    }

    scheduler_lock ();
    console_print ("\n\n");
    console_print ("Summary\n");
    console_print ("-------\n");
    console_print ("  Supported: %u\n", CONFIG_MAX_MUTEX);
    console_print ("  Allocated: %u\n", dll_size (&g_mutex_container.used_));
    console_print ("  .BSS Used: %u\n", ((address_t)&g_mutex_container 
        - (address_t)g_mutex_pool) + sizeof (g_mutex_container));
    console_print ("\n");
    console_print ("Statistics\n");
    console_print ("----------\n");
    console_print ("  No Object: %u\n", g_mutex_container.stats_noobj_);
    console_print ("\n");
    console_print ("Mutex Details\n");
    console_print ("-------------\n");
    (void) dll_traverse (&g_mutex_container.used_, mutex_dump_for_each, 0);
    console_print ("\n");
    scheduler_unlock ();
}

