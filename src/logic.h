#ifndef _LOGIC_H_
#define _LOGIC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "file_descriptors.h"

#define N_UDP_TRIES 3   // number of times that udp tries to establish connection
#define FIND_TIMEOUT 15 // number of seconds for the find timeout
// STDIN FUNCTIONS
void new_stdin (cmd_struct *cmd);
void entry (cmd_struct *cmd);
void sentry (cmd_struct *cmd);
void leave();
void show();
void find (cmd_struct *cmd);

// TCP FUNCTIONS
void tcpFnd(Fd_Node* active_node, int key, char* starting_ip, int starting_port, int starting_sv);
void tcpKey(Fd_Node* active_node, int key, char* owner_ip, int owner_port, int owner_of_key_sv);
void tcpSuccconf(Fd_Node* active_node);
void tcpSucc(Fd_Node* active_node, int new_succ_sv, char* new_succ_ip, int new_succ_port);
void tcpNew(Fd_Node* active_node, int entry_key_sv, char* entry_ip, int entry_port, int sender_fd);

#endif