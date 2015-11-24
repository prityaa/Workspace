
#include "socket.h"

int createBroadcastSocket(sRespondedIPAddress *respondedIP, int IPIndex);
{
	int socketDescriptor = -1;
	struct sockaddr_in serv_addr, cli_addr;
	char *Message = "IN CreateSocket"; 
	
	debug_printf("respondedIP->respondedIPAddress[%d] = %s\n", IPIndex, respondedIP->respondedIPAddress[IPIndex]);
	
	//int socket(int domain, int type, int protocol);
	if( (socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		 debug_printf("socket Could not Open\n");
		 return -1;
	}
	else 
		debug_printf("Socket Is Opened\n");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));	
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = respondedIP->respondedIPAddress[IPIndex];
	serv_addr.sin_port = htons(PORT_IN_THREAD);
     
    if (bind(socketDescriptor, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
		
    if ((numbytes=sendto(socketDescriptor, broadcastMessage, strlen(Message), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
	{
		perror("sendto");
		exit(1);
	}
	else
		debug_printf("Message sent \n");  


}

int createListenerSocket(sRespondedIPAddress *respondedIP, int IPIndex)
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    return 0;
}
//int authenticaton(void);
