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


#define UPD_RCV_SIZE 33 // strlen (EKEY 11 12 123.123.123.123 11111\0) = 33! biggest udp message that can be sent
//      TCP_RCV_SIZE is in file_descriptors.h

#define ERR_ARGS_TCP -2                 //Error in arguments
#define ERR_INCOMP_MSG_TCP -1           //Message incomplete
#define RECV_TIMEOUT 3 // seconds

// STDIN
#define NEW_STDIN_NUM_ARGS 1
#define ENTRY_NUM_ARGS 4
#define SENTRY_NUM_ARGS 4
#define LEAVE_NUM_ARGS 0
#define SHOW_NUM_ARGS 0
#define FIND_NUM_ARGS 1
#define EXIT_NUM_ARGS 0

#define NEW_STDIN 0
#define ENTRY 1
#define SENTRY 2
#define LEAVE 3
#define SHOW 4
#define FIND 5
#define EXIT 6


// TCP
#define FND_NUM_ARGS 4
#define KEY_NUM_ARGS 4
#define SUCCCONF_NUM_ARGS 0
#define SUCC_NUM_ARGS 3
#define NEW_NUM_ARGS 3

#define FND 0
#define KEY 1
#define SUCCCONF 2
#define SUCC 3
#define NEW 4

int set_udp_server();
int udp_set_send_recv (char* ip, int port, char *msg_in, char *msg_out);

int initTcpServer();
int forwardHandler(int active_fd);

int stdinHandler(void);
void udpHandler(void);
void tcpHandler(int sock_fd, Fd_Node* active_node);

int parseCommandTcp(Fd_Node* active_node, char* read_buff, int read_bytes, char *command, int *first_int,  int* second_int, char *ip, int *port);
int getTcpCommandArgs(Fd_Node* active_node, char** args, int num_args, int *first_int,  int* second_int, char *ip, int *port);

void listenHandler(void);


#endif