#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE 1024

void read_arguments(int argc, char *argv[], int *port, char *ip);
int validate_number(char *str);
int validate_ip(char *ip);
int validate_port(int port);
int read_command(char *command);

#endif