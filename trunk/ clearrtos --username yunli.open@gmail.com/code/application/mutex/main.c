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
 *   Date: 08/16/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#include "main.h"
#include "device.h"
#include "mutex.h"
#include "console.h"

static void task_high (const char _name [], void *_p_arg)
{
    mutex_handle_t mutex = (mutex_handle_t)_p_arg;

    console_print ("%s: going to take lock\n", _name);
    (void) mutex_lock (mutex, WAIT_FOREVER);
    console_print ("%s: lock is taken\n", _name);
    console_print ("%s: going to sleep\n", _name);
    (void) task_sleep (1000);
    console_print ("%s: is waken\n", _name);
    console_print ("%s: going to free lock\n", _name);
    (void) mutex_unlock (mutex);
    console_print ("%s: lock is freed\n", _name);
}

static void task_low (const char _name [], void *_p_arg)
{
    mutex_handle_t mutex = (mutex_handle_t)_p_arg;

    console_print ("%s: going to take lock\n", _name);
    (void) mutex_lock (mutex, WAIT_FOREVER);
    console_print ("%s: lock is taken\n", _name);
    console_print ("%s: going to free lock\n", _name);
    (void) mutex_unlock (mutex);
    console_print ("%s: lock is freed\n", _name);

    //lint -e{746}
    mutex_dump ();
    //lint -e{746}
    multitasking_stop ();
}


error_t module_testapp (system_state_t _state)
{
    static task_handle_t high;
    static task_handle_t low;
    STACK_DECLARE (stack_for_high, 1024);
    STACK_DECLARE (stack_for_low, 1024);
    static mutex_handle_t mutex;
    static device_handle_t ctrlc_handle;
    
    if (STATE_INITIALIZING == _state) {
        (void) mutex_create (&mutex, "Test", true);
        
        (void) task_create (&low, "Task Low", 16, stack_for_low, sizeof (stack_for_low));
        (void) task_start (low, task_low, mutex);
        
        (void) task_create (&high, "Task High", 11, stack_for_high, sizeof (stack_for_high));
        (void) task_start (high, task_high, mutex);
        
        (void) device_open (&ctrlc_handle, "/dev/ui/ctrlc", 0);
    }
    else if (STATE_DESTROYING == _state) {
        (void) device_close (ctrlc_handle);
        (void) task_delete (high);
        (void) task_delete (low);
        (void) mutex_delete (mutex);
    }
    return 0;
}

int module_registration_entry (int argc, char *argv [])
{
    UNUSED (argc);
    UNUSED (argv);
    
    (void) module_register ("Interrupt", MODULE_INTERRUPT, CPU_LEVEL, module_interrupt);
    (void) module_register ("Device", MODULE_DEVICE, DRIVER_LEVEL, module_device);
    (void) module_register ("Timer", MODULE_TIMER, OS_LEVEL, module_timer);
    (void) module_register ("Task", MODULE_TASK, OS_LEVEL, module_task);
    (void) module_register ("Mutex", MODULE_MUTEX, OS_LEVEL, module_mutex);
    (void) module_register ("TestApp", MODULE_TESTAPP, APPLICATION_LEVEL3, module_testapp);
    return 0;
}

