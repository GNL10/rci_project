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
    servaddr.sin_addr.s_addr = INADDR_ANY; // accepts any address
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

/* EFND i
    Sets up the udp client
    sends EFND i message
    waits to receive EKEY message
*/
void EFND (char *ip, int port, int i) {
    int sockfd; 
    char buffer[MAXLINE];
    char *message;
    struct sockaddr_in servaddr; 
    char *command = "EFND";
    
    message = (char *) malloc((strlen(command) + i%10 + 1)*sizeof(char));
    if (message == NULL) { 
        printf("Memory not allocated.\n"); 
        exit(0); 
    }

    sprintf(message, "%s %d", command, i);
    
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port); 
    servaddr.sin_addr.s_addr = inet_addr(ip); 

    int n;
    socklen_t len; 
    
    // send EFND i message 
    sendto(sockfd, (const char *)message, strlen(message), 
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
    free(message);
    
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
    buffer[n] = '\0'; 
    printf("UDP client received: %s\n", buffer); 
  	// must analyse message
    close(sockfd);
}