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

//lint -esym(728, g_scheduler_locked_count, g_statistics)

#include "console.h"
#include "clib.h"
#include "task.h"
#include "idle.h"
#include "errtask.h"
#include "clock.h"
#include "hook.h"

#define TASK_PRIORITY_LEVELS    BITS_SUPPORTED
#define TASK_LAST_INDEX         (TASK_PRIORITY_LEVELS - 1)
#define TASK_PRIORITY_LOWEST    TASK_LAST_INDEX

#define IDLE_TASK_PRIORITY      TASK_PRIORITY_LOWEST

#define MAGIC_NUMBER_TASK       0x5441534BL

// keep this in line with stack_unit_t
#define STACK_WIDTH_IN_BYTES    4
#define STACK_WIDTH_SHIFT4BYTE  2
#define MAGIC_NUMBER_STACK      0xDEAD5AA5L

#define is_invalid_handle(_handle) (((_handle) == null) || \
    ((_handle)->magic_number_ != MAGIC_NUMBER_TASK))

typedef struct {
    statistic_t scheduled_;
    statistic_t overflowed_;
    statistic_t invalid_handle_;
} task_statistic_t;

static task_t g_task_pool [TASK_PRIORITY_LEVELS];
task_bitmap_t g_ready_bitmap;
static task_handle_t g_priority_map [TASK_PRIORITY_LEVELS];

static device_handle_t g_tick_handle;
static int g_scheduler_locked_count;

static dll_t g_allocated_task;

static task_statistic_t g_statistics;

static task_handle_t g_task_running;
static task_handle_t g_task_idle;
static task_context_t g_start_context;

static bool g_multitasking_started;

// this is the entry function for all tasks, it calls the entry function setting
// by the user via task_start ()
static void task_main ()
{
    task_handle_t handle = g_task_running;

    global_interrupt_enable (INTERRUPT_ENABLED);
    // the entry function should not return, otherwise, it means the task is
    // going to be exited
    handle->entry_ (handle->name_, handle->argument_);
    // delete the task if the entry function returns, task_delete () could 
    // return an error code if this is the second time to delete it, so, always
    // ignore return value
    (void) task_delete (handle);
}

static void task_timer_callback (timer_handle_t _handle, void *_arg)
{
    task_handle_t p_task = (task_handle_t)_arg;
    interrupt_level_t level;
    
    UNUSED (_handle);
    
    level = global_interrupt_disable ();
    if (TASK_STATE_WAITING == p_task->state_) {
        (void) task_state_change (p_task, TASK_STATE_READY);
        p_task->ecode_ = ERROR_T (ERROR_TASK_WAIT_TIMEOUT);
    }
    global_interrupt_enable (level);
}

