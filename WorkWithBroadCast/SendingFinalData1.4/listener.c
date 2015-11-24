
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#include <unistd.h>
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

#define MYPORT "8888"

#define MAXBUFLEN 100

#ifdef DEBUG
#define debug_printf printf
#else
#define debug_printf
#endif

typedef int boolean;

char *IPAddress[128];
static IPIndex;

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
		return &(((struct sockaddr_in*)sa)->sin_addr);

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, n, rv, numbytes, addr_len;
	
	char ipAddress[20], broadcasterIPAddress[20], s[INET6_ADDRSTRLEN];
    char *array = "eth0", buf[1024];
    
    struct ifreq ifr;
	struct sockaddr_storage their_addr;
	struct addrinfo hints, *servinfo, *p;
	
	boolean firstByte = 1, authentication = 0;
	
	memset(&hints, 0, sizeof hints);
	
	hints.ai_family = AF_INET; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	
	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
		{
			perror("listener: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("listener: bind");
			continue;
		}
	break;
	}
	
	if (p == NULL) 
	{
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);
	printf("Waiting For Message to come ...\n");
	addr_len = sizeof their_addr;
	debug_printf("sockfd : %d\n", sockfd);
	
	//while(1)
	{	
		printf("I am Recieving Data\n");			
		memset(buf, '\0', 1024);
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) 
		{
			perror("recvfrom");
			exit(1);
		}
		debug_printf("Recieved Data in Listener : %s\n", buf);
		
		// responding to broacast address
		debug_printf("firstByte : %d && buf : %s\n", firstByte, buf);
		
		if( firstByte && !strcmp(buf, "WHO") )
		{
			strcpy( broadcasterIPAddress, 
						inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),s, sizeof s) 
					   );
	
			printf("Broadcaster :  %s\n", broadcasterIPAddress);
			
			buf[numbytes] = '\0';
			
			debug_printf("data : %s\n", buf);
			
			ifr.ifr_addr.sa_family = AF_INET;
			strncpy(ifr.ifr_name, array, IFNAMSIZ - 1);
			
			ioctl(sockfd, SIOCGIFADDR, &ifr);
			
			strcpy(ipAddress, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));
			debug_printf("IP Address is ( %s ) : %s\n", array, ipAddress );
			
			
			if((numbytes = sendto(sockfd, ipAddress, strlen(ipAddress), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
			{
				perror("sendto");
				exit(1);
			}
			else
			{ 
				debug_printf("sent to  %s\n", broadcasterIPAddress);
				firstByte = 0;
			}	
		}
	}
	
	close(sockfd);
	
	createListenerSocket(IPAddress, IPIndex);
	
return 0;
}


