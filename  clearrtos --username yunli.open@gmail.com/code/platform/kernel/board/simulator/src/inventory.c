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
 *   Date: 09/03/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "device.h"
#include "clock.h"
#include "console.h"
#include "ctrlc.h"
#if 0
#include "flash.h"
#include "39vf040.h"
#endif
#include "heap.h"

//lint -e718 -e746 -e530

static device_clock_t g_device_tick;
static device_ctrlc_t g_device_ctrlc;
#if 0
static device_nor_flash_t g_device_39vf040;
#endif

error_t device_registration_main ()
{
    error_t ecode;

    // 1) for Tick
    ecode = clock_driver_install ("/dev/clock/");
    if (ecode != 0) {
        return ecode;
    }
    ecode = clock_device_register ("/dev/clock/tick", &g_device_tick);
    if (ecode != 0) {
        return ecode;
    }

    // 2) for Ctrl+C
    ecode = ctrlc_driver_install ("/dev/ui/");
    if (ecode != 0) {
        return ecode;
    }
    ecode = ctrlc_device_register ("/dev/ui/ctrlc", &g_device_ctrlc);
    if (ecode != 0) {
        return ecode;
    }

#if 0
    // 3) for 39VF040 NOR flash, the capacity of it is 512K bytes
    ecode = flash_39vf040_driver_install ("/dev/flash/nor/");
    if (ecode != 0) {
        return ecode;
    }
    ecode = nor_flash_register ("/dev/flash/nor/39vf040", &g_device_39vf040, 0x400000,
        4096, 12, 128);
    if (ecode != 0) {
        return ecode;
    }
#endif
    return 0;
}

#define SYSTEM_MEM_SIZE         (16*1024*1024)
#define SYSTEM_ALIGNMENT_BITS   3

void heap_info_get (heap_info_t *_p_info)
{
    void* p_heap = malloc (SYSTEM_MEM_SIZE);
    if (0 == p_heap) {
        console_print ("Error: cannot malloc for heap\n");
        exit (-1);
    }

    _p_info->start_ = (address_t) p_heap;
    _p_info->end_ = ((address_t)p_heap) + SYSTEM_MEM_SIZE;
    _p_info->alignment_in_bits_ = SYSTEM_ALIGNMENT_BITS;
}

