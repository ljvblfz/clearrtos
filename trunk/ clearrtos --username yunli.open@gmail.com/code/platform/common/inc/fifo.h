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

#ifndef __FIFO_H
#define __FIFO_H

#include "primitive.h"

// FIFO stands for First In First Out

#define FIFO_BUFFER_DECLARE(_name, _element_size, _capacity) \
    static byte_t _name [_element_size * _capacity]

typedef struct {
    // how many elements the FIFO can contain
    usize_t capacity_;
    // buffer for containing elements
    byte_t *buffer_addr_start_;
    byte_t *buffer_addr_end_;
    // element size
    usize_t element_size_;
    // how many elements are put into the FIFO
    usize_t count_;
    // position for getting next element
    byte_t *cursor_get_;
    // position for putting element
    byte_t *cursor_put_;
} fifo_t, *fifo_handler_t;

#ifdef  __cplusplus
extern "C" {
#endif

void fifo_init (fifo_handler_t _handler, void *_buffer, 
    usize_t _element_size, usize_t _capacity);
void fifo_element_put (fifo_handler_t _handler, const void *_p_element);
void fifo_element_get (fifo_handler_t _handler, void *_p_element);

//lint -e818

static inline bool fifo_is_empty (const fifo_handler_t _handler)
{
    return (bool) (0 == _handler->count_);
}

static inline bool fifo_is_full (const fifo_handler_t _handler)
{
    return (bool) (_handler->count_ == _handler->capacity_);
}

static inline usize_t fifo_capacity (const fifo_handler_t _handler)
{
    return _handler->capacity_;
}

static inline usize_t fifo_count (const fifo_handler_t _handler)
{
    return _handler->count_;
}

static inline usize_t fifo_element_size (const fifo_handler_t _handler)
{
    return _handler->element_size_;
}

//lint +e818

#ifdef __cplusplus
}
#endif

#endif

