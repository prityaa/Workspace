
/* Program to select function that lilstens from each listen with one recvfrom and sendto */

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

#define TRUE   1
#define FALSE  0
#define PORT 8888
#define BROADCAST "10.1.1.255"
 
#ifdef DEBUG
	#define debug_printf printf
#else 
	#define debug_printf
#endif

typedef int boolean;

int main(int argc, char *argv[])
{
	int sockfd;
	int numbytes, optlen, broadcast = 1, addr_len = 0, opt = TRUE, 
	    i, clientsock[5], rc, numsocks = 0, maxsocks = 5;
	
	char buf[1024];  ;  //data buffer of 1K
    char *broadcastMessage = "WHO";
    	   
    struct sockaddr_in their_addr; // connector's address information
	struct hostent *he;
	struct sockaddr_in servaddr;  
    
    boolean firstByte = 0, authentication = 0;
    
	fd_set fds, readfds;
	
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
	
	// get the host info
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		perror("socket");
		exit(1);
	}
	
	// this call is what allows broadcast packets to be sent:
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1) 
	{
		perror("setsockopt (SO_BROADCAST)");
		exit(1);
	}

	their_addr.sin_family = AF_INET; 			// host byte order
	their_addr.sin_port = htons(PORT); 	// short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
	
	if ((numbytes=sendto(sockfd, broadcastMessage, strlen(broadcastMessage), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
	{
		perror("sendto");
		exit(1);
	}
	
	debug_printf("data broadcasted\n");
	
	if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
   
    getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, &optlen);
	if (opt != 0) 
		debug_printf("SO_REUSEADDR enabled on s!\n");
	 
    // add the listener to the master set
	while(1) 
	{
		int count = 0;
		char *respondedIPAddress[255];
		int IPIndex = 0;
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
		else
		{ 
			++count;
			debug_printf("Select success and count %d\n", count);
		}
		for (i = 0; i < 5; i++) 
		{	debug_printf("Sock (%d) in for loop\n", sockfd );
			if (FD_ISSET(sockfd, &readfds)) 
			{
				debug_printf("i = %d\n", i);
				if (i == sockfd) 
				{
					debug_printf("numsocks : %d, maxsocks : %d\n", numsocks, maxsocks);
					if (numsocks < maxsocks) 
					{
						FD_SET(clientsock[numsocks], &fds);
						numsocks++;
					} 
					else 
						debug_printf("Ran out of socket space \n");
				}
				else 
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
					
					respondedIPAddress[IPIndex] = (char *)malloc(strlen(buf));
					strcpy(respondedIPAddress[IPIndex], buf);
					printf("Connected IP :	 %s\n", buf);
					printf("respondedIPAddress[%d] buffer : %s\n",IPIndex, respondedIPAddress[IPIndex]);
					
					IPIndex ++;
					
					
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
					/*
					char buf1[128];
					memset(buf1, '\0', 128);
					
					rval = recvfrom(sockfd, buf1, 128, 0, NULL,NULL);
					debug_printf("rval : %d\n", rval);
					if(rval<0)
						perror("READ");
					else
						debug_printf("READ successful\n");
						
					debug_printf("Recieved Code : %s buf1  : %s\n",buf, buf1);
				
					if(!strcmp(buf, buf1))
					{
						memset(buf, '\0',1024);
						snprintf(buf, 1024,"ARTYAA FINAL DATA\n", rand()%100000);    	
					
						if ((numbytes = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
						{
							perror("sendto");
							exit(1);
						}
						
					}
					else 
						debug_printf("Authentication Failed\n");	
				*/
					
				}
			}
			else
				debug_printf(" FD_ISSET not set\n ");
		}
	
	int stopScreen;	
	printf("Enter to stop screen\n");
	scanf("%d", &stopScreen);
	
	}
	close(sockfd);
	return 0;
}
