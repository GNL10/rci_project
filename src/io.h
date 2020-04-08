#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/select.h>
#include <netinet/in.h>


#define BUFFER_SIZE 64

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


void read_arguments(int argc, char *argv[]);

int validate_number(char *str);
int validate_ip(char *ip);
int validate_port(int port);
int validate_key(int key);

void read_command_line(char *command_line);
int get_command_code(char * command);
int pollFd(fd_set* _rd_set);


#endif