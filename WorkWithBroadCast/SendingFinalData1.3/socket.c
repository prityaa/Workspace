
#include "socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


int createBroadcastSocket(char *IPAddress, int IPIndex)
{
	struct sockaddr_in si_other;
	int broadCasterSocket, i, slen=sizeof(si_other);
	char buf[1024];

	if ((broadCasterSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		debug_printf("socket Failed\n");

	//bzero((void *) &si_other, sizeof(si_other));
	memset((char *) &si_other, 0, sizeof(si_other));
	
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT_IN_THREAD);
	
	if (inet_aton("10.1.1.135", &si_other.sin_addr)==0) 
	{
		debug_printf("inet_aton() Failed\n");
		exit(1);
	}

	for (i=0; i<10; i++) 
	{
		printf("Sending packet %d\n", i);
		sprintf(buf, "This is packet %d\n", i);
		if (sendto(broadCasterSocket, buf, 1024, 0, &si_other, slen)==-1)
		debug_printf("sendto() Failed\n");
	}

	close(broadCasterSocket);
	return 0;

}

int createListenerSocket(char *IPAddress, int IPIndex)
{
    struct sockaddr_in si_me, si_other;
	int listenerSocket, i, slen=sizeof(si_other);
	char buf[1024];

	if ((listenerSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		debug_printf("Socket Failed\n");

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT_IN_THREAD);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenerSocket, &si_me, sizeof(si_me))==-1)
		debug_printf("bind Failed\n");

	for (i=0; i<10; i++) 
	{
		if (recvfrom(listenerSocket, buf, 1024, 0, &si_other, &slen)==-1)// second socket stores the info of other socket i.e. si_other
			debug_printf("recvfrom Failed\n");
		
		printf("Received packet from %s:%d\nData: %s\n\n", 
		inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
	}

	close(listenerSocket);
	return 0;

}
