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
std::regex list_all("lt");
//[================================================================================================-->]
std::regex usage_help ("--help");
std::regex usage_quit ("--quit");
std::regex usage_find ("--find");
std::regex usage_new ("--new");
std::regex usage_delete ("--delete");
std::regex usage_open ("--open");
std::regex usage_close ("--close");
std::regex usage_rename ("--rename");
std::regex usage_attr ("--attr");
std::regex usage_merge ("--merge");
std::regex usage_tag ("--tag");
std::regex usage_untag ("--untag");
std::regex usage_cd ("--cd");
std::regex usage_read ("--read");
std::regex usage_write ("--write");
std::regex usage_copy ("--copy");
//[================================================================================================-->]
std::regex help_1 ("-h --help");
std::regex help_2 ("-h --quit");
std::regex help_3 ("-h --find");
std::regex help_4 ("-h --new");
std::regex help_5 ("-h --delete");
std::regex help_6 ("-h --open");
std::regex help_7 ("-h --close");
std::regex help_8 ("-h --rename");
std::regex help_9 ("-h --attr");
std::regex help_10 ("-h --merge");
std::regex help_11 ("-h --tag");
std::regex help_12 ("-h --untag");
std::regex help_13 ("-h --cd");
std::regex help_14 ("-h --read");
std::regex help_15 ("-h --write");
std::regex help_16 ("-h --copy");
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
//[    Regex for "read" Commands  ]
//[================================================================================================-->]
std::regex read_x_path("read (/[0-9a-zA-Z_]+)+[0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)? -b [0-9]+");
std::regex read_x_cwd("read [0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)? -b [0-9]+");
std::regex read_path("read (/[0-9a-zA-Z_]+)+[0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)?");
std::regex read_cwd("read [0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)?");
//[================================================================================================-->]
//[    Regex for "write" Commands  ]
//[================================================================================================-->]
std::regex write_x_path("write (/[0-9a-zA-Z_]+)+[0-9a-zA-Z_]+((\\.)[a-zA-Z_]+) -b [0-9]+");
std::regex write_x_cwd("write [0-9a-zA-Z_]+((\\.)[a-zA-Z_]+)? -b [0-9]");
std::regex write_path("");
std::regex write_cwd("");
std::regex append_path("");
std::regex append_x_path("");
std::regex append_cwd("");
std::regex append_x_cwd("");
//[================================================================================================-->]
//[    Regex for "write" Commands  ]
//[================================================================================================-->]
std::regex copy_path("");
std::regex copy_cwd("");
//[================================================================================================-->]
//
//
//
//
//
//
//
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
    else if(std::regex_match(command,list_all)){return LIST_ALL;}
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
    else if(std::regex_match(command,append_path)){return APPND_FP;}
    else if(std::regex_match(command,append_x_path)){return APPND_XFPF;}
    else if(std::regex_match(command,append_cwd)){return APPND_FCWD;}
    else if(std::regex_match(command,append_x_cwd)){return APPND_XFCWDF;}
    else if(std::regex_match(command,copy_path)){return CPY_FP;}
    else if(std::regex_match(command,copy_cwd)){return CPY_FCWD;}
    else{return 0;}
}
//[================================================================================================]
int check_usage(std::string input)
{
    if(std::regex_match(input,usage_help)){return UHELP;}
    else if(std::regex_match(input,usage_quit)){return UQUIT;}
    else if(std::regex_match(input,usage_find)){return UFIND;}
    else if(std::regex_match(input,usage_new)){return UNEW;}
    else if(std::regex_match(input,usage_delete)){return UDEL;}
    else if(std::regex_match(input,usage_open)){return UOPEN;}
    else if(std::regex_match(input,usage_close)){return UCLOSE;}
    else if(std::regex_match(input,usage_rename)){return URNAME;}
    else if(std::regex_match(input,usage_attr)){return UATTR;}
    else if(std::regex_match(input,usage_merge)){return UMERG;}
    else if(std::regex_match(input,usage_tag)){return UTAG;}
    else if(std::regex_match(input,usage_untag)){return UUTAG;}
    else if(std::regex_match(input,usage_cd)){return UCD;}
    else if(std::regex_match(input,usage_read)){return UREAD;}
    else if(std::regex_match(input,usage_write)){return UWRITE;}
    else if(std::regex_match(input,usage_copy)){return UCOPY;}
    else{return 0;}
}
//[================================================================================================]
int check_help(std::string input)
{
    if(std::regex_match(input,help_1)){return UHELP;}
    else if(std::regex_match(input,help_2)){return UQUIT;}
    else if(std::regex_match(input,help_3)){return UFIND;}
    else if(std::regex_match(input,help_4)){return UNEW;}
    else if(std::regex_match(input,help_5)){return UDEL;}
    else if(std::regex_match(input,help_6)){return UOPEN;}
    else if(std::regex_match(input,help_7)){return UCLOSE;}
    else if(std::regex_match(input,help_8)){return URNAME;}
    else if(std::regex_match(input,help_9)){return UATTR;}
    else if(std::regex_match(input,help_10)){return UMERG;}
    else if(std::regex_match(input,help_11)){return UTAG;}
    else if(std::regex_match(input,help_12)){return UUTAG;}
    else if(std::regex_match(input,help_13)){return UCD;}
    else if(std::regex_match(input,help_14)){return UREAD;}
    else if(std::regex_match(input,help_15)){return UWRITE;}
    else if(std::regex_match(input,help_16)){return UCOPY;}
    else{return 0;}
}
//[================================================================================================]

#endif
