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
 *   Date: 09/01/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include <signal.h>
#include <sys/time.h>
#include "device.h"
#include "ctrlc.h"
#include "errctrlc.h"
#include "console.h"
#include "task.h"

void ctrlc_interrupt_handler (device_handle_t _handle)
{
    ctrlc_handle_t handle = (ctrlc_handle_t) _handle;
    
    if (handle->is_opened_) {
        console_print ("\nInfo: Ctrl+C pressed\n");
        multitasking_stop ();
    }
}

static error_t ctrlc_open (device_handle_t _handle, open_mode_t _mode)
{
    ctrlc_handle_t handle = (ctrlc_handle_t)_handle;

    UNUSED (_mode);
    
    if (handle->is_opened_) {
        return ERROR_T (ERROR_CTRLC_OPEN_OPENED);
    }

    handle->is_opened_ = true;
    interrupt_enable (_handle->interrupt_vector_);
    console_print ("\nInfo: press Ctrl+C to terminate!\n");
    return 0;
}

static error_t ctrlc_close (device_handle_t _handle)
{
    ctrlc_handle_t handle = (ctrlc_handle_t)_handle;
    
    interrupt_disable (_handle->interrupt_vector_);
    handle->is_opened_ = false;
    return 0;
}

error_t ctrlc_driver_install (const char _name[])
{
    device_operation_t opt = {
        .open_ = ctrlc_open, 
        .close_ = ctrlc_close,
        .read_ = 0,
        .write_ = 0,
        .control_ = 0
    };
    
    return driver_install (_name, &opt, 0);
}

error_t ctrlc_device_register (const char _name [], ctrlc_handle_t _handle)
{
    _handle->common_.interrupt_vector_ = SIGINT;
    _handle->common_.interrupt_handler_= ctrlc_interrupt_handler;
    _handle->is_opened_ = false;
    return device_register (_name, &_handle->common_);
}

