#include "connections.h"

#define MAXLINE 1024 

// becomes ready to receive udp messages on sockfd
int set_udp_server() {
	int sockfd; 
    struct sockaddr_in servaddr, cliaddr; 
    int port = 8080; 	// to be changed

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY accepts any address
    servaddr.sin_port = htons(port);
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    
    // must now wait on select for the file descriptor update
    
    char buffer[MAXLINE]; 
    char *message = "EKEY i succ succ.IP succ.port"; 
    int n;
    socklen_t len;
  
    len = sizeof(cliaddr);  //len is value/resuslt 
  
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                &len); 
    buffer[n] = '\0'; 
    printf("UDP server received : %s\n", buffer);
    sendto(sockfd, (const char *)message, strlen(message),  
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
            len); 
      
    return sockfd; 
}  

int set_udp_cli (char *ip, int port, struct sockaddr_in *addr) {
    int sockfd; 
    
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(addr, 0, sizeof(*addr)); 
      
    // Filling server information 
    addr->sin_family = AF_INET; 
    addr->sin_port = htons(port); 
    addr->sin_addr.s_addr = inet_addr(ip);

    return sockfd;
}