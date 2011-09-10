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
 *   Date: 08/14/2010                                                         *
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
#include "semaphore.h"
#include "console.h"

static void task_consumer (const char _name [], void *_p_arg)
{
    semaphore_handle_t semaphore = (semaphore_handle_t)_p_arg;

    console_print ("%s: going to take semaphore\n", _name);
    (void) semaphore_take (semaphore, WAIT_FOREVER);
    console_print ("%s: taken\n", _name);
}

static void task_producer (const char _name [], void *_p_arg)
{
    semaphore_handle_t semaphore = (semaphore_handle_t)_p_arg;

    console_print ("%s: going to give semaphore\n", _name);
    (void) semaphore_give (semaphore);
    console_print ("%s: given\n", _name);
    (void) task_sleep (1000);

    //lint -e{746}
    semaphore_dump ();
    //lint -e{746}
    multitasking_stop ();
}
    
error_t module_testapp (system_state_t _state)
{
    static task_handle_t producer;
    static task_handle_t consumer;
    STACK_DECLARE (stack_for_producer, 1024);
    STACK_DECLARE (stack_for_consumer, 1024);
    static semaphore_handle_t semaphore;
    static device_handle_t ctrlc_handle;

    if (STATE_INITIALIZING == _state) {
        (void) semaphore_create (&semaphore, "Test", 0);
        
        (void) task_create (&producer, "Producer", 16, stack_for_producer, 
            sizeof (stack_for_producer));
        (void) task_start (producer, task_producer, semaphore);
        
        (void) task_create (&consumer, "Consumer", 11, stack_for_consumer, 
            sizeof (stack_for_consumer));
        (void) task_start (consumer, task_consumer, semaphore);
        
        (void) device_open (&ctrlc_handle, "/dev/ui/ctrlc", 0);
    }
    else if (STATE_DESTROYING == _state) {
        (void) device_close (ctrlc_handle);
        (void) task_delete (consumer);
        (void) task_delete (producer);
        (void) semaphore_delete (semaphore);
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
    (void) module_register ("Semaphore", MODULE_SEMAPHORE, OS_LEVEL, module_semaphore);
    (void) module_register ("TestApp", MODULE_TESTAPP, APPLICATION_LEVEL3, module_testapp);
    return 0;
}

