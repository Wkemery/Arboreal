////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ErrorClass.h
//  Error structure for use in exceptions
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//  
//  Mon. | Feb. 5th | 2018 | 8:30 AM
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ERR_H
#define ERR_H

#define SHM_GET_ERR 1
#define SHM_ATT_ERR 2
#define SHM_DET_ERR 3
#define SHM_RMV_ERR 4

#define SOK_CRT_ERR 5
#define SOK_BND_ERR 6

#define SOK_CLOSE_ERR 7
#define SOK_UNLNK_ERR 8
#define SOK_CNNCT_ERR 9

#define SOK_SEND_ERR 10
#define SOK_RECV_ERR 11

#define SOK_LSTN_ERR 12
#define SOK_ACPT_ERR 13
#define SOK_GTPR_ERR 14
#define FORK_FAILED 15 

struct ERR
{
    std::string where;
    std::string what;
    std::string why;

    ERR(int ver, int val, int lnum)
    {
        std::string errmsg = strerror(errno);

        if(ver == 1) // Command Line Interface
        {
            if(val == SHM_GET_ERR)
            {
                what = "[CLI|WHAT]: SHARED MEMORY CREATE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:  " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SHM_ATT_ERR)
            {
                what = "[CLI|WHAT]: SHARED MEMORY ATTACH FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:  " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SHM_DET_ERR)
            {
                what = "[CLI|WHAT]: SHARED MEMORY DETACH FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:  " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SHM_RMV_ERR)
            {
                what = "[CLI|WHAT]: SHARED MEMORY REMOVE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_CRT_ERR)
            {
                what = "[CLI|WHAT]: CLIENT SOCKET CREATE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_BND_ERR)
            {
                what = "[CLI|WHAT]: CLIENT SOCKET BIND FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_CLOSE_ERR)
            {
                what = "[CLI|WHAT]: CLIENT SOCKET CLOSE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_UNLNK_ERR)
            {
                what = "[CLI|WHAT]: CLIENT SOCKET UNLINK FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_CNNCT_ERR)
            {
                what = "[CLI|WHAT]: CLIENT SOCKET CONNECT FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_SEND_ERR)
            {
                what = "[CLI|WHAT]: CLIENT SOCKET SEND FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_RECV_ERR)
            {
                what = "[CLI|WHAT]: CLIENT SOCKET RECEIVE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == FORK_FAILED)
            {
                what = "[CLI|WHAT]: FORK FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[CLI|WHY]:   " + errmsg + "\n";
                where = "[CLI|WHERE]: cli.cpp @ Line #" + std::to_string(lnum) + "\n";
            }

        }
        else if(ver == 2) // Liaison Process
        {
            if(val == SHM_GET_ERR)
            {
                what = "[LIAISON|WHAT]: SHARED MEMORY GET FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:  " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SHM_ATT_ERR)
            {
                what = "[LIAISON|WHAT]: SHARED MEMORY ATTACH FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:  " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SHM_DET_ERR)
            {
                what = "[LIAISON|WHAT]: SHARED MEMORY DETACH FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:  " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SHM_RMV_ERR)
            {
                what = "[LIAISON|WHAT]: SHARED MEMORY REMOVE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_CRT_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET CREATE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_BND_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET BIND FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_CLOSE_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET CLOSE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_UNLNK_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET UNLINK FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_CNNCT_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET CONNECT FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_SEND_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET SEND FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_RECV_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET RECEIVE FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_LSTN_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET LISTEN FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_ACPT_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET ACCEPT FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
            else if(val == SOK_GTPR_ERR)
            {
                what = "[LIAISON|WHAT]: SERVER SOCKET GET PEER NAME FAILED - E# = " + std::to_string(errno) + "\n";
                why = "[LIAISON|WHY]:   " + errmsg + "\n";
                where = "[LIAISON|WHERE]: liason_helper.hpp @ Line #" + std::to_string(lnum) + "\n";
            }
        }
        else
        {
            what = "INVALID VERSION NUMBER\n";
            why = "NULL\n";
            where = "NULL\n";
        }
    }
};

#endif
