#include "connections.h"
#include "file_descriptors.h"

#define MAXLINE 1024 

extern int fd_vec[NUM_FIXED_FD];

// becomes ready to receive udp messages on sockfd
int set_udp_server(char *ip, int port) {
	int sockfd; 
    struct sockaddr_in servaddr, cliaddr; 

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = inet_addr(ip); //INADDR_ANY accepts any address
    servaddr.sin_port = htons(port);
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    return sockfd; 
}  

int set_udp_cli (char *ip, int port) {
    int sockfd; 
    
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    return sockfd;
}

void udp_send (int sockfd, char *ip, int port, char *message) {
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr)); 
      

    // Filling server information 
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(port); 
    addr.sin_addr.s_addr = inet_addr(ip);

    sendto(sockfd, (const char *)message, strlen(message), 
        MSG_CONFIRM, (const struct sockaddr *) &addr,  
            sizeof(addr)); 
    printf("UDP client sent message: %s\n", message);
	return;
}

void udp_recv (int sockfd, char *ip, int port, char *message) {
    struct sockaddr_in addr;
    int n;
    socklen_t len;

    memset(&addr, 0, sizeof(addr)); 
    
    // Filling server information 
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(port); 
    addr.sin_addr.s_addr = inet_addr(ip);
    n = recvfrom(sockfd, (char *)message, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &addr, &len); 
    message[n] = '\0';
    printf("UDP client received message: %s\n", message); 
    return;
}

int initTcpServer(char* ip, int port){
    struct sockaddr_in local_addr;
	socklen_t size_addr = 0;
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
	printf(" socket created and binded \n");

	if(listen(server_fd, 2) == -1){																//Verificar se não houve erro a fazer listen
		perror("listen");
		exit(-1);
	}

	return server_fd;
}

void listenHandler(){
    int new_fd;
    struct sockaddr_in new_addr;
    socklen_t size_addr = 0;

    if((new_fd = accept(fd_vec[LISTEN_FD], (struct sockaddr*)&new_addr, &size_addr)) == -1){	    //Verficiar se não houve erro a fazer accept
        perror("accept");
        exit(-1);
	}
}