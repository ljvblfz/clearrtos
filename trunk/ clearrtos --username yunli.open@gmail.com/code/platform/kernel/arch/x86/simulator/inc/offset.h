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
 *   Date: 08/27/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __OFFSET_H
#define __OFFSET_H

// stack frame offset
#define FRAME_OFFSET_PC         0
#define FRAME_OFFSET_PARAM0     4
#define FRAME_OFFSET_PARAM1     8

// offset for registers of context
#define CONTEXT_OFFSET_EBX      0
#define CONTEXT_OFFSET_ESI      4
#define CONTEXT_OFFSET_EDI      8
#define CONTEXT_OFFSET_EBP      12
#define CONTEXT_OFFSET_ESP      16
#define CONTEXT_OFFSET_EIP      20

#endif

