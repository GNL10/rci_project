#ifndef _CONNECTIONS_H_
#define _CONNECTIONS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <unistd.h> 

#include "file_descriptors.h"


#define UPD_RCV_SIZE 1024

#define ERR_ARGS_TCP -2                 //Error in arguments
#define ERR_INCOMP_MSG_TCP -1           //Message incomplete

int set_udp_server(char *ip, int port);
int set_udp_cli (char *ip, int port, struct sockaddr_in *serv_addr);
int udp_send (int sockfd, char *message, struct sockaddr* addr);
int udp_recv (int sockfd, char *message, struct sockaddr* addr);

int initTcpServer(char* ip, int port);
void forwardHandler(int active_fd);

void stdinHandler(void);
void udpHandler(void);
void tcpHandler(int sock_fd, Fd_Node* active_node);
int parseCommandTcp(Fd_Node* active_node, char* read_buff, int read_bytes, char *command, int *key,  char *name, char *ip, int *port);

int get_TCP_code (char *command);
void listenHandler(void);


#endif