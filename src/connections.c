#include "connections.h"
#include "file_descriptors.h"

#define MAXLINE 1024 

extern int fd_vec[NUM_FIXED_FD];

// becomes ready to receive udp messages on sockfd
int set_udp_server(char *ip, int port) {
	int sockfd; 
    struct sockaddr_in servaddr; 

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    
      
    // Filling server information 
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &(servaddr.sin_addr));
    servaddr.sin_port = htons(port);
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    printf("UDP socket created and binded.\n");
    return sockfd; 
}  

int set_udp_cli (char *ip, int port, struct sockaddr_in *serv_addr) {
    int sockfd; 
    
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    serv_addr->sin_family = AF_INET; 
    serv_addr->sin_port = htons(port); 
    serv_addr->sin_addr.s_addr = inet_addr(ip);

    return sockfd;
}

void udp_send (int sockfd, char *message, struct sockaddr* addr) {
    sendto(sockfd, (const char *)message, strlen(message), 
        MSG_CONFIRM, (const struct sockaddr *) addr,  
            sizeof(*addr));
}

int udp_recv (int sockfd, char *message, struct sockaddr* addr) {
    int n;
    socklen_t len;

    n = recvfrom(sockfd, (char *)message, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &addr, 
                &len); 
    return n;
}

int initTcpServer(char* ip, int port){
    struct sockaddr_in local_addr;
    int server_fd;
	
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){										//Verificar se não houve erro a criar a socket
		perror("socket: ");
		exit(-1);
	}

	local_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &(local_addr.sin_addr));                         
    local_addr.sin_port = htons(port); 

	if(bind(server_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0){					//Verificar se não houve erro a fazer bind
		perror("bind");
		exit(-1);
	}
	printf("TCP socket created and binded.\n");

	if(listen(server_fd, 2) == -1){																//Verificar se não houve erro a fazer listen
		perror("listen");
		exit(-1);
	}

	return server_fd;
}