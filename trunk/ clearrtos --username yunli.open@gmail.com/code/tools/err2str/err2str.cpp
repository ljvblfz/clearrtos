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
 *   Date: 09/11/2009                                                         *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
  REVISION HISTORY
  ================
  
  Date     Version  Name          Description
  -------- -------  ------------  --------------------------------------------

  -------- -------  ------------  --------------------------------------------

 ******************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

//lint -e534 -e952

int main (const int argc, char * const argv[])
{
    if (1 == argc) {
        cout << "No enough parameter" << endl;
        return -1;
    }
    
    // before we parse the file, check all the file is valid
    int file_count = argc - 1;
    while (file_count > 0) {
        fstream err_file (argv[file_count], ios::in); 

        if(!err_file.is_open())
        {
            cout << "File cannot be opened (" << argv[file_count] << ")" << endl;
            return -2;
        }

        file_count --;
    }

    ostringstream stream_errstr_init;

    // parse the file one by one
    file_count = argc - 1;
    while (file_count > 0) {
        fstream err_file(argv[file_count], ios::in);
        
        string module_id;
        string module_error_id; // also for the last module_error_id
        ostringstream stream_mod_errstr;
        
        // get the line one by one
        string line;
        //lint -e{716}
        while(1) {
            getline (err_file, line);
            if (!err_file.good ())
                break;

            string::size_type begin, end, count;

            // remove comment in the line
            const string comment_delim ("//");
            begin = line.find (comment_delim);
            if (begin != string::npos) {
                line.erase (begin);
            }

            // get error definition string
            const string delims(" ,=\t\r\n/)");
            begin = line.find ("ERROR_");
            if (begin == string::npos)
                continue;

            end = line.find_first_of (delims, begin);
            if (end == string::npos)
                count = line.length () - begin;
            else
                count = end - begin;

            module_error_id = line.substr (begin, count);
            
            if (module_id.empty ()) {
                begin = line.rfind ("MODULE_");
                if (begin != string::npos) {
                    end = line.find_first_of (delims, begin);
                    if (end != string::npos)
                        module_id = line.substr (begin, end - begin);
                }
            }

            if (module_id.empty ()) {
                cout << "Cannot get module ID (" << argv[file_count] << ")" << endl;
                return -3;
            }
            
// EXAMPLE:
//    g_errstr_MODULE_MEMORY[MODULE_ERROR(ERROR_MBLOCK_INIT_INVPARAM)] = "ERROR_MBLOCK_INIT_INVPARAM";
            stream_mod_errstr << "    g_errstr_" << module_id
                << "[MODULE_ERROR(" << module_error_id
                << ")] = \"" << module_error_id << "\";" << endl;
        }

// EXAMPLE:
// #include "errmem.h"
//
// static const char *g_errstr_MODULE_MEMORY[MODULE_ERROR(ERROR_MEMORY_END) + 1];
        string::size_type pos;
        string include_file (argv[file_count]);

        pos = include_file.find_last_of ("//");
        if (pos != string::npos) {
            include_file = include_file.substr (++ pos);
        }
        
        cout << "#include \"" << include_file << "\"" << endl << endl;
        cout << "static const char *g_errstr_" << module_id 
            << "[MODULE_ERROR(" << module_error_id
            << ") + 1];" << endl;

        cout << endl;

// EXAMPLE:
// void errstr_MODULE_MEMORY_init ()
// {
       cout << "void errstr_" << module_id << "_init ()" << endl;

        cout << "{" << endl;

        cout << stream_mod_errstr.str ();
// EXAMPLE:
//     g_errstr_array[MODULE_MEMORY].available_ = 1;
//     g_errstr_array[MODULE_MEMORY].last_error_ = MODULE_ERROR(ERROR_MEMORY_END);
//     g_errstr_array[MODULE_MEMORY].error_array_ = g_errstr_MODULE_MEMORY;
//}
        cout << endl;
        cout << "    g_errstr_array[" << module_id 
            << "].available_ = 1;" << endl;

        cout << "    g_errstr_array[" << module_id 
            << "].last_error_ = MODULE_ERROR(" 
            << module_error_id << ");" << endl;

        cout << "    g_errstr_array[" << module_id 
            << "].error_array_ = g_errstr_" 
            << module_id << ";" << endl;

        cout << "}" << endl;
// EXAMPLE:
//     errstr_MODULE_MEMORY_init ();

        stream_errstr_init << "    errstr_" << module_id << "_init ();" << endl;

        file_count --;
    }
    
// EXAMPLE:
// void static errstr_init ()
// {
//     errstr_MODULE_MEMORY_init ();
// }
    cout << endl << "static void errstr_init ()" << endl;
    cout << "{" << endl;
    cout << stream_errstr_init.str ();
    cout << "}" << endl;

    return 0;
}

