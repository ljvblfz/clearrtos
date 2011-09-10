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
 *   Date: 08/06/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __CLOCK_H
#define __CLOCK_H

#include "primitive.h"
#include "interrupt.h"
#include "module.h"

#define OPTION_TICK_START   (((int)(MODULE_CLOCK) << 8) + 1)
#define OPTION_TICK_STOP    (((int)(MODULE_CLOCK) << 8) + 2)

typedef struct {
    device_t common_;
    bool is_opened_;
    void (*tick_process_) (); 
} device_clock_t, *clock_handle_t;

#ifdef  __cplusplus
extern "C"
{
#endif

error_t clock_driver_install (const char _name[]);
error_t clock_device_register (const char _name [], clock_handle_t _handle);

#ifdef __cplusplus
}
#endif

#endif

