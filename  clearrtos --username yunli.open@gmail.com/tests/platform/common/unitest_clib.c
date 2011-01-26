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
 *   Date: 08/23/2010                                                         *
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
#include "clib.h"

void unitest_main (int argc, char *argv[])
{
    char buffer [32];
    char greeting [] = "Have a good day!\n";
    char hi [] = "Hi";

    memset (buffer, 0, sizeof (buffer));
    UNITEST_EQUALS (buffer [0], 0);
    UNITEST_EQUALS (buffer [sizeof (buffer) - 1], 0);
    
    memset (buffer, 0xFF, sizeof (buffer));
    UNITEST_EQUALS (buffer [0], (char) 0xFF);
    UNITEST_EQUALS (buffer [sizeof (buffer) - 1], (char) 0xFF);

    strncpy (buffer, greeting, sizeof (buffer));
    UNITEST_EQUALS (buffer [0], 'H');
    UNITEST_EQUALS (buffer [sizeof (greeting) - 1], 0);
    UNITEST_EQUALS (buffer [sizeof (greeting) - 2], '\n');
    
    memset (buffer, 0, sizeof (buffer));
    strncpy (buffer, greeting, 10);
    UNITEST_EQUALS (buffer [0], 'H');
    UNITEST_EQUALS (buffer [9], 'o');
    
    strncpy (buffer, hi, sizeof (buffer));
    UNITEST_EQUALS (buffer [0], 'H');
    UNITEST_EQUALS (buffer [1], 'i');
    UNITEST_EQUALS (buffer [2], 0);

    memset (buffer, 0, sizeof (buffer));
    memcpy (buffer, greeting, sizeof (greeting));
    UNITEST_EQUALS (buffer [0], 'H');
    UNITEST_EQUALS (buffer [sizeof (greeting) - 1], 0);
    UNITEST_EQUALS (buffer [sizeof (greeting) - 2], '\n');
    
    memset (buffer, 0, sizeof (buffer));
    memcpy (buffer, &greeting [1], sizeof (greeting) - 1);
    UNITEST_EQUALS (buffer [0], 'a');
    UNITEST_EQUALS (buffer [sizeof (greeting) - 2], 0);
    UNITEST_EQUALS (buffer [sizeof (greeting) - 3], '\n');
    
    memset (buffer, 0, sizeof (buffer));
    memcpy (buffer, &greeting [2], sizeof (greeting) - 2);
    UNITEST_EQUALS (buffer [0], 'v');
    UNITEST_EQUALS (buffer [sizeof (greeting) - 3], 0);
    UNITEST_EQUALS (buffer [sizeof (greeting) - 4], '\n');
    
    memset (buffer, 0, sizeof (buffer));
    memcpy (buffer, &greeting [3], sizeof (greeting) - 3);
    UNITEST_EQUALS (buffer [0], 'e');
    UNITEST_EQUALS (buffer [sizeof (greeting) - 4], 0);
    UNITEST_EQUALS (buffer [sizeof (greeting) - 5], '\n');
    
    memset (buffer, 0, sizeof (buffer));
    buffer [0] = (char) 0xFF;
    memcpy (&buffer [1], greeting, sizeof (greeting));
    UNITEST_EQUALS (buffer [0], (char) 0xFF);
    UNITEST_EQUALS (buffer [sizeof (greeting)], 0);
    UNITEST_EQUALS (buffer [sizeof (greeting) - 1], '\n');
    
    memset (buffer, 0, sizeof (buffer));
    buffer [0] = (char) 0xFF;
    buffer [sizeof (greeting) + 2] = (char) 0xFF;
    memcpy (&buffer [2], greeting, sizeof (greeting));
    UNITEST_EQUALS (buffer [0], (char) 0xFF);
    UNITEST_EQUALS (buffer [sizeof (greeting) + 2], (char) 0xFF);
    UNITEST_EQUALS (buffer [sizeof (greeting) + 1], 0);
    UNITEST_EQUALS (buffer [sizeof (greeting)], '\n');

    memset (buffer, 0, sizeof (buffer));
    buffer [0] = (char) 0xFF;
    buffer [9] = (char) 0xFF;
    memcpy (&buffer [1], greeting, 8);
    UNITEST_EQUALS (buffer [0], (char) 0xFF);
    UNITEST_EQUALS (buffer [9], (char) 0xFF);
    UNITEST_EQUALS (buffer [1], 'H');
    UNITEST_EQUALS (buffer [8], 'g');

    UNITEST_EQUALS (memcmp (greeting, greeting, sizeof (greeting)), 0);
}

