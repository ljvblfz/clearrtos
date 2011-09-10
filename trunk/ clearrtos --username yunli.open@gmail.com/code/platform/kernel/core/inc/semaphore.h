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
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include "syncobj.h"
#include "module.h"

typedef struct {
    sync_object_t object_;
    usize_t count_;
} semaphore_t, *semaphore_handle_t;

#ifdef  __cplusplus
extern "C" {
#endif

error_t module_semaphore (system_state_t _state);

error_t semaphore_create (semaphore_handle_t *_p_handle, const char _name [], 
    usize_t _count);
error_t semaphore_delete (semaphore_handle_t _handle);
error_t semaphore_try_to_take (semaphore_handle_t _handle);
error_t semaphore_take (semaphore_handle_t _handle, msecond_t _timeout);
error_t semaphore_give (semaphore_handle_t _handle);
usize_t semaphore_count_get (const semaphore_handle_t _handle);
void semaphore_dump ();

#ifdef __cplusplus
}
#endif

#endif

