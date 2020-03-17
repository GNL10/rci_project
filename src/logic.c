#include "logic.h"
#include "connections.h"

void entry (int key, char *boot, char *ip, int port) {
	struct sockaddr_in addr;
	int sockfd;
	char message[8];
    char *command = "EFND";
    char buffer[BUFFER_SIZE];
    int n;
    socklen_t len;

	sockfd = set_udp_cli(ip, port, &addr);
	// send EFND
    sprintf(message, "%s %d", command, key);
    sendto(sockfd, (const char *)message, strlen(message), 
        MSG_CONFIRM, (const struct sockaddr *) &addr,  
            sizeof(addr)); 
    printf("UDP client sent message: %s\n", message);

    // recv EKEY
    // what if it does not receive a message ?
    n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &addr, &len); 
    buffer[n] = '\0'; 
    printf("UDP client received message: %s\n", buffer); 
  	// must analyse message
    close(sockfd);
}