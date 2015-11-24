
/* Program to Create threads */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "posixThread.h"
#include "socket.h"
#include "IPHWStructure.h"
#include "authentication.h"

#include "debugPrintf.h"

int createPosixThread(char *IPAddress[], char *HWAddress[], int *IPIndex)
{	
	debug_printf("\nEntering createPosixThread\n");
	
	IPHWAddress *addresses[*IPIndex];	
	pthread_t thread[*IPIndex];
	int  pthreadCreateReturn[*IPIndex], i;
	
	debug_printf("IPIndex %d\n", *IPIndex);
	
	for(i = 0; i < *IPIndex; i++)
	{
		addresses[i] = (IPHWAddress *)malloc(sizeof(IPHWAddress));
		addresses[i]->IP = (char *)malloc(strlen(IPAddress[i]-1));
		addresses[i]->HW = (char *)malloc(strlen(HWAddress[i]-1));
		
		memset(addresses[i]->HW, '\0',strlen(IPAddress[i]));
		memset(addresses[i]->IP, '\0',strlen(IPAddress[i]));
		
		strcpy(addresses[i]->IP, IPAddress[i]); 
		strcpy(addresses[i]->HW, HWAddress[i]);
			
		debug_printf("addresses[%d]->IP : %s, addresses[%d]->IP : %s\n", i, addresses[i]->IP, i, addresses[i]->HW);
		
		pthreadCreateReturn[i] = pthread_create( &thread[i], NULL, sendDataOverPosixThread, (void *)addresses[i]);
		if(pthreadCreateReturn[i])
		{
			perror("pthread_create");
			continue;
		}
		else 
			debug_printf("Created Thread[%d], IPAddress[%d] = %s, HWAddress[%d] = %s\n", i, i, IPAddress[i], i, HWAddress[i]);
	}
	
	for(i = 0; i < *IPIndex; i++)
		pthread_join( thread[i], NULL);
	/*
	for(i = 0; i < IPIndex; i++)
	{
		free(addresses[i]->IP);
		free(addresses[i]->HW);
		free(addresses[i]);
	}	
	*/	
	printf("Adddresses freeed \n");
	printf("Data Sent Over Threads and connection Lost\n");
	debug_printf("Exting createPosixThread\n\n");
	return -1;
}

int *sendDataOverPosixThread(void *ptr)
{
	debug_printf("\nEnering the sendDataOverPosixThread \n");
	
	IPHWAddress *addresses;
	int createBroadcastSocketReturn = -1, athenticationReturn = -1;
	char *IPAddress;
	char *HWAddress;
	
	addresses = (IPHWAddress *) ptr; 
	IPAddress = addresses->IP;
	HWAddress = addresses->HW;
	
	debug_printf("addresses->IP : %s, addresses->HW : %s\n", addresses->IP, addresses->HW); 
	
	/*
	athenticationReturn = athentication(addresses);
	if(!athenticationReturn) 
	{
		debug_printf("Thead Created and sending data to %s\n", IPAddress);
		
		createBroadcastSocketReturn = createBroadcastSocket(IPAddress);
		debug_printf("createPosixThreadRetrun = %d\n", createBroadcastSocketReturn);
	}
	else
	{
		debug_printf("authentication failed\n");
		return -1;
	}	
	*/
	
	createBroadcastSocketReturn = createBroadcastSocket(IPAddress);
	debug_printf("createPosixThreadRetrun = %d\n", createBroadcastSocketReturn);
	if(createBroadcastSocketReturn == -1)
		printf("\n\nCONNECTION : %s LOST \n\n", IPAddress);
	
	debug_printf("Exiting the sendDataOverPosixThread \n\n");
}
