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
 *   Date: 01/17/2011                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __INJECTOR_H
#define __INJECTOR_H

#include "error.h"

typedef enum {
    INJECTION_POINT_TIMER_ALLOC,
    INJECTION_POINT_TIMER_FREE,
    INJECTION_POINT_TIMER_START,
        
    // !!! NOTE: please always put the INJECTION_POINT_COUNT and 
    // INJECTION_POINT_LAST at the end of this enum
    INJECTION_POINT_COUNT,
    INJECTION_POINT_LAST = (INJECTION_POINT_COUNT - 1)
} injection_point_t;

#ifdef  __cplusplus
extern "C" {
#endif

void error_inject (injection_point_t _point, error_t _error, void *_p_data);
int injected_error_get (injection_point_t _point, void **_p_data_ptr);

#ifdef __cplusplus
}
#endif

#endif

