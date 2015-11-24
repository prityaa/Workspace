
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

int *sendDataOverPosixThread(void *ptr)
{
	debug_printf("\nEnering the sendDataOverPosixThread \n");
	
	IPHWAddress *addresses;
	int createBroadcastSocketReturn = -1, athenticationReturn = -1;
	char IPAddress[20];
	char *HWAddress;
	
	addresses = (IPHWAddress *) ptr; 
	debug_printf("addresses->IP : %s, addresses->HW : %s\n", addresses->IP, addresses->HW); 

	HWAddress = (char *)malloc(strlen(addresses->HW));
	
	strcpy(IPAddress, addresses->IP);
	strcpy(HWAddress, addresses->HW);
	
	debug_printf("IPAddress : %s, HWAddress : %s\n", IPAddress, HWAddress);
	
	createBroadcastSocketReturn = createBroadcastSocket(addresses->IP);
	debug_printf("createPosixThreadRetrun = %d\n", createBroadcastSocketReturn);
	if(createBroadcastSocketReturn == -1)
		printf("\n\nCONNECTION : %s LOST \n\n", IPAddress);
	
	debug_printf("Exiting the sendDataOverPosixThread \n\n");
}
