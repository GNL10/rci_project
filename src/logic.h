#ifndef _LOGIC_H_
#define _LOGIC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void entry (int key, char *name, char *ip, int port);
int parse_command (char *str, char *command, int *key,  char *name, char *ip, int *port);


#endif