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
 *   Date: 08/22/2010                                                         *
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
#include "queue.h"
#include "console.h"

#define CONFIG_MAX_ELEMENT   20

static task_handle_t g_task_producer;
static task_handle_t g_task_consumer;

STACK_DECLARE (g_stack_for_producer, 1024);
STACK_DECLARE (g_stack_for_consumer, 1024);

QUEUE_BUFFER_DECLARE (g_queue_buffer, sizeof (char), CONFIG_MAX_ELEMENT);
static queue_handle_t g_queue;

static void task_producer (const char _name [], void *_p_arg)
{
    queue_handle_t p_queue = (queue_handle_t)_p_arg;
    char greeting [] = "Have a good day!\n";
    char *p_char = greeting;
    int len = sizeof (greeting) - 1;

    UNUSED (_name);
    
    while (len -- > 0) {
        (void) queue_message_send (p_queue, p_char ++);
    }
}

static void task_consumer (const char _name [], void *_p_arg)
{
    queue_handle_t p_queue = (queue_handle_t)_p_arg;
    char ch;

    UNUSED (_name);

    while (!queue_is_empty (p_queue)) {
        (void) queue_message_receive (p_queue, 0, &ch);
        console_print ("%c", ch);
    }

    //lint -e{746}
    queue_dump ();
    //lint -e{746}
    multitasking_stop ();
}

error_t module_testapp (system_state_t _state)
{
    static device_handle_t ctrlc_handle;
    
    if (STATE_INITIALIZING == _state) {
        (void) queue_create ("Test", &g_queue, g_queue_buffer, sizeof (char), 
            CONFIG_MAX_ELEMENT);
        
        (void) task_create (&g_task_producer, "Producer", 11, 
            g_stack_for_producer, sizeof (g_stack_for_producer));
        (void) task_start (g_task_producer, task_producer, g_queue);

        (void) task_create (&g_task_consumer, "Consumer", 13, 
            g_stack_for_consumer, sizeof (g_stack_for_consumer));
        (void) task_start (g_task_consumer, task_consumer, g_queue);
        
        (void) device_open (&ctrlc_handle, "/dev/ui/ctrlc", 0);
    }
    else if (STATE_DESTROYING == _state) {
        (void) device_close (ctrlc_handle);
        (void) task_delete (g_task_consumer);
        (void) task_delete (g_task_producer);
        (void) queue_delete (g_queue);
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
    (void) module_register ("Queue", MODULE_QUEUE, OS_LEVEL, module_queue);
    (void) module_register ("TestApp", MODULE_TESTAPP, APPLICATION_LEVEL3, module_testapp);
    return 0;
}

