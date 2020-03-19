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

#define BUFFER_SIZE 1024

#define TCP_RCV_SIZE 128

int set_udp_server(char *ip, int port);
int set_udp_cli (char *ip, int port, struct sockaddr_in *serv_addr);
void udp_send (int sockfd, char *message, struct sockaddr* addr);
int udp_recv (int sockfd, char *message, struct sockaddr* addr);

int initTcpServer(char* ip, int port);
void forwardHandler(int active_fd);

#endif