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

#include "offset.h"

    .globl _context_restore
    .globl context_restore
    .align 4

_context_restore:
context_restore:
    // get _p_context of context_restore ()
    movl FRAME_OFFSET_PARAM0(%esp), %ecx
    
    // take 2nd parameter of context_restore () as return value of context_save ()
    movl FRAME_OFFSET_PARAM1(%esp), %eax
    
    // restore registers from context
    movl (CONTEXT_OFFSET_EIP)(%ecx), %edx
    movl (CONTEXT_OFFSET_EBX)(%ecx), %ebx
    movl (CONTEXT_OFFSET_ESI)(%ecx), %esi
    movl (CONTEXT_OFFSET_EDI)(%ecx), %edi
    movl (CONTEXT_OFFSET_EBP)(%ecx), %ebp
    movl (CONTEXT_OFFSET_ESP)(%ecx), %esp
    
    // jump to context_save ()
    jmp *%edx

