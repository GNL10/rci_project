#ifndef _LOGIC_H_
#define _LOGIC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void entry (int key, char *name, char *ip, int port);
int parse_command (char *str, char *command, int *key,  char *name, char *ip, int *port);

void tcpFnd(int key, char* starting_ip, int starting_port, int starting_sv);
void tcpKey(int key, char* owner_ip, int owner_port, int owner_of_key_sv);
void tcpSucconf(void);
void tcpSucc(int new_succ_sv, char* new_succ_ip, int new_succ_port);
void tcpNew(int entry_key_sv, char* entry_ip, int entry_port, int sender_fd);

#endif