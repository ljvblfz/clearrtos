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
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "device.h"
#include "errdev.h"
#include "interrupt.h"
#include "clib.h"
#include "console.h"

#define DRIVER_LAST_INDEX       (CONFIG_MAX_DRIVER - 1)
#define MAGIC_NUMBER_DRIVER     0x44524956L
#define MAGIC_NUMBER_DEVICE     0x44455649L

#define is_invalid_handler(_handler) (((_handler) == null) || \
    ((_handler)->magic_number_ != MAGIC_NUMBER_DEVICE) || \
    ((_handler)->driver_->magic_number_ != MAGIC_NUMBER_DRIVER))

typedef struct {
    statistic_t stats_invalid_index_;
    statistic_t stats_invalid_binding_;
} device_statistics_t;

static driver_t g_driver_pool [CONFIG_MAX_DRIVER];
static device_handler_t g_interrupt_map [CONFIG_MAX_INTERRUPT];
//lint -esym(728, g_statistics) -esym(551, g_statistics)
static device_statistics_t g_statistics;

static void device_interrupt_handler (int _vector)
{
    if (_vector > INTERRUPT_LAST_INDEX) {
        interrupt_level_t level = global_interrupt_disable ();
        g_statistics.stats_invalid_index_ ++;
        global_interrupt_enable (level);
        return;
    }
    if (null == g_interrupt_map [_vector]) {
        interrupt_level_t level = global_interrupt_disable ();
        g_statistics.stats_invalid_binding_ ++;
        global_interrupt_enable (level);
        return;
    }
    
    g_interrupt_map [_vector]->interrupt_handle_ (g_interrupt_map [_vector]);
}

error_t module_device (system_state_t _state)
{
    static device_handler_t console_handler;

    if (STATE_INITIALIZING == _state) {
        static device_console_t g_device_console;
        error_t ecode;
        
        device_interrupt_handler_install (device_interrupt_handler);
        
        // make the Console ready and open it as soon as possible
        ecode = console_driver_install ("/dev/uart/");
        if (ecode != 0) {
            return ecode;
        }
        ecode = console_device_register ("/dev/uart/com0", &g_device_console);
        if (ecode != 0) {
            return ecode;
        }
        ecode = device_open (&console_handler, "/dev/uart/com0", 0);
        if (ecode != 0) {
            return ecode;
        }
        console_handler_set (console_handler);
        
        (void) device_registration_main ();
    }
    else if (STATE_DESTROYING == _state) {
        for (int idx = 1; idx <= DRIVER_LAST_INDEX; idx ++) {
            if ((MAGIC_NUMBER_DRIVER != g_driver_pool [idx].magic_number_) ||
                (0 == g_driver_pool [idx].count_opened_)) {
                continue;
            }
            console_print ("Error: device(s) on driver \"%s\" is/are not closed\n", 
                g_driver_pool [idx].name_);
        }
        (void) device_close (console_handler);
    }
    return 0;
}

error_t driver_install (const char _name [], const device_operation_t *_p_operation, 
    device_mode_t _mode)
{
    int idx;
    driver_handler_t p_driver = null;
    const char *dev_root = "/dev/";

    if ((null == _name) || (strncmp (_name, dev_root, strlen (dev_root)) != 0)) {
        // a driver has no name?
        return ERROR_T (ERROR_DRIVER_INVNAME);
    }
    if (null == _p_operation || null == _p_operation->open_ ||
        null == _p_operation->close_) {
        return ERROR_T (ERROR_DRIVER_INVOPT);
    }
    // a driver only can be installed before system is UP
    if (system_state () > STATE_INITIALIZING) {
        return ERROR_T (ERROR_DRIVER_INVSTATE);
    }
    // check whether the driver is already installed
    for (idx = 0; idx <= DRIVER_LAST_INDEX; idx ++) {
        if ((MAGIC_NUMBER_DRIVER == g_driver_pool [idx].magic_number_) &&
            (0 == strncmp (g_driver_pool [idx].name_, _name, strlen (_name)))) {
            return ERROR_T (ERROR_DRIVER_INSTALLED);
        }
    }

    for (idx = 0; idx <= DRIVER_LAST_INDEX; idx ++) {
        if (g_driver_pool [idx].magic_number_ != MAGIC_NUMBER_DRIVER) {
            p_driver = &g_driver_pool [idx];
            break;
        }
    }
    if (null == p_driver) {
        return ERROR_T (ERROR_DRIVER_NODRV);
    }
    p_driver->magic_number_ = MAGIC_NUMBER_DRIVER;
    p_driver->operation_ = *_p_operation;
    p_driver->mode_ = _mode;
    strncpy (p_driver->name_, _name, sizeof (p_driver->name_) - 1);
    p_driver->name_ [sizeof (p_driver->name_) - 1] = 0;
    dll_init (&p_driver->devices_);
    return 0;
}

