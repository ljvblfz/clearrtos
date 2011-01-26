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
 *   Date: 03/08/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
#include "unitest.h"
#include "dllht.h"

static ukey_t mem_hash_key_calc (const void *_key, msize_t _key_length)
{
    return 0;
}

static bool mem_compare_addr_equal (dll_t *_p_dll, dll_node_t *_p_node, void *_p_key)
{
    return true;
}

void unitest_main (int argc, char *argv[])
{
    dllht_t list;
    dllht_bucket_t buckets [4];
    
    UNUSED (argc);
    UNUSED (argv);

    
    dllht_init (&list, buckets, 4,
        mem_hash_key_calc, mem_compare_addr_equal);
    UNITEST_EQUALS (dllht_pop_entry (&list, (void *)33, 4), 0);
}

