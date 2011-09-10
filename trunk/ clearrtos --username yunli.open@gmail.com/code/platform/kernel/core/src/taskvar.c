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
#include "task.h"
#include "errtask.h"
#include "hook.h"
#include "taskvar.h"

#define TASK_VARIABLE_LAST_INDEX    (CONFIG_MAX_TASK_VARIABLE - 1)

static task_variable_node_t g_variable_pool [CONFIG_MAX_TASK_VARIABLE];
static dll_t g_variable_free;

static bool task_variable_store (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    task_variable_node_t *p_node = (task_variable_node_t *)_p_node;
    value_t *p_value = (value_t *)p_node->address_;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    p_node->value_ = *p_value;
    return true;
}

static bool task_variable_restore (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    task_variable_node_t *p_node = (task_variable_node_t *)_p_node;
    value_t *p_value = (value_t *)p_node->address_;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    *p_value = p_node->value_;
    return true;
}

static void task_variable_switch_hook (task_handle_t _from, task_handle_t _to)
{
    (void) dll_traverse (&_from->variable_, task_variable_store, 0);
    (void) dll_traverse (&_to->variable_, task_variable_restore, 0);
}

static error_t task_variable_init ()
{
    int idx = 0;
    
    for (; idx <= TASK_VARIABLE_LAST_INDEX; idx ++) {
        dll_push_tail (&g_variable_free, &g_variable_pool [idx].node_);
    }
    return task_switch_hook_add (task_variable_switch_hook);
}

error_t task_variable_add (value_t *_p_value)
{
    interrupt_level_t level;
    static bool initialized = false;
    task_variable_node_t *p_node;
    task_handle_t handle = task_self ();

    level = global_interrupt_disable ();
    if (is_invalid_task (handle) || is_in_interrupt ()) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_VARIABLE_ADD_INVTASK);
    }
    if (!initialized) {
        error_t ecode = task_variable_init ();
        if (0 != ecode) {
            global_interrupt_enable (level);
            return ecode;
        }
        initialized = true;
    }

    p_node = (task_variable_node_t *)dll_pop_head (&g_variable_free);
    if (null == p_node) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_VARIABLE_ADD_NOVAR);
    }
    p_node->address_ = (address_t)_p_value;
    dll_push_tail (&handle->variable_, &p_node->node_);
    global_interrupt_enable (level);
    return 0;
}

static bool task_variable_find (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    task_variable_node_t *p_node = (task_variable_node_t *)_p_node;

    UNUSED (_p_dll);
    
    if (p_node->address_ == (address_t)_p_arg) {
        return false;
    }
    return true;
}

error_t task_variable_remove (value_t *_p_value)
{
    interrupt_level_t level;
    task_variable_node_t *p_node;
    task_handle_t handle = task_self ();

    level = global_interrupt_disable ();
    if (is_invalid_task (handle) || is_in_interrupt ()) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_VARIABLE_REMOVE_INVTASK);
    }
    p_node = (task_variable_node_t *)dll_traverse (&handle->variable_,
        task_variable_find, _p_value);
    if (null == p_node) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_VARIABLE_REMOVE_NOTFOUND);
    }
    dll_remove (&handle->variable_, &p_node->node_);
    dll_push_tail (&g_variable_free, &p_node->node_);
    global_interrupt_enable (level);
    return 0;
}

