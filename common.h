#ifndef COMMON_H
#define COMMON_H

//defined in main.cpp
extern char szApplicationDir[];
extern char szAppCacheDir[];

#define FOLDER_DICT     "dict"

#define ORG_NAME    "Wang Soft"
#define APP_NAME    "EveryDaySpellingBee"

#define MODE_NA                     0
#define MODE_PRACTICE               1
#define MODE_QUIZ                   2
#define MODE_PLACE                  3
#define MODE_LEARNING               4

#define RC_UNKNOWN          0
#define RC_FINISHED_ALL     1
#define RC_ERR              2
#define RC_OK               3

#define RC_CORRECT          100
#define RC_FAILED           101
#define RC_HELP             102
#define RC_SKIP             103
#define RC_RETRY            104

#endif // COMMON_H
