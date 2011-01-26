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
 *   Date: 08/30/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/


#include "ioport.h"

void port_write (address_t _addr, port_type_t _type, port_size_t _size, 
    u32_t _value)
{
    UNUSED (_addr);
    UNUSED (_type);
    UNUSED (_size);
    UNUSED (_value);
}

u32_t port_read (address_t _addr, port_type_t _type, port_size_t _size)
{
    UNUSED (_addr);
    UNUSED (_type);
    UNUSED (_size);
    return 0;
}

