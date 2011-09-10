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

static error_t console_open (device_handle_t _handle, open_mode_t _mode)
{
    console_handle_t handle = (console_handle_t)_handle;

    UNUSED (_mode);
    
    if (handle->is_opened_) {
        return ERROR_T (ERROR_CONSOLE_OPEN_OPENED);
    }

    handle->is_opened_ = true;
    return 0;
}

static error_t console_close (device_handle_t _handle)
{
    console_handle_t handle = (console_handle_t)_handle;

    handle->is_opened_ = false;
    return 0;
}

static error_t console_write (device_handle_t _handle, const void *_buf, 
    usize_t _size)
{
    UNUSED (_handle);
    
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

error_t console_device_register (const char _name [], console_handle_t _handle)
{
    _handle->common_.interrupt_vector_ = INTERRUPT_NONE;
    _handle->is_opened_ = false;
    return device_register (_name, &_handle->common_);
}

static device_handle_t g_console_handle;

void console_handle_set (device_handle_t _handle)
{
    g_console_handle = _handle;
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
    if (0 == g_console_handle) {
        log_lost ++;
    }
    else {
        (void) device_write (g_console_handle, buffer, (usize_t)length);
        // log out a meesage to notify that there is log lost
        if (log_lost != 0) {
            length = snprintf (buffer, sizeof (buffer) - 1, "Warning: there"
                " is/are %d lines of log lost\n", log_lost);
            log_lost = 0;
        }
    }
}

