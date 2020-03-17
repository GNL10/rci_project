#include "logic.h"
#include "connections.h"

void entry (int key, char *boot, char *ip, int port) {
	int sockfd;
	char message[8];
    char *command = "EFND";
    char buffer[BUFFER_SIZE];
    
	sockfd = set_udp_cli(ip, port);
	
	// send EFND
    sprintf(message, "%s %d", command, key);
    udp_send(sockfd, ip, port, message);

    // recv EKEY
    // what if it does not receive a message ?
    udp_recv(sockfd, ip, port, buffer);
  	// must analyse message
   

    int succ_key, succ_port;
    char recv_command[BUFFER_SIZE], succ_name[BUFFER_SIZE], succ_ip[BUFFER_SIZE];

    if (parse_command(buffer, recv_command, &succ_key, succ_name, succ_ip, &succ_port) == 0) {
        printf("UDP message not valid: %s\n", buffer);
        close(sockfd);
        exit(0);
    }
    // validate command and args
    close(sockfd);
}

int parse_command (char *str, char *command, int *key,  char *name, char *ip, int *port) {
    int n;
    n = sscanf(str, "%s %d %s %s %d", command, key, name, ip, port);
    return n;
}