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
 *   Date: 10/01/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __CLIB_H
#define __CLIB_H

#include "primitive.h"

#ifdef  __cplusplus
extern "C"
{
#endif

char *strncpy (char *_s1, const char *_s2, usize_t _n);
void *memset (void *_dstpp, const int _c, usize_t _len);
void *memcpy (void *_dstpp, const void *_srcpp, usize_t _len);
int memcmp (void *_s1, void *_s2, usize_t _len);
int strncmp (const char *_s1, const char *_s2, usize_t _n);
usize_t strlen (const char *_str);
int strcmp (const char *_p1, const char *_p2);

#ifdef __cplusplus
}
#endif

#endif

