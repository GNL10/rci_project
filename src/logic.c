#include "logic.h"
#include "connections.h"
#include "io.h"
#include "file_descriptors.h"

extern int fd_vec[NUM_FIXED_FD];

void entry (int key, char *boot, char *ip, int port) {
	int sockfd;
	char message[8];
    char *command = "EFND";
    char buffer[BUFFER_SIZE];
    
    sprintf(message, "%s %d", command, key);
	
	sockfd = set_udp_cli(ip, port);
	// send EFND i
    //udp_send(sockfd, ip, port, message);

    // recv EKEY
    // what if it does not receive a message ? !!!!!!!!!!!
    //udp_recv(sockfd, ip, port, buffer);
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

void stdinHandler() {
    char command[MAX_LINE];
    int code;

    code = read_command(command);

    printf("Inserted command was: %s\nCode: %d\n", command, code);
}

int parse_command (char *str, char *command, int *key,  char *name, char *ip, int *port) {
    return sscanf(str, "%s %d %s %s %d", command, key, name, ip, port);
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
    sendto(fd_vec[UDP_FD], (const char *)"THIS MESSAGE WAS SENT BY THE SERVER", strlen("THIS MESSAGE WAS SENT BY THE SERVER"), 
        MSG_CONFIRM, (const struct sockaddr *) &cli_addr,  
            sizeof(cli_addr)); 
}

void listenHandler(){
    int new_fd;
    struct sockaddr_in new_addr;
    socklen_t size_addr = 0;

    if((new_fd = accept(fd_vec[LISTEN_FD], (struct sockaddr*)&new_addr, &size_addr)) == -1){	    //Verficiar se não houve erro a fazer accept
        perror("accept");
        exit(-1);
	}
    fdInsertNode(new_fd);
}