#ifndef _FILE_DESCRIPTORS_H_
#define _FILE_DESCRIPTORS_H_

#include <stdlib.h>
#include <stdio.h>

#define NUM_FIXED_FD 5

#define LISTEN_FD 0
#define UDP_FD 1
#define STDIN_FD 2
#define SUCCESSOR_FD 3
#define PREDECESSOR_FD 4

typedef struct Fd_Node_Struct{
    int fd;
    struct Fd_Node_Struct* next;
    struct Fd_Node_Struct* prev;
}Fd_Node;

Fd_Node* newNode(int fd);
void deleteNode(Fd_Node* del_node);
void delStack();

#endif