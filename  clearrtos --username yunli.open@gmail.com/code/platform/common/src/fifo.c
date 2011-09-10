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
 *   Date: 07/30/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#include "fifo.h"
#include "clib.h"

void fifo_init (fifo_handle_t _handle, void *_buffer, 
    usize_t _element_size, usize_t _capacity)
{
    memset (_handle, 0, sizeof (*_handle));
    memset (_buffer, 0, _element_size * _capacity);
    _handle->capacity_ = _capacity;
    _handle->buffer_addr_start_ = _buffer;
    _handle->buffer_addr_end_ = _handle->buffer_addr_start_;
    _handle->buffer_addr_end_ += _element_size * _capacity;
    _handle->element_size_ = _element_size;
    _handle->count_ = 0;
    _handle->cursor_get_ = _buffer;
    _handle->cursor_put_ = _buffer;
}

void fifo_element_put (fifo_handle_t _handle, const void *_p_element)
{
    if (_handle->element_size_ == sizeof (int)) {
        // there is a assumption that the address of buffer_addr_start_ is
        // aligned with the size of int *
        *(int *)(void *)_handle->cursor_put_ = *(int *)(void *)_p_element;
    }
    else if (_handle->element_size_ == sizeof (short)) {
        *(short *)(void *)_handle->cursor_put_ = *(short *)(void *)_p_element;
    }
    else if (_handle->element_size_ == sizeof (char)) {
        *(char *)_handle->cursor_put_ = *(char *)_p_element;
    }
    else {
        memcpy (_handle->cursor_put_, _p_element, _handle->element_size_);
    }
    _handle->count_ ++;
    _handle->cursor_put_ += _handle->element_size_;
    if (_handle->cursor_put_ >= _handle->buffer_addr_end_) {
        _handle->cursor_put_ = _handle->buffer_addr_start_;
    }
}

void fifo_element_get (fifo_handle_t _handle, void *_p_element)
{
    void *p_element = _handle->cursor_get_;
    _handle->count_ --;
    _handle->cursor_get_ += _handle->element_size_;
    if (_handle->cursor_get_ >= _handle->buffer_addr_end_) {
        _handle->cursor_get_ = _handle->buffer_addr_start_;
    }
    if (_handle->element_size_ == sizeof (int)) {
        // there is a assumption that the address of buffer_addr_start_ is
        // aligned with the size of int *
        *(int *)_p_element = *(int *)p_element;
    }
    else if (_handle->element_size_ == sizeof (char)) {
        *(char *)_p_element = *(char *)p_element;
    }
    else {
        memcpy (_p_element, p_element, _handle->element_size_);
    }
}

