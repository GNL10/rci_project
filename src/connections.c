#include "connections.h"
#include "file_descriptors.h"
#include "logic.h"
#include "io.h"


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

int udp_send (int sockfd, char *message, struct sockaddr* addr) {
    int n;
    n = sendto(sockfd, (const char *)message, strlen(message), 
        MSG_CONFIRM, (const struct sockaddr *) addr,  
            sizeof(*addr));
    if (n == -1)
        perror("ERROR:sendto");
    else
        printf("[UDP] Sent: %s\n", message);
    return n;
}

int udp_recv (int sockfd, char *message, struct sockaddr* addr) {
    int n;
    socklen_t len;

    n = recvfrom(sockfd, (char *)message, UPD_RCV_SIZE,  
                MSG_WAITALL, (struct sockaddr *) &addr, 
                &len); 
    if (n == -1)
        perror("ERROR: recvfrom");
    else{
        message[n] = '\0';
        printf("[UDP] Recv: %s\n", message);
    }
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

void forwardHandler(int active_fd){

    if(active_fd == fd_vec[LISTEN_FD]){
        listenHandler();
    }else if(active_fd == fd_vec[UDP_FD]){
        udpHandler();
    }else if(active_fd == fd_vec[STDIN_FD]){
        stdinHandler();
    }else{              //Generic TCP incoming message
        tcpHandler(active_fd);
    }
    
}

void stdinHandler() {
    char command_line[BUFFER_SIZE];
    int key, port, args_num;
    char name[PARAM_SIZE], ip[INET6_ADDRSTRLEN], command[PARAM_SIZE];

    read_command_line(command_line);
    args_num = parse_command(command_line, command, &key, name,  ip, &port);
    switch(get_command_code(command)) {
        case 0:     // new
            if (args_num == 1+1)
                printf("NEW FUNCTION TO BE DEFINED\n");
            else
                printf("The entry command needs 1 argument\nUsage: new <key>\n");
            break;
        case 1:     // entry
            if (args_num == 1+4)
                entry(key, name, ip, port);
            else
                printf("The entry command needs 4 arguments\nUsage: entry <key> <name> <ip> <port>\n");
            break;
        case 2:     // sentry
            if (args_num == 1+4)
                printf("SENTRY FUNCTION TO BE DEFINED\n");
            else
                printf("The sentry command needs 4 arguments\nUsage: sentry <key> <name> <ip> <port>\n");
            break;
        case 3:     // leave
            printf("LEAVE FUNCTION TO BE DEFINED\n");
            break;
        case 4:     // show
            printf("SHOW FUNCTION TO BE DEFINED\n");
            break;
        case 5:     // find
            if (args_num == 1+1)
                printf("FIND FUNCTION TO BE DEFINED\n");
            else
                printf("The find command needs 1 argument\nUsage: find <key>\n");
            break;
        case -2:    // exit
            printf("EXIT FUNCTION TO BE DEFINED\n");
            break;
        default :   // incorrect command
            printf("Command not recognized\n");
            printf("Available commands:\n");
            printf("\tnew <key>\n\tentry <key> <name> <ip> <port>\n");
            printf("\tsentry <key> <name> <ip> <port>\n\tleave\n\tshow\n\tfind <key>\n\texit\n\n\n");
            break;
        
    }
}

void udpHandler(void) {
    char message[UPD_RCV_SIZE];
    struct sockaddr_in cli_addr;
    int n;
    socklen_t len;

    memset(&cli_addr, 0, sizeof(cli_addr));
    len = sizeof(cli_addr);

    n = recvfrom(fd_vec[UDP_FD], (char *)message, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &cli_addr, &len); 
    message[n] = '\0';
    printf("UDP message was received: %s\n", message);
    
    sendto(fd_vec[UDP_FD], (const char *)"EKEY 15 name 127.0.0.1 6000", strlen("EKEY 15 name 127.0.0.1 6000"), 
        MSG_CONFIRM, (const struct sockaddr *) &cli_addr,  
            sizeof(cli_addr)); 
}

void tcpHandler(int sock_fd){
    char buff[TCP_RCV_SIZE];
    int args_num, key, port;
    char ip[INET6_ADDRSTRLEN], name[PARAM_SIZE], command[PARAM_SIZE];

    if(read(sock_fd, buff, sizeof(buff)) < 0){
        printf("Client %d disconnected abruptly\n", sock_fd);
        fdDeleteFd(sock_fd);
        close(sock_fd);
        return;
    }
    args_num = parse_command(buff, command, &key, name,  ip, &port);

    switch(get_TCP_code(command)) {
        case 0:     // FND
            if (args_num == 1+4)
                printf("FND FUNCTION TO BE DEFINED\n");
            else
                printf("ERROR: FND needs 4 arguments\n");
            break;
        case 1:     // KEY
            if (args_num == 1+4)
                printf("KEY FUNCTION TO BE DEFINED\n");
            else
                printf("ERROR: KEY needs 4 arguments\n");
            break;
        case 2:     // SUCCONF
            if (args_num == 1)
                printf("SUCCONF FUNCTION TO BE DEFINED\n");
            else
                printf("ERROR: SUCCONF needs 0 arguments\n");
            break;
        case 3:     // SUCC
            if (args_num == 1 + 4)
                printf("SUCC FUNCTION TO BE DEFINED\n");
            else
                printf("ERROR: SUCC needs 4 arguments\n");
             break;
        case 4:     // NEW
            if (args_num == 1 + 4)
                printf("NEW FUNCTION TO BE DEFINED\n");
            else
                printf("ERROR: NEW needs 4 arguments\n");
             break;
        default :   // incorrect command
            printf("TCP command not recognized. Ignoring...\n");
            break; 
    }
}

int parseCommandTcp(char *buff, char *command, int *key,  char *name, char *ip, int *port) {
    int num_args = 0;
    char dummy[10];
    int i;

    //Find the end of message char (\n)
    for(i = 0; i < TCP_RCV_SIZE && buff[i] != '\0'; i++){
        if(buff[i] == '\n'){
            buff[i] = '\0';                           //sscanf ignores everything after \n
            i = -1;
            break;
        }
    }

    //Check if a \n has been found
    if(i != -1){
        return -1;
    }

    // E se receber menos argumentos do que estava a espera ?
    if((num_args = sscanf(buff, "%"PARAM_SIZE_STR"s %d %"PARAM_SIZE_STR"s %"PARAM_SIZE_STR"s %d %9s", command, key, name, ip, port, dummy)) == 6){
        printf("TCP stream recieved has too many arguments\n");
        return -1;
    }
    
    return num_args;
}

int get_TCP_code (char *command) {
	if (!strcmp(command, "FND"))
		return 0;
	else if (!strcmp(command, "KEY"))
		return 1;
	else if (!strcmp(command, "SUCCONF"))
		return 2;
	else if (!strcmp(command, "SUCC"))
		return 3;
	else if (!strcmp(command, "NEW"))
		return 4;
	else 	// invalid command
		return -1;
}

void listenHandler(void){
    int new_fd;
    struct sockaddr_in new_addr;
    socklen_t size_addr = 0;

    if((new_fd = accept(fd_vec[LISTEN_FD], (struct sockaddr*)&new_addr, &size_addr)) == -1){	    //Verficiar se não houve erro a fazer accept
        perror("accept");
        exit(-1);
	}
    fdInsertNode(new_fd);
}