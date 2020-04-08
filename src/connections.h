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
#include "io.h"
#include "utils.h"

#define UPD_RCV_SIZE 64 
//      TCP_RCV_SIZE is in file_descriptors.h

#define PORT_STR_SIZE 6

#define ERR_ARGS_TCP -2                 //Error in arguments
#define ERR_INCOMP_MSG_TCP -1           //Message incomplete
#define RECV_TIMEOUT 3 // seconds

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

#define KEY_FLAG_EMPTY 0
#define KEY_FLAG_STDIN 1
#define KEY_FLAG_UDP 2

int set_udp_server();
int udp_set_send_recv (char* ip, int port, char *msg_in, char *msg_out);

int initTcpServer();
int forwardHandler(int active_fd);

int stdinHandler(void);
void udpHandler(void);
void tcpHandler(int sock_fd, Fd_Node* active_node);

int parseCommandTcp(Fd_Node* active_node, char* read_buff, int read_bytes, char *command, int *first_int,  int* second_int, char *ip, int *port);
int getTcpCommandArgs(Fd_Node* active_node, char args[][PARAM_SIZE], int num_args, int *first_int,  int* second_int, char *ip, int *port);

void listenHandler(void);
int init_tcp_client(char ip[], int port);

int write_n (int fd, char *message);
#endif