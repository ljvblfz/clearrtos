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
 *   Date: 08/25/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include "module.h"
#include "task.h"

#ifdef  __cplusplus
extern "C" {
#endif

int module_registration_entry (int argc, char *argv []);

int main (int argc, char *argv [])
{
    if (module_registration_entry (argc, argv) != 0) {
        printf ("Error: module registration failure\n");
        return -1;
    }
    printf ("\nSystem is going to be up\n");
    if (0 != system_up ()) {
        printf ("Error: system cannot be up\n");
        return -1;
    }
    multitasking_start ();
    printf ("\nSystem is going to be down\n");
    system_down ();
    
    return 0;
}

#ifdef  __cplusplus
}
#endif

#endif

