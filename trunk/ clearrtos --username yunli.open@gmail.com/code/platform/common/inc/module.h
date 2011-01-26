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
 *   Date: 08/29/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __MODULE_H
#define __MODULE_H

#include "error.h"

typedef enum {
    MODULE_MODULE,          // for module management
    MODULE_INTERRUPT,       // for interrupt management
    MODULE_DEVICE,          // for device management
    MODULE_CLOCK,           // for clock management
    MODULE_CONSOLE,         // for device of console
    MODULE_CTRLC,           // for handling Ctrl+C on Linux/Cygwin
    MODULE_FLASH,           // for device of flash
    MODULE_TIMER,           // for timer management
    MODULE_TASK,            // for task
    MODULE_SYNC,            // for task sync object management
    MODULE_SEMAPHORE,       // for semaphore management
    MODULE_MUTEX,           // for mutex management
    MODULE_QUEUE,           // for queue management
    MODULE_HEAP,            // for heap management
    MODULE_MPOOL,           // for memory pool management
    
    MODULE_TESTAPP,         // for Test Application

    // !!! NOTE: please always put the MODULE_COUNT and MODULE_LAST at the 
    // end of this enum
    MODULE_COUNT,
    MODULE_LAST = (MODULE_COUNT - 1)
} module_t;

typedef enum {
    LEVEL_FIRST,
    CPU_LEVEL = LEVEL_FIRST,
    PERIPHERALS_LEVEL,
    DRIVER_LEVEL,
    OS_LEVEL,
    
    // for platform layer
    PLATFORM_LEVEL0,
    PLATFORM_LEVEL1,
    PLATFORM_LEVEL2,
    PLATFORM_LEVEL3,
    PLATFORM_LEVEL4,
    PLATFORM_LEVEL5,
    PLATFORM_LEVEL6,
    PLATFORM_LEVEL7,
    
    // for framework layer
    FRAMEWORK_LEVEL0,
    FRAMEWORK_LEVEL1,
    FRAMEWORK_LEVEL2,
    FRAMEWORK_LEVEL3,
    FRAMEWORK_LEVEL4,
    FRAMEWORK_LEVEL5,
    FRAMEWORK_LEVEL6,
    FRAMEWORK_LEVEL7,
    
    // for application layer
    APPLICATION_LEVEL0,
    APPLICATION_LEVEL1,
    APPLICATION_LEVEL2,
    APPLICATION_LEVEL3,
    APPLICATION_LEVEL4,
    APPLICATION_LEVEL5,
    APPLICATION_LEVEL6,
    APPLICATION_LEVEL7,

    // LEVEL_COUNT and LEVEL_LAST must be put at the end of this enum
    LEVEL_COUNT,
    LEVEL_LAST = (LEVEL_COUNT - 1)
} init_level_t;

typedef enum {
    STATE_INITIALIZING,
    STATE_UP,
    STATE_DOWN,
    STATE_DESTROYING
} system_state_t;

typedef error_t (*module_callback_t)(system_state_t _state);

#ifdef  __cplusplus
extern "C" {
#endif

error_t system_up ();
void system_down ();
error_t module_register (const char _name [], module_t _module, 
    init_level_t _level, module_callback_t _callback);
system_state_t system_state ();

#ifdef __cplusplus
}
#endif

#endif

