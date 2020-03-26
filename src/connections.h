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

#define UPD_RCV_SIZE 1024
#define TCP_RCV_SIZE 128
#define RECV_TIMEOUT 3 // seconds

int set_udp_server();
int udp_set_send_recv (char* ip, int port, char *msg_in, char *msg_out);

int initTcpServer();
void forwardHandler(int active_fd);

void stdinHandler(void);
void udpHandler(void);
void tcpHandler(int sock_fd);
int parseCommandTcp(char *buff, char *command, int *key,  char *name, char *ip, int *port);
int get_TCP_code (char *command);
void listenHandler(void);

#endif