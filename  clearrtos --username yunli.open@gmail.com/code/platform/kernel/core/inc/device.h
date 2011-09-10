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
 *   Date: 08/27/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __DEVICE_H
#define __DEVICE_H

#include "config.h"
#include "primitive.h"
#include "dll.h"
#include "error.h"
#include "module.h"

typedef u32_t device_mode_t;
typedef u32_t open_mode_t;
typedef int control_option_t;
typedef struct type_device device_t, *device_handle_t;

typedef struct {
    error_t (*open_) (device_handle_t _handle, open_mode_t _mode);
    error_t (*close_) (device_handle_t _handle);
    error_t (*read_) (device_handle_t _handle, void *_buf, usize_t _size);
    error_t (*write_) (device_handle_t _handle, const void *_buf, usize_t _size);
    error_t (*control_) (device_handle_t _handle, control_option_t _option,
        int _int_arg, void *_ptr_arg);
} device_operation_t;

typedef struct {
    dll_t devices_;
    device_operation_t operation_;
    device_mode_t mode_;
    magic_number_t magic_number_;
    int count_opened_;
    char name_ [NAME_MAX_LENGTH*2 + 1];
} driver_t, *driver_handle_t;

struct type_device {
    dll_node_t node_;
    driver_handle_t driver_;
    magic_number_t magic_number_;
    char name_ [NAME_MAX_LENGTH + 1];
    // below variables should be initialized by each driver
    int interrupt_vector_;
    void (*interrupt_handler_) (device_handle_t _handle);
};

#ifdef  __cplusplus
extern "C"
{
#endif

error_t module_device (system_state_t _state);
error_t device_registration_main ();

error_t driver_install (const char _name [], const device_operation_t *_p_operation, 
    device_mode_t _mode);
error_t device_register (const char _name [], device_handle_t _handle);

error_t device_open (device_handle_t *_p_handle, const char _name [], 
    open_mode_t _mode);
error_t device_close (device_handle_t _handle);
error_t device_read (device_handle_t _handle, void *_buf, usize_t _size);
error_t device_write (device_handle_t _handle, const void *_buf, usize_t _size);
error_t device_control (device_handle_t _handle, control_option_t _option,
    int _int_arg, void *_ptr_arg);

#ifdef __cplusplus
}
#endif

#endif