error_t task_state_change (task_handle_t _handle, task_state_t _new_state)
{
    error_t ecode = 0;

    if (_handle->state_ == _new_state) {
        return 0;
    }
    
    switch (_new_state)
    {
    case TASK_STATE_CREATED:
        {
            #define TIMER_PREFIX    "Task:"
            char timer_name [NAME_MAX_LENGTH + 1] = TIMER_PREFIX;
            
            // initialize the stack with value of MAGIC_NUMBER_STACK
            stack_unit_t *p_unit = (stack_unit_t *)_handle->stack_base_;
            int count = _handle->stack_size_ >> STACK_WIDTH_SHIFT4BYTE;
            
            while (count > 0) {
                *p_unit ++ = MAGIC_NUMBER_STACK;
                count --;
            }
            context_init (&_handle->context_, _handle->stack_base_, 
                _handle->stack_size_, (address_t) task_main);
            strncpy (&timer_name [sizeof (TIMER_PREFIX)], _handle->name_, 
                sizeof (timer_name) - (sizeof (TIMER_PREFIX) + 1));
            timer_name [sizeof (timer_name) - 1] = 0;
            ecode = timer_alloc (&_handle->timer_, timer_name, TIMER_TYPE_INTERRUPT);
            if (0 != ecode) {
                return ecode;
            }
        }
        break;
    case TASK_STATE_READY:
        {
            if (TASK_STATE_RUNNING == _handle->state_) {
                break;
            }
            if (timer_is_started (_handle->timer_)) {
                (void) timer_stop (_handle->timer_, null);
            }
            task_bitmap_bit_set (&g_ready_bitmap, _handle->priority_);
            g_priority_map [_handle->priority_] = _handle;
        }
        break;
    case TASK_STATE_RUNNING:
        {
        }
        break;
    case TASK_STATE_SUSPENDING:
        {
            if (timer_is_started (_handle->timer_)) {
                (void) timer_stop (_handle->timer_, &_handle->timeout_);
            }
            task_bitmap_bit_clear (&g_ready_bitmap, _handle->priority_);
        }
        break;
    case TASK_STATE_WAITING:
        {
            if (_handle->timeout_ != 0) {
                (void) timer_start (_handle->timer_, _handle->timeout_, 
                    task_timer_callback, _handle);
                _handle->timeout_ = 0;
            }
            task_bitmap_bit_clear (&g_ready_bitmap, _handle->priority_);
        }
        break;
    case TASK_STATE_DELETED:
        {
            task_bitmap_bit_clear (&g_ready_bitmap, _handle->priority_);
            (void) timer_free (_handle->timer_);
        }
        break;
    }
    _handle->state_ = _new_state;
    return 0;
}

void task_schedule (preschedule_callback_t _callback)
{
    interrupt_level_t level;
    task_handle_t running, successor;
    bool overflowed = false;

    level = global_interrupt_disable ();
    if (!g_multitasking_started) {
        global_interrupt_enable (level);
        return;
    }
    if (is_in_interrupt () || g_scheduler_locked_count != 0) {
        global_interrupt_enable (level);
        return;
    }
    successor = g_priority_map [task_bitmap_lowest_bit_get (&g_ready_bitmap)];
    if (successor == g_task_running) {
        global_interrupt_enable (level);
        return;
    }
    running = g_task_running;
    g_task_running = successor;
    if (0 == is_stack_overflowed (running, &overflowed)) {
        if (overflowed) {
            console_print ("\n\nError: stack overflow for task \"%s\"\n\n", 
                running->name_);
            (void) task_suspend (running);
            g_statistics.overflowed_ ++;
        }
    }
    if (TASK_STATE_RUNNING == running->state_) {
        (void) task_state_change (running, TASK_STATE_READY);
    }
    (void) task_state_change (successor, TASK_STATE_RUNNING);
    g_statistics.scheduled_ ++;
    successor->stats_scheduled_ ++;
    if (null != _callback) {
        _callback (running, successor);
    }
    task_switch_hook_traverse (running, successor);
    context_switch (&running->context_, &successor->context_);
    global_interrupt_enable (level);
}

void task_schedule_in_interrupt ()
{
    interrupt_level_t level;
    task_handle_t running, successor;
    bool overflowed = false;
    
    level = global_interrupt_disable ();
    if (0 != g_scheduler_locked_count) {
        global_interrupt_enable (level);
        return;
    }
    
    successor = g_priority_map [task_bitmap_lowest_bit_get (&g_ready_bitmap)];
    if (successor == g_task_running) {
        global_interrupt_enable (level);
        return;
    }
    running = g_task_running;
    g_task_running = successor;
    if (0 == is_stack_overflowed (running, &overflowed)) {
        if (overflowed) {
            //console_print ("\n\nError: stack overflow for task \"%s\"\n\n", 
            //    running->name_);
            (void) task_suspend (running);
            g_statistics.overflowed_ ++;
        }
    }
    if (TASK_STATE_RUNNING == running->state_) {
        (void) task_state_change (running, TASK_STATE_READY);
    }
    (void) task_state_change (successor, TASK_STATE_RUNNING);
    g_statistics.scheduled_ ++;
    successor->stats_scheduled_ ++;
    task_switch_hook_traverse (running, successor);
    context_switch_in_interrupt (&running->context_, &successor->context_);
    global_interrupt_enable (level);
}

