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
 *   Date: 08/14/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __VARIABLE_H
#define __VARIABLE_H

#include "primitive.h"
#include "dll.h"

typedef struct {
    dll_node_t node_;
    address_t address_;
    value_t value_;
} task_variable_node_t;

#ifdef  __cplusplus
extern "C" {
#endif

error_t task_variable_add (value_t *_p_value);
error_t task_variable_remove (value_t *_p_value);

#ifdef __cplusplus
}
#endif

#endif

