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
 *   Date: 08/10/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __SYNCOBJ_H
#define __SYNCOBJ_H

#include "dll.h"
#include "bitmap.h"
#include "error.h"

typedef struct type_sync_object sync_object_t, *sync_object_handle_t;

typedef bool (*sync_point_enter_t) (sync_object_handle_t _handle);
typedef void (*sync_point_wait_t) (sync_object_handle_t _handle);
typedef bool (*sync_point_exit_t) (sync_object_handle_t _handle, error_t *_p_ecode);
typedef void (*sync_point_wake_t) (sync_object_handle_t _handle);

typedef struct {
    sync_point_enter_t enter_;
    sync_point_wait_t wait_;
    sync_point_exit_t exit_;
    sync_point_wake_t wake_;
} sync_operation_t, *sync_operation_handle_t;

typedef struct {
    magic_number_t magic_number_;
    dll_t free_;
    dll_t used_;
    sync_operation_t opt_;
    // statistics
    statistic_t stats_noobj_;
} sync_container_t, *sync_container_handle_t;

struct type_sync_object {
    dll_node_t node_;
    magic_number_t magic_number_;
    task_bitmap_t pending_bitmap_;
    sync_container_handle_t container_;
    char name_ [NAME_MAX_LENGTH + 1];
};

#ifdef  __cplusplus
extern "C" {
#endif

error_t sync_container_init (sync_container_handle_t _handle, void *_p_objects, 
    usize_t _obj_count, usize_t _obj_size, const sync_operation_handle_t _opt);
error_t sync_object_alloc (sync_container_handle_t _container, 
    sync_object_handle_t *_p_handle, const char _name []);
error_t sync_object_free (sync_object_handle_t _handle);
error_t sync_point_try_to_enter (sync_object_handle_t _handle);
error_t sync_point_enter (sync_object_handle_t _handle, msecond_t _timeout);
error_t sync_point_exit (sync_object_handle_t _handle);

#ifdef __cplusplus
}
#endif

#endif

