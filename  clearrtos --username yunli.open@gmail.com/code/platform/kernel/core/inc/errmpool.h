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
 *   Date: 11/24/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __ERRMPOOL_H
#define __ERRMPOOL_H

#include "module.h"

enum {
    ERROR_MPOOL_CREATE_INVCONTEXT = ERROR_BEGIN (MODULE_MPOOL),
    ERROR_MPOOL_CREATE_INVPTR,
    ERROR_MPOOL_CREATE_NOPOOL,
    ERROR_MPOOL_DELETE_INVHANDLE,
    ERROR_MPOOL_ALLOC_INVHANDLE,
    ERROR_MPOOL_FREE_INVHANDLE,
    ERROR_MPOOL_FREE_OUTOFRANGE,
    ERROR_MPOOL_FREE_INVALIGNMENT,
    ERROR_MPOOL_FREE_INVADDR,
    ERROR_MPOOL_FREE_NOTINUSE
};

#endif

