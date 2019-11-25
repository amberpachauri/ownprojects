
/*
* Dated : 11/July/2018 , 16:00PM
* Author  Amber Pachauri
* Purpose: This is C program module created with concept
*    of operations to CURL library requests, to URL to compute 
*    diagnostics on Network activity - bandwith, lookup, connection etc.
* 
*  
* This file is library module, interface to user-level 
* to sends' response to user program, with data and result from Network.   
*
*/


#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "libINC/common.h"
#include "samknowsDTMAPI.h"


/*****************global decalrations & variables ******************************/
int   initsmkwsDTMAPI =0;
int   stopsmkwsDTMAPI =0;
static st_samknowsDTMAPI 	*psamknowsDTMAPI = NULL;

pthread_mutex_t 	msglock;
pthread_mutex_t 	lock;

void  __samknowsDTMAPI_init(st_usermessage *usrmsg, st_samknowsDTMAPI *psamknowsDTMAPI);
void  __samknowsDTMAPI_start(int cperform); 
void  __samknowsDTMAPI_setbuffer(void *contents, size_t size, size_t nmemb);
size_t  __samknowsDTMAPI_callback(void *contents, size_t size, size_t nmemb, void *userp);

/**************** library export declarations ***********************************/
esamknwsDTMAPIStatus    init_samknowsDTMAPI (st_usermessage *usermsg, char **header);
esamknwsDTMAPIStatus	start_samknowsDTMAPI (int ccount);



/****************** library specific definitions******************************/


/*
* purpose : internal library call to return handle.
* param   : void
* return :  pointer to library instance.
*/
st_samknowsDTMAPI*  __samknowsDTMAPI_getinstance ( void ) 
{
	return psamknowsDTMAPI;
}


/*
* purpose : internal library call to initialize instance.
* param   : message - pointer to user message structure.
* 	  : psamknowsDTMAPI - pointer to library structure.
*
* return :  void.
*/
void __samknowsDTMAPI_init(st_usermessage *message, st_samknowsDTMAPI *psamknowsDTMAPI)
{

	int rescode = CURLE_OK;

	if (psamknowsDTMAPI) {
		
		fprintf(stderr,"debug_lib: library instance(%u).\n", psamknowsDTMAPI);
		strncpy(psamknowsDTMAPI->url, message->url, (strlen(message->url) +1));
		psamknowsDTMAPI->url[strlen(message->url) +1] = 0;

		fprintf(stderr,"debug_lib: library url(%s) .\n", psamknowsDTMAPI->url);
		//psamknowsDTMAPI->usermessage.esamknowsopt = usrmsg->esamknowsopt;
		psamknowsDTMAPI->curlinfo.dspeed = message->dspeed;
		psamknowsDTMAPI->curlinfo.dsize = message->dsize;
		psamknowsDTMAPI->curlinfo.dtime = message->dtime;

		curl_global_init(CURL_GLOBAL_ALL);	
		// init curl library - block level parameters	

		psamknowsDTMAPI->pcurl = curl_easy_init();
	
		/*extract user,opt params*/ 
		rescode = curl_easy_setopt(psamknowsDTMAPI->pcurl, CURLOPT_URL, psamknowsDTMAPI->url);   //set url
		rescode += curl_easy_setopt(psamknowsDTMAPI->pcurl, CURLOPT_WRITEFUNCTION, __samknowsDTMAPI_callback);   //set callback 
		//curl_easy_setopt(psamknowsDTMAPI->curl_handle, CURLOPT_WRITEDATA, (void *)&cdatastor);
		
		if ( psamknowsDTMAPI->headers ) {

			/*append headers*/
			int ilist=0;
			psamknowsDTMAPI->curlheader = NULL;
			while (psamknowsDTMAPI->headers[ilist])  {
			    psamknowsDTMAPI->curlheader = curl_slist_append (psamknowsDTMAPI->curlheader, psamknowsDTMAPI->headers[ilist]); 
			    if (psamknowsDTMAPI->curlheader) {
				rescode += curl_easy_setopt(psamknowsDTMAPI->pcurl, CURLOPT_HTTPHEADER, psamknowsDTMAPI->curlheader);
				fprintf(stderr, "debug_lib: set curl-header (%s) \n", psamknowsDTMAPI->headers[ilist]);
			    }
			   ilist++;		
			}
		}
		
		if ( CURLE_OK == rescode )	
			fprintf(stderr, "debug_lib: library initialised, status (%d)\n", rescode);
	} 
	
	return;
}


