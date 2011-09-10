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
 *   Date: 12/02/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __MPOOL_H
#define __MPOOL_H

#include "config.h"
#include "primitive.h"
#include "error.h"
#include "module.h"
#include "dll.h"
#include "mutex.h"

#define MPOOL_MEMORY_DECLARE(_node_name, _buf_name, _type, _count) \
    static mpool_node_t _node_name [_count]; \
    static _type _buf_name [_count]; 

typedef struct {
    dll_node_t node_;
    address_t addr_;
    bool in_use_;
} mpool_node_t;

typedef struct {
    dll_node_t node_;
    magic_number_t magic_number_;
    char name_ [NAME_MAX_LENGTH + 1];
    address_t addr_start_;
    address_t addr_end_;
    msize_t buffer_size_;
    msize_t buffer_count_;
    int buffer_size_in_bits_;
    bool apply_shift_;
    dll_t free_buffer_;
    mpool_node_t *p_node_;
    // statistic
    statistic_t stats_nobuf_;
} mpool_t, *mpool_handle_t;

#ifdef  __cplusplus
extern "C" {
#endif
    
error_t mpool_create (const char _name [], mpool_handle_t *_p_handle, 
    void *_node, void *_buffer, msize_t _buffer_size, msize_t _buffer_count);
error_t mpool_delete (mpool_handle_t _handle);
void* mpool_buffer_alloc (mpool_handle_t _handle);
error_t mpool_buffer_free (mpool_handle_t _handle, void* _p_buf);
void mpool_dump ();
error_t module_mpool (system_state_t _state);

#ifdef __cplusplus
}
#endif

#endif

