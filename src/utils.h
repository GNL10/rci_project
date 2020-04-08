#ifndef _UTILS_H_
#define _UTILS_H_

#include<stdlib.h>
#include<stdio.h>
#include <arpa/inet.h>

#define SELF 0
#define SUCC1 1
#define SUCC2 2

#define SERVERS_NUM 3   // number of servers in server_vec

#define PARAM_SIZE 10       // must be PARAM_SIZE_STR + 1 !!
#define PARAM_SIZE_STR "9" // must be PARAM_SIZE - 1 !!

typedef struct {
    char action[PARAM_SIZE];
    int key;
    int key_2;
    char ip[INET_ADDRSTRLEN];
    int port;
    int args_n;
} cmd_struct;


typedef struct {
    char ip[INET_ADDRSTRLEN];
    int key;
    int port;
    
}server_info;

int appendVector(char* src, char* dest, int dest_avai_index, int num_elements);

int parse_command (char *str, cmd_struct *cmd);
int validate_parameters(cmd_struct *cmd);
int key_in_succ (int key);

#endif