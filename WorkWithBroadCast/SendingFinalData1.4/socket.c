
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

int createBroadcastSocket(char *IPAddress)
{
	debug_printf("\nEntering createBroadcastSocket\n");

	struct timeval tv;
	struct sockaddr_in si_other;
	int broadCasterSocket, i, slen=sizeof(si_other);
	int opt = 1, optlen;
	int recvRet = -1;
	char buf[1024];
	
	debug_printf("IPAddress = %s\n", IPAddress);
	
	if ((broadCasterSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		perror("socket Failed\n");
	
	inetAtonLabel :
	memset((char *) &si_other, 0, sizeof(si_other));
	
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT_IN_THREAD);
	
	if (inet_aton(IPAddress, &si_other.sin_addr)==0) 
	{
		perror("inet_aton()");
		goto inetAtonLabel;
	}

	for (i=0; ;i++) 
	{
		fd_set fds, readfds;
		int sendToRet = -1, selectRet;
		
		sleep(1);
		debug_printf("Sending packet %d to %s\n", i, IPAddress);
		memset(buf, '\0', 1024);
		sprintf(buf, "Prityaa%d", i);
		
		if (sendto(broadCasterSocket, buf, strlen(buf), 0, &si_other, slen) == -1)
		{
			perror("sendto() Failed\n");
			continue;
		}	
		
		FD_ZERO(&readfds);
		FD_SET(broadCasterSocket, &readfds);		
		
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		
		debug_printf("Waiting for data to arrive at port\n");
		
		selectRet = select(broadCasterSocket+1, &readfds, NULL, NULL, &tv);
		printf("selectRet : %d\n", selectRet);
		if (selectRet == -1) 
		{
			perror("Select");
			break;
		}
		
		else if(selectRet == 0 )
		{
			{	
				debug_printf("Time out for Recievng data from  %s\n", IPAddress);
				break;
			}
		}	
		else
			perror("Select");
		
		if (FD_ISSET(broadCasterSocket, &readfds)) 
		{
			int rval = 0;
			bzero(buf, sizeof(buf));
			
			memset(buf, '\0', 1024);
			
			int recvRet = recvfrom(broadCasterSocket, buf, 1024, 0, &si_other, &slen);
			if (recvRet == -1)
				debug_printf("recvfrom Failed\n");
		
			if(recvRet<0)
				perror("READ");
			else
				debug_printf("READ successful\n");
		}
		else
			debug_printf(" FD_ISSET not set\n ");
		
		printf("ACK : %s:%d\t Data: %s\n", 
		inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
			
	
	}
	close(broadCasterSocket);
	printf("\n\nCONNECTION : %s LOST \n\n", inet_ntoa(si_other.sin_addr));
	debug_printf("Exiting createBroadcastSocket\n\n");
	
	return -1;
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

	for (i=0;; i++) 
	{
		if (recvfrom(listenerSocket, buf, 1024, 0, &si_other, &slen)==-1)// second socket stores the info of other socket i.e. si_other
			debug_printf("recvfrom Failed\n");
		
		printf("Received packet from %s:%d\nData: %s\n\n", 
		inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
	
		int sendToRet = sendto(listenerSocket, "ACK AARTYAA", strlen("ACK AARTYAA"), 0, &si_other, slen);
		
		printf("sendToRet = %d\n", sendToRet);
		if (sendToRet == -1)
			perror("sendto() Failed\n");
	}
	
	close(listenerSocket);
	return 0;

}
