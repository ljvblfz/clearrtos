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
 *   Date: 08/29/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "module.h"
#include "device.h"

typedef struct {
    device_t common_;
    bool is_opened_;
} device_console_t, *console_handle_t;

#ifdef  __cplusplus
extern "C"
{
#endif

error_t console_driver_install (const char _name[]);
error_t console_device_register (const char _name [], console_handle_t _handle);

void console_handle_set (device_handle_t _handle);
void console_print (const char* _format, ...);

#ifdef  __cplusplus
}
#endif

#endif

