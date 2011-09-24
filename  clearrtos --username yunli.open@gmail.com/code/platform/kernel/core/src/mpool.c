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
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "mpool.h"
#include "errmpool.h"
#include "console.h"
#include "alignment.h"
#include "task.h"

#define MAGIC_NUMBER_MPOOL      0x504F4F4CL
#define MPOOL_LAST_INDEX        (CONFIG_MAX_MPOOL - 1)

#define is_invalid_handle(_handle) \
    ((_handle == null) || ((_handle)->magic_number_ != MAGIC_NUMBER_MPOOL))

static mpool_t g_mpool_pool [CONFIG_MAX_MPOOL];
static dll_t g_free_mpool;
static dll_t g_used_mpool;

static void mpool_init ()
{
    int idx;
    
    dll_init (&g_free_mpool);
    for (idx = 0; idx <= MPOOL_LAST_INDEX; idx ++) {
        dll_push_tail (&g_free_mpool, &g_mpool_pool [idx].node_);
    }
}

error_t mpool_create (const char _name [], mpool_handle_t *_p_handle, 
    void *_node, void *_buffer, msize_t _buffer_size, msize_t _buffer_count)
{
    static bool initialized = false;
    mpool_node_t *p_node = _node;
    address_t buffer_addr = (address_t) _buffer;
    interrupt_level_t level;
    mpool_handle_t handle;
    usize_t index;

    if (is_in_interrupt ()) {
        return ERROR_T (ERROR_MPOOL_CREATE_INVCONTEXT);
    }
    if (null == _node || null == _buffer) {
        return ERROR_T (ERROR_MPOOL_CREATE_INVPTR);
    }

    level = global_interrupt_disable ();
    if (!initialized) {
        mpool_init ();
        initialized = true;
    }
    handle = (mpool_handle_t) dll_pop_head (&g_free_mpool);
    if (null == handle) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_MPOOL_CREATE_NOPOOL);
    }
    dll_push_tail (&g_used_mpool, &handle->node_);
    global_interrupt_enable (level);
    
    dll_init (&handle->free_buffer_);
    for (index = 0; index < _buffer_count; ++ index) {
        p_node->addr_ = buffer_addr;
        p_node->in_use_ = false;
        dll_push_tail (&handle->free_buffer_, &p_node->node_);
        p_node ++;
        buffer_addr += _buffer_size;
    }
        
    if (0 == _name) {
        handle->name_ [0] = 0;
    }
    else {
        strncpy (handle->name_, _name, (usize_t)sizeof (handle->name_));
        handle->name_ [sizeof (handle->name_) - 1] = 0;
    }
    handle->addr_start_ = (address_t) _buffer;
    handle->addr_end_ =  buffer_addr;
    handle->p_node_ = _node;
    handle->buffer_size_ = _buffer_size;
    handle->buffer_count_ = _buffer_count;
    handle->apply_shift_ = convert_to_shift_bits ((u32_t) _buffer_size, 
        &handle->buffer_size_in_bits_);
    handle->magic_number_ = MAGIC_NUMBER_MPOOL;
    *_p_handle = handle;
    return 0;
}

error_t mpool_delete (mpool_handle_t _handle)
{
    interrupt_level_t level;
    bool all_freed;
    char name [NAME_MAX_LENGTH + 1];

    if (is_in_interrupt () && STATE_UP == system_state ()) {
        return ERROR_T (ERROR_MPOOL_CREATE_INVCONTEXT);
    }
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_MPOOL_DELETE_INVHANDLE);
    }
    
    _handle->magic_number_ = 0;
    all_freed = (_handle->buffer_count_ == dll_size (&_handle->free_buffer_));
    if (!all_freed) {
        strncpy (name, _handle->name_, sizeof (name));
    }
    dll_remove (&g_used_mpool, &_handle->node_);
    dll_push_tail (&g_free_mpool, &_handle->node_);
    global_interrupt_enable (level);

    if (!all_freed) {
        console_print ("Error: buffer is not freed completely before deleting"
            " mpool \"%s\"\n", name);
    }
    return 0;
}

