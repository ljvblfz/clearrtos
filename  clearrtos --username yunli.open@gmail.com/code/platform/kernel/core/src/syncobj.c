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
 *   Date: 08/16/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "syncobj.h"
#include "errsync.h"
#include "interrupt.h"
#include "task.h"
#include "errtask.h"
#include "clib.h"

#define MAGIC_NUMBER_SYNCOBJ        0x53594E43L
#define MAGIC_NUMBER_CONTAINER      0x4D414749L

#define is_invalid_handle(_handle) \
    ((_handle == null) || ((_handle)->magic_number_ != MAGIC_NUMBER_SYNCOBJ))

//lint -e{818}
error_t sync_container_init (sync_container_handle_t _handle, void *_p_objects, 
    usize_t _obj_count, usize_t _obj_size, const sync_operation_handle_t _opt)
{
    interrupt_level_t level;
    usize_t idx = 0;
    char *p_objects = (char *)(_p_objects);
    sync_object_handle_t p_object;

    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_SYNC_INIT_INVCONTEXT);
    }

    level = global_interrupt_disable ();
    if (MAGIC_NUMBER_CONTAINER == _handle->magic_number_) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_SYNC_INIT_INVINIT);
    }
    memset (_handle, 0, sizeof (*_handle));
    _handle->magic_number_ = MAGIC_NUMBER_CONTAINER;
    global_interrupt_enable (level);
    
    dll_init (&_handle->free_);
    dll_init (&_handle->used_);
    _handle->opt_ = *_opt;
    memset (_p_objects, 0, _obj_count * _obj_size);
    for (; idx < _obj_count; idx ++, p_objects += _obj_size) {
        //lint -e{826}
        p_object = (sync_object_handle_t) p_objects;
        dll_push_tail (&_handle->free_, &p_object->node_);
    }
    return 0;
}

error_t sync_object_alloc (sync_container_handle_t _container, 
    sync_object_handle_t *_p_handle, const char _name [])
{
    interrupt_level_t level;
    sync_object_handle_t handle;

    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_SYNC_ALLOC_INVCONTEXT);
    }
    
    *_p_handle = null;
    level = global_interrupt_disable ();
    handle = (sync_object_handle_t) dll_pop_head (&_container->free_);
    if (null == handle) {
        *_p_handle = null;
        _container->stats_noobj_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_SYNC_ALLOC_NOOBJ);
    }
    memset (handle, 0, sizeof (*handle));
    task_bitmap_init (&handle->pending_bitmap_);
    if (0 == _name) {
        handle->name_ [0] = 0;
    }
    else {
        strncpy (handle->name_, _name, (usize_t)sizeof (handle->name_) - 1);
        handle->name_ [sizeof (handle->name_) - 1] = 0;
    }
    handle->container_ = _container;
    handle->magic_number_ = MAGIC_NUMBER_SYNCOBJ;
    dll_push_tail (&_container->used_, &handle->node_);
    global_interrupt_enable (level);
    *_p_handle = handle;
    return 0;
}

error_t sync_object_free (sync_object_handle_t _handle)
{
    interrupt_level_t level;
    sync_container_handle_t container;

    if (is_in_interrupt () && STATE_UP == system_state ()) {
        return ERROR_T (ERROR_SYNC_FREE_INVCONTEXT);
    }
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_SYNC_FREE_INVHANDLE);
    }
    container = _handle->container_;
    if (task_bitmap_is_empty (&_handle->pending_bitmap_)) {
        // no task is pending on this object
        _handle->magic_number_ = 0;
        dll_remove (&container->used_, &_handle->node_);
        dll_push_tail (&container->free_, &_handle->node_);
        global_interrupt_enable (level);
        return 0;
    }
    // there is/are task(s) pending on this object, wait it up one by one
    do {
        bit_t bit = task_bitmap_lowest_bit_get (&_handle->pending_bitmap_);
        task_handle_t p_task = task_from_priority ((task_priority_t)bit);
        task_bitmap_bit_clear (&_handle->pending_bitmap_, bit);
        if (is_invalid_task (p_task)) {
            continue;
        }
        p_task->ecode_ = ERROR_T (ERROR_SYNC_FREE_DELETED);
        (void) task_state_change (p_task, TASK_STATE_READY);
    } while (!task_bitmap_is_empty (&_handle->pending_bitmap_));
    _handle->magic_number_ = 0;
    dll_remove (&container->used_, &_handle->node_);
    dll_push_tail (&container->free_, &_handle->node_);
    global_interrupt_enable (level);
    task_schedule (null);
    return 0;
}

error_t sync_point_try_to_enter (sync_object_handle_t _handle)
{
    interrupt_level_t level;

    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_SYNC_ENTER_INVCONTEXT);
    }
    
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_SYNC_ENTER_INVHANDLE);
    }
    if (is_invalid_task (task_self ())) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_SYNC_ENTER_INVTASK);
    }
    if (_handle->container_->opt_.enter_ (_handle)) {
        global_interrupt_enable (level);
        return 0;
    }
    global_interrupt_enable (level);
    return ERROR_T (ERROR_SYNC_ENTER_TRYAGAIN);
}

error_t sync_point_enter (sync_object_handle_t _handle, msecond_t _timeout)
{
    interrupt_level_t level;
    sync_container_handle_t container;
    task_handle_t p_task = task_self ();

    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_SYNC_ENTER_INVCONTEXT);
    }

    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_SYNC_ENTER_INVHANDLE);
    }
    if (is_invalid_task (p_task)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_SYNC_ENTER_INVTASK);
    }
    container = _handle->container_;
    if (container->opt_.enter_ (_handle)) {
        global_interrupt_enable (level);
        return 0;
    }
    // the object isn't available and need to block the task
    task_bitmap_bit_set (&_handle->pending_bitmap_, p_task->priority_);
    p_task->timeout_ = _timeout;
    (void) task_state_change (p_task, TASK_STATE_WAITING);
    p_task->ecode_ = 0;
    container->opt_.wait_ (_handle);
    global_interrupt_enable (level);
    task_schedule (null);
    //lint -e{650}
    if (ERROR_TASK_WAIT_TIMEOUT == MODULE_ERROR (p_task->ecode_)) {
        p_task->ecode_ = ERROR_T (ERROR_SYNC_ENTER_TIMEOUT);
    }
    return p_task->ecode_;
}

error_t sync_point_exit (sync_object_handle_t _handle)
{
    interrupt_level_t level;
    sync_container_handle_t container;
    error_t ecode = 0;

    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_SYNC_LEAVE_INVHANDLE);
    }
    container = _handle->container_;
    if (container->opt_.exit_ (_handle, &ecode)) {
        global_interrupt_enable (level);
        return 0;
    }
    if (0 != ecode) {
        global_interrupt_enable (level);
        return ecode;
    }
    // there is/are task(s) pending on this object, get the highest priority
    // task to be READY
    container->opt_.wake_ (_handle);
    global_interrupt_enable (level);
    task_schedule (null);
    return 0;
}

