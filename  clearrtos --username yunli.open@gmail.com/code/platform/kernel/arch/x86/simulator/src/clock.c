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

void tick_interrupt_handler (device_handler_t _handler)
{
    clock_handler_t handler = (clock_handler_t)_handler;
    handler->tick_process_ ();
}

static error_t clock_open (device_handler_t _handler, open_mode_t _mode)
{
    clock_handler_t handler = (clock_handler_t)_handler;

    UNUSED (_mode);
    
    if (handler->is_opened_) {
        return ERROR_T (ERROR_CLOCK_OPEN_OPENED);
    }

    handler->is_opened_ = true;
    return 0;
}

static error_t clock_close (device_handler_t _handler)
{
    clock_handler_t handler = (clock_handler_t)_handler;
    
    handler->is_opened_ = false;
    return 0;
}

//lint -e{818}
static error_t clock_control (device_handler_t _handler, 
    control_option_t _option, int _int_arg, void *_ptr_arg)
{
    clock_handler_t handler = (clock_handler_t)_handler;
    
    switch (_option)
    {
    case OPTION_TICK_START:
        {
            struct itimerval tv;
            
            tv.it_value.tv_sec = 0;
            tv.it_value.tv_usec = (long) _int_arg*1000; // msec to usec
            tv.it_interval.tv_sec = 0;
            tv.it_interval.tv_usec = (long) _int_arg*1000; // msec to usec
            (void) setitimer (ITIMER_REAL, &tv, 0);
            //lint -e{611}
            handler->tick_process_ = (interrupt_handler_t) _ptr_arg;

            interrupt_enable (_handler->interrupt_vector_);
        }
        break;
    case OPTION_TICK_STOP:
        {
            struct itimerval  tv;
            struct sigaction  act;

            interrupt_disable (_handler->interrupt_vector_);
            
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

error_t clock_device_register (const char _name [], clock_handler_t _handler)
{
    _handler->common_.interrupt_vector_ = SIGALRM;
    _handler->common_.interrupt_handle_ = tick_interrupt_handler;
    _handler->is_opened_ = false;
    return device_register (_name, &_handler->common_);
}

