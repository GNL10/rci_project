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
#define RECV_TIMEOUT 3 // seconds

int set_udp_server();
int udp_set_send_recv (char* ip, int port, char *msg_in, char *msg_out);

int initTcpServer();
int forwardHandler(int active_fd);

int stdinHandler(void);
void udpHandler(void);
void tcpHandler(int sock_fd, Fd_Node* active_node);
int parseCommandTcp(Fd_Node* active_node, char* read_buff, int read_bytes, char *command, int *key,  char *name, char *ip, int *port);

int get_TCP_code (char *command);
void listenHandler(void);


#endif