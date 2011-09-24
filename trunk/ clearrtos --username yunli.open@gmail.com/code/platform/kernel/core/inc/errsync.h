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
 *   Date: 08/14/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
 
#ifndef __ERRSYNC_H
#define __ERRSYNC_H

#include "module.h"

enum {
    // for object
    ERROR_SYNC_INIT_INVINIT = ERROR_BEGIN (MODULE_SYNC),
    ERROR_SYNC_INIT_INVCONTEXT,
    ERROR_SYNC_ALLOC_INVCONTEXT,
    ERROR_SYNC_ALLOC_NOOBJ,
    ERROR_SYNC_FREE_INVCONTEXT,
    ERROR_SYNC_FREE_INVHANDLE,
    ERROR_SYNC_FREE_DELETED,
    ERROR_SYNC_ENTER_INVHANDLE,
    ERROR_SYNC_ENTER_INVTASK,
    ERROR_SYNC_ENTER_INVCONTEXT,
    ERROR_SYNC_ENTER_TRYAGAIN,
    ERROR_SYNC_ENTER_TIMEOUT,
    ERROR_SYNC_LEAVE_INVHANDLE,
    ERROR_SYNC_LEAVE_INVTASK,

    // for mutex
    ERROR_MUTEX_INVCONTEXT,
    ERROR_MUTEX_NOTOWNER,
    
    // for event
    ERROR_EVENT_RECV_INVRECEIVER,
    ERROR_EVENT_RECV_INVPTR,
    ERROR_EVENT_RECV_INVCONTEXT,
    ERROR_EVENT_RECV_INVOPT,
    ERROR_EVENT_RECV_TIMEOUT,
    ERROR_EVENT_CLEAR_INVCONTEXT,

    // for queue
    ERROR_QUEUE_CREATE_INVCONTEXT,
    ERROR_QUEUE_CREATE_NOQUE,
    ERROR_QUEUE_DELETE_INVCONTEXT,
    ERROR_QUEUE_DELETE_INVHANDLE,
    ERROR_QUEUE_SEND_INVHANDLE,
    ERROR_QUEUE_SEND_FULL,
    ERROR_QUEUE_RECV_INVHANDLE,
    ERROR_QUEUE_EMPTY_INVHANDLE,
    ERROR_QUEUE_FULL_INVHANDLE
};

#endif

