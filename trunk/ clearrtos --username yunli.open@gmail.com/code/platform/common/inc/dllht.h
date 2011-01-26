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
 *   Date: 10/05/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __DLLHT_H
#define __DLLHT_H

#include "primitive.h"
#include "dll.h"

// DLLHT stands for Double-Linked List Hash Table

// callback function for calculating hash key
typedef ukey_t (*calculate_callback_t)(const void *_key, usize_t _key_length);

typedef struct {
    dll_t dll_;
} dllht_bucket_t;

typedef struct {
    calculate_callback_t key_calc_func_;
    traverse_callback_t compare_func_;
    usize_t bucket_size_in_bits_;
    usize_t bucket_size_;          // bucket_size = (1 << bucket_size_in_bits)
    dllht_bucket_t *buckets_;
} dllht_t;

#ifdef  __cplusplus
extern "C" {
#endif

void dllht_init (dllht_t *_p_dllht, dllht_bucket_t *_buckets, 
    usize_t _bucket_size, calculate_callback_t _key_calc_func, traverse_callback_t _compare_func);
void dllht_push_entry (const dllht_t *_p_dllht, const void *_key, 
    usize_t _key_length, dll_node_t *_p_node);
dll_node_t *dllht_pop_entry (const dllht_t *_p_dllht, void *_key, 
    usize_t _key_length);
void dllht_dump (dllht_t *_p_dllht);


#ifdef __cplusplus
}
#endif

#endif

