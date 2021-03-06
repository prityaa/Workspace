#ifndef SOCKET_H_
#define SOCKET_H_

#define PORT 				8888
#define PORT_IN_THREAD 		9999	
#define BROADCAST 			"10.1.1.255"
#define TRUE   				1
#define FALSE  				0

#ifdef DEBUG
	#define debug_printf printf
#else 
	#define debug_printf
#endif

typedef int boolean;

/*
typedef struct sRespondedIPAddress
{
	char *respondedIPAddress[255];
	int IPIndex;

}sRespondedIPAddress;

*/

#ifdef BROADCASTER 
int createBroadcastSocket(char *IPAddress, int IPIndex);
#endif

#ifdef LISTENER
int createListenerSocket(char *IPAddress, int IPIndex);
#endif
	
#endif
