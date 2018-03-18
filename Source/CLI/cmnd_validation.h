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
#include "CommandCodes.h"
//[================================================================================================-->]
//[    General Regex   ]
//[================================================================================================-->]
std::regex history_val ("history [0-9]+");
std::regex change_dir ("cd (/[0-9a-zA-Z_]*)+");
std::regex change_dir_rl ("cd \\.(/[0-9a-zA-Z_]+)+");
//[================================================================================================-->]
//[    Regex for "find" Commands    ]
//[================================================================================================-->]
std::regex find_tags ("find -t [\\[\\{,0-9a-zA-Z_\\]\\}]*");
std::regex find_files ("find -f \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\]");
//[================================================================================================-->]
//[    Regex for "new" Commands    ]
//[================================================================================================-->]
std::regex new_tags ("new -t \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
std::regex new_files ("new -f \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\]");
std::regex new_file ("new (/[0-9a-zA-Z_]+)+[0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)?");
//[================================================================================================-->]
//[    Regex for "delete" Commands  ]
//[================================================================================================-->]
std::regex del_tags ("delete -t \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
std::regex del_files ("delete -f \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\]");
std::regex del_file ("delete (/[0-9a-zA-Z_]*)*/[0-9a-zA-Z]+(\\.[a-zA-Z]+)?");
//[================================================================================================-->]
//[    Regex for "open" Commands    ]
//[================================================================================================-->]
std::regex open_files ("open (-r|-w|-x) (/[0-9a-zA-Z_]+)+[0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)?");
std::regex open_file_cd ("open (-r|-w|-x) [0-9a-zA_Z]+(\\.[a-zA-Z]+)?");
//[================================================================================================-->]
//[    Regex for "close" Commands   ]
//[================================================================================================-->]
std::regex close_files ("close (/[0-9a-zA-Z_]+)+[0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)?");
std::regex close_file_cd ("close [0-9a-zA_Z]+(\\.[a-zA-Z]+)?");
//[================================================================================================-->]
//[    Regex for "rename" Commands    ]
//[================================================================================================-->]
std::regex rename_tags ("rename -t \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\] => \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
std::regex rename_files ("rename (/[0-9a-zA-Z_]*)*/[0-9a-zA-Z]+(\\.[a-zA-Z]+)? => [0-9a-zA-Z]+(\\.[a-zA-Z]+)?");
std::regex rename_file_cd("rename [0-9a-zA_Z]+(\\.[a-zA-Z]+)? => [0-9a-zA_Z]+(\\.[a-zA-Z]+)?");
//[================================================================================================-->]
//[    Regex for "get attributes" Commands    ]
//[================================================================================================-->]
std::regex get_attrs ("attr (/[0-9a-zA-Z_]*)*/[0-9a-zA-Z]+(\\.[a-zA-Z]+)?");
std::regex get_attr_cd ("attr [0-9a-zA_Z]+(\\.[a-zA-Z]+)?");
//[================================================================================================-->]
//[    Regex for "merge" Commands    ]
//[================================================================================================-->]
std::regex merge_1_1 ("merge [0-9a-zA-Z_]+ -> [0-9a-zA-Z_]+");
std::regex merge_m_1 ("merge \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\] -> [0-9a-zA-Z_]+[a-zA-Z_0-9]*");
//[================================================================================================-->]
//[    Regex for "tag" Commands    ]
//[================================================================================================-->]
std::regex add_tags ("tag (/[0-9a-zA-Z_]*)*/[0-9a-zA-Z]+(\\.[a-zA-Z]+)? \\+> \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
std::regex tag_files ("tag \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\] \\+> \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
//[================================================================================================-->]
//[    Regex for "untag" Commands  ]
//[================================================================================================-->]
std::regex untag_file ("untag (/[0-9a-zA-Z_]*)*/[0-9a-zA-Z]+(\\.[a-zA-Z]+)? \\-> \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
std::regex untag_files ("untag \\[([0-9a-zA-Z_]+)(\\.[a-zA-Z]+)?(,([0-9a-zA-Z_]+)(\\.[0-9a-zA-Z_]+)?)*\\] \\-> \\[([0-9a-zA-Z_]+)(,[0-9a-zA-Z_]+)*\\]");
//[================================================================================================-->]
std::regex read_x_path("");
std::regex read_x_cwd("");
std::regex read_path("");
std::regex read_cwd("");
//[================================================================================================-->]
std::regex write_x_path("");
std::regex write_x_cwd("");
std::regex write_path("");
std::regex write_cwd("");
//[================================================================================================]
std::regex copy_path("");
std::regex copy_cwd("");
//[================================================================================================]
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
    if(std::regex_match(command,find_tags)){return FIND_TS;}
    else if(std::regex_match(command,find_files)){return FIND_FS;}
    else if(std::regex_match(command,new_tags)){return NEW_TS;}
    else if(std::regex_match(command,new_files)){return NEW_FS;}
    else if(std::regex_match(command,new_file)){return NEW_FP;}
    else if(std::regex_match(command,del_tags)){return DEL_TS;}
    else if(std::regex_match(command,del_files)){return DEL_FS;}
    else if(std::regex_match(command,del_file)){return DEL_FP;}
    else if(std::regex_match(command,open_files)){return OPEN_FP;}
    else if(std::regex_match(command,open_file_cd)){return OPEN_F;}
    else if(std::regex_match(command,close_files)){return CLOSE_FP;}
    else if(std::regex_match(command,close_file_cd)){return CLOSE_F;}
    else if(std::regex_match(command,rename_tags)){return RNAME_TS;}
    else if(std::regex_match(command,rename_files)){return RNAME_FP;}
    else if(std::regex_match(command,rename_file_cd)){return RNAME_FS;}
    else if(std::regex_match(command,get_attrs)){return ATTR_FP;}
    else if(std::regex_match(command,get_attr_cd)){return ATTR_FS;}
    else if(std::regex_match(command,merge_1_1)){return MERG_1_1;}
    else if(std::regex_match(command,merge_m_1)){return MERG_M_1;}
    else if(std::regex_match(command,add_tags)){return TAG_FP;}
    else if(std::regex_match(command,tag_files)){return TAG_FS;}
    else if(std::regex_match(command,untag_file)){return UTAG_FP;}
    else if(std::regex_match(command,untag_files)){return UTAG_FS;}
    else if(std::regex_match(command,change_dir)){return CD_ABS;}
    else if(std::regex_match(command,change_dir_rl)){return CD_RLP;}
    else if(std::regex_match(command,read_x_path)){return READ_XP;}
    else if(std::regex_match(command,read_x_cwd)){return READ_XCWD;}
    else if(std::regex_match(command,read_path)){return READ_FP;}
    else if(std::regex_match(command,read_cwd)){return READ_FCWD;}
    else if(std::regex_match(command,write_path)){return WRITE_FP;}
    else if(std::regex_match(command,write_cwd)){return WRITE_FCWD;}
    else if(std::regex_match(command,write_x_path)){return WRITE_XFPF;}
    else if(std::regex_match(command,write_x_cwd)){return WRITE_XFCWDF;}
    else if(std::regex_match(command,copy_path)){return CPY_FP;}
    else if(std::regex_match(command,copy_cwd)){return CPY_FCWD;}
    else{return 0;}
}
//[================================================================================================]



#endif