void idle_task_spawn ()
{
    STACK_DECLARE (stack, CONFIG_IDLE_TASK_STACK_SIZE);
    
    (void) task_create (&g_task_idle, "Idle", IDLE_TASK_PRIORITY, 
        stack, sizeof (stack));
    (void) task_start (g_task_idle, task_entry_idle, 0);
}

static bool task_check_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    task_handle_t handle = (task_handle_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    console_print ("Error: task \"%s\" isn't deleted\n", handle->name_);
    return true;
}

error_t module_task (system_state_t _state)
{
    if (STATE_INITIALIZING == _state) {
        memset (&g_priority_map [0], 0, sizeof (g_priority_map));
        task_bitmap_init (&g_ready_bitmap);
        idle_task_spawn ();
    }
    else if (STATE_DESTROYING == _state) {
        (void) task_delete (g_task_idle);
        // check whether all tasks created have been deleted or not, if not take
        // them as error
        (void) dll_traverse (&g_allocated_task, task_check_for_each, 0);
    }
    return 0;
}

void multitasking_start ()
{
    interrupt_level_t level;

    level = global_interrupt_disable ();
    if (g_multitasking_started) {
        global_interrupt_enable (level);
        return;
    }
    g_multitasking_started = true;
    g_task_running = g_priority_map [task_bitmap_lowest_bit_get (&g_ready_bitmap)];
    g_task_running->stats_scheduled_ ++;
    g_statistics.scheduled_ ++;
    interrupt_exit_callback_install (task_schedule_in_interrupt);
    global_interrupt_enable (level);
        
    // open and start the tick
    if (device_open (&g_tick_handle, "/dev/clock/tick", 0) != 0) {
        console_print ("Error: cannot open tick device");
    }
    if (device_control (g_tick_handle, OPTION_TICK_START, 
        (int)CONFIG_TICK_DURATION_IN_MSEC, tick_process) != 0) {
        console_print ("Error: cannot start tick device");
    }
    
    context_switch (&g_start_context, &g_task_running->context_);
}

void multitasking_stop ()
{
    interrupt_level_t level;

    level = global_interrupt_disable ();
    if (!g_multitasking_started) {
        global_interrupt_enable (level);
        return;
    }
    g_multitasking_started = false;
    global_interrupt_enable (level);
    
    if (device_control (g_tick_handle, OPTION_TICK_STOP, 0, 0) != 0) {
        console_print ("Error: cannot stop tick device");
    }
    if (device_close (g_tick_handle) != 0) {
        console_print ("Error: cannot close tick device");
    }
    
    context_switch (&g_task_running->context_, &g_start_context);
}

error_t task_create (task_handle_t *_p_handle, const char _name [], 
    task_priority_t _priority, stack_unit_t *_stack_base, usize_t _stack_bytes)
{
    task_handle_t handle;
    interrupt_level_t level;
    error_t ecode;
    
    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_TASK_CREATE_INVCONTEXT);
    }
    if (_priority > TASK_LAST_INDEX) {
        return ERROR_T (ERROR_TASK_CREATE_INVPRIO);
    }

    handle = &g_task_pool [_priority];
    
    level = global_interrupt_disable ();
    if (MAGIC_NUMBER_TASK == handle->magic_number_) {
        ecode = ERROR_T (ERROR_TASK_CREATE_PRIOINUSE);
        goto error;
    }
    memset (handle, 0, sizeof (*handle));
    handle->magic_number_ = MAGIC_NUMBER_TASK;
    global_interrupt_enable (level);

    if (0 == _name) {
        handle->name_ [0] = 0;
    }
    else {
        strncpy (handle->name_, _name, (usize_t)sizeof (handle->name_) - 1);
        handle->name_ [sizeof (handle->name_) - 1] = 0;
    }
    handle->timeout_ = 0;
    handle->priority_ = _priority;
    handle->stack_base_ = ((address_t)_stack_base + (STACK_WIDTH_IN_BYTES - 1)) &
        (~(STACK_WIDTH_IN_BYTES - 1));
    handle->stack_size_ = _stack_bytes - (handle->stack_base_ - 
        (address_t)_stack_base);
    handle->stack_size_ &= ~(STACK_WIDTH_IN_BYTES - 1);
    
    level = global_interrupt_disable ();
    ecode = task_state_change (handle, TASK_STATE_CREATED);
    if (0 != ecode) {
        handle->magic_number_ = 0;
        goto error;
    }
    dll_push_tail (&g_allocated_task, &handle->node_);
    task_create_hook_traverse (handle);
    global_interrupt_enable (level);

    *_p_handle = handle;
    return 0;
    
