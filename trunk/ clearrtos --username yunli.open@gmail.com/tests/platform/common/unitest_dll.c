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
 *   Date: 03/08/2010                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/
#include "unitest.h"
#include "dll.h"

static int KEY_HEAD = 3344;
static int KEY_MIDDLE = 5566;
static int KEY_TAIL = 7788;

typedef struct
{
    dll_node_t node_;
    int key_;
} ut_node_t;

static bool find_by_key (dll_t *_p_list, dll_node_t *_p_node, void *_p_arg)
{
    ut_node_t *p_test_node = (ut_node_t *)_p_node;
    int *p_key = (int *)_p_arg;
    
    UNUSED (_p_list);
        
    if ((*p_key) == p_test_node->key_) {
        return false;
    }
    return true;
}

void unitest_main (int argc, char *argv[])
{
    dll_t list, list_merged;
    ut_node_t node_head, node_middle, node_tail;
    
    UNUSED (argc);
    UNUSED (argv);
    
    // prepare the node structure
    node_head.node_.next_ = (dll_node_t *)0xaaaa;
    node_head.node_.prev_ = (dll_node_t *)0xbbbb;

    node_head.key_ = KEY_HEAD;
    node_middle.key_ = KEY_MIDDLE;
    node_tail.key_ = KEY_TAIL;

    // for testing dll_node_init ()
    dll_node_init (&node_head.node_);
    UNITEST_EQUALS (node_head.node_.next_, 0);
    UNITEST_EQUALS (node_head.node_.prev_, 0);

    list.count_ = 0x5a5a;
    list.head_ = (dll_node_t *)0xaaaa;
    list.tail_ = (dll_node_t *)0xbbbb;
    
    // for testing dll_size ()
    UNITEST_EQUALS (dll_size (&list), 0x5a5a);
    
    // for testing dll_head ()
    UNITEST_EQUALS (dll_head (&list), (dll_node_t *)0xaaaa);
    
    // for testing dll_tail ()
    UNITEST_EQUALS (dll_tail (&list), (dll_node_t *)0xbbbb);
    
    // for testing dll_init ()
    dll_init (&list);
    UNITEST_EQUALS (dll_size (&list), 0);
    UNITEST_EQUALS (dll_head (&list), 0);
    UNITEST_EQUALS (dll_tail (&list), 0);

    // for testing dll_push_head ()
    dll_push_head (&list, &node_head.node_);
    UNITEST_EQUALS (dll_size (&list), 1);
    UNITEST_EQUALS (dll_head (&list), &node_head.node_);
    UNITEST_EQUALS (dll_tail (&list), dll_head (&list));

    // for testing dll_push_tail ()
    dll_push_tail (&list, &node_tail.node_);
    UNITEST_EQUALS (dll_size (&list), 2);
    UNITEST_EQUALS (dll_head (&list), &node_head.node_);
    UNITEST_EQUALS (dll_tail (&list), &node_tail.node_);

    // for testing dll_next ()
    UNITEST_EQUALS (dll_next (&list, &node_head.node_), &node_tail.node_);

    // for testing dll_prev ()
    UNITEST_EQUALS (dll_prev (&list, &node_tail.node_), &node_head.node_);

    // for testing dll_insert_after ()
    dll_insert_after (&list, &node_head.node_, &node_middle.node_);
    UNITEST_EQUALS (dll_size (&list), 3);
    UNITEST_EQUALS (dll_head (&list), &node_head.node_);
    UNITEST_EQUALS (dll_tail (&list), &node_tail.node_);
    UNITEST_EQUALS (dll_next (&list, &node_head.node_), &node_middle.node_);

    // for testing dll_remove ()
    dll_remove (&list, &node_middle.node_);
    UNITEST_EQUALS (dll_size (&list), 2);
    UNITEST_EQUALS (dll_head (&list), &node_head.node_);
    UNITEST_EQUALS (dll_tail (&list), &node_tail.node_);
    UNITEST_EQUALS (dll_next (&list, &node_head.node_), &node_tail.node_);

    // for testing dll_insert_before ()
    dll_insert_before (&list, &node_tail.node_, &node_middle.node_);
    UNITEST_EQUALS (dll_size (&list), 3);
    UNITEST_EQUALS (dll_head (&list), &node_head.node_);
    UNITEST_EQUALS (dll_tail (&list), &node_tail.node_);
    UNITEST_EQUALS (dll_next (&list, &node_head.node_), &node_middle.node_);

    // for testing dll_pop_head ()
    UNITEST_EQUALS (dll_pop_head (&list), &node_head.node_);
    UNITEST_EQUALS (dll_size (&list), 2);
    UNITEST_EQUALS (dll_head (&list), &node_middle.node_);
    
    // for testing dll_pop_tail ()
    UNITEST_EQUALS (dll_pop_tail (&list), &node_tail.node_);
    UNITEST_EQUALS (dll_size (&list), 1);
    UNITEST_EQUALS (dll_tail (&list), &node_middle.node_);

    // for testing dll_traverse ()
    UNITEST_EQUALS (dll_traverse (&list, find_by_key, (void *) &KEY_HEAD), 0);
    UNITEST_EQUALS (dll_traverse (&list, find_by_key, (void *) &KEY_MIDDLE), 
        (dll_node_t *)&node_middle.node_);
    UNITEST_EQUALS (dll_traverse (&list, find_by_key, (void *) &KEY_TAIL), 0);
    UNITEST_EQUALS (dll_traverse (&list, (traverse_callback_t)0, (void *) &KEY_TAIL), 0);

    // for testing dll_merge ()
    dll_init (&list_merged);
    dll_push_head (&list_merged, &node_head.node_);
    dll_push_tail (&list_merged, &node_tail.node_);
    dll_merge (&list, &list_merged);
    UNITEST_EQUALS (dll_size (&list), 3);
    UNITEST_EQUALS (dll_head (&list), &node_middle.node_);
    UNITEST_EQUALS (dll_tail (&list), &node_tail.node_);
    UNITEST_EQUALS (dll_next (&list, &node_middle.node_), &node_head.node_);
    UNITEST_EQUALS (dll_size (&list_merged), 0);
    UNITEST_EQUALS (dll_head (&list_merged), 0);
    UNITEST_EQUALS (dll_tail (&list_merged), 0);
    dll_merge (&list_merged, &list);
    UNITEST_EQUALS (dll_size (&list_merged), 3);
    UNITEST_EQUALS (dll_head (&list_merged), &node_middle.node_);
    UNITEST_EQUALS (dll_tail (&list_merged), &node_tail.node_);
    UNITEST_EQUALS (dll_next (&list_merged, &node_middle.node_), &node_head.node_);
    //dll_merge (&list_merged, &list);
    
    // for testing dll_split ()
    dll_split (&list_merged, &list, &node_head.node_, false);
    UNITEST_EQUALS (dll_size (&list_merged), 1);
    UNITEST_EQUALS (dll_head (&list_merged), &node_middle.node_);
    UNITEST_EQUALS (dll_tail (&list_merged), &node_middle.node_);
    UNITEST_EQUALS (dll_size (&list), 2);
    UNITEST_EQUALS (dll_head (&list), &node_head.node_);
    UNITEST_EQUALS (dll_tail (&list), &node_tail.node_);
    dll_merge (&list_merged, &list);
    dll_split (&list_merged, &list, &node_head.node_, true);
    UNITEST_EQUALS (dll_size (&list_merged), 2);
    UNITEST_EQUALS (dll_head (&list_merged), &node_middle.node_);
    UNITEST_EQUALS (dll_tail (&list_merged), &node_head.node_);
    UNITEST_EQUALS (dll_size (&list), 1);
    UNITEST_EQUALS (dll_head (&list), &node_tail.node_);
    UNITEST_EQUALS (dll_tail (&list), &node_tail.node_);
}

