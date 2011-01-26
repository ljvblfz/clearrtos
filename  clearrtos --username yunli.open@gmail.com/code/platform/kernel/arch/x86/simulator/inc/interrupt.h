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
 *   Date: 07/30/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "primitive.h"
#include "error.h"
#include "module.h"
#include "clib.h"

#define CONFIG_MAX_INTERRUPT    64
#define INTERRUPT_LAST_INDEX    (CONFIG_MAX_INTERRUPT - 1)
#define INTERRUPT_NONE          (-1)

#define memory_barrier() asm volatile ("" ::: "memory")
#define interrupt_flash() interrupt_enable (level); \
    memory_barrier (); \
    level = interrupt_disable ();

typedef void (*interrupt_handler_t) (int _vector);
typedef void (*interrupt_exit_callback_t) ();

typedef enum {
    INTERRUPT_ENABLED,
    INTERRUPT_DISABLED
} interrupt_level_t;

#ifdef  __cplusplus
extern "C" {
#endif

error_t module_interrupt (system_state_t _state);

void device_interrupt_handler_install (interrupt_handler_t _handler);
void interrupt_exit_callback_install (interrupt_exit_callback_t _handler);
interrupt_handler_t interrupt_handler_install (int _vector, 
    interrupt_handler_t _handler);
void interrupt_enable (int _vector);
void interrupt_disable (int _vector);
void interrupt_enter ();
void interrupt_exit ();
bool is_in_interrupt ();

static inline void global_interrupt_enable (interrupt_level_t _level)
{
    int status;
    
    extern sigset_t g_signal_mask;

    if (INTERRUPT_ENABLED == _level) {
        status = sigprocmask (SIG_UNBLOCK, &g_signal_mask, 0);
    }
    else {
        status = sigprocmask (SIG_BLOCK, &g_signal_mask, 0);
    }
    if (status) {
        printf ("Error: global_interrupt_enable() is failed\n");
    }
}

static inline interrupt_level_t global_interrupt_disable ()
{
    int status;
    sigset_t old_mask;
    
    extern sigset_t g_signal_mask_empty;
    extern sigset_t g_signal_mask;

    (void) sigemptyset (&old_mask);
    status = sigprocmask (SIG_BLOCK, &g_signal_mask, &old_mask);
    if (status) {
        printf ("Error: global_interrupt_disable() is failed\n");
    }
    if (memcmp ((void *)&g_signal_mask_empty, (void *)&old_mask, 
        sizeof(sigset_t))) {
        return INTERRUPT_DISABLED;
    }
    return INTERRUPT_ENABLED;
}

static inline interrupt_level_t interrupt_level_get ()
{
    sigset_t old_mask;
    
    extern sigset_t g_signal_mask_empty;

    (void) sigemptyset (&old_mask);
    (void) sigprocmask (SIG_BLOCK, 0, &old_mask);

    if (memcmp ((void *)&g_signal_mask_empty, (void *)&old_mask, 
        sizeof(sigset_t))) {
        return INTERRUPT_DISABLED;
    }
    return INTERRUPT_ENABLED;
}

#ifdef __cplusplus
}
#endif

#endif

