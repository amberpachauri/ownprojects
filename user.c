
/*
* Dated : 11/July/2018 , 16:50PM
* Author  Amber Pachauri
* Purpose: This is C program utility created with concept
*    of operations to CURL library request to URL to compute 
*    for diagnostics of Network activity - bandwith, lookup, connection etc.
* 
*  
* This file at User-level has interface to library - libsmknwsDTMAPI.so
* to retrieve all network specific data for tests.   
*
*/


#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include "libINC/common.h"

/************************* global declarations & variables *********************/

void userdata_samknowsDTMAPI ( esamknwsDTMAPIStatus esamknwsDTMAPI, void *userdata, void *apiuser, int len);

/************************* external declarations ******************************/
extern 	esamknwsDTMAPIStatus init_samknowsDTMAPI (st_usermessage *usermsg, char **headers);
extern 	esamknwsDTMAPIStatus start_samknowsDTMAPI (int count);




st_usermessage	usermssg = {0};

void userdata_samknowsDTMAPI ( esamknwsDTMAPIStatus esamknwsDTMAPI, void *userdata, void *apiuser, int len)
{

	st_usermessage	*user = (st_usermessage*) apiuser;
	user->message = userdata;


	switch (esamknwsDTMAPI) 
	{
		case eSAMKNWSSTATUS_OK: // read any other success
			break;
		case eSAMKNWSSTATUS_MSG: // read all message
			//fprintf(stderr,"debug_app: Rx'd len: (%d) userMssg: (%s) \n", usermssg.msglen, (char*)userdata);
			/* TODO: alloc / realloc - store incomming messages ?
			 * memcpy( usermssg.message, userdata, len);
			 */
			usermssg.msglen += len;
			/* append user buffer */
			break;
		case eSAMKNWSSTATUS_DONE: // read last message
			// extract user buffer, read info 
			usermssg.dconnect += user->dconnect;
			usermssg.dappconnect += user->dappconnect;
			usermssg.dnmlookup += user->dnmlookup;
			usermssg.dtransfer += user->dtransfer;
			usermssg.dpretransfer += user->dpretransfer;
			usermssg.dspeed += user->dspeed;
			usermssg.dsize += user->dsize;
			usermssg.dtime += user->dtime;
			
			usermssg.status = user->status;			
		    	fprintf(stderr," \nSKTEST; <IP server>;<%d>;<%9.6fs>;<%9.6fs>;<%9.6fs>;<%9.6fs>\n", ussermssg.status,   					                                (double)user->dnmlookup, (double)usermssg.dconnect, (double)usermssg.dtransfer,
						       (double)usermssg.dtime 	);
    			
			usermssg.msglen = 0;
			usermssg.msglen = 0;
			//free (usermssg.message);
			break;
		case eSAMKNWSSTATUS_ERR: 
			fprintf(stderr,"debug_app: Rx'd len: (%d) userMssg: (%s) \n", usermssg.msglen, (char*)userdata);
			break;
		default : /* no action required ??*/
			fprintf(stderr,"debug_app: Rx'd unexpected message (%d), needs implementation?? \n", esamknwsDTMAPI);
			break;
	}



}

void usage_help()
{
	printf(" Usage: SKTEST -u<request url> [header] [samples] ... \n");
	printf(" \t  -u \t --url     \turl to CURL request\n");
	printf(" \t  -h,-i\t --header \tadd header to CURL\n");
	printf(" \t  -n \t--samples \tnumber of iteration request to CURL\n");
}


int main ( int argc, char **argv)
{

	char *url = 0;
	char *samples = 0;
	char *header = 0;
	
	int optcmd =0 , ilist=0;
	char **lstheaders = NULL; 	//MAX_HEADERS;


	while (( optcmd = getopt (argc, argv, "Ns:h:i:u:N")) != -1) {
	 	switch (optcmd)
	       	{
			case 'u': if( NULL == (url = optarg))
				  	fprintf(stderr,"debug_app: *Url is (null).\n");	  
				  break;
			case 'i':
			case 'h': if( NULL == (header = strdup(optarg))) 
					 fprintf(stderr,"debug_app: 'Header is (null).\n");
				  if (!ilist)
					lstheaders = (char**) malloc (sizeof(char*) * MAX_HEADERS);
				  /*add headers*/
				  lstheaders[ilist] = (char*) malloc (sizeof(char) * (SIZE_HEADERS));
				  strncpy(lstheaders[ilist], header, strlen(header)+1);
				  lstheaders[ilist][strlen(header)+1] = 0;  // assert 
				  fprintf(stderr,"debug_app: 'Header is (%s).\n",header);

				   ilist++;

				  free(header); 	
				  break;
			case 'N':
			case 's': if( NULL == (samples = optarg))
					  fprintf(stderr,"debug_app: Samples is (null).\n");
				  break;
			
			default: usage_help();
				 if (lstheaders) free(lstheaders);
				 exit(0);

		}

		if (header) {
		}
	}

	if ( (!url) || (!samples) )  {
		if (lstheaders) free(lstheaders);
		printf("\n\n");
		usage_help();

		exit(0);
	}
	if ( (url) || (lstheaders) || (samples) ) {
		
		//printf("library inputs: (url: %s), (header: %s), (samples: %s)\n\n", url, header, samples);
		if ( atoi(samples) <= 0) { /*samples options*/
			
			printf("debug_app: set 's' > 0\n");
			if (lstheaders) free(lstheaders);
			printf("\n\n");
			usage_help();
			exit(0);
		}
		
		/*set url*/
		strncpy(&usermssg.url, url, strlen(url)+1);
		usermssg.url[strlen(url)+1] = 0;
		
		/*initialise & start DTMAPI*/
		esamknwsDTMAPIStatus status = eSAMKNWSSTATUS_NONE;

		status = init_samknowsDTMAPI( &usermssg, lstheaders);

		if ( eSAMKNWSSTATUS_OK == status ) {
			status = start_samknowsDTMAPI(atoi(samples));
		}

		if ( eSAMKNWSSTATUS_OK == status )
			stop_samknowsDTMAPI();


	}

      if (lstheaders) free(lstheaders);

      double iter = (double) samples;
      fprintf(stderr," SKTEST; <IP server>;<response code>;<Median(%9.6fs)>;<Median(%9.6fs)>;<Median(%9.6fs)>;<Median(%9.6fs)>\n",   					      ((double)usermssg.dnmlookup/iter), ((double)usermssg.dconnect/iter), ((double)usermssg.dtransfer/ iter),
						       ((double)usermssg.dtime/iter) );


	return 0;
}
