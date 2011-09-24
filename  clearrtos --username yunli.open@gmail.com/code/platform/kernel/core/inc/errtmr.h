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
 *   Date: 10/26/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __ERRTMR_H
#define __ERRTMR_H

#include "module.h"

enum {
    ERROR_TIMER_ALLOC_INVHANDLE = ERROR_BEGIN (MODULE_TIMER),
    ERROR_TIMER_ALLOC_INVCB,
    ERROR_TIMER_ALLOC_NOTIMER,
    ERROR_TIMER_FREE_INVHANDLE,
    ERROR_TIMER_START_INVHANDLE,
    ERROR_TIMER_START_INVSTATE,
    ERROR_TIMER_RESTART_INVHANDLE,
    ERROR_TIMER_RESTART_INVSTATE,
    ERROR_TIMER_STOP_INVHANDLE,
    ERROR_TIMER_STOP_INVSTATE,
    ERROR_TIMER_STATE_INVSTATE
};

#endif

