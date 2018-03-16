////////////////////////////////////////////////////////////////////////////////////////////////////
//
// cmnd_validation.cpp
// Regex for command validations
// Primary Author: Adrian Barberis
// For "Arboreal" Senior Design Project
//
//  Mon. | Feb. 5th | 2018 | 8:30 AM
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CMND_VAL_H
#define CMND_VAL_H


#include <regex>
//[================================================================================================]
//[    General Regex   ]
//[================================================================================================]
std::regex history_val ("history [0-9]+");
std::regex change_dir ("cd (/[0-9a-zA-Z_]*)+");
//[================================================================================================]
//[    Regex for "find" Commands    ]
//[================================================================================================-->]
std::regex find_tags ("find -t [\\[\\{,0-9a-zA-Z_\\]\\}]*");
std::regex find_files ("find -f \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\]");
//[================================================================================================-->]
//[    Regex for "new" Commands    ]
//[================================================================================================-->]
std::regex new_tags ("new -t \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
std::regex new_files ("new -f \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\]");
std::regex new_files_t_inc ("new ([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)? -t \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
//[================================================================================================-->]
//[    Regex for "delete" Commands  ]
//[================================================================================================-->]
std::regex del_tags ("delete -t \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
std::regex del_files ("delete -f \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\]");
std::regex del_file ("delete (/[0-9a-zA-Z_]*)*/[0-9a-zA-Z]+(\\.[a-zA-Z]+)?");
//[================================================================================================-->]
//[    Regex for "open" Command    ]
//[================================================================================================-->]
std::regex open_files ("open (/[0-9a-zA-Z_]+)+[0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)?");
//[================================================================================================-->]
//[    Regex for "close" Command   ]
//[================================================================================================-->]
std::regex close_files ("close (/[0-9a-zA-Z_]+)+[0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)?");
//[================================================================================================-->]
//[    Regex for "rename" Commands    ]
//[================================================================================================-->]
std::regex rename_tags ("rename -t \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\] -n \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
std::regex rename_files ("rename -f \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\] -n \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\]");
//[================================================================================================-->]
//[    Regex for "get attributes" Command    ]
//[================================================================================================-->]
std::regex get_attrs ("attr \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\]");
//[================================================================================================-->]
//[    Regex for "merge" Commands    ]
//[================================================================================================]
std::regex merge_1_1 ("merge [0-9a-zA-Z_]+ -> [0-9a-zA-Z_]+");
std::regex merge_m_1 ("merge \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\] -> [0-9a-zA-Z_]+[a-zA-Z_0-9]*");
//[================================================================================================-->]
//[    Regex for "tag" Commands    ]
//[================================================================================================]
std::regex tag_files ("tag \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\] -> [0-9a-zA-Z_]+");
std::regex add_tags ("tag ([0-9a-zA-Z_]+)(\\.[a-zA-Z_]+)? -> \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
//[================================================================================================-->]
/*  Check if the inputted command corresponds to a legal command and return the comand id
 *  Note that command Id's do not start at "1",
 *  This is because values 1-3 are reserved for the following (in order):
 *  - "help"
 *  - "quit"
 *  - "history"
 *
 * @ command: The user inputted command
 */
//[================================================================================================]
int check_command(std::string command)
{
    // Need to set up better error messages
    if(std::regex_match(command,find_tags)){return 4;}
    else if(std::regex_match(command,find_files)){return 5;}
    else if(std::regex_match(command,new_tags)){return 6;}
    else if(std::regex_match(command,new_files)){return 7;}
    else if(std::regex_match(command,new_files_t_inc)){return 8;}
    else if(std::regex_match(command,del_tags)){return 9;}
    else if(std::regex_match(command,del_files)){return 10;}
    else if(std::regex_match(command,del_file)){return 11;}
    else if(std::regex_match(command,open_files)){return 12;}
    else if(std::regex_match(command,close_files)){return 13;}
    else if(std::regex_match(command,rename_tags)){return 14;}
    else if(std::regex_match(command,rename_files)){return 15;}
    else if(std::regex_match(command,get_attrs)){return 16;}
    else if(std::regex_match(command,merge_1_1)){return 17;}
    else if(std::regex_match(command,merge_m_1)){return 18;}
    else if(std::regex_match(command,add_tags)){return 19;}
    else if(std::regex_match(command,tag_files)){return 20;}
    else if(std::regex_match(command,change_dir)){return 21;}
    else{return 0;}
}
//[================================================================================================]



#endif
