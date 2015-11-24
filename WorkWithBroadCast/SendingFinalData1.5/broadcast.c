
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

int IPIndex;
int IPAddressCounter;

int main(int argc, char *argv[])
{
	int sockfd;
	int numbytes, optlen, broadcast = 1, addr_len = 0, opt = TRUE, 
	    i, clientsock[5], rc, numsocks = 0, maxsocks = 5, count = 0;
	int createPosixThreadRetrun = 0, selectReturn = 0, recvfromReturn = 0, sendtoReturn = 0;
					
	char buf[1024];  //data buffer of 1K
    char *broadcastMessage="WHO";
    char *IPAddress[253], *HWAddress[253], *token;
    
	struct sockaddr_in their_addr; // connector's address information
	struct hostent *he;
	struct sockaddr_in servaddr;  
    struct timeval tv;
    
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
	if (opt != 0) 
		perror("getsockopt");
	
	IPIndex = 0;
	
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
			
			IPAddress[IPIndex] = (char *)malloc(strlen(buf));
			HWAddress[IPIndex] = (char *)malloc(strlen(buf));
			
			token = strtok(buf, ":");
			strcpy(IPAddress[IPIndex], token);	
			
			token = strtok(NULL, ":");
			strcpy(HWAddress[IPIndex], token);
			
			IPIndex ++;
			IPAddressCounter++;
			debug_printf("IPAddressCounter = %d, IPAddress[%d] %s and HWAddress[%d] %s\n",IPAddressCounter,  IPIndex-1, IPAddress[IPIndex-1], IPIndex-1, HWAddress[IPIndex-1]);
		}
		else
		{
			debug_printf(" FD_ISSET Failed\n ");
			goto fdSetLabel;
		}	
	}
	close(sockfd);	
	
	debug_printf("Presented IP Addresses in LAN\n");
	for(i = 0; i<IPIndex; i++)
		debug_printf("IPAddress[%d] : %s, HWAddress[%d] : %s\n", i, IPAddress[i], i, HWAddress[i]);
	debug_printf("\n");
	
	createPosixThreadRetrun = createPosixThread(IPAddress, HWAddress, &IPIndex);
	debug_printf("createPosixThreadRetrun = %d\n", createPosixThreadRetrun);
	if(createPosixThreadRetrun == -1)
	{
		debug_printf("Starting Broadcasting \"WHO\" Mssge Again\n");
		
	}
	
	for(i = 0; i < IPIndex; i++)
	{
		free(IPAddress[IPIndex]);
		free(HWAddress[IPIndex]);
	}	
	sleep(10);	
	goto startLabel;
	return 0;
}
