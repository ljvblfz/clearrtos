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
 *   Date: 11/26/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------
                                     
  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "mlocation.h"

#define LOCATION(a) #a,
static const char *g_locations [] = {LOCATIONS};
#undef LOCATION

const char *loc2str (mlocation_t _loc)
{
    if (_loc >= MLOCATION_END) {
        return "invalid location number";
    }
    return g_locations [_loc];
}

