#include "logic.h"
#include "connections.h"
#include "io.h"

void entry (int key, char *boot, char *ip, int port) {
	int sockfd;
	char message[8];
    char *command = "EFND";
    char buffer[BUFFER_SIZE];
    
    sprintf(message, "%s %d", command, key);
	
	sockfd = set_udp_cli(ip, port);
	// send EFND i
    udp_send(sockfd, ip, port, message);

    // recv EKEY
    // what if it does not receive a message ? !!!!!!!!!!!
    udp_recv(sockfd, ip, port, buffer);
  	close(sockfd);
  	
  	// must analyse message
   
    int succ_key, succ_port;
    char recv_command[BUFFER_SIZE], succ_name[BUFFER_SIZE], succ_ip[BUFFER_SIZE];

    if (parse_command(buffer, recv_command, &succ_key, succ_name, succ_ip, &succ_port) != 5) {
        printf("Message not valid: %s\n", buffer);
        exit(0);
    }
    if (strcmp(recv_command, "EKEY")) {
    	printf("Wrong command. UDP connection was expecting EKEY\n");
    	printf("Received instead %s\n", buffer);
    	exit(0);
    }
    if(validate_ip(succ_ip) == 0) {
    	printf("ERROR: IP ADDRESS IS NOT VALID!\n");
    	exit(0);
    }
    if(validate_port(succ_port) == 0) {
    	printf("ERROR: PORT IS NOT VALID!\n");
    	exit(0);
    }
    // needs to validate key!!!
    printf("Success, read command: %s\n", buffer);

    // validate command and args
    
}

int parse_command (char *str, char *command, int *key,  char *name, char *ip, int *port) {
    return sscanf(str, "%s %d %s %s %d", command, key, name, ip, port);
}

