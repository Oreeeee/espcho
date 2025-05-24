#ifndef CONFIG_H
#define CONFIG_H

/*
    This is an example config for espcho
    To use this, you need to make a copy of this file
    and name it config.h

    Make sure to configure it to your preferences
*/

#define CHO_WIFI_SSID "Cho-WiFi" /* Your WiFi name */
#define CHO_WIFI_PASS "recorderinthesandybridge" /* Your WiFi password */
#define CHO_PORT 13381 /* Port on which the TCP server will listen on */

/* Approved hardcoded credentials */
/* TODO: Store credentials in SQLite */
#define CHO_APPROVED_USER "peppy"
#define CHO_APPROVED_PASSWORD "cad6c40404b487337334537b29608ffc" // recorderinthesandybridge
#define CHO_APPROVED_USERID 2


/* Max connections on Bancho at the same time */
#define CHO_MAX_CONNECTIONS 5

/* Uncomment to disable authentication checks */
/* This also generates a random user id for users */
// #define CHO_DISABLE_AUTH

/* Uncomment to disable brownout detector */
//#define CHO_DISABLE_BROWOUT

/* Uncomment to show logs from Pinger */
//#define CHO_LOG_PINGER

#endif