/*
* purpose : Internal library callback buffer write. Informs user with 
*           message contents, length and type.
* param   : contents 	- message to request, write from remote.
* 	  : size	- length of data received.
* 	  : nmemb	- number of elements of data received.
*
* return :  void. Informs user with message contents, length and type.
*/
void  __samknowsDTMAPI_setbuffer(void *contents, size_t size, size_t nmemb)
{

	char *buffer = (char*) malloc(size+1 * nmemb);
	memset( buffer, 0, (size*nmemb));	
	memcpy( buffer, contents, nmemb);
	buffer[nmemb] = 0;

	int len = strlen(buffer)+1;
      	//fprintf(stderr,"debug_lib: Rx'd size(%d), nelements(%d) \n", size, nmemb); 
	
	st_usermessage user = {0};	
	if (psamknowsDTMAPI) {	
		psamknowsDTMAPI->message = (void*) buffer;  // malloc'd ??
		psamknowsDTMAPI->msglen = len;

		user.message =(char*) buffer;
		user.msglen = len;
		//fprintf(stderr,"debug_lib: rx'd length: (%d)  data (%s)\n", len, (char*)psamknowsDTMAPI->message);
		
		userdata_samknowsDTMAPI (eSAMKNWSSTATUS_MSG, (void*) buffer, (void*) &user, len);
		/*send user message*/	
	}
	
	free(buffer);

	return;	
}


/*
* purpose : internal library callback .
* param   : contents 	- message to request, write from remote.
* 	  : size	- length of data received.
* 	  : nmemb	- number of elements of data received.
*
* return :  length of data received.
*/
size_t  __samknowsDTMAPI_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	pthread_mutex_lock(&msglock);	
	/* set lock to serialize buffer write*/

	__samknowsDTMAPI_setbuffer(contents, size, nmemb);

	pthread_mutex_unlock(&msglock);	
	/* set unlock */

	return (size *nmemb);
}



