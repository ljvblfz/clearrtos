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
 *   Date: 12/27/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __CONFIG_H
#define __CONFIG_H

// for Task Bitmap
// Summary:
//   Supported bits is calculated by "CONFIG_MAX_BITMAP_ROW * CONFIG_MAX_BIT_PER_ROW".
//   The max bit supported by this module is 1024 with below configuration:
//       typedef unsigned int task_bitmap_row_t;
//       CONFIG_MAX_BITMAP_ROW = 32;
//       CONFIG_MAX_BIT_PER_ROW = 32;
//   The min bits supported by this module is 8 with below configuration:
//       typedef unsigned char task_bitmap_row_t;
//       CONFIG_MAX_BITMAP_ROW = 1;
//       CONFIG_MAX_BIT_PER_ROW = 8;
#define CONFIG_MAX_BITMAP_ROW               4
#define CONFIG_MAX_BIT_PER_ROW              32
typedef unsigned int task_bitmap_row_t;

// for Task Stack
typedef unsigned int stack_unit_t;

// for Idle Task
#define CONFIG_IDLE_TASK_STACK_SIZE         1024

// for Task Variable
#define CONFIG_MAX_TASK_VARIABLE            32

// for Task Hook
#define CONFIG_MAX_TASK_CREATE_HOOK         8
#define CONFIG_MAX_TASK_SWITCH_HOOK         8
#define CONFIG_MAX_TASK_DELETE_HOOK         8

// for Sync Module
#define CONFIG_MAX_QUEUE                    8
#define CONFIG_MAX_MUTEX                    32
#define CONFIG_MAX_SEMAPHORE                32

// for Memory Pool
#define CONFIG_MAX_MPOOL                    2

// for Driver Management
#define CONFIG_MAX_DRIVER                   8

// for Timer Management
#define CONFIG_TICK_DURATION_IN_MSEC        10
#define CONFIG_MAX_BUCKET                   13
#define CONFIG_MAX_TIMER                    128
#define CONFIG_TIMER_TASK_STACK_SIZE        2048
#define CONFIG_TIMER_TASK_PRIORITY          8
#define CONFIG_TIMER_QUEUE_SIZE             1024
#define CONFIG_INTERRUPT_FLASH_FREQUENCY    16

#endif

