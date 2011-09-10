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
 
#ifndef __EVENT_H
#define __EVENT_H

#include "syncobj.h"

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
typedef u32_t event_option_t;
#define __event_option_defined__
#endif

#define EVENT_WAIT_ALL          0x01
#define EVENT_WAIT_ANY          0x02
#define EVENT_RETURN_ALL        0x04
#define EVENT_RETURN_EXPECTED   0x08

#ifdef  __cplusplus
extern "C" {
#endif

error_t event_receive (event_set_t _expected, event_set_handle_t _received, 
    msecond_t _timeout, event_option_t _option);
error_t event_send (task_handle_t _handle, event_set_t _sent);
error_t event_clear ();

#ifdef __cplusplus
}
#endif

#endif

