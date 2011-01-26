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
 *   Date: 08/03/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#include "clib.h"
#include "bitmap.h"
#include "alignment.h"

static int g_bits_per_row;

#if CONFIG_MAX_BITMAP_ROW > 32
#error "CONFIG_MAX_BITMAP_ROW should be defined less than or equal to 32"
#endif

void task_bitmap_init (task_bitmap_handler_t _handler)
{
    _handler->row_bitmap_ = 0;
    memset (_handler->buffer_, 0, sizeof (_handler->buffer_));
    (void) convert_to_shift_bits (CONFIG_MAX_BIT_PER_ROW, &g_bits_per_row);
}

void task_bitmap_bit_set (task_bitmap_handler_t _handler, bit_t _bit)
{
    bit_t row = _bit >> g_bits_per_row;
    bit_t bit = (bit_t)1 << (_bit & (CONFIG_MAX_BIT_PER_ROW - 1));

    _handler->buffer_ [row] |= bit;
    _handler->row_bitmap_ |= ((bit_t)1 << row);
}

void task_bitmap_bit_clear (task_bitmap_handler_t _handler, bit_t _bit)
{
    bit_t row = _bit >> g_bits_per_row;
    bit_t bit = (bit_t)1 << (_bit & (CONFIG_MAX_BIT_PER_ROW - 1));

    _handler->buffer_ [row] &= ~bit;
    if (0 == _handler->buffer_ [row]) {
        _handler->row_bitmap_ &= ~((bit_t)1 << row);
    }
}

static unsigned char g_bitmap_table [] = {
    0xFF, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

static inline bit_t bitmap_to_bit (u32_t _bitmap)
{
    int bitmap_byte;

    if (0 == _bitmap) {
        return INVALID_BIT;
    }
    
    bitmap_byte = _bitmap & 0xFF;
    if (0 != bitmap_byte) {
        return g_bitmap_table [bitmap_byte];
    }
    
    bitmap_byte = (_bitmap & 0xFF00) >> 8;
    if (0 != bitmap_byte) {
        return g_bitmap_table [bitmap_byte] + 8;
    }
    
    bitmap_byte = (_bitmap & 0xFF0000) >> 16;
    if (0 != bitmap_byte) {
        return g_bitmap_table [bitmap_byte] + 16;
    }
    
    bitmap_byte = (_bitmap & 0xFF000000) >> 24;
    return g_bitmap_table [bitmap_byte] + 24;
}

//lint -e{818}
bit_t task_bitmap_lowest_bit_get (const task_bitmap_handler_t _handler)
{
    bit_t row, bit;

    row = bitmap_to_bit (_handler->row_bitmap_);
    if (INVALID_BIT == row) {
        return row;
    }

    bit = bitmap_to_bit (_handler->buffer_[row]);
    bit += row << g_bits_per_row;
    return bit;
}

//lint -e{818}
bool task_bitmap_is_empty (const task_bitmap_handler_t _handler)
{
    return (bool)(0 == _handler->row_bitmap_);
}

