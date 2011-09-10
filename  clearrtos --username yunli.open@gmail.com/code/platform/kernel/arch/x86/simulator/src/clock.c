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
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include <signal.h>
#include <sys/time.h>
#include "device.h"
#include "clock.h"
#include "errclock.h"
#include "console.h"

void tick_interrupt_handler (device_handle_t _handle)
{
    clock_handle_t handle = (clock_handle_t)_handle;
    handle->tick_process_ ();
}

static error_t clock_open (device_handle_t _handle, open_mode_t _mode)
{
    clock_handle_t handle = (clock_handle_t)_handle;

    UNUSED (_mode);
    
    if (handle->is_opened_) {
        return ERROR_T (ERROR_CLOCK_OPEN_OPENED);
    }

    handle->is_opened_ = true;
    return 0;
}

static error_t clock_close (device_handle_t _handle)
{
    clock_handle_t handle = (clock_handle_t)_handle;
    
    handle->is_opened_ = false;
    return 0;
}

//lint -e{818}
static error_t clock_control (device_handle_t _handle, 
    control_option_t _option, int _int_arg, void *_ptr_arg)
{
    clock_handle_t handle = (clock_handle_t)_handle;
    
    switch (_option)
    {
    case OPTION_TICK_START:
        {
            struct itimerval tv;
            
            //lint -e{611}
            handle->tick_process_ = (interrupt_handler_t) _ptr_arg;
            
            tv.it_value.tv_sec = 0;
            tv.it_value.tv_usec = (long) _int_arg*1000; // msec to usec
            tv.it_interval.tv_sec = 0;
            tv.it_interval.tv_usec = (long) _int_arg*1000; // msec to usec
            (void) setitimer (ITIMER_REAL, &tv, 0);

            interrupt_enable (_handle->interrupt_vector_);
        }
        break;
    case OPTION_TICK_STOP:
        {
            struct itimerval  tv;
            struct sigaction  act;

            interrupt_disable (_handle->interrupt_vector_);
            
            memset (&act, 0, sizeof(act));
            act.sa_handler = SIG_IGN;
            (void) sigaction (SIGALRM, &act, 0);

            memset (&tv, 0, sizeof(tv));
            (void) setitimer (ITIMER_REAL, &tv, 0);
        }
        break;
    default:
        return ERROR_T (ERROR_CLOCK_CONTROL_INVOPT);
    }
    return 0;
}

error_t clock_driver_install (const char _name[])
{
    device_operation_t opt = {
        .open_ = clock_open, 
        .close_ = clock_close,
        .control_ = clock_control,
    };
    return driver_install (_name, &opt, 0);
}

error_t clock_device_register (const char _name [], clock_handle_t _handle)
{
    _handle->common_.interrupt_vector_ = SIGALRM;
    _handle->common_.interrupt_handler_ = tick_interrupt_handler;
    _handle->is_opened_ = false;
    return device_register (_name, &_handle->common_);
}