error:
    global_interrupt_enable (level);
    return ecode;
}

error_t task_delete (task_handle_t _handle)
{
    interrupt_level_t level;
    bool schedule_needed = false;
    error_t ecode = 0;

    if (is_in_interrupt () && STATE_UP == system_state ()) {
        return ERROR_T (ERROR_TASK_DELETE_INVCONTEXT);
    }
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        g_statistics.invalid_handle_ ++;
        ecode = ERROR_T (ERROR_TASK_DELETE_INVHANDLE);
        goto error;
    }
    ecode = task_state_change (_handle, TASK_STATE_DELETED);
    if (0 != ecode) {
        goto error;
    }
    _handle->magic_number_ = 0;
    if (_handle == g_task_running) {
        schedule_needed = true;
    }
    dll_remove (&g_allocated_task, &_handle->node_);
    task_delete_hook_traverse (_handle);
    global_interrupt_enable (level);
    
    // only task deleting itself needs a re-schedule, if a task is deleted
    // by other running task, that's to say the running task's priority is
    // higher than deleted one, so we don't need to do a re-schedule.
    if (schedule_needed) {
        task_schedule (null);
    }
    return 0;

 error:
    global_interrupt_enable (level);
    return ecode;
}

error_t task_start (task_handle_t _handle, task_entry_t _entry, void *_p_arg)
{
    interrupt_level_t level;
    error_t ecode;

    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        g_statistics.invalid_handle_ ++;
        ecode = ERROR_T (ERROR_TASK_START_INVHANDLE);
        goto error;
    }
    if (TASK_STATE_CREATED != _handle->state_) {
        ecode = ERROR_T (ERROR_TASK_START_INVOP);
        goto error;
    }
    _handle->entry_ = _entry;
    _handle->argument_ = _p_arg;
    (void) task_state_change (_handle, TASK_STATE_READY);
    global_interrupt_enable (level);
    
    task_schedule (null);
    return 0;

error:
    global_interrupt_enable (level);
    return ecode;
}

error_t task_suspend (task_handle_t _handle)
{
    interrupt_level_t level;
    bool schedule_needed = false;
    error_t ecode;

    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        g_statistics.invalid_handle_ ++;
        ecode = ERROR_T (ERROR_TASK_SUSPEND_INVHANDLE);
        goto error;
    }
    if (TASK_STATE_CREATED == _handle->state_) {
        ecode = ERROR_T (ERROR_TASK_SUSPEND_NOTSTARTED);
        goto error;
    }
    
    ecode = task_state_change (_handle, TASK_STATE_SUSPENDING);
    if (0 != ecode) {
        goto error;
    }
    if (_handle == g_task_running) {
        schedule_needed = true;
    }
    global_interrupt_enable (level);
    // only task suspending itself needs a re-schedule, if a task is suspended
    // by other running task, that's to say the running task's priority is
    // higher than suspended one, so we don't need to do a re-schedule.
    if (schedule_needed) {
        task_schedule (null);
    }
    return 0;

error:
    global_interrupt_enable (level);
    return ecode;
}

