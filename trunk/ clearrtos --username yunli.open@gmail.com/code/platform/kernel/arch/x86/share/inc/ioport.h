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
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __IOPORT_H
#define __IOPORT_H

#include "primitive.h"

typedef enum {
    // I/O space is standalone
    PORT_TYPE_IO,
    // I/O space is the same with memory space
    PORT_TYPE_MEM
} port_type_t;

typedef enum {
    PORT_SIZE_BIT8,
    PORT_SIZE_BIT16,
    PORT_SIZE_BIT32
} port_size_t;

#ifdef  __cplusplus
extern "C"
{
#endif

void port_write (address_t _addr, port_type_t _type, port_size_t _size, u32_t _value);
u32_t port_read (address_t _addr, port_type_t _type, port_size_t _size);

#ifdef __cplusplus
}
#endif

#endif

