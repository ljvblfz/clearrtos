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
 
#ifndef __CPU_H
#define __CPU_H

#include "primitive.h"
#include "interrupt.h"

#ifndef __register_t_defined
typedef int register_t;
#endif

typedef struct {
    register_t ebx_;
    register_t esi_;
    register_t edi_;
    register_t ebp_;
    register_t esp_;
    register_t eip_;
} general_purpose_registers_t;

typedef struct {
    general_purpose_registers_t gpr_;
} task_context_t;

#ifdef  __cplusplus
extern "C"
{
#endif

void context_init (task_context_t *_p_context, address_t _stack_base, 
    usize_t _stack_size, address_t _task_entry);
void context_switch (task_context_t *_p_current, task_context_t *_p_next);
void context_switch_in_interrupt (task_context_t *_p_current, 
    task_context_t *_p_next);
int context_save (task_context_t *_p_context);
void context_restore (task_context_t *_p_context, int _value);

#ifdef __cplusplus
}
#endif

#endif

