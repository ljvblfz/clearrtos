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
 *   Date: 08/23/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#include <string.h>
#include "unitest.h"
#include "bitmap.h"

void unitest_main (int argc, char *argv[])
{
    task_bitmap_t bitmap;
    task_bitmap_t zero = {{0}};
    
    UNUSED (argc);
    UNUSED (argv);

    // verity the functionality of task_bitmap_init ()
    memset (&bitmap, 0xFF, sizeof (bitmap));
    task_bitmap_init (&bitmap);
    UNITEST_EQUALS (memcmp (&bitmap, &zero, sizeof (bitmap)), 0);

    // verity bit setting and bit clearing
    task_bitmap_bit_set (&bitmap, 0);
    UNITEST_EQUALS (task_bitmap_lowest_bit_get (&bitmap), 0);
    task_bitmap_bit_set (&bitmap, 1);
    task_bitmap_bit_clear (&bitmap, 0);
    UNITEST_EQUALS (task_bitmap_lowest_bit_get (&bitmap), 1);
    task_bitmap_bit_clear (&bitmap, 1);
    task_bitmap_bit_set (&bitmap, LAST_BIT);

    task_bitmap_bit_set (&bitmap, 9);
    UNITEST_EQUALS (task_bitmap_lowest_bit_get (&bitmap), 9);
    task_bitmap_bit_clear (&bitmap, 9);

    task_bitmap_bit_set (&bitmap, 17);
    UNITEST_EQUALS (task_bitmap_lowest_bit_get (&bitmap), 17);
    task_bitmap_bit_clear (&bitmap, 17);
    
    UNITEST_EQUALS (task_bitmap_lowest_bit_get (&bitmap), LAST_BIT);
    task_bitmap_bit_clear (&bitmap, LAST_BIT);
    UNITEST_EQUALS (task_bitmap_lowest_bit_get (&bitmap), INVALID_BIT);

    // verify task_bitmap_is_empty ()
    UNITEST_EQUALS (task_bitmap_is_empty (&bitmap), true);
}