void* mpool_buffer_alloc (mpool_handle_t _handle)
{
    interrupt_level_t level;
    mpool_node_t *p_node;
    
    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return null;
    }
    p_node = (mpool_node_t *)dll_pop_head (&_handle->free_buffer_);
    if (0 == p_node) {
        _handle->stats_nobuf_ ++;
        global_interrupt_enable (level);
        return null;
    }
    global_interrupt_enable (level);
    p_node->in_use_ = true;
    return (void *)p_node->addr_;
}

error_t mpool_buffer_free (mpool_handle_t _handle, void* _p_buf)
{
    address_t free_addr = (address_t)_p_buf;
    interrupt_level_t level;
    msize_t index;

    level = global_interrupt_disable ();
    if (is_invalid_handle (_handle)) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_MPOOL_FREE_INVHANDLE);
    }
    
    if (free_addr < _handle->addr_start_ || free_addr >= _handle->addr_end_) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_MPOOL_FREE_OUTOFRANGE);
    }
    free_addr -= _handle->addr_start_;
    if (_handle->apply_shift_ && 0 == (free_addr & (_handle->buffer_size_ - 1))) {
        index = free_addr >> _handle->buffer_size_in_bits_;
    }
    else if (0 == (free_addr % _handle->buffer_size_)) {
        index = free_addr/_handle->buffer_size_;
    }
    else {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_MPOOL_FREE_INVALIGNMENT);
    }

    if (_handle->p_node_[index].addr_ != (address_t)_p_buf) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_MPOOL_FREE_INVADDR);
    }
    if (_handle->p_node_[index].in_use_ != true) {
        global_interrupt_enable (level);
        return ERROR_T (ERROR_MPOOL_FREE_NOTINUSE);
    }
    
    _handle->p_node_[index].in_use_ = false;
    dll_push_tail (&_handle->free_buffer_, &_handle->p_node_[index].node_);
    global_interrupt_enable (level);
    
    return 0;
}

static bool mpool_dump_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    mpool_handle_t handle = (mpool_handle_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);
    
    console_print ("  Name: %s\n", handle->name_);
    console_print ("    Buffer Size: %u\n", handle->buffer_size_);
    console_print ("       Capacity: %u\n", handle->buffer_count_);
    console_print ("      Available: %u\n", dll_size (&handle->free_buffer_));
    console_print ("      No Buffer: %u\n", handle->stats_nobuf_);
    console_print ("\n");
    return true;
}

void mpool_dump ()
{
    if (is_in_interrupt ()) {
        return;
    }

    scheduler_lock ();
    console_print ("Summary\n");
    console_print ("-------\n");
    console_print ("  Supported: %u\n", CONFIG_MAX_MPOOL);
    console_print ("  Allocated: %u\n", dll_size (&g_used_mpool));
    console_print ("  .BSS Used: %d\n", ((usize_t)&g_used_mpool - 
        (usize_t)&g_mpool_pool [0]) + sizeof (g_used_mpool));
    console_print ("\n");
    console_print ("Pool Details\n");
    console_print ("------------\n");
    (void) dll_traverse (&g_used_mpool, mpool_dump_for_each, 0);
    console_print ("\n");
    scheduler_unlock ();
}

static bool mpool_check_for_each (dll_t *_p_dll, dll_node_t *_p_node, void *_p_arg)
{
    mpool_handle_t handle = (mpool_handle_t)_p_node;

    UNUSED (_p_dll);
    UNUSED (_p_arg);

    console_print ("Error: memory pool \"%s\" isn't deleted\n", handle->name_);
    return true;
}

error_t module_mpool (system_state_t _state)
{
    if (STATE_DESTROYING == _state) {
        (void) dll_traverse (&g_used_mpool, mpool_check_for_each, 0);
    }
    return 0;
}

