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
 *   Date: 08/23/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#include "unitest.h"
#include "device.h"
#include "clock.h"
#include "timer.h"
#include "task.h"
#include "errtask.h"

#define TASK_PRIORITY_LEVELS    BITS_SUPPORTED
#define TASK_LAST_INDEX         (TASK_PRIORITY_LEVELS - 1)

static task_handle_t g_handle [TASK_PRIORITY_LEVELS];
static stack_unit_t g_stack [TASK_PRIORITY_LEVELS][2048];

// below tasks used for testing task_suspend () and task_resume ()
static bool g_task_running = false;
static void task_suspending (const char _name [], void *_p_arg)
{
    task_handle_t handle = task_from_priority (1);

    UNITEST_EQUALS (task_suspend (handle), 0);
    (void) task_sleep (100);
    UNITEST_EQUALS (g_task_running, false);
    UNITEST_EQUALS (task_resume (handle), 0);
    UNITEST_EQUALS (task_resume (handle), 
        ERROR_T (ERROR_TASK_RESUME_NOTSUSPENDED));
    // yeild CPU to make suspended task gets chance to run
    (void) task_sleep (0);
    UNITEST_EQUALS (g_task_running, true);

    UNITEST_EQUALS (task_delete (handle), 0);
    UNITEST_EQUALS (task_suspend (handle), ERROR_T (ERROR_TASK_SUSPEND_INVHANDLE));

    (void) task_create (&g_handle [1], "test", 1, g_stack [1], sizeof (g_stack [0]));
            
    // return to the unitest_main () for next test
    multitasking_stop ();
}

static void task_suspended (const char _name [], void *_p_arg)
{
    g_task_running = true;
    // yeild CPU to make suspending task gets chance to run
    (void) task_sleep (0);
}

void unitest_main (int argc, char *argv[])
{
    task_priority_t prio;
    bool failed = false;
    error_t ecode;
    
    UNUSED (argc);
    UNUSED (argv);
    
    UNITEST_EQUALS (module_interrupt (STATE_INITIALIZING), 0);
    UNITEST_EQUALS (module_device (STATE_INITIALIZING), 0);
    UNITEST_EQUALS (module_timer (STATE_INITIALIZING), 0);
    UNITEST_EQUALS (module_task (STATE_INITIALIZING), 0);
    UNITEST_EQUALS (module_interrupt (STATE_UP), 0);
    UNITEST_EQUALS (module_device (STATE_UP), 0);
    UNITEST_EQUALS (module_timer (STATE_UP), 0);
    UNITEST_EQUALS (module_task (STATE_UP), 0);

    // note, idle task takes priority of TASK_LAST_INDEX
    for (prio = 0; prio < TASK_LAST_INDEX; prio ++) {
        if (prio == CONFIG_TIMER_TASK_PRIORITY) {
            continue;
        }
        ecode = task_create (&g_handle [prio], "test", prio, g_stack [prio], 
            sizeof (g_stack [prio]));
        if (0 != ecode) {
            printf ("Error: prio = %d, errstr = %s\n", prio, errstr (ecode));
            failed |= true;
        }
    }
    UNITEST_EQUALS (failed, false);
    UNITEST_EQUALS (dll_size (unitest_get_allocated_dll ()), TASK_PRIORITY_LEVELS);

    UNITEST_EQUALS (task_create (&g_handle [0], "test", 1024, g_stack [prio], 
            sizeof (g_stack [0])), ERROR_T (ERROR_TASK_CREATE_INVPRIO));
    
    UNITEST_EQUALS (task_create (&g_handle [0], "test", 0, g_stack [0], 
            sizeof (g_stack [0])), ERROR_T (ERROR_TASK_CREATE_PRIOINUSE));

    UNITEST_EQUALS (task_start (g_handle [0], task_suspending, 0), 0);
    UNITEST_EQUALS (task_start (g_handle [1], task_suspended, 0), 0);

    multitasking_start ();

    task_dump ();
    
    failed = false;
    for (prio = 0; prio < TASK_LAST_INDEX; prio ++) {
        if (prio == CONFIG_TIMER_TASK_PRIORITY) {
            continue;
        }
        ecode = task_delete (g_handle [prio]);
        if (0 != ecode) {
            printf ("Error: prio = %d, errstr = %s\n", prio, errstr (ecode));
            failed |= true;
        }
    }
    UNITEST_EQUALS (failed, false);
    UNITEST_EQUALS (dll_size (unitest_get_allocated_dll ()), 2);

    UNITEST_EQUALS (module_task (STATE_DOWN), 0);
    UNITEST_EQUALS (module_timer (STATE_DOWN), 0);
    UNITEST_EQUALS (module_device (STATE_DOWN), 0);
    UNITEST_EQUALS (module_interrupt (STATE_DOWN), 0);
    UNITEST_EQUALS (module_task (STATE_DESTROYING), 0);
    UNITEST_EQUALS (module_timer (STATE_DESTROYING), 0);
    UNITEST_EQUALS (module_device (STATE_DESTROYING), 0);
    UNITEST_EQUALS (module_interrupt (STATE_DESTROYING), 0);

}