/*
* purpose : Internal library callback buffer write. Informs user with 
*           message contents, length and type.
* param   : contents 	- message to request, write from remote.
* 	  : size	- length of data received.
* 	  : nmemb	- number of elements of data received.
*
* return :  void. Informs user with message contents, length and type.
*/
void  __samknowsDTMAPI_start(int cperform) 
{

	if ( (initsmkwsDTMAPI)      /* set initflag */
		&& (psamknowsDTMAPI) /* assert pointer?*/	) 	{
		
		/* continnue , extract user,opt params ... */
	        fprintf(stderr,"debug_lib: using instance(%u), url (%s).\n",psamknowsDTMAPI, psamknowsDTMAPI->url);	
		curl_easy_setopt(psamknowsDTMAPI->pcurl, CURLOPT_WRITEFUNCTION, __samknowsDTMAPI_callback);   //set url
		
		/*set error-buffer*/
		//memset( (void*) psamknowsDTMAPI->curlbufferr, 0 , CURLOPT_ERRORBUFFER);
		//curl_easy_setopt(psamknowsDTMAPI->pcurl, CURLOPT_ERRORBUFFER, psamknowsDTMAPI->curlbufferr);   //set error buffer

		long response;
		int iloop = cperform+1 ;

		
		while( --iloop) { 	// number of iterations

			pthread_mutex_lock(&lock);
			/*set lock for curl request*/

			psamknowsDTMAPI->curlcode = curl_easy_perform(psamknowsDTMAPI->pcurl);

			if ( CURLE_OK == psamknowsDTMAPI->curlcode)  {	/* url was success! */
				while( psamknowsDTMAPI->curlcode != CURLM_OK)  
					psamknowsDTMAPI->curlcode = curl_easy_getinfo( psamknowsDTMAPI->pcurl, CURLINFO_RESPONSE_CODE, &response);
				psamknowsDTMAPI->curlcode = curl_easy_getinfo( psamknowsDTMAPI->pcurl, CURLINFO_RESPONSE_CODE, &response);
		
				/* collect statistics*/
				int infocode = CURLM_OK;
				CURLInfo cinfo = psamknowsDTMAPI->curlinfo;
				infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_CONNECT_TIME, &cinfo.dconnect);
				infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_APPCONNECT_TIME, &cinfo.dappconnect);
	                        infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_NAMELOOKUP_TIME, &cinfo.dnmlookup);
        	                infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_PRETRANSFER_TIME, &cinfo.dpretransfer);
                	        infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_REDIRECT_TIME, &cinfo.dredirect);
                        	infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_STARTTRANSFER_TIME, &cinfo.dtransfer);
	                        infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_SPEED_DOWNLOAD, &cinfo.dspeed);
        	                infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_SIZE_DOWNLOAD, &cinfo.dsize);
                	        infocode += curl_easy_getinfo(psamknowsDTMAPI->pcurl, CURLINFO_TOTAL_TIME, &cinfo.dtime);
		
				if ( CURLM_OK == infocode ) {	//safe to read all params

					psamknowsDTMAPI->curlinfo.dconnect = cinfo.dconnect;
					psamknowsDTMAPI->curlinfo.dappconnect = cinfo.dappconnect;
					psamknowsDTMAPI->curlinfo.dnmlookup = cinfo.dnmlookup;
					psamknowsDTMAPI->curlinfo.dpretransfer = cinfo.dpretransfer;
					psamknowsDTMAPI->curlinfo.dredirect = cinfo.dredirect;
					psamknowsDTMAPI->curlinfo.dtransfer = cinfo.dtransfer;
					psamknowsDTMAPI->curlinfo.dspeed = cinfo.dspeed;
					psamknowsDTMAPI->curlinfo.dtime = cinfo.dtime;

					psamknowDTMAPI->status = response;

					st_usermessage user = {0};	
					/* curl message done*/
					fprintf(stderr,"debug_lib: iterate(%d) infocode(%d) curlcode-%d \n",
						          	iloop, infocode,psamknowsDTMAPI->curlcode);
		
				        user.dconnect = cinfo.dconnect;
				        user.dappconnect = cinfo.dappconnect;
			        	user.dnmlookup = cinfo.dnmlookup;
				        user.dpretransfer = cinfo.dpretransfer;
				        user.dtransfer = cinfo.dtransfer; 
				        user.dredirect = cinfo.dredirect;
			        	user.dspeed = cinfo.dspeed;
				        user.dsize = cinfo.dsize;
			        	user.dtime = cinfo.dtime;
					
					user.status = response;

					userdata_samknowsDTMAPI (eSAMKNWSSTATUS_DONE, (void*) &user.message, (void*) &user, 0);
					/*send user message*/	
				}
			} else 
				fprintf(stderr,"debug_lib: url look-up.?\n");	
			
			pthread_mutex_unlock(&lock);
			/*set lock for curl request*/
		}
	}
	return;	
}


/*
* purpose : Internal library routine for allocations. 
* params  : void.
*
* return : status on library instance. success ( OK) 
*/
esamknwsDTMAPIStatus __samknowsDTMAPI_setinstance ( void ) 
{
	int status = eSAMKNWSSTATUS_OK;

	psamknowsDTMAPI = (st_samknowsDTMAPI *) malloc (sizeof(st_samknowsDTMAPI) * (sizeof(st_samknowsDTMAPI*)));
	if ( NULL == psamknowsDTMAPI )  {
		fprintf(stderr,"debug_lib: resource allocation problem\n");
		status = eSAMKNWSSTATUS_ALLOC;
	}

	return status;
}