error_t device_register (const char _name [], device_handler_t _handler)
{
    int idx;
    driver_handler_t p_driver = null;
    const char *p_name;
    
    if (null == _name) {
        // a device has no name?
        return ERROR_T (ERROR_DEVICE_REGISTER_INVNAME);
    }
    if (null == _handler) {
        return ERROR_T (ERROR_DEVICE_REGISTER_INVHANDLER);
    }
    // a device only can be registered before system is UP
    if (system_state () > STATE_INITIALIZING) {
        return ERROR_T (ERROR_DRIVER_INVSTATE);
    }
    
    // find the driver for the device
    for (idx = 0; idx <= DRIVER_LAST_INDEX; idx ++) {
        if ((MAGIC_NUMBER_DRIVER == g_driver_pool [idx].magic_number_) &&
            (0 == strncmp (g_driver_pool [idx].name_, _name, 
            strlen (g_driver_pool [idx].name_)))) {
            p_driver = &g_driver_pool [idx];
            break;
        }
    }
    if (null == p_driver) {
        return ERROR_T (ERROR_DEVICE_REGISTER_NODRV);
    }
    if (_handler->interrupt_vector_ != INTERRUPT_NONE) {
        g_interrupt_map [_handler->interrupt_vector_] = _handler;
    }
    _handler->driver_ = p_driver;
    // strip the driver name from device name before passing it into the device
    // open () operation
    p_name = &_name [strlen (p_driver->name_)];
    strncpy (_handler->name_, p_name, sizeof (_handler->name_) - 1);
    _handler->name_ [sizeof (_handler->name_) - 1] = 0;
    _handler->magic_number_ = MAGIC_NUMBER_DEVICE;
    dll_push_tail (&p_driver->devices_, &_handler->node_);
    return 0;
}

static bool device_find (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    device_handler_t handler = (device_handler_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    // if find the device, return false to termniate traversal
    if (0 == strcmp (_p_arg, handler->name_)) {
        return false;
    }
    return true;
}

error_t device_open (device_handler_t *_p_handler, const char _name [], 
    open_mode_t _mode)
{
    int idx;
    driver_handler_t p_driver = null;
    const char *p_name;
    device_handler_t handler;
    error_t ecode;

    if (null == _name) {
        return ERROR_T (ERROR_DEVICE_OPEN_INVNAME);
    }

    // find the driver for the device
    for (idx = 0; idx <= DRIVER_LAST_INDEX; idx ++) {
        if ((MAGIC_NUMBER_DRIVER == g_driver_pool [idx].magic_number_) &&
            (0 == strncmp (g_driver_pool [idx].name_, _name, 
            strlen (g_driver_pool [idx].name_)))) {
            p_driver = &g_driver_pool [idx];
            break;
        }
    }
    if (null == p_driver) {
        return ERROR_T (ERROR_DEVICE_OPEN_NODRV);
    }
    // strip the driver name from device name before passing it into the device
    // open () operation
    p_name = &_name [strlen (p_driver->name_)];
    // find the device
    handler = (device_handler_t) dll_traverse (&p_driver->devices_, 
        device_find, (void *)p_name);
    if (0 == handler) {
        return ERROR_T (ERROR_DEVICE_OPEN_NODEV);
    }
    // we don't need to consider race condition for calling driver's open ()
    // operation, intead, the specific device driver should be responsible.
    ecode = p_driver->operation_.open_ (handler, _mode);
    if (0 == ecode) {
        interrupt_level_t level = global_interrupt_disable ();
        p_driver->count_opened_ ++;
        global_interrupt_enable (level);
        *_p_handler = handler;
    }
    return ecode; 
}

error_t device_close (device_handler_t _handler)
{
    error_t ecode;
    
    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_DEVICE_CLOSE_INVHANDLER);
    }

    ecode = _handler->driver_->operation_.close_ (_handler);
    if (0 == ecode) {
        interrupt_level_t level = global_interrupt_disable ();
        _handler->driver_->count_opened_ --;
        global_interrupt_enable (level);
    }
    return ecode;
}

error_t device_read (device_handler_t _handler, void *_buf, usize_t _size)
{
    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_DEVICE_READ_INVHANDLER);
    }

    if (null == _handler->driver_->operation_.read_) {
        return ERROR_T (ERROR_DEVICE_READ_NOTSUPPORT);
    }
    return _handler->driver_->operation_.read_ (_handler, _buf, _size);
}

error_t device_write (device_handler_t _handler, const void *_buf, usize_t _size)
{
    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_DEVICE_WRITE_INVHANDLER);
    }

    if (null == _handler->driver_->operation_.write_) {
        return ERROR_T (ERROR_DEVICE_WRITE_NOTSUPPORT);
    }
    return _handler->driver_->operation_.write_ (_handler, _buf, _size);
}

error_t device_control (device_handler_t _handler, control_option_t _option,
    int _int_arg, void *_ptr_arg)
{
    if (is_invalid_handler (_handler)) {
        return ERROR_T (ERROR_DEVICE_CONTROL_INVHANDLER);
    }

    if (null == _handler->driver_->operation_.control_) {
        return ERROR_T (ERROR_DEVICE_CONTROL_NOTSUPPORT);
    }
    return _handler->driver_->operation_.control_ (_handler, _option, _int_arg,
        _ptr_arg);
}
