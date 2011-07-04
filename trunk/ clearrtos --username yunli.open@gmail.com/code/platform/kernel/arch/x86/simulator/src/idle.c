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
 *   Date: 05/22/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include <sys/time.h>
#include <errno.h>
#include "console.h"
#include "task.h"

//lint -esym(728, g_tick_occurred_count, g_tick_delayed)

static bool g_tick_occurred_count;
static statistic_t g_tick_delayed;

void task_entry_idle (const char _name [], void *_p_arg)
{
    interrupt_level_t level;
    
    UNUSED (_name);
    UNUSED (_p_arg);

    //lint -e{716}
    while (1) {
        if (select (0, 0, 0, 0, 0) < 0) {
            if (errno != EINTR) {
                // should never happen
                console_print ("Fatal: non expected error occurred in"
                    " task_entry_idle ()\n");
            }

            level = global_interrupt_disable ();
            if (g_tick_occurred_count > 1) {
                g_tick_delayed ++;
            }
            while (g_tick_occurred_count > 0) {
                g_tick_occurred_count --;
                global_interrupt_enable (level);
                interrupt_enter ();
                timer_fire ();
                interrupt_exit ();
                level = global_interrupt_disable ();
            }
            global_interrupt_enable (level);
            
            task_schedule (null);
            continue;
        }
    }
}

void tick_process ()
{
    interrupt_level_t level;

    level = global_interrupt_disable ();
    g_tick_occurred_count ++;
    global_interrupt_enable (level);
}

statistic_t tick_delayed ()
{
    return g_tick_delayed;
}
