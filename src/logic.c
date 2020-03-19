#include "logic.h"
#include "connections.h"
#include "io.h"
#include "file_descriptors.h"

extern int fd_vec[NUM_FIXED_FD];

void stdinHandler() {
    char command_line[MAX_LINE];
    int key, port, args_num;
    char name[MAX_LINE], ip[MAX_LINE], command[MAX_LINE];
    
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
    char buffer[BUFFER_SIZE], s_name[MAX_LINE], s_ip[MAX_LINE];
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
    parse_and_validate(buffer, 4, "EKEY", &s_key, s_name, s_ip, &s_port);
    printf("Success. Read parameters: key: %d | name: %s | ip: %s | port %d\n", s_key, s_name, s_ip, s_port);
    printf("Must now make a tcp connection\n");
}

void udpHandler() {
    char message[MAX_LINE];
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



