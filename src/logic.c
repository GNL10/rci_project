#include "logic.h"
#include "connections.h"
#include "file_descriptors.h"
#include "io.h"


extern int fd_vec[NUM_FIXED_FD];

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

