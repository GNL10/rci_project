#ifndef _FILE_DESCRIPTORS_H_
#define _FILE_DESCRIPTORS_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define NUM_FIXED_FD 5

#define TCP_RCV_SIZE 128

#define LISTEN_FD 0
#define UDP_FD 1
#define STDIN_FD 2
#define SUCCESSOR_FD 3
#define PREDECESSOR_FD 4

typedef struct Fd_Node_Struct{
    char buff[TCP_RCV_SIZE];
    struct Fd_Node_Struct* next;
    struct Fd_Node_Struct* prev;
    int fd;
    int buff_avai_index;            //First index avaiable in buff
    char ip[INET_ADDRSTRLEN];
    int port;
}Fd_Node;

void fdInsertNode(int fd, char ip[], int port);
void fdDeleteNode(Fd_Node* del_node);
void fdDeleteStack(void);
void fdDeleteFd(int del_fd);
void fdSetAllSelect(fd_set* rd_set);
int fdMaxFdValue(void);
int fdPollFd(fd_set* _rd_set);
Fd_Node* fdFindNode(int fd);


#endif