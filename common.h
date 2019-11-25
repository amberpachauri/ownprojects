
/*
* Dated : 12/July/2018 , 10:00PM
* Author  Amber Pachauri
* Purpose: This is Header file for user specific data
*    to perform operations to CURL library, request to URL to compute 
*    for diagnostics of Network activity - bandwith, lookup, connection etc.
* 
*  
* This file is implemented at user-level mostly, but contains some common definitions
*
*/



#ifndef __COMMON_H_
#define __COMMON_H_

typedef struct {

        char    url[256];
        void    *message;
        void    *error;
        int     status;
	int 	msglen;
        /* ... */ 
	double dconnect;
        double dappconnect;
        double dnmlookup;
        double dpretransfer;
        double dtransfer;
        double dredirect;
        double dspeed;
        double dsize;
        double dtime;

} st_usermessage;

typedef enum { 
	eSAMKNWSSTATUS_ERR = -1, /* signal other states*/

	eSAMKNWSSTATUS_NONE = 0, /* signals none */
	eSAMKNWSSTATUS_OK = 1, /*signals sams' exec*/ 
	eSAMKNWSSTATUS_MSG = 2, /*signal sams'  incommin */
	eSAMKNWSSTATUS_DONE = 3, /*signal sams' done */

	eSAMKNWSSTATUS_ALLOC = 4 /*signals sams' resource allocation*/ 


} esamknwsDTMAPIStatus;

#define MAX_HEADERS	(10)
#define SIZE_HEADERS	(64)
#endif
