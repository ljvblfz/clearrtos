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

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "console.h"
#include "errcon.h"
#include "device.h"
#include "clib.h"
#include "interrupt.h"

static error_t console_open (device_handler_t _handler, open_mode_t _mode)
{
    console_handler_t handler = (console_handler_t)_handler;

    UNUSED (_mode);
    
    if (handler->is_opened_) {
        return ERROR_T (ERROR_CONSOLE_OPEN_OPENED);
    }

    handler->is_opened_ = true;
    return 0;
}

static error_t console_close (device_handler_t _handler)
{
    console_handler_t handler = (console_handler_t)_handler;

    handler->is_opened_ = false;
    return 0;
}

static error_t console_write (device_handler_t _handler, const void *_buf, 
    usize_t _size)
{
    UNUSED (_handler);
    
    // write to the STDOUT for simulating a Console
    write (0, _buf, _size);
    return 0;
}

error_t console_driver_install (const char _name[])
{
    device_operation_t opt = {
        .open_ = console_open, 
        .close_ = console_close,
        .read_ = 0,
        .write_ = console_write,
        .control_ = 0
    };
    
    return driver_install (_name, &opt, 0);
}

error_t console_device_register (const char _name [], console_handler_t _handler)
{
    _handler->common_.interrupt_vector_ = INTERRUPT_NONE;
    _handler->is_opened_ = false;
    return device_register (_name, &_handler->common_);
}

static device_handler_t g_console_handler;

void console_handler_set (device_handler_t _handler)
{
    g_console_handler = _handler;
}

void console_print (const char* _format, ...)
{
    va_list arglist = 0;
    static char buffer [1024];
    static int length;
    static int log_lost = 0;

    va_start (arglist, _format);
    length = vsnprintf (buffer, sizeof (buffer) - 1, _format, arglist);
    va_end (arglist);
    if (0 == g_console_handler) {
        log_lost ++;
    }
    else {
        (void) device_write (g_console_handler, buffer, (usize_t)length);
        // log out a meesage to notify that there is log lost
        if (log_lost != 0) {
            length = snprintf (buffer, sizeof (buffer) - 1, "Warning: there"
                " is/are %d lines of log lost\n", log_lost);
            log_lost = 0;
        }
    }
}

