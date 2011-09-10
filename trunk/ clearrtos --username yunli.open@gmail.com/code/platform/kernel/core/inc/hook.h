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
 *   Date: 08/14/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __HOOK_H
#define __HOOK_H

#include "primitive.h"
#include "task.h"

typedef void (*task_create_hook_t) (task_handle_t _handle);
typedef void (*task_switch_hook_t) (task_handle_t _from, task_handle_t _to);
typedef void (*task_delete_hook_t) (task_handle_t _handle);

#ifdef  __cplusplus
extern "C" {
#endif

error_t task_create_hook_add (task_create_hook_t _hook);
error_t task_create_hook_remove (task_create_hook_t _hook);
void task_create_hook_traverse (task_handle_t _handle);
error_t task_switch_hook_add (task_switch_hook_t _hook);
error_t task_switch_hook_remove (task_switch_hook_t _hook);
void task_switch_hook_traverse (task_handle_t _from, task_handle_t _to);
error_t task_delete_hook_add (task_delete_hook_t _hook);
error_t task_delete_hook_remove (task_delete_hook_t _hook);
void task_delete_hook_traverse (task_handle_t _handle);

#ifdef __cplusplus
}
#endif

#endif

