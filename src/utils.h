#ifndef _UTILS_H_
#define _UTILS_H_

#include<stdlib.h>
#include<stdio.h>
#include <arpa/inet.h>
#include <ctype.h>

#define SELF 0
#define SUCC1 1
#define SUCC2 2

#define SERVERS_NUM 3   // number of servers in server_vec

#define PARAM_SIZE 32       // must be PARAM_SIZE_STR + 1 !!
#define PARAM_SIZE_STR "31" // must be PARAM_SIZE - 1 !!
#define INET_ADDRSTRLEN_STR "15" // must be INET_ADDRSTRLEN - 1

#define BUFFER_SIZE 128

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



void read_arguments(int argc, char *argv[]);
int appendVector(char* src, char* dest, int dest_avai_index, int num_elements);

void read_command_line(char *command_line);
int get_command_code(char * command);
int parse_command (char *str, cmd_struct *cmd);

int validate_number(char *str);
int validate_ip(char *ip);
int validate_port(int port);
int validate_key(int key);
int validate_parameters(cmd_struct *cmd);

int key_in_succ (int key);

void shiftArray(char* array, int start_idx, int vec_size);

#endif