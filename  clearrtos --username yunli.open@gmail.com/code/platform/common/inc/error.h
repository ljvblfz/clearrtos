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
 *   Date: 08/29/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __ERROR_H
#define __ERROR_H

// since GLIB defines the error_t, to avoid conflicting we have to apply the 
// macro __error_t_defined which is defined in GLIB to determine whether the
// error_t should be defined here or not
#ifndef __error_t_defined
typedef int error_t;
#define __error_t_defined
#endif

#define MODULE_BITS     15
#define ERROR_BITS      16

// if the ERROR_T_SIZE is 4 then the ERROR_MARK should be 0x80000000
// ERROR_MARK is used to make a number as a negative integer
#define ERROR_MARK      (1 << (MODULE_BITS + ERROR_BITS))

#define ERROR_BEGIN(_module_id) ((_module_id) << ERROR_BITS)

#define ERROR_T(_module_error) /*lint -e{648} */(ERROR_MARK | (_module_error))

#define MODULE_ERROR(_error_t) /*lint -e{778} */((_error_t) & ((1 << ERROR_BITS) - 1))
#define MODULE_ID(_error_t) /*lint -e{778,648} */(module_t)(((_error_t) & ~(ERROR_MARK)) >> ERROR_BITS)

#ifdef  __cplusplus
extern "C" {
#endif

const char *errstr (error_t _error);

#ifdef  __cplusplus
}
#endif

#endif

