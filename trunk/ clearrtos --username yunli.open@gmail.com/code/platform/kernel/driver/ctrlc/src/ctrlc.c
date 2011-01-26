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

void ctrlc_interrupt_handler (device_handler_t _handler)
{
    ctrlc_handler_t handler = (ctrlc_handler_t) _handler;
    
    if (handler->is_opened_) {
        console_print ("\nInfo: Ctrl+C pressed\n");
        multitasking_stop ();
    }
}

static error_t ctrlc_open (device_handler_t _handler, open_mode_t _mode)
{
    ctrlc_handler_t handler = (ctrlc_handler_t)_handler;

    UNUSED (_mode);
    
    if (handler->is_opened_) {
        return ERROR_T (ERROR_CTRLC_OPEN_OPENED);
    }

    handler->is_opened_ = true;
    interrupt_enable (_handler->interrupt_vector_);
    console_print ("\nInfo: press Ctrl+C to terminate!\n");
    return 0;
}

static error_t ctrlc_close (device_handler_t _handler)
{
    ctrlc_handler_t handler = (ctrlc_handler_t)_handler;
    
    interrupt_disable (_handler->interrupt_vector_);
    handler->is_opened_ = false;
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

error_t ctrlc_device_register (const char _name [], ctrlc_handler_t _handler)
{
    _handler->common_.interrupt_vector_ = SIGINT;
    _handler->common_.interrupt_handle_= ctrlc_interrupt_handler;
    _handler->is_opened_ = false;
    return device_register (_name, &_handler->common_);
}

