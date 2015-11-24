
/* Program to create NEW udp socket  and send data to listener */

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

#include "socket.h"

char *IPAddress[128];
static IPIndex;
static IPAddressCounter;
int main(int argc, char *argv[])
{
	 
	int sockfd;
	int numbytes, optlen, broadcast = 1, addr_len = 0, opt = TRUE, 
	    i, clientsock[5], rc, numsocks = 0, maxsocks = 5;
	
	char buf[1024];  ;  //data buffer of 1K
   
	struct sockaddr_in their_addr; // connector's address information
	struct hostent *he;
	struct sockaddr_in servaddr;  
    
    //sRespondedIPAddress *respondedIP;

	boolean firstByte = 0, authentication = 0;
    
	fd_set fds, readfds;
	
	start:
	//respondedIP = (sRespondedIPAddress *)malloc(sizeof(*respondedIP));
	//memset(&respondedIP, 0, sizeof(*respondedIP));
	
 
	//bzero(&serveraddr, sizeof(struct sockaddr_in));
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);

	bzero((void *) &servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("10.1.1.255");
 
	if ((he=gethostbyname(BROADCAST)) == NULL) 
	{
		perror("gethostbyname");
		exit(1);
	}
	
	debug_printf("socket \n");
	// get the host info
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		perror("socket");
		exit(1);
	}
	debug_printf("setsockopt \n");
	// this call is what allows broadcast packets to be sent:
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) 
	{
		perror("setsockopt (SO_BROADCAST)");
		exit(1);
	}

	their_addr.sin_family = AF_INET; 			// host byte order
	their_addr.sin_port = htons(PORT); 	// short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
	
	 char *broadcastMessage="WHO";
	
	debug_printf("sendto \n");
	debug_printf("broadcastMessage = %s\n", broadcastMessage);
	
	broadcast:
	numbytes = sendto(sockfd, broadcastMessage, 124, 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
	debug_printf("numbytes = %d\n", numbytes);
	if (numbytes == -1)
	{
		perror("sendto");
		exit(1);
	}
		
	debug_printf("Data Broadcasted\n");
	if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
	
    getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, &optlen);
	if (opt != 0) 
		perror("SO_REUSEADDR enabled on s!\n");
	 
	debug_printf("While \n"); 
	
	IPIndex = 0;
	
	while(1) 
	{
		int count = 0;
		debug_printf("IPIndex = %d\n", IPIndex);
		
		//int IPIndex = 0;
		struct timeval tv;
		
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);		
		printf("Waiing for port to arrive data\n");
		
		/* Wait up to five seconds. */
		tv.tv_sec = 5;
		tv.tv_usec = 0;
	
		rc = select(sockfd+1, &readfds, NULL, NULL, &tv);
		debug_printf("RC : %d\n", rc);
		if (rc == -1) 
		{
			perror("Select");
			break;
		}
		else if(rc == 0 )
		{
			if(!IPAddressCounter)
				goto broadcast;
			
			else
			{	
				debug_printf("Time out searching again for ");
				IPAddressCounter = 0;
				break;
			}
		}	
		else
		{ 
			++count;
			debug_printf("Select success and count %d\n", count);
		}
		
		//for (i = 0; i < 1; i++) 
		{
			debug_printf("Sock (%d) in for loop\n", sockfd );
			if (FD_ISSET(sockfd, &readfds)) 
			{
					int rval = 0;
					bzero(buf, sizeof(buf));
					
					memset(buf, '\0', 1024);
					
					rval = recvfrom(sockfd, buf, 1024, 0, NULL,NULL);
					debug_printf("rval : %d\n", rval);
					if(rval<0)
						perror("READ");
					else
						debug_printf("READ successful\n");
					
					IPAddress[IPIndex] = (char *)malloc(strlen(buf));
					strcpy(IPAddress[IPIndex], buf);
					printf("Connected IP :	 %s\n", buf);
					printf("respondedIPAddress[%d] buffer : %s\n",IPIndex, IPAddress[IPIndex]);
					
					IPIndex ++;
					IPAddressCounter++;
					debug_printf("IPAddressCounter = %d,  IPIndex = %d\n",IPAddressCounter, IPIndex);
					
					//int snprintf(char *str, size_t size, const char *format, ...);
					memset(buf, '\0',1024);
					//srand(time(0));
					snprintf(buf, 1024,"FUEL SENSOR DATA HAS REACHED TO %s\n", buf);    	
					
					debug_printf("sending code %s\n", buf);	
					
					if ((numbytes = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
					{
						perror("sendto");
						exit(1);
					}
			}
			else
				debug_printf(" FD_ISSET not set\n ");
		}
	sleep(3);
	}
	close(sockfd);	
	
	for(i = 0; i<2; i++)
		printf("respondedIPAddress[%d] buffer : %s\n", i, IPAddress[i]);
	sleep(3);	
	createBroadcastSocket(IPAddress, 0);
	goto start;
	return 0;
}
