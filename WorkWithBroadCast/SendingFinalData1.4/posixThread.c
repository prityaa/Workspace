
/* Program to Create threads */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "posixThread.h"
#include "socket.h"

int createPosixThread(const char *IPAddress[], int *IPIndex)
{	
	debug_printf("\nEntering createPosixThread\n");
		
	pthread_t thread[*IPIndex];
	int  pthreadCreateReturn[*IPIndex], i;
	
	debug_printf("IPIndex %d\n", *IPIndex);
	for(i = 0; i < *IPIndex; i++)
	{
		pthreadCreateReturn[i] = pthread_create( &thread[i], NULL, sendDataOverPosixThread, (void *)IPAddress[i]);
		if(pthreadCreateReturn[i])
		{
			perror("pthread_create");
			continue;
		}
		else 
			debug_printf("Created Thread[%d], IPaddress[%d] = %s\n", i, *IPIndex, IPAddress[i]);
	}
	
	for(i = 0; i < *IPIndex; i++)
		pthread_join( thread[i], NULL);
	
	printf("Data Sent Over Threads and connection Lost\n");
	debug_printf("Exting createPosixThread\n\n");
	return -1;
}

int *sendDataOverPosixThread( void *ptr)
{
	debug_printf("\nEnering the sendDataOverPosixThread \n");
	
	int createBroadcastSocketReturn = -1;
	char *IPAddress;
	
	IPAddress = (char *) ptr;
	printf("Thead Created and sending data to %s\n", IPAddress);
	
	createBroadcastSocketReturn = createBroadcastSocket(IPAddress);
	debug_printf("createPosixThreadRetrun = %d\n", createBroadcastSocketReturn);
	if(createBroadcastSocketReturn == -1)
		printf("\n\nCONNECTION : %s LOST \n\n", IPAddress);
	
	debug_printf("Exiting the sendDataOverPosixThread \n\n");
}
