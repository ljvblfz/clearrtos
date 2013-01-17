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
  
  Date        Version  Name          Description
  ----------  -------  ------------  -------------------------------------------

  ----------  -------  ------------  -------------------------------------------

 ******************************************************************************/

#include "clib.h"
#include "context.h"

//lint -e123 -esym(752, _first_context_init) -esym(754, old_esp_)

#define CPU_STACK_ALIGNMENT     4
#define BOTTOM_MAGIC_NUMBER     0xDEADDEAD

#define weak_alias(name, aliasname) _weak_alias (name, aliasname)
#define _weak_alias(name, aliasname) \
    extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));

typedef struct {
    register_t ebx_, esi_, edi_, eip_;
    task_context_t *p_context_;
    register_t old_esp_;
} root_frame_t;

static void first_context_init (void (*_task_entry)(), 
    task_context_t *_p_context) __attribute__((noinline));
static void first_context_init (void (*_task_entry)(), task_context_t *_p_context)
{
    if (context_save (_p_context)) {
        _task_entry ();
    }
}

//lint -e{715, 818}
static void root_frame_init (root_frame_t *_p_frame)
{
asm volatile(
    // save old ESP into root_frame_t
    "movl %%esp, 0x14(%0)    \n"
    // switch to new stack which starts from _p_frame
    "movl %0, %%esp          \n"
    // initialize EBX
    "popl %%ebx              \n"
    // initialize ESI
    "popl %%esi              \n"
    // initialize EDI
    "popl %%edi              \n"
    // The reason why we don't want to initialize the EBP is when the code is 
    // build with -O2 option, the GCC will optimize the code to use EBP at the
    // end of first_context_init (). So if we initialize the EBP here it will 
    // cause crash.
    "call _first_context_init \n"
    "movl 8(%%esp), %%esp    \n"
    ::"r"(_p_frame):"ebx","esi","edi"
    );
}

void context_init (task_context_t *_p_context, address_t _stack_base, 
    usize_t _stack_size, address_t _task_entry)
{
#if defined(__i386__)
    address_t stack_high;
    root_frame_t *p_frame;

    stack_high = _stack_base + _stack_size;
    stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

    p_frame = (root_frame_t *)(stack_high - sizeof (root_frame_t));
    p_frame->ebx_ = (register_t) BOTTOM_MAGIC_NUMBER;
    p_frame->esi_ = (register_t) BOTTOM_MAGIC_NUMBER;
    p_frame->edi_ = (register_t) BOTTOM_MAGIC_NUMBER;
    p_frame->eip_ = (register_t) _task_entry;
    //lint -e{545}
    p_frame->p_context_ = _p_context;

    root_frame_init (p_frame);
#else
#error "Oops! Unsupported CPU!"
#endif
}

void context_switch (task_context_t *_p_current, task_context_t *_p_next)
{
    if (0 == context_save (_p_current)) {
        context_restore (_p_next, 1);
    }
}

void context_switch_in_interrupt (task_context_t *_p_current, 
    task_context_t *_p_next)
{
    UNUSED (_p_current);
    UNUSED (_p_next);
    // TBD
}

weak_alias (first_context_init, _first_context_init)

