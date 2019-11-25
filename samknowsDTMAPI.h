
/*
* Dated : 12/July/2018 , 16:50PM 
* Author  Amber Pachauri
* Purpose: This is Header file for library specific data
*    to perform operations with CURL-library, and request to URL 
*    for diagnostics of Network activity - bandwith, lookup, connection etc.
* 
*  
* This file is implemented to support library interface - libsmknwsDTMAPI.so
*
*/



#ifndef __SMKWSDTMAPI_H_
#define __SMKWSDTMAPI_H_

#include <curl/curl.h>
typedef enum {

        eSAMURLINFO_APPCONNECT_TIME     = CURLINFO_APPCONNECT_TIME,
        eSAMURLINFO_CONNECT_TIME        = CURLINFO_CONNECT_TIME,
        eSAMURLINFO_NAMELOKUP_TIME      = CURLINFO_NAMELOOKUP_TIME,
        eSAMURLINFO_TRANSFER_TIME       = CURLINFO_PRETRANSFER_TIME,
        eSAMURLINFO_REDIRECT_TIME       = CURLINFO_REDIRECT_TIME,
        eSAMURLINFO_START_TIME          = CURLINFO_STARTTRANSFER_TIME,
        /* ... */
        eSAMURLINFO_TOTAL_TIME          = CURLINFO_TOTAL_TIME,
        /* ... */
        eSAMURLINFO_SPEED_DOWNLOAD      = CURLINFO_SPEED_DOWNLOAD,
        eSAMURLINFO_SIZE_DOWNLOAD       = CURLINFO_SIZE_DOWNLOAD,
        /* ... */
        /* ... */

}tesamknowsUrlInfo;


typedef enum {

        eSAMURLOPT_URL = 0x0300,

        /* ... */
        /* ... */

        eSAMURLOPT_ERRBUFFER  = 0x0020,
        eSAMURLOPT_STDERR  = 0x0400,            //= CURLOPT_STDERR,

        /* ... */
        /* ... */
        eSAMURLOPT_MAX  =       0xFFFF

}tesamknowsOptNetwork;

//typedef enum { eSAMKNWSSTATUS_OK = 0, eSAMKNWSSTATUS_ERR = -1 } esamknwsDTMAPIStatus;

/* pre-defined struct header */
//
//struct CURLMsg {
//   CURLMSG msg;       /* what this message means */
//   CURL *easy_handle; /* the handle it concerns */
//   union {
//     void *whatever;    /* message-specific data */
//     CURLcode result;   /* return code for transfer */
//   } data;
// };

/*library structure internal*/
typedef struct __st_curlinfo {

        double dconnect;
        double dappconnect;
        double dnmlookup;
        double dpretransfer;
        double dtransfer;
        double dredirect;
        double dspeed;
        double dsize;
        double dtime;

        /* ... */
        /* ... */

}CURLInfo;

/*library structure internal*/
typedef struct __st_samknowsDTMAPI {

        CURL         	*pcurl;
        CURLcode     	curlcode;
        CURLMsg      	curlmessage;
        CURLInfo	curlinfo;
        void    	*message;
        void    	*error;
        int     	status;
        int     	msglen;

	char		**headers;
	struct		curl_slist	*curlheader;	
        char    	curlbufferr[CURL_ERROR_SIZE];
        char    	url[256];

} st_samknowsDTMAPI;


#endif

