
/* Program to create udp socket with threading  get hw addresses and athentication */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <pthread.h>

#include "socket.h"
#include "posixThread.h"
#include "debugPrintf.h"
#include "IPHWStructure.h"

int IPAddressCounter;

int main(int argc, char *argv[])
{
	int sockfd;
	int numbytes, optlen, broadcast = 1, addr_len = 0, opt = TRUE, 
	    i, clientsock[5], rc, numsocks = 0, maxsocks = 5, count = 0;
	int index, createPosixThreadRetrun = 0, selectReturn = 0, recvfromReturn = 0, sendtoReturn = 0;
	
	char buf[1024];  //data buffer of 1K
    char *broadcastMessage="WHO";
    char *IPAddress[253], *HWAddress[253], *token;
    
	struct sockaddr_in their_addr; // connector's address information
	struct hostent *he;
	struct sockaddr_in servaddr;  
    struct timeval tv;
    
    IPHWAddress *addresses[255];
    
    fd_set fds, readfds;
    boolean firstByte = 0, authentication = 0;
    
    /* startLabel */
    startLabel :																
	
	/* gethostbynameLabel */
	gethostbynameLabel :												
	bzero((void *) &servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("10.1.1.255");
	
	if ((he=gethostbyname(BROADCAST)) == NULL) 
	{
		perror("gethostbyname");
		debug_printf("gethostbyname Failed\n");
		goto gethostbynameLabel;
	}
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		perror("Socket");
		debug_printf("Socket Creatiion Failed\n");
		goto startLabel;
	}
	
	/* setsockoptLabel */
	setsockoptLabel :
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) 
	{
		perror("setsockopt");
		debug_printf("setsockopt Failed\n");
		goto setsockoptLabel;
	}

	their_addr.sin_family = AF_INET; 			// host byte order
	their_addr.sin_port = htons(PORT); 	// short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
	
	debug_printf("Sending BroadcastMessage = %s\n", broadcastMessage);
	
	/* broadcastLabel */
	broadcastLabel :
	sendtoReturn = sendto(sockfd, broadcastMessage, 124, 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
	debug_printf("sendtoReturn = %d\n", sendtoReturn);
	if (sendtoReturn == -1)
	{
		perror("sendto");
		debug_printf("sendto Failed\n");
		goto broadcastLabel;
	}
	
	/* setsockoptReuseLabel */
	setsockoptReuseLabel :
	if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        debug_printf("setsockopt Failed\n");
        goto setsockoptReuseLabel;
    }
	
    getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, &optlen);
	if (opt == -1) 
		perror("getsockopt");
	
	index = 0;
	
	while(1) 
	{
		/* fdSetLabel */
		fdSetLabel :
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);		
		debug_printf("Waiting for data to arrive at port\n");
		
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		selectReturn = select(sockfd+1, &readfds, NULL, NULL, &tv);
		debug_printf("selectReturn = %d\n", selectReturn);
		if (selectReturn == -1) 
		{
			perror("Select");
			break;
		}
		else if(selectReturn == 0 )
		{
			if(!IPAddressCounter)
				goto broadcastLabel;
			
			else
			{	
				debug_printf("Time Out For Broadcasting And Sending Data to All Listeners\n");
				IPAddressCounter = 0;
				break;
			}
		}	
		else
		{ 
			++count;
			debug_printf("Select Count %d\n", count);
		}
		
		if (FD_ISSET(sockfd, &readfds)) 
		{
			int rval = 0;
			bzero(buf, sizeof(buf));
			
			memset(buf, '\0', 1024);
			
			/* recvfromLabel */
			recvfromLabel :
			recvfromReturn = recvfrom(sockfd, buf, 1024, 0, NULL,NULL);
			debug_printf("recvfromReturn : %d\n", recvfromReturn);
			if(recvfromReturn<0)
			{
				perror("READ");
				debug_printf("recvfrom Failed\n");
				goto recvfromLabel;
			}	
			debug_printf("Recieved Buffer %s\n", buf);
			
			addresses[index] = (IPHWAddress *)malloc(sizeof(IPHWAddress));
			addresses[index]->IP = (char *)malloc(strlen(buf));
			addresses[index]->HW = (char *)malloc(strlen(buf));
			//addresses[index]->IPIndex = (int *)malloc(sizeof(int));
			
			token = strtok(buf, ":");
			strcpy(addresses[index]->IP, token);	
			
			token = strtok(NULL, ":");
			strcpy(addresses[index]->HW, token);
			
			index ++;
			IPAddressCounter++;
			
			debug_printf("addresses[%d]->IP %s and addresses[%d]->IP %s\n", index-1, addresses[index-1]->IP, index-1, addresses[index-1]->HW);
		}
		else
		{
			debug_printf(" FD_ISSET Failed\n ");
			goto fdSetLabel;
		}	
	}
	close(sockfd);	
	
	debug_printf("index : %d\n", index);	
	printf("Presented IP Addresses in LAN : \n");
	for(i = 0; i < index; i++)
		printf("IPAddress[%d] : %s, HWAddress[%d] : %s\n", i, addresses[i]->IP, i, addresses[i]->HW);
		
	debug_printf("\n");

	pthread_t thread[index];
	int  pthreadCreateReturn[index];
	
	for(i = 0; i < index; i++)
	{
		pthreadCreateReturn[i] = pthread_create( &thread[i], NULL, sendDataOverPosixThread, (void *)addresses[i]);
		if(pthreadCreateReturn[i])
		{
			perror("pthread_create");
			continue;
		}
		else 
			debug_printf("Thread[%d] Created IPAddress[%d] : %s, HWAddress[%d] : %s\n", i, i, addresses[i]->IP, i, addresses[i]->HW);
	}
	sleep(1);
		
	for(i = 0; i < index; i++)
		free(addresses[i]);
		
	sleep(10);	
	
	goto startLabel;
	return 0;
}
