#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024
#define PARAMETER_SIZE 64
#define IP_SIZE 128

void read_arguments(int argc, char *argv[], int *port, char *ip);

int validate_number(char *str);
int validate_ip(char *ip);
int validate_port(int port);
int validate_key(int key);
int parse_and_validate (char *buffer, char *cmd_in, int *key, char *name, char *ip, int *port);

void read_command_line(char *command_line);
int parse_command (char *str, char *command, int *key,  char *name, char *ip, int *port);
int get_command_code(char * command);
int pollFd(fd_set* _rd_set);


#endif