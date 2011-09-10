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
 *   Date: 08/22/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __QUEUE_H
#define __QUEUE_H

#include "semaphore.h"
#include "fifo.h"
#include "dll.h"

#define QUEUE_BUFFER_DECLARE(_name, _element_size, _capacity) \
    static byte_t _name [_element_size * _capacity]

typedef struct {
    dll_node_t node_;
    semaphore_handle_t semaphore_;
    magic_number_t magic_number_;
    fifo_t fifo_;
    char name_ [NAME_MAX_LENGTH + 1];
} queue_t, *queue_handle_t;

#ifdef  __cplusplus
extern "C" {
#endif

error_t module_queue (system_state_t _state);

error_t queue_create (const char _name [], queue_handle_t *_p_handle, 
    void *_buffer, usize_t _element_size, usize_t _capacity);
error_t queue_delete (queue_handle_t _handle);
error_t queue_message_send (queue_handle_t _handle, const void *_p_element);
error_t queue_message_receive (queue_handle_t _handle, msecond_t _timeout, 
    void *_p_element);
bool queue_is_empty (const queue_handle_t _handle);
bool queue_is_full (const queue_handle_t _handle);
void queue_dump ();

#ifdef __cplusplus
}
#endif

#endif

