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
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __MUTEX_H
#define __MUTEX_H

#include "syncobj.h"
#include "module.h"
#include "interrupt.h"

#ifndef __task_priority_defined__
typedef u32_t task_priority_t;
#define __task_priority_defined__
#endif

#ifndef __task_handler_defined__
struct type_task;
typedef struct type_task task_t, *task_handler_t;
#define __task_handler_defined__
#endif

typedef struct {
    sync_object_t object_;
    task_handler_t owner_;
    bool inherited_;
    task_priority_t original_;
    bool is_recursive_;
    usize_t reference_;
} mutex_t, *mutex_handler_t;

#ifdef  __cplusplus
extern "C" {
#endif

error_t module_mutex (system_state_t _state);

error_t mutex_create (mutex_handler_t *_p_handler, const char _name [], 
    bool _recursive);
error_t mutex_delete (mutex_handler_t _handler);
error_t mutex_try_to_lock (mutex_handler_t _handler);
error_t mutex_lock (mutex_handler_t _handler, msecond_t _timeout);
error_t mutex_unlock (mutex_handler_t _handler);
void mutex_dump ();

#ifdef __cplusplus
}
#endif

#endif

