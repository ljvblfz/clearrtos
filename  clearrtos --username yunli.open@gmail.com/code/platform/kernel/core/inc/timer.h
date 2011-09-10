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
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __TIMER_H
#define __TIMER_H

#include "primitive.h"
#include "error.h"
#include "dll.h"
#include "module.h"

typedef struct type_timer *timer_handle_t;

// callback function for timer expiration
typedef void (*expiry_callback_t)(timer_handle_t _handle, void *_arg);

typedef enum {
    TIMER_CREATED,
    TIMER_STARTED,
    TIMER_STOPPED
} timer_state_t;

typedef enum {
    TIMER_TYPE_INTERRUPT,
    TIMER_TYPE_TASK
} timer_type_t;

typedef struct type_timer {
    dll_node_t node_;
    expiry_callback_t callback_;
    timer_state_t state_;
    void *arg_;
    usize_t ticks_;
    usize_t bucket_index_;
    usize_t round_;
    magic_number_t magic_number_;
    char name_ [NAME_MAX_LENGTH + 1];
    timer_type_t type_;
} timer_instance_t;

#ifdef  __cplusplus
extern "C" {
#endif

error_t module_timer (system_state_t _state);

int timer_lock_init ();
void timer_fire ();
error_t timer_alloc (timer_handle_t *_p_handle, const char *_name,
    timer_type_t _type);
error_t timer_free (timer_handle_t _handle);
error_t timer_start (timer_handle_t _handle, msecond_t _duration, 
    expiry_callback_t _cb, void *_arg);
error_t timer_restart (timer_handle_t _handle);
error_t timer_stop (timer_handle_t _handle, msecond_t *_p_remained);
bool timer_is_started (const timer_handle_t _handle);
void timer_dump ();

#ifdef __cplusplus
}
#endif

#endif

