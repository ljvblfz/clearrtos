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

#include "error.h"
#include "injector.h"

#ifdef UNITEST

typedef struct {
    error_t error_;
    void *p_data_;
} injection_data_t;

static injection_data_t g_data_array [INJECTION_POINT_COUNT];

void error_inject (injection_point_t _point, error_t _error, void *_p_data)
{
    if (_point > INJECTION_POINT_LAST) {
        return;
    }
    
    g_data_array [_point].error_ = _error;
    g_data_array [_point].p_data_ = _p_data;
}

int injected_error_get (injection_point_t _point, void **_p_data_ptr)
{
    if (_point > INJECTION_POINT_LAST) {
        return 0;
    }

    *_p_data_ptr = g_data_array [_point].p_data_;
    return g_data_array [_point].error_;
}

#endif

