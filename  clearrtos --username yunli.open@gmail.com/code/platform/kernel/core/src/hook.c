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
 
#include "config.h"
#include "hook.h"
#include "errtask.h"

#define TASK_CREATE_HOOK_LAST_INDEX     (CONFIG_MAX_TASK_CREATE_HOOK - 1)
#define TASK_SWITCH_HOOK_LAST_INDEX     (CONFIG_MAX_TASK_SWITCH_HOOK - 1)
#define TASK_DELETE_HOOK_LAST_INDEX     (CONFIG_MAX_TASK_DELETE_HOOK - 1)

static task_create_hook_t g_create_table [CONFIG_MAX_TASK_CREATE_HOOK];
static task_switch_hook_t g_switch_table [CONFIG_MAX_TASK_SWITCH_HOOK];
static task_delete_hook_t g_delete_table [CONFIG_MAX_TASK_DELETE_HOOK];

static bool hook_add (void *_table [], void *_hook, int _last_index)
{
    int idx = 0;
    
    for (; idx <= _last_index; idx ++) {
        if (_table [idx] != null) {
            continue;
        }
        _table [idx] = _hook;
        return true;
    }
    return false;
}

//lint -e{818}
static bool hook_remove (void *_table [], void *_hook, int _last_index)
{
    int idx = 0;
    
    for (; idx <= _last_index && _table [idx] != null; idx ++) {
        if (_table [idx] == _hook) {
            _table [idx] = null;
            idx ++;
            goto move;
        }
    }
    return false;
    
move:
    // move all following hook(s) up
    for (; idx <= _last_index && _table [idx] != null; idx ++) {
        _table [idx - 1] = _table [idx];
        _table [idx] = null;
    }
    return true;
}

error_t task_create_hook_add (task_create_hook_t _hook)
{
    interrupt_level_t level = global_interrupt_disable ();
    if (!hook_add ((void **)g_create_table, _hook, 
        TASK_CREATE_HOOK_LAST_INDEX)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_HOOK_CREATE_NOROOM);
    }
    global_interrupt_enable (level);
    return 0;
}

error_t task_create_hook_remove (task_create_hook_t _hook)
{
    interrupt_level_t level = global_interrupt_disable ();
    if (!hook_remove ((void **)g_create_table, _hook, 
        TASK_CREATE_HOOK_LAST_INDEX)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_HOOK_CREATE_NOTFOUND);
    }
    global_interrupt_enable (level);
    return 0;
}

// this function will be called in KERNEL mode, we don't need to take a lock
void task_create_hook_traverse (task_handle_t _handle)
{
    int idx = 0;
    
    for (; idx <= TASK_CREATE_HOOK_LAST_INDEX && g_create_table [idx] != null;
        idx ++) {
        g_create_table [idx] (_handle);
    }
}

error_t task_switch_hook_add (task_switch_hook_t _hook)
{
    interrupt_level_t level = global_interrupt_disable ();
    if (!hook_add ((void **)g_switch_table, _hook, 
        TASK_SWITCH_HOOK_LAST_INDEX)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_HOOK_SWITCH_NOROOM);
    }
    global_interrupt_enable (level);
    return 0;
}

error_t task_switch_hook_remove (task_switch_hook_t _hook)
{
    interrupt_level_t level = global_interrupt_disable ();
    if (!hook_remove ((void **)g_switch_table, _hook, 
        TASK_SWITCH_HOOK_LAST_INDEX)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_HOOK_SWITCH_NOTFOUND);
    }
    global_interrupt_enable (level);
    return 0;
}

// this function will be called in KERNEL mode, we don't need to take a lock
void task_switch_hook_traverse (task_handle_t _from, task_handle_t _to)
{
    int idx = 0;
    
    for (; idx <= TASK_SWITCH_HOOK_LAST_INDEX && g_switch_table [idx] != null;
        idx ++) {
        g_switch_table [idx] (_from, _to);
    }
}

error_t task_delete_hook_add (task_delete_hook_t _hook)
{
    interrupt_level_t level = global_interrupt_disable ();
    if (!hook_add ((void **)g_delete_table, _hook, 
        TASK_DELETE_HOOK_LAST_INDEX)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_HOOK_DELETE_NOROOM);
    }
    global_interrupt_enable (level);
    return 0;
}

error_t task_delete_hook_remove (task_delete_hook_t _hook)
{
    interrupt_level_t level = global_interrupt_disable ();
    if (!hook_remove ((void **)g_delete_table, _hook, 
        TASK_DELETE_HOOK_LAST_INDEX)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_HOOK_DELETE_NOTFOUND);
    }
    global_interrupt_enable (level);
    return 0;
}

// this function will be called in KERNEL mode, we don't need to take a lock
void task_delete_hook_traverse (task_handle_t _handle)
{
    int idx = 0;
    
    for (; idx <= TASK_DELETE_HOOK_LAST_INDEX && g_delete_table [idx] != null;
        idx ++) {
        g_delete_table [idx] (_handle);
    }
}

