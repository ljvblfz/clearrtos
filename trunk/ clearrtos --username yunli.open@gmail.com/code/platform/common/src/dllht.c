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
 
#include "dllht.h"

// DLLHT stands for Double-Linked List Hash Table

void dllht_init (dllht_t *_p_dllht, dllht_bucket_t *_buckets, 
    usize_t _bucket_size, calculate_callback_t _key_calc_func, traverse_callback_t _compare_func)
{
    usize_t index;
    
    for (index = 0; index < _bucket_size; ++ index) {
        dll_init (&_buckets[index].dll_);
    }
    
    _p_dllht->buckets_ = _buckets;
    _p_dllht->key_calc_func_ = _key_calc_func;
    _p_dllht->compare_func_ = _compare_func;
}

void dllht_push_entry (const dllht_t *_p_dllht, const void *_key, usize_t _key_length, 
    dll_node_t *_p_node)
{
    usize_t index = _p_dllht->key_calc_func_ (_key, _key_length);
    
    dll_push_tail (&_p_dllht->buckets_[index].dll_, _p_node);
}

dll_node_t *dllht_pop_entry (const dllht_t *_p_dllht, void *_key, usize_t _key_length)
{
    usize_t index = _p_dllht->key_calc_func_ (_key, _key_length);

    dll_node_t *p_node = dll_traverse (&_p_dllht->buckets_[index].dll_, 
        _p_dllht->compare_func_, _key);

    if (0 != p_node)
        dll_remove (&_p_dllht->buckets_[index].dll_, p_node);
    
    return p_node;
}

void dllht_dump (dllht_t *_p_dllht)
{
    UNUSED (_p_dllht);
}

