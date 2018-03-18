// Defines Command Codes For FileSystem
// 
//
//
//
//
//

#ifndef COM_CODES
#define COM_CODES

/* Find Files By Tag */
static const int FIND_TS = 400;

/* Find Files By Name */
static const int FIND_FS = 401;

/* Create A New File From Anywhere (Must Supply File Path) */
static const int NEW_FP = 300;

/* Create 1 Or More New Tags */
static const int NEW_TS = 301;

/* Create 1 Or More New Files Within The Current Working Directory */
static const int NEW_FS = 302;

/* Delete A File (Must Supply File Path) */
static const int DEL_FP = 500;

/* Delete A Tag(s) (Must Be Empty) */
static const int DEL_TS = 501;

/* Delete A File(s) (From Current Working Directory) */
static const int DEL_FS = 502;

/* Open A File For Operations (Must Supply File Path) */
static const int OPEN_FP = 200;

/* Open A File (From Current Working Directory) */
static const int OPEN_F = 201;

/* Close A File (Must Supply File Path) */
static const int CLOSE_FP = 600;

/* Close A File (From Current Working Directory) */
static const int CLOSE_F = 601;

/* Rename File(s) (Must Supply File Path) */
static const int RNAME_FP = 100;

/* Rename Tag(s) */
static const int RNAME_TS = 101;

/* Rename File(s) (From Current Working Directory) */
static const int RNAME_FS = 102;

/* Get File Attributes (Must Supply File Path) */
static const int ATTR_FP = 700;

/* Get File Attributes (From Current Working Directory) */
static const int ATTR_FS = 701;

/* Merge One Tag Into Another */
static const int MERG_1_1 = 801;

/* Merge Many Tags Into One */
static const int MERG_M_1 = 802;

/* Tag File (Must Supply File Path) */
static const int TAG_FP = 900;

/* Tag File(s) (From Current Working Directory) */
static const int TAG_FS = 901;

/* Untag File (Must Supply File Path) */
static const int UTAG_FP = 1000;

/* Untag File(s) (From Current Working Directory) */
static const int UTAG_FS = 1001;

/* Change Directory (Absolute Path) */
static const int CD_ABS = 2222;

/* Change Directory (Relative Path) */
static const int CD_RLP = 1112;

/* Read X Bytes From File (Must Supply Path) */
static const int READ_XP = 3000;

/* Read Whole File (Must Supply Path) */
static const int READ_FP = 3300;

/* Read X Bytes From File (From Current Working Directory) */
static const int READ_XCWD = 3001;

/* Read Whole File (From Current Working Directory) */
static const int READ_FCWD = 3002;

/* Write To File (Must Supply File Path) */
static const int WRITE_FP = 4000;

/* Write X Bytes From File To File (Must Supply File Paths) */
static const int WRITE_XFPF = 4400;

/* Write To File (From Current Working Directory) */
static const int WRITE_FCWD = 4001;

/* Write X Bytes From File To File (From Current Working Directory) */
static const int WRITE_XFCWDF = 4002;

/* Copy Contents Of One File To Another (Overwrites File; Must Supply File Paths) */
static const int CPY_FP = 6000;

/* Copy Contents Of One File To Another (Overwrites File; From Current Working Directory */
static const int CPY_FCWD = 6001;

/* Quit Interface */
static const int QUIT = 999;

/* Fatal Error */
static const int FTL_ERR = 9999;

/* Handshake */
static const int HANDSHK = 0;

#endif









