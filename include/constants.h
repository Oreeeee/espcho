#ifndef CONSTANTS_H
#define CONSTANTS_H

#define BUFFER_SIZE 1024 /* Default size of buffer data */

#define CHO_MAX_LOGIN_STR 36 /* Max size for C2S login packet */

#define LOGIN_WRONG_PASS -1
#define LOGIN_NEED_UPDATE -2
#define LOGIN_BANNED -3

/* UserStats completness enum */
#define CHO_STATS_STATUS 0
#define CHO_STATS_STATISTICS 1
#define CHO_STATS_FULL 2

/* Presence filter */
#define CHO_PRESENCE_FILTER_NOPLAYERS 0
#define CHO_PRESENCE_FILTER_ALL 1
#define CHO_PRESENCE_FILTER_FRIENDS 2

#endif
