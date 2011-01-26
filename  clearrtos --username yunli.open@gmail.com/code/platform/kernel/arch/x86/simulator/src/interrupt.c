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
 *   Date: 07/30/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "primitive.h"
#include "interrupt.h"

sigset_t g_signal_mask_empty;
sigset_t g_signal_mask;

static int g_interrupt_nested_count;
static interrupt_handler_t g_interrupt_vector [CONFIG_MAX_INTERRUPT];
static interrupt_handler_t g_device_interrupt_handler;
static interrupt_exit_callback_t g_interrupt_exit_callback;

void interrupt_enter ()
{
    interrupt_level_t level;

    level = global_interrupt_disable ();
    g_interrupt_nested_count ++;
    global_interrupt_enable (level);
}

void interrupt_exit ()
{
    interrupt_level_t level;

    level = global_interrupt_disable ();
    g_interrupt_nested_count --;
    global_interrupt_enable (level);
    // give an opportunity to the scheduler for task switch
    if (0 == g_interrupt_nested_count && 0 != g_interrupt_exit_callback) {
        //g_interrupt_exit_callback ();
    }
}

bool is_in_interrupt ()
{
    return (bool) (g_interrupt_nested_count != 0);
}

static void interrupt_handler (int _vector)
{
    interrupt_enter ();
    if (g_device_interrupt_handler != null) {
        g_device_interrupt_handler (_vector);
    }
    else {
        // oops! log error? hold on, we are in interrupt!
    }
    interrupt_exit ();
}

static void signal_handler (int _signal)
{
    g_interrupt_vector [_signal] (_signal);
}

static void signal_init ()
{
    struct sigaction  action;
    sigset_t mask;

    mask = g_signal_mask;
    (void) sigprocmask (SIG_UNBLOCK, &mask, 0);

    action.sa_handler = signal_handler;
    action.sa_mask = mask;
    action.sa_flags = SA_RESTART;
    (void) sigaction (SIGINT, &action, 0);
    (void) sigaction (SIGQUIT, &action, 0);
    (void) sigaction (SIGKILL, &action, 0);
    (void) sigaction (SIGALRM, &action, 0);
    (void) sigaction (SIGIO, &action, 0);
    (void) sigaction (SIGSTOP, &action, 0);
}

void device_interrupt_handler_install (interrupt_handler_t _handler)
{
    g_device_interrupt_handler = _handler;
}

void interrupt_exit_callback_install (interrupt_exit_callback_t _handler)
{
    g_interrupt_exit_callback = _handler;
}

interrupt_handler_t interrupt_handler_install (int _vector, 
    interrupt_handler_t _handler)
{
    interrupt_handler_t old;

    if (_vector > INTERRUPT_LAST_INDEX) {
        return null;
    }
    
    old = g_interrupt_vector [_vector];
    g_interrupt_vector [_vector] = _handler;
    return old;
}

void interrupt_enable (int _vector)
{
    UNUSED (_vector);
    // do nothing on Linux for simplifying
}

void interrupt_disable (int _vector)
{
    UNUSED (_vector);
    // do nothing on Linux for simplifying
}

error_t module_interrupt (system_state_t _state)
{
    if (STATE_INITIALIZING == _state) {
        for (int vector = 0; vector <= INTERRUPT_LAST_INDEX; vector ++) {
            g_interrupt_vector [vector] = interrupt_handler;
        }
        (void) sigemptyset (&g_signal_mask_empty);
        (void) sigfillset (&g_signal_mask);
        g_interrupt_nested_count = 0;
        signal_init ();
        global_interrupt_enable (INTERRUPT_ENABLED);
    }
    else if (STATE_DESTROYING == _state) {
        global_interrupt_enable (INTERRUPT_DISABLED);
    }
    return 0;
}

