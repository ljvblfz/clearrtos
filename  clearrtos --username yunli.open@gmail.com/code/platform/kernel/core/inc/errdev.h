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
 *   Date: 08/27/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __ERRDEV_H
#define __ERRDEV_H

#include "module.h"

enum {
    // for driver
    ERROR_DRIVER_NODRV = ERROR_BEGIN (MODULE_DEVICE),
    ERROR_DRIVER_INVSTATE,
    ERROR_DRIVER_INVNAME,
    ERROR_DRIVER_INVOPT,
    ERROR_DRIVER_INSTALLED,

    // for device
    ERROR_DEVICE_REGISTER_INVNAME,
    ERROR_DEVICE_REGISTER_INVHANDLE,
    ERROR_DEVICE_REGISTER_NODRV,
    ERROR_DEVICE_OPEN_INVNAME,
    ERROR_DEVICE_OPEN_NODRV,
    ERROR_DEVICE_OPEN_NODEV,
    ERROR_DEVICE_OPEN_INVSTATE,
    ERROR_DEVICE_CLOSE_INVHANDLE,
    ERROR_DEVICE_READ_INVHANDLE,
    ERROR_DEVICE_READ_NOTSUPPORT,
    ERROR_DEVICE_WRITE_INVHANDLE,
    ERROR_DEVICE_WRITE_NOTSUPPORT,
    ERROR_DEVICE_CONTROL_INVHANDLE,
    ERROR_DEVICE_CONTROL_NOTSUPPORT
};

#endif

