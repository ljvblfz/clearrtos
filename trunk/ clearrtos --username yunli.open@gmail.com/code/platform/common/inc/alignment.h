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
 *   Date: 10/04/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __ALIGNMENT_H
#define __ALIGNMENT_H

#include "primitive.h"

#define ROUND_UP(_addr, _alignment) (((_addr) + (_alignment - 1)) & ~(_alignment - 1))
#define ROUND_DOWN(_addr, _alignment)   ((_addr) & ~(_alignment - 1))
#define IS_ALIGNED(_addr, _alignment)   (0 == ((_addr) & (_alignment - 1)))

#define ALIGN(_addr, _alignment) \
    (((_addr) + ((_alignment) - 1)) & ~((_alignment) - 1))

#ifdef  __cplusplus
extern "C"
{
#endif

/*
 * 1) this function will convert a value to a shift bits format, for example, 
 *    convert 2 to 1 and 4 to 2 and 32 to 5, etc
 * 2) if the value is a power of 2 it returns true otherwise false
 * 3) the value in the _p_in_bits makes sense only when function returns true
 */
bool convert_to_shift_bits (const u32_t _value, int *_p_in_bits);

#ifdef __cplusplus
}
#endif

#endif

