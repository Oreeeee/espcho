#ifndef BANCHOPACKETS_H
#define BANCHOPACKETS_H


// PLEASE READ THE b338 ENUM ORDER PLZZZZZZZZZZZ
// Don't let the UserStats incident happen again
#define CHOPKT_CHANGE_STATUS 0
#define CHOPKT_CLIENT_SEND_MESSAGE 1
#define CHOPKT_CLIENT_EXIT 2
#define CHOPKT_REQUEST_STATUS 3
#define CHOPKT_PONG 4
#define CHOPKT_LOGINREPLY 5
#define CHOPKT_SERVER_SEND_MESSAGE 7
#define CHOPKT_PING 8
#define CHOPKT_USER_STATS 12
#define CHOPKT_CLIENT_MESSAGE_PRIVATE 26
#define CHOPKT_CHANNEL_JOIN 64
#define CHOPKT_CHANNEL_JOIN_SUCCESS 65
#define CHOPKT_CHANNEL_AVAILABLE 66
#define CHOPKT_CHANNEL_AVAILABLE_AUTOJOIN 68
#define CHOPKT_USER_PERMISSIONS 72
#define CHOPKT_RECEIVE_UPDATES 80

#endif
