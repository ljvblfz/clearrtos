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
 *   Date: 08/15/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "event.h"
#include "errsync.h"
#include "interrupt.h"
#include "task.h"
#include "errtask.h"

//lint -e655

error_t event_receive (event_set_t _expected, event_set_handle_t _received, 
    msecond_t _timeout, event_option_t _option)
{
    interrupt_level_t level;
    event_option_t wait_option = EVENT_WAIT_ALL | EVENT_WAIT_ANY;
    event_option_t return_option = EVENT_RETURN_ALL | EVENT_RETURN_EXPECTED;
    task_handle_t p_task;

    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_EVENT_RECV_INVCONTEXT);
    }
    if (0 == _expected) {
        return 0;
    }
    if (null == _received) {
        return ERROR_T (ERROR_EVENT_RECV_INVPTR);
    }
    if ((wait_option == (wait_option & _option)) ||
        (0 == (int)(wait_option & _option)) ||
        (return_option == (return_option & _option)) ||
        (0 == (int)(return_option & _option))) {
        return ERROR_T (ERROR_EVENT_RECV_INVOPT);
    }
    
    level = global_interrupt_disable ();
    p_task = task_self ();
    if (is_invalid_task (p_task)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_EVENT_RECV_INVCONTEXT);
    }
again:
    // if expected event(s) is/are available, return it
    if (EVENT_WAIT_ALL == (_option & EVENT_WAIT_ALL)) {
        if ((_expected & p_task->event_received_) == _expected) {
            if (EVENT_RETURN_ALL == (_option & EVENT_RETURN_ALL)) {
                *_received = p_task->event_received_;
            }
            else {
                *_received = _expected;
                p_task->event_received_ &= ~(*_received);
            }
            global_interrupt_enable (level);
            return 0;
        }
    }
    else {
        if ((_expected & p_task->event_received_) != 0) {
            if (EVENT_RETURN_ALL == (_option & EVENT_RETURN_ALL)) {
                *_received = p_task->event_received_;
            }
            else {
                *_received = _expected & p_task->event_received_;
                p_task->event_received_ &= ~(*_received);
            }
            global_interrupt_enable (level);
            return 0;
        }
    }
    // run here it means we need to block the task
    p_task->timeout_ = _timeout;
    (void) task_state_change (p_task, TASK_STATE_WAITING);
    p_task->ecode_ = 0;
    p_task->event_expected_ = _expected;
    p_task->event_option_ = _option;
    global_interrupt_enable (level);
    task_schedule (null);
    level = global_interrupt_disable ();
    p_task->event_option_ = (event_option_t) 0;
    if (0 == p_task->ecode_) {
        // event(s) has/have received and need to return the event(s) expected
        goto again;
    }
    global_interrupt_enable (level);
    //lint -e{650}
    if (ERROR_TASK_WAIT_TIMEOUT == MODULE_ERROR (p_task->ecode_)) {
        p_task->ecode_ = ERROR_T (ERROR_EVENT_RECV_TIMEOUT);
    }
    return p_task->ecode_;
}

error_t event_send (task_handle_t _handle, event_set_t _sent)
{
    interrupt_level_t level;
    bool wakeup_needed = false;

    level = global_interrupt_disable ();
    if (is_invalid_task (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_EVENT_RECV_INVRECEIVER);
    }
    _handle->event_received_ |= _sent;
    if (0 == (int)_handle->event_option_) {
        global_interrupt_enable (level);
        return 0;
    }
    if (EVENT_WAIT_ALL == (_handle->event_option_ & EVENT_WAIT_ALL)) {
        if ((_handle->event_expected_ & _handle->event_received_) == 
            _handle->event_expected_) {
            wakeup_needed = true;
        }
    }
    else {
        if ((_handle->event_expected_ & _handle->event_received_) != 0) {
            wakeup_needed = true;
        }
    }
    if (!wakeup_needed) {
        global_interrupt_enable (level);
        return 0;
    }
    (void) task_state_change (_handle, TASK_STATE_READY);
    global_interrupt_enable (level);
    task_schedule (null);
    return 0;
}

error_t event_clear ()
{
    interrupt_level_t level;
    task_handle_t p_task = task_self ();

    level = global_interrupt_disable ();
    if (is_invalid_task (p_task)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_EVENT_CLEAR_INVCONTEXT);
    }
    p_task->event_received_ = 0;
    global_interrupt_enable (level);
    return 0;
}

