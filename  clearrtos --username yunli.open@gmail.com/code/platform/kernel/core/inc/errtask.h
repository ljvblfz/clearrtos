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
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __ERRTASK_H
#define __ERRTASK_H

#include "module.h"

enum {
    // for task
    ERROR_TASK_CREATE_PRIOINUSE = ERROR_BEGIN (MODULE_TASK),
    ERROR_TASK_CREATE_INVCONTEXT,
    ERROR_TASK_CREATE_INVPRIO,
    ERROR_TASK_DELETE_INVCONTEXT,
    ERROR_TASK_DELETE_INVHANDLE,
    ERROR_TASK_START_INVHANDLE,
    ERROR_TASK_START_INVOP,
    ERROR_TASK_RESTART_INVHANDLE,
    ERROR_TASK_RESTART_INVOP,
    ERROR_TASK_SUSPEND_INVHANDLE,
    ERROR_TASK_SUSPEND_NOTSTARTED,
    ERROR_TASK_RESUME_INVHANDLE,
    ERROR_TASK_RESUME_NOTSUSPENDED,
    ERROR_TASK_SLEEP_INVCONTEXT,
    ERROR_TASK_STACK_INVHANDLE,
    ERROR_TASK_STACK_INVCONTEXT,
    ERROR_TASK_WAIT_TIMEOUT,
    ERROR_TASK_DUMP_INVCONTEXT,

    // for task hook
    ERROR_TASK_HOOK_CREATE_NOROOM,
    ERROR_TASK_HOOK_CREATE_NOTFOUND,
    ERROR_TASK_HOOK_SWITCH_NOROOM,
    ERROR_TASK_HOOK_SWITCH_NOTFOUND,
    ERROR_TASK_HOOK_DELETE_NOROOM,
    ERROR_TASK_HOOK_DELETE_NOTFOUND,

    // for task variable
    ERROR_TASK_VARIABLE_ADD_INVTASK,
    ERROR_TASK_VARIABLE_ADD_NOVAR,
    ERROR_TASK_VARIABLE_REMOVE_INVTASK,
    ERROR_TASK_VARIABLE_REMOVE_NOTFOUND
};

#endif

