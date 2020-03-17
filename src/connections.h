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

int set_udp_server(char *ip, int port);
int set_udp_cli (char *ip, int port);
int initTcpServer(char* ip, int port);

#endif