error_t task_resume (task_handle_t _handle)
{
    interrupt_level_t level;
    error_t ecode;

    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        g_statistics.invalid_handle_ ++;
        ecode = ERROR_T (ERROR_TASK_RESUME_INVHANDLE);
        goto error;
    }
    if (TASK_STATE_SUSPENDING != _handle->state_) {
        ecode = ERROR_T (ERROR_TASK_RESUME_NOTSUSPENDED);
        goto error;
    }
    if (0 == _handle->timeout_) {
        (void) task_state_change (_handle, TASK_STATE_READY);
    }
    else {
        (void) task_state_change (_handle, TASK_STATE_WAITING);
    }
    global_interrupt_enable (level);
    
    task_schedule (null);
    return 0;

error:
    global_interrupt_enable (level);
    return ecode;
}

static void task_yeild_cpu (task_handle_t _from, task_handle_t _to)
{
    UNUSED (_to);
    (void) task_state_change (_from, TASK_STATE_READY);
}

error_t task_sleep (msecond_t _duration)
{
    task_handle_t handle = g_task_running;
    interrupt_level_t level;
    preschedule_callback_t callback = null;
    
    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_TASK_SLEEP_INVCONTEXT);
    }
    
    if (0 == _duration) {
        // Duration equals to 0, that means the task wants to yield the CPU.
        callback = task_yeild_cpu;
    }
    level = global_interrupt_disable ();
    handle->timeout_ = _duration;
    (void) task_state_change (handle, TASK_STATE_WAITING);
    global_interrupt_enable (level);

    task_schedule (callback);
    return 0;
}

task_handle_t task_self ()
{
    return g_task_running;
}

//lint -e{818}
bool is_invalid_task (const task_handle_t _handle)
{
    interrupt_level_t level;
    bool is_invalid;
    
    level = global_interrupt_disable ();
    is_invalid = is_invalid_handle (_handle);
    global_interrupt_enable (level);
    return is_invalid;
}

void task_priority_change (task_handle_t _handle, task_priority_t _to)
{
    if ((TASK_STATE_READY == _handle->state_) || 
        (TASK_STATE_RUNNING == _handle->state_)) {
        task_bitmap_bit_clear (&g_ready_bitmap, _handle->priority_);
        g_priority_map [_handle->priority_] = null;
        _handle->priority_ = _to;
        task_bitmap_bit_set (&g_ready_bitmap, _handle->priority_);
        g_priority_map [_handle->priority_] = _handle;
    }
    else {
        _handle->priority_ = _to;
    }
}

task_handle_t task_from_priority (task_priority_t _priority)
{
    if (_priority > TASK_LAST_INDEX) {
        return null;
    }

    return &g_task_pool [_priority];
}

void scheduler_lock ()
{
    interrupt_level_t level;

    if (is_in_interrupt ()) {
        return;
    }
    
    level = global_interrupt_disable ();
    if (!g_multitasking_started) {
        global_interrupt_enable (level);
        return;
    }
    g_scheduler_locked_count ++;
    global_interrupt_enable (level);
}

void scheduler_unlock ()
{
    interrupt_level_t level;

    if (is_in_interrupt ()) {
        return;
    }
    level = global_interrupt_disable ();
    if (!g_multitasking_started) {
        global_interrupt_enable (level);
        return;
    }
    g_scheduler_locked_count --;
    global_interrupt_enable (level);
    
    task_schedule (null);
}

//lint -e{818}
error_t is_stack_overflowed (const task_handle_t _handle, bool *_p_overflowed)
{
    interrupt_level_t level;
    stack_unit_t *p_top;

    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        g_statistics.invalid_handle_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_STACK_INVHANDLE);
    }
    p_top = (stack_unit_t *)_handle->stack_base_;
    if (p_top [0] != MAGIC_NUMBER_STACK || p_top [1] != MAGIC_NUMBER_STACK) {
        *_p_overflowed = true;
    }
    else {
        *_p_overflowed = false;
    }
    global_interrupt_enable (level);
    
    return 0;
}

