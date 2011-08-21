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
 
#include "dll.h"

/* DLL stands for Double-Linked List */

void dll_insert_before (dll_t *_p_dll, dll_node_t *_p_ref, dll_node_t *_p_inserted)
{
    if (0 == _p_ref->prev_) {
        _p_dll->head_ = _p_inserted;
        _p_ref->prev_ = _p_inserted;
        _p_inserted->next_ = _p_ref;
        _p_inserted->prev_ = 0;
    }
    else {
        _p_ref->prev_->next_ = _p_inserted;
        _p_inserted->prev_ = _p_ref->prev_;
        _p_inserted->next_ = _p_ref;
        _p_ref->prev_ = _p_inserted;
    }
    _p_dll->count_++;
}

void dll_insert_after (dll_t *_p_dll, dll_node_t *_p_ref, dll_node_t *_p_inserted)
{
    if (0 == _p_ref->next_) {
        _p_dll->tail_ = _p_inserted;
        _p_ref->next_ = _p_inserted;
        _p_inserted->next_ = 0;
        _p_inserted->prev_ = _p_ref;
    }
    else {
        _p_ref->next_->prev_ = _p_inserted;
        _p_inserted->prev_ = _p_ref;
        _p_inserted->next_ = _p_ref->next_;
        _p_ref->next_ = _p_inserted;
        
    }
    _p_dll->count_++;
}

void dll_push_head (dll_t *_p_dll, dll_node_t *_p_node)
{
    if (0 == _p_dll->head_) {
        _p_dll->head_ = _p_dll->tail_ = _p_node;
        _p_node->next_ = _p_node->prev_ = 0;
    }
    else {
        _p_node->next_ = _p_dll->head_;
        _p_node->prev_ = 0;
        _p_dll->head_->prev_ = _p_node;
        _p_dll->head_ = _p_node;
    }

    _p_dll->count_ ++;
}

void dll_push_tail (dll_t *_p_dll, dll_node_t *_p_node)
{
    if (0 == _p_dll->tail_) {
        _p_dll->head_ = _p_dll->tail_ = _p_node;
        _p_node->next_ = _p_node->prev_ = 0;
    }
    else {
        dll_node_t *p_tail = _p_dll->tail_;
        
        p_tail->next_ = _p_node;
        _p_node->prev_ = p_tail;
        _p_node->next_ = 0;
        _p_dll->tail_ = _p_node;
    }

    _p_dll->count_ ++;
}

dll_node_t *dll_pop_head (dll_t *_p_dll)
{
    dll_node_t *p_node = _p_dll->head_;

    if (0 == p_node) {
        return 0;
    }
    
    _p_dll->count_--;
    _p_dll->head_ = p_node->next_;
    (0 == _p_dll->head_) ?
        (_p_dll->tail_ = 0) : (p_node->next_->prev_ = 0);
    p_node->next_ = p_node->prev_ = 0;
    return p_node;
}

dll_node_t *dll_pop_tail (dll_t *_p_dll)
{
    dll_node_t *p_node = _p_dll->tail_;

    if (0 == p_node) {
        return 0;
    }
    
    _p_dll->count_ --;
    _p_dll->tail_ = p_node->prev_;
    (0 == _p_dll->tail_) ?
        (_p_dll->head_ = 0) : (p_node->prev_->next_ = 0);
    p_node->next_ = p_node->prev_ = 0;
    return p_node;
}

void dll_remove (dll_t *_p_dll, dll_node_t *_p_node)
{
    (0 == _p_node->prev_) ? 
        (_p_dll->head_ = _p_node->next_) : (_p_node->prev_->next_ = _p_node->next_);
    (0 == _p_node->next_) ?
        (_p_dll->tail_ = _p_node->prev_) : (_p_node->next_->prev_ = _p_node->prev_);
    _p_node->prev_ = _p_node->next_ = 0;
    _p_dll->count_--;
}

dll_node_t *dll_traverse (dll_t *_p_dll, traverse_callback_t _cb, void *_p_arg)
{
    register dll_node_t *p_node = _p_dll->head_;

    if (null == _cb) {
        return 0;
    }
    
    while ((0 != p_node) && ((*_cb) (_p_dll, p_node, _p_arg))) {
        p_node = p_node->next_;
    }

    return p_node;
}

dll_node_t *dll_rtraverse (dll_t *_p_dll, traverse_callback_t _cb, 
    void *_p_arg)
{
    register dll_node_t *p_node = _p_dll->tail_;

    if (null == _cb) {
        return 0;
    }
    
    while ((0 != p_node) && ((*_cb) (_p_dll, p_node, _p_arg))) {
        p_node = p_node->prev_;
    }

    return p_node;
}

void dll_merge (dll_t *_p_to, dll_t *_p_from)
{
    if (0 == _p_to->count_) {
        *_p_to = *_p_from;
    }
    else if (0 != _p_from->count_) {
        _p_from->head_->prev_ = _p_to->tail_;
        _p_to->tail_->next_ = _p_from->head_;
        _p_to->tail_ = _p_from->tail_;
        _p_to->count_ += _p_from->count_;
    }
    dll_init (_p_from);
}

void dll_split (dll_t *_p_orig, dll_t *_p_derived, dll_node_t *_p_breakpoint, 
    bool _breakpoint_belongs_to_orig)
{
    dll_node_t *p_node;
    
    if (_breakpoint_belongs_to_orig) {
        if (0 == _p_breakpoint->next_) {
            dll_init (_p_derived);
            return;
        }
        *_p_derived = *_p_orig;
        _p_orig->tail_ = _p_breakpoint;
        _p_derived->head_ = _p_breakpoint->next_;
        _p_breakpoint->next_->prev_ = 0;
        _p_breakpoint->next_ = 0;
    }
    else {
        if (0 == _p_breakpoint->prev_) {
            *_p_derived = *_p_orig;
            dll_init (_p_orig);
            return;
        }
        *_p_derived = *_p_orig;
        _p_orig->tail_ = _p_breakpoint->prev_;
        _p_derived->head_ = _p_breakpoint;
        _p_breakpoint->prev_->next_ = 0;
        _p_breakpoint->prev_ = 0;
    }

    _p_orig->count_ = 0;
    p_node = _p_orig->head_;
    while (p_node != 0) {
        _p_orig->count_ ++;
        p_node = p_node->next_;
    }

    _p_derived->count_ -= _p_orig->count_;
}

