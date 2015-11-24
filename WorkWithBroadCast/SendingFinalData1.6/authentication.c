
/* Code for Getting Authentication */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "IPHWStructure.h"
#include "authentication.h"
#include "debugPrintf.h"

int athentication(IPHWAddress *addresses)
{
	debug_printf("Entering athentication\n");
	
	IPHWAddress *temp = NULL;
	char *AuthenticationFileName = "Authentication"; 
	FILE *AuthenticationFilePtr = NULL;
	int writeReturn = -1, readReturn = -1;
	
	debug_printf("addresses->IP : %s, addresses->HW : %s\n", addresses->IP, addresses->HW);
	
	/*** Start of  Reading IP and HW From File ***/
	
	if( (AuthenticationFilePtr = fopen(AuthenticationFileName, "r")) == NULL )
	{
		perror("file");
		return -1;
	}
	else 
		debug_printf("AuthenticationFile FILE Opened for Reading\n");
	
	
	while(!feof(AuthenticationFilePtr))
	{
		char *readBuffer = NULL;
		readBuffer = (char *)malloc(sizeof(char) * 64);
		memset(readBuffer, '\0', 64);
		if( fgets (readBuffer, 64, AuthenticationFilePtr)!=NULL ) 
		{
			debug_printf("readBuffer %s", readBuffer);
			if(strstr(readBuffer, addresses->IP) && strstr(readBuffer, addresses->HW))
			{
				debug_printf("Already Authenticated\n");
				return 0;
			}
			else
				continue;
		}
	}
	debug_printf("Not Athenticated Yet You Need to Athenticate \n");
	
	
	
	/*** Start of  Writing IP and HW From File ***/
	/*	
	if( (AuthenticationFilePtr = fopen(AuthenticationFileName, "a+")) == NULL )
	{
		perror("file");
		return -1;
	}
	else 
		debug_printf("AuthenticationFile FILE Opened for Writing\n");
	
	writeReturn = fprintf(AuthenticationFilePtr, "%s:%s\n", addresses->IP, addresses->HW);
	debug_printf("writeReturn : %d\n");
	if(writeReturn < 0)
	{
		perror("fprintf");
		return -1;
	}	
	
	fclose(AuthenticationFilePtr);
	*/
	/*** End of  Writing IP and HW From File ***/
	
	
	
	debug_printf("Exiting athentication\n");
	
	/*** End of Reading IP and HW From File ***/
	return 0;
}	

int sendOTP(IPHWAddress *addresses)
{
	int authenticationSocket = -1;
	struct sockaddr_in si_other;
		
	
