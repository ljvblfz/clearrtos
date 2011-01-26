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
 *   Date: 11/23/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __ERRHEAP_H
#define __ERRHEAP_H

#include "module.h"

enum {
    ERROR_HEAP_INIT_INVADDR = ERROR_BEGIN (MODULE_HEAP),
    ERROR_HEAP_INIT_INVALIGN,
    ERROR_HEAP_ALLOC_NOTINIT,
    ERROR_HEAP_ALLOC_INVSIZE,
    ERROR_HEAP_ALLOC_INVLOC,
    ERROR_HEAP_ALLOC_NOMEM1,
    ERROR_HEAP_ALLOC_NOMEM2,
    ERROR_HEAP_ALLOC_INVCONTEXT,
    ERROR_HEAP_FREE_NOTINIT,
    ERROR_HEAP_FREE_INVSIZE,
    ERROR_HEAP_FREE_INVBUF,
    ERROR_HEAP_FREE_INVMBLOCK1,
    ERROR_HEAP_FREE_INVMBLOCK2,
    ERROR_HEAP_FREE_INVMBLOCK3,
    ERROR_HEAP_FREE_INVMHEAD,
    ERROR_HEAP_FREE_INVMTAIL
};

#endif

