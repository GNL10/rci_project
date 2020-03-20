#include "logic.h"
#include "connections.h"
#include "io.h"
#include "file_descriptors.h"

extern int fd_vec[NUM_FIXED_FD];


void stdinHandler() {
    char command_line[BUFFER_SIZE];
    int key, port, args_num;
    char name[PARAMETER_SIZE], ip[INET6_ADDRSTRLEN], command[PARAMETER_SIZE];

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

void entry (int key, char *name, char *ip, int port) {
    int sockfd, s_key, s_port;
    char message[8];
    char buffer[BUFFER_SIZE], s_name[PARAMETER_SIZE], s_ip[INET6_ADDRSTRLEN];
    struct sockaddr_in serv_addr;

    sprintf(message, "%s %d", "EFND", key);
    
    sockfd = set_udp_cli(ip, port, &serv_addr);
    
    // send EFND i 
    udp_send(sockfd, message, (struct sockaddr *) &serv_addr);

    // recv EKEY
    // what if it does not receive a message ? !!!!!!!!!!!
    udp_recv(sockfd, buffer, (struct sockaddr *) &serv_addr);
    close(sockfd);
    
    // must analyse message
    if (parse_and_validate(buffer, "EKEY", &s_key, s_name, s_ip, &s_port) == 4 +1 ) {
        printf("Success. Read parameters: key: %d | name: %s | ip: %s | port %d\n", s_key, s_name, s_ip, s_port);
        printf("Must now make a tcp connection\n");
    }
    else {
        printf("Received wrong udp message\n");
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
    char ip[INET6_ADDRSTRLEN], name[PARAMETER_SIZE], command[PARAMETER_SIZE];

    if(read(sock_fd, buff, sizeof(buff)) < 0){
        printf("Client %d disconnected\n", sock_fd);
        fdDeleteFd(sock_fd);
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