
/* Listener Program to Listen Broadcast Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "authentication.h"
#include "debugPrintf.h"

#define MYPORT "8888"

#define MAXBUFLEN 100

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
	
	char ipAddress[20], broadcasterIPAddress[20], s[INET6_ADDRSTRLEN], buferToSend[1024], HWAddress[20];
    char *interface = "eth0", buf[1024];
    unsigned char *mac, MACAddress[48];
    
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
	debug_printf("Waiting For Message to come ...\n");
	addr_len = sizeof their_addr;
	
	memset(buferToSend, '\0', 1024);

	debug_printf("I am Recieving Data\n");			
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
		strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
		
		ioctl(sockfd, SIOCGIFADDR, &ifr);
		
		strcpy(ipAddress, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));
		debug_printf("IP Address is ( %s ) : %s\n", interface, ipAddress );
		
		ioctl(sockfd, SIOCGIFHWADDR, &ifr);

		mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
		sprintf(MACAddress,"%.2x%.2x%.2x%.2x%.2x%.2x" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		debug_printf("MAC Address %s \n\n", MACAddress);			
		
		strcat(buferToSend, ipAddress);
		strcat(buferToSend, ":");
		strcat(buferToSend, MACAddress);
		debug_printf("buferToSend : %s\n", buferToSend);
		
		if((numbytes = sendto(sockfd, buferToSend, strlen(buferToSend), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
		{
			perror("sendto");
			exit(1);
		}
		else
		{ 
			debug_printf("sent to  %s\n", ipAddress);
			firstByte = 0;
		}	
	}
	
	close(sockfd);
	
	//recieveOTP(void)
	
	createListenerSocket(IPAddress, IPIndex);
	
return 0;
}


