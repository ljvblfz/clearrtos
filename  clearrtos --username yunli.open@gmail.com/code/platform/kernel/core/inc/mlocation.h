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
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __MLOCATION_H
#define __MLOCATION_H

#define LOCATIONS \
    LOCATION(EXAMPLE_MAIN_1) \
    LOCATION(EXAMPLE_MAIN_2) \
    LOCATION(EXAMPLE_MAIN_3) \
    LOCATION(EXAMPLE_MAIN_4) \

#define LOCATION(a) a,
typedef enum {
    LOCATIONS
    MLOCATION_END
} mlocation_t;
#undef LOCATION

#ifdef  __cplusplus
extern "C" {
#endif

const char *loc2str (mlocation_t _loc);

#ifdef __cplusplus
}
#endif

#endif

