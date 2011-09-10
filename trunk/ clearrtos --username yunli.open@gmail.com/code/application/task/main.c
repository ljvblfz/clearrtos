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
 *   Date: 07/29/2010                                                         *
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
#include "console.h"

static void task_entry_overflow (const char _name [], void *_p_arg)
{
    // create a BIG local variable for simulating stack overflow
    int buffer [900];
    memset (buffer, 0, sizeof (buffer));
         
    UNUSED (_p_arg);
    
    //lint -e{716}
    while (1) {
        console_print ("%s", _name);
        (void) fflush (stdout);
        (void) task_sleep (500);
    }
}

static void task_entry (const char _name [], void *_p_arg)
{
    UNUSED (_p_arg);

    //lint -e{716}
    while (1) {
        console_print ("%s", _name);
        (void) fflush (stdout);
        (void) task_sleep (500);
    }
}

// pad is used for giving a cushion before g_stack_for_task0 and 
// g_stack_for_task1. Without this cushion the stack overflow of task0 will have
// side effect on other task(s).
STACK_DECLARE (pad, 1024);
STACK_DECLARE (g_stack_for_task0, 1024);
STACK_DECLARE (g_stack_for_task1, 1024);
STACK_DECLARE (g_stack_for_task2, 1024);
static task_handle_t g_task0;
static task_handle_t g_task1;
static task_handle_t g_task2;
static timer_handle_t g_timer;
static device_handle_t g_ctrlc_handle;

static void timer_callback (timer_handle_t _handle, void *_arg)
{
    UNUSED (_handle);
    UNUSED (_arg);

    //lint -e{746}
    task_dump ();
    //lint -e{746}
    multitasking_stop ();
}

error_t module_testapp (system_state_t _state)
{
    // memset for pad is for preventing the OS from optimizing it out
    memset (pad, 0, sizeof (pad));

    if (STATE_INITIALIZING == _state) {
        (void) task_create (&g_task0, "0", 11, g_stack_for_task0, sizeof (g_stack_for_task0));
        (void) task_start (g_task0, task_entry_overflow, 0);

        (void) task_create (&g_task1, "1", 16, g_stack_for_task1, sizeof (g_stack_for_task1));
        (void) task_start (g_task1, task_entry, 0);

        (void) task_create (&g_task2, "2", 19, g_stack_for_task2, sizeof (g_stack_for_task2));
        (void) task_start (g_task2, task_entry, 0);
        
        (void) timer_alloc (&g_timer, "Dump", TIMER_TYPE_TASK);
        (void) timer_start (g_timer, 5000, timer_callback, 0);
        
        (void) device_open (&g_ctrlc_handle, "/dev/ui/ctrlc", 0);
    }
    else if (STATE_DESTROYING == _state) {
        (void) device_close (g_ctrlc_handle);
        (void) timer_free (g_timer);
        (void) task_delete (g_task2);
        (void) task_delete (g_task1);
        (void) task_delete (g_task0);
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

