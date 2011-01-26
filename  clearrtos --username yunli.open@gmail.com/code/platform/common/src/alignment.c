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
 *   Date: 10/07/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#include "alignment.h"
/*
 * 1) this function will convert a value to a shift bits, for example, convert 
 *    2 to 1 and 4 to 2 and 32 to 5, etc.
 * 2) if the value is a power of 2 it returns true otherwise false.
 * 3) the value in the _p_in_bits makes sense only when function returns true.
 */
bool convert_to_shift_bits (const u32_t _value, int *_p_in_bits)
{
    unsigned int byte, in_bits = 0, num_of_bits = 8, mask_bit = 0x01;
    
    if ((0 == _value) || (!IS_ALIGNED (_value, _value))) {
        return false;
    }

    while (0 == (byte = (0xFF & _value))) {
        byte >>= 8;
        in_bits += 8;
    }
    
    while (num_of_bits > 0) {
        if ((byte & mask_bit) != 0) {
            *_p_in_bits = (int)in_bits;
            return true;
        }
        mask_bit <<= 1;
        in_bits ++;
        num_of_bits --;
    }

    return false;
}

