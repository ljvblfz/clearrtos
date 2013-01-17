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
 *   Date: 11/26/2010                                                         *
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
#include "heap.h"
#include "console.h"

static void task_test (const char _name [], void *_p_arg)
{
    void *p_buf1, *p_buf2, *p_buf3;
    error_t result;

    UNUSED (_name);
    UNUSED (_p_arg);

    console_print ("\n");
    console_print ("===========\n");
    console_print ("Before Test ->\n");
    console_print ("===========\n");
    heap_dump ();
    
    console_print ("=============================\n");
    console_print ("Test Case 1): allocate 1 byte ->\n");
    console_print ("=============================\n");
    p_buf1 = heap_alloc (1, EXAMPLE_MAIN_1, &result);
    console_print ("    Allocated Addr: %p\n", p_buf1);
    heap_dump ();

    console_print ("================================\n");
    console_print ("Test Case 2): allocate 32K bytes ->\n");
    console_print ("================================\n");
    p_buf2 = heap_alloc (32*1024, EXAMPLE_MAIN_2, &result);
    console_print ("    Allocated Addr: %p\n", p_buf2);
    heap_dump ();
    
    console_print ("================================\n");
    console_print ("Test Case 3): allocate 64K bytes ->\n");
    console_print ("================================\n");
    p_buf3 = heap_alloc (64*1024, EXAMPLE_MAIN_3, &result);
    console_print ("    Allocated Addr: %p\n", p_buf3);
    heap_dump ();
    
    console_print ("============================\n");
    console_print ("Test Case 4): free 32K bytes ->\n");
    console_print ("============================\n");
    if ((result = heap_free (p_buf2)) != 0) {
        console_print ("heap_free () returns %s!\n", errstr (result));
    }
    console_print ("    Freed Addr: %p\n", p_buf2);
    heap_dump ();
    
    console_print ("======================================\n");
    console_print ("Test Case 5): allocate 64K bytes again ->\n");
    console_print ("======================================\n");
    p_buf2 = heap_alloc (64*1024, EXAMPLE_MAIN_4, &result);
    console_print ("    Allocated Addr: %p\n", p_buf2);
    heap_dump ();
    
    console_print ("=========================\n");
    console_print ("Test Case 6): free 1 byte ->\n");
    console_print ("=========================\n");
    if ((result = heap_free (p_buf1)) != 0) {
        console_print ("heap_free () returns %s!\n", errstr (result));
    }
    console_print ("    Freed Addr: %p\n", p_buf1);
    heap_dump ();
    
    console_print ("============================\n");
    console_print ("Test Case 7): free 64K bytes ->\n");
    console_print ("============================\n");
    if ((result = heap_free (p_buf2)) != 0) {
        console_print ("heap_free () returns %s!\n", errstr (result));
    }
    console_print ("    Freed Addr: %p\n", p_buf2);
    heap_dump ();
#if 0    
    console_print ("============================\n");
    console_print ("Test Case 8): free 64K bytes ->\n");
    console_print ("============================\n");
    if ((result = heap_free (p_buf3)) != 0) {
        console_print ("heap_free () returns %s!\n", errstr (result));
    }
    console_print ("    Freed Addr: %p\n", p_buf3);
    heap_dump ();
#endif
    multitasking_stop ();
}

error_t module_testapp (system_state_t _state)
{
    static task_handle_t handle;
    STACK_DECLARE (stack, 1024);
    
    if (STATE_INITIALIZING == _state) {
        (void) task_create (&handle, "Test", 16, stack, sizeof (stack));
        (void) task_start (handle, task_test, 0);
    }
    else if (STATE_DESTROYING == _state) {
        (void) task_delete (handle);
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
    (void) module_register ("Heap", MODULE_HEAP, OS_LEVEL, module_heap);
    (void) module_register ("TestApp", MODULE_TESTAPP, APPLICATION_LEVEL3, module_testapp);
    return 0;
}
    