.
/*
* purpose : Internal library routine for release allocations.
* 	    for structure, headers (if any) , global block level (curl) 	 
* params  : void.
*
* return : void. 
*/
void	__samknowsDTMAPI_release ( void ) 
{
	st_samknowsDTMAPI *psmknwsDTMAPI = NULL;
	psmknwsDTMAPI = __samknowsDTMAPI_getinstance();

	fprintf(stderr, "debug_lib: resource release ... (%u)\n", psmknwsDTMAPI);
	if ( psmknwsDTMAPI  )   {  /*sanity check for instance*/
	
		curl_easy_cleanup(psmknwsDTMAPI->pcurl);	
	
		if (psamknowsDTMAPI->headers)  
			free (psamknowsDTMAPI->headers);
		
		if (psmknwsDTMAPI->curlheader) 
			curl_slist_free_all(psmknwsDTMAPI->curlheader);

		curl_global_cleanup();
		/*release curl block*/

		free( psmknwsDTMAPI );
		/*release library instance*/
	}
	
}


/*
* purpose : Internal library routine for release allocations.
* 	    for structure, headers (if any) , global block level (curl) 	 
* params  : void.
*
* return : void. 
*/
esamknwsDTMAPIStatus __samknowsDTMAPI_alloc ( char **headers )
{
	int status = eSAMKNWSSTATUS_OK;
	if ( (psamknowsDTMAPI) ) {

		if ( headers )  {	
		
			psamknowsDTMAPI->headers = (char**) malloc (sizeof(char*) * MAX_HEADERS);
			int i=0;
			while (headers[i] != 0)  {
				psamknowsDTMAPI->headers[i] = strdup(headers[i]);
				fprintf(stderr,"debug_lib: Rx'd header (%s)\n", psamknowsDTMAPI->headers[i]);
				i++;
			}
		}
		/* allow options ... */
	}
	
	return status;
}


/*
* purpose : exported library routine to user to initialize. 
* params  : usermsg - pointer to user structure.
* 	    headers - pointer to headers in argv format.		 
*
* return : status of library , esamknwsDTMAPIStatus 
*/
esamknwsDTMAPIStatus init_samknowsDTMAPI (st_usermessage *usermsg, char **headers)
{
	int status = eSAMKNWSSTATUS_OK;

	if ( !psamknowsDTMAPI ) { 
	
		 status  = __samknowsDTMAPI_setinstance();
		 if ( ( eSAMKNWSSTATUS_OK == status)  && 
				 ( headers ) ) 	{
			status = __samknowsDTMAPI_alloc(headers);
			
			if (eSAMKNWSSTATUS_OK == status)
				__samknowsDTMAPI_init(usermsg, psamknowsDTMAPI);
		 }
		
	} else
		status = eSAMKNWSSTATUS_ERR;
	
	return	status;
}


/*
* purpose : exported routine to user to start library.
*           sets flag to control library feature.  
* params  : count   - iterations to curl requests.
*
* return : status of library , esamknwsDTMAPIStatus 
*/
esamknwsDTMAPIStatus start_samknowsDTMAPI (int count)
{
	int status = eSAMKNWSSTATUS_OK;

	if ( (count > 0) && (psamknowsDTMAPI) )   {
		fprintf(stderr,"debug_lib: start test-metrics(%u) ...\n", psamknowsDTMAPI);	
		initsmkwsDTMAPI = 1;  // allow metrics, diagnostics ? 
		__samknowsDTMAPI_start(count);
	} else
		status = eSAMKNWSSTATUS_ERR;

	return status;
}



/*
* purpose : exported routine to user to start library.
*           sets flag to control library feature.  
* params  : count   - iterations to curl requests.
*
* return : status of library , esamknwsDTMAPIStatus 
*/
void stop_samknowsDTMAPI (void)
{
	fprintf(stderr, "debug_lib: stop test-metrics(%u) ...\n", psamknowsDTMAPI );
	__samknowsDTMAPI_release ();

	return;
}





