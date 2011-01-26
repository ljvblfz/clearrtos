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
 *   Date: 01/23/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __ERRMOD_H
#define __ERRMOD_H

#include "module.h"

enum {
    // for module management
    ERROR_MODULE_REG_INVMODULE = ERROR_BEGIN (MODULE_MODULE),
    ERROR_MODULE_REG_INVLEVEL,
    ERROR_MODULE_REG_INVCB,
    ERROR_MODULE_REGISTERED,
    ERROR_MODULE_INIT_FAILURE,
    ERROR_MODULE_UP_FAILURE,
    ERROR_MODULE_DOWN_FAILURE
};

#endif

