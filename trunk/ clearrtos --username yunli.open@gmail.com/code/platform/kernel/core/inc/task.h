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
 *   Date: 08/11/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __TASK_H
#define __TASK_H

#include "config.h"
#include "dll.h"
#include "module.h"
#include "context.h"
#include "bitmap.h"
#include "timer.h"
#include "event.h"

#ifndef __task_priority_defined__
typedef u32_t task_priority_t;
#define __task_priority_defined__
#endif

#ifndef __task_handle_defined__
struct type_task;
typedef struct type_task task_t, *task_handle_t;
#define __task_handle_defined__
#endif

#ifndef __event_set_defined__
typedef u32_t event_set_t, *event_set_handle_t;
#define __event_set_defined__
#endif

#ifndef __event_option_defined__
typedef enum type_event_option event_option_t;
#define __event_option_defined__
#endif

typedef void (*task_entry_t) (const char _name [], void *_p_arg);
typedef void (*preschedule_callback_t) (task_handle_t _from, task_handle_t _to);

#define STACK_DECLARE(_name, _size) static stack_unit_t _name [_size]

typedef enum {
    TASK_STATE_CREATED = 0x001,
    // task is ready to be scheduled
    TASK_STATE_READY = 0x002,
    TASK_STATE_RUNNING = 0x004,
    TASK_STATE_SUSPENDING = 0x008,
    // task is waiting for timeout, semaphore or mutex
    TASK_STATE_WAITING = 0x010,
    TASK_STATE_DELETED = 0x020
} task_state_t;

// task control block
struct type_task {
    dll_node_t node_;
    magic_number_t magic_number_;
    
    task_context_t context_;
    task_state_t state_;
    timer_handle_t timer_;
    msecond_t timeout_;
    error_t ecode_;

    // statistics
    statistic_t stats_scheduled_;
    
    // user provided information
    task_priority_t priority_;
    address_t stack_base_;
    usize_t stack_size_;
    task_entry_t entry_;
    void *argument_;
    char name_ [NAME_MAX_LENGTH + 1];

    dll_t variable_;
    
    // for supporting event
    event_set_t event_expected_;
    event_set_t event_received_;
    event_option_t event_option_;
};

#ifdef  __cplusplus
extern "C" {
#endif

// for module initialization
error_t module_task (system_state_t _state);

// for task environment control
void multitasking_start ();
void multitasking_stop ();

// for task control
error_t task_create (task_handle_t *_p_handle, const char _name [], 
    task_priority_t _priority, stack_unit_t *_stack_base, usize_t _stack_bytes);
error_t task_delete (task_handle_t _handle);
error_t task_start (task_handle_t _handle, task_entry_t _entry, void *_p_arg);
error_t task_suspend (task_handle_t _handle);
error_t task_resume (task_handle_t _handle);
error_t task_sleep (msecond_t _duration);
task_handle_t task_self ();
task_handle_t task_from_priority (task_priority_t _priority);
error_t is_stack_overflowed (const task_handle_t _handle, bool *_p_overflowed);
error_t stack_used_percentage (const task_handle_t _handle, int *_p_percentage);
void task_dump ();

void scheduler_lock ();
void scheduler_unlock ();

// below functions should ONLY be called by semphore, mutex or event module
// rather then by application
bool is_invalid_task (const task_handle_t _handle);
error_t task_state_change (task_handle_t _handle, task_state_t _new_state);
void task_schedule (preschedule_callback_t _callback);
void task_priority_change (task_handle_t _handle, task_priority_t _to);

#ifdef UNITEST
dll_t *unitest_get_allocated_dll ();
#endif

#ifdef __cplusplus
}
#endif

#endif

