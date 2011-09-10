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
 *   Date: 10/04/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#ifndef __DLL_H
#define __DLL_H

#include "primitive.h"

/* DLL stands for Double-Linked List */

typedef struct dll_node {
    struct dll_node *prev_;
    struct dll_node *next_;
} dll_node_t, *dll_node_handle_t;

typedef struct {
    dll_node_t *head_;
    dll_node_t *tail_;
    usize_t count_;
} dll_t, *dll_handle_t;

typedef bool (*traverse_callback_t)(dll_t *, dll_node_t *, void *);

#ifdef  __cplusplus
extern "C" {
#endif

static inline void dll_init (dll_t *_p_dll)
{
    _p_dll->head_ = _p_dll->tail_ = 0;
    _p_dll->count_ = 0;
}

static inline void dll_node_init (dll_node_t *_p_node)
{
    _p_node->next_ = _p_node->prev_ = 0;
}

static inline usize_t dll_size (const dll_t *_p_dll)
{
    return _p_dll->count_;
}

static inline dll_node_t *dll_head (const dll_t *_p_dll)
{
    return _p_dll->head_;
}

static inline dll_node_t *dll_tail (const dll_t *_p_dll)
{
    return _p_dll->tail_;
}

static inline dll_node_t *dll_next (const dll_t *_p_dll, const dll_node_t *_p_node)
{
    UNUSED (_p_dll);
    return _p_node->next_;
}

static inline dll_node_t *dll_prev (const dll_t *_p_dll, const dll_node_t *_p_node)
{
    UNUSED (_p_dll);
    return _p_node->prev_;
}

void dll_insert_before (dll_t *_p_dll, dll_node_t *_p_ref, dll_node_t *_p_inserted);
void dll_insert_after (dll_t *_p_dll, dll_node_t *_p_ref, dll_node_t *_p_inserted);
void dll_push_head (dll_t *_p_dll, dll_node_t *_p_node);
void dll_push_tail (dll_t *_p_dll, dll_node_t *_p_node);
dll_node_t *dll_pop_head (dll_t *_p_dll);
dll_node_t *dll_pop_tail (dll_t *_p_dll);
void dll_remove (dll_t *_p_dll, dll_node_t *_p_node);
dll_node_t *dll_traverse (dll_t *_p_dll, traverse_callback_t _cb, void *_p_arg);
dll_node_t *dll_rtraverse (dll_t *_p_dll, traverse_callback_t _cb, 
    void *_p_arg);
void dll_merge (dll_t *_p_to, dll_t *_p_from);
void dll_split (dll_t *_p_orig, dll_t *_p_derived, dll_node_t *_p_breakpoint, 
    bool _breakpoint_belongs_to_orig);

#ifdef __cplusplus
}
#endif

#endif

