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
#include "event.h"
#include "console.h"

#define EVENT_1   0x01
#define EVENT_2   0x02

task_handle_t g_task1;
task_handle_t g_task2;
task_handle_t g_task3;

static void task_task1 (const char _name [], void *_p_arg)
{
    event_set_t received;

    UNUSED (_p_arg);
    
    console_print ("%s: going to receive EVENT_1 or EVENT_2\n", _name);
    (void) event_receive (EVENT_1 | EVENT_2, &received, WAIT_FOREVER, 
        EVENT_WAIT_ANY | EVENT_RETURN_EXPECTED);
    console_print ("%s: EVENT_1 or EVENT_2 (%x) received\n", _name, received);
    console_print ("%s: going to send EVENT_2\n", _name);
    (void) event_send (g_task2, EVENT_2);
    console_print ("%s: EVENT_2 is sent\n", _name);
}

static void task_task2 (const char _name [], void *_p_arg)
{
    event_set_t received;

    UNUSED (_p_arg);
    
    console_print ("%s: going to send EVENT_1\n", _name);
    (void) event_send (g_task1, EVENT_1);
    console_print ("%s: EVENT_1 is sent\n", _name);
    console_print ("%s: going to receive EVENT_1 and EVENT_2\n", _name);
    (void) event_receive (EVENT_1 | EVENT_2, &received, WAIT_FOREVER, 
        EVENT_WAIT_ALL | EVENT_RETURN_EXPECTED);
    console_print ("%s: EVENT_1 and EVENT_2 (%x) received\n", _name, received);
}

static void task_task3 (const char _name [], void *_p_arg)
{
    UNUSED (_p_arg);
    
    console_print ("%s: going to send EVENT_1\n", _name);
    (void) event_send (g_task2, EVENT_1);
    console_print ("%s: EVENT_1 is sent\n", _name);

    //lint -e{746}
    multitasking_stop ();
}

error_t module_testapp (system_state_t _state)
{
    STACK_DECLARE (stack_for_task1, 1024);
    STACK_DECLARE (stack_for_task2, 1024);
    STACK_DECLARE (stack_for_task3, 1024);
    static device_handle_t ctrlc_handle;
    
    if (STATE_INITIALIZING == _state) {
        (void) task_create (&g_task1, "Task1", 16, stack_for_task1, 
            sizeof (stack_for_task1));
        (void) task_start (g_task1, task_task1, 0);
        
        (void) task_create (&g_task2, "Task2", 11, stack_for_task2, 
            sizeof (stack_for_task2));
        (void) task_start (g_task2, task_task2, 0);
        
        (void) task_create (&g_task3, "Task3", 20, stack_for_task3, 
            sizeof (stack_for_task3));
        (void) task_start (g_task3, task_task3, 0);
        
        (void) device_open (&ctrlc_handle, "/dev/ui/ctrlc", 0);
    }
    else if (STATE_DESTROYING == _state) {
        (void) device_close (ctrlc_handle);
        (void) task_delete (g_task3);
        (void) task_delete (g_task2);
        (void) task_delete (g_task1);
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
    (void) module_register ("TestApp", MODULE_TESTAPP, APPLICATION_LEVEL3, module_testapp);
    return 0;
}