//lint -e{818}
error_t stack_used_percentage (const task_handle_t _handle, int *_p_percentage)
{
    interrupt_level_t level;
    stack_unit_t *p_top;
    int nondirty_count = 0;

    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_TASK_STACK_INVCONTEXT);
    }
    
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        g_statistics.invalid_handle_ ++;
        global_interrupt_enable (level);
        return ERROR_T (ERROR_TASK_STACK_INVHANDLE);
    }
    p_top = (stack_unit_t *)_handle->stack_base_;
    // we don't want to lock the interrupt to worsen the response for interrupt
    // so we can lock the scheduler instead
    scheduler_lock ();
    global_interrupt_enable (level);

    while (MAGIC_NUMBER_STACK == *p_top) {
        nondirty_count ++;
        p_top ++;
    }
    *_p_percentage = 100 - 
        nondirty_count*100/(int)(_handle->stack_size_ >> STACK_WIDTH_SHIFT4BYTE);

    scheduler_unlock ();
    return 0;
}

//lint -e{818}
static const char *task_state_description (const task_handle_t _handle)
{
    interrupt_level_t level;
    task_state_t state;
    
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        g_statistics.invalid_handle_ ++;
        global_interrupt_enable (level);
        return "INVALID";
    }
    state = _handle->state_;
    global_interrupt_enable (level);
    
    switch (state)
    {
    case TASK_STATE_CREATED:
        return "CREATED";
    case TASK_STATE_READY:
        return "READY";
    case TASK_STATE_RUNNING:
        return "RUNNING";
    case TASK_STATE_SUSPENDING:
        return "SUSPENDING";
    case TASK_STATE_WAITING:
        return "WAITING";
    case TASK_STATE_DELETED:
        return "DELETED";
    default:
        return "INVALID";
    }
}

static bool task_dump_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    task_handle_t handle = (task_handle_t)_p_node;
    bool overflowed;
    int percentage;

    UNUSED (_p_dll);
    UNUSED (_p_arg);
    
    (void) is_stack_overflowed (handle, &overflowed);
    (void) stack_used_percentage (handle, &percentage);
    console_print ("  Name: %s\n", handle->name_);
    console_print ("      Priority: %u\n", handle->priority_);
    console_print ("    Stack Base: %u\n", handle->stack_base_);
    if (overflowed) {
        console_print ("    Stack Size: %u bytes (overflowed)\n", 
            handle->stack_size_);
    }
    else {
        console_print ("    Stack Size: %u bytes (%u%% used)\n", 
            handle->stack_size_, percentage);
    }
    console_print ("     Scheduled: %u\n", handle->stats_scheduled_);
    console_print ("         State: %s\n", task_state_description (handle));
    console_print ("         Event: %x\n", handle->event_received_);
    console_print ("\n");
    return true;
}

void task_dump ()
{
    if (is_in_interrupt ()) {
        return;
    }
    
    scheduler_lock ();
    console_print ("\n\n");
    console_print ("Summary\n");
    console_print ("-------\n");
    console_print ("  Supported: %u\n", TASK_PRIORITY_LEVELS);
    console_print ("  Allocated: %u\n", dll_size (&g_allocated_task));
    console_print ("  .BSS Used: %u\n", ((address_t)&g_multitasking_started 
        - (address_t)g_task_pool) + sizeof (g_multitasking_started));
    console_print ("\n");
    console_print ("Statistics\n");
    console_print ("----------\n");
    console_print ("    Task Scheduled: %u\n", g_statistics.scheduled_);
    console_print ("  Stack Overflowed: %u\n", g_statistics.overflowed_);
    console_print ("    Invalid Handle: %u\n", g_statistics.invalid_handle_);
    console_print ("\n");
    console_print ("Task Details\n");
    console_print ("------------\n");
    (void) dll_traverse (&g_allocated_task, task_dump_for_each, 0);
    console_print ("\n");
    scheduler_unlock ();
}

#ifdef UNITEST
dll_t *unitest_get_allocated_dll ()
{
    return &g_allocated_task;
}


#endif

