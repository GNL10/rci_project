#include <stdio.h> 
#include <stdlib.h> 
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/select.h>
#include "connections.h"
#include "logic.h"
#include "utils.h"
#include "file_descriptors.h"

int fd_vec[NUM_FIXED_FD] = {-1, -1, -1, -1, -1};	// -1 so it is different from stin's fd = 0
Fd_Node* fd_stack = NULL;

server_info serv_vec[SERVERS_NUM] = {{.key = -1}, {.key = -1}, {.key = -1}};

int key_flag = KEY_FLAG_EMPTY; 	// KEY_FLAG_EMPTY if not waiting for a key
					// KEY_FLAG_STDIN if waiting for a stdin find call
					// KEY_FLAG_UDP if waiting for a find called by udp

void (*forward_tcp_cmd[5])() = {tcpFnd, tcpKey, tcpSuccconf, tcpSucc, tcpNew};

int main(int argc, char const *argv[]){
	int max_numbered_fd;
	char end_flag = 0;
	int active_fd;
	fd_set rd_set;			//read set
	struct sigaction action;


	memset(&action, 0, sizeof(action));
	action.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &action, NULL) == -1) {
		printf("ERROR: sigaction failed!");
		exit(EXIT_FAILURE);
	}

	read_arguments(argc, (char**) argv);

	FD_ZERO(&rd_set);									// clear the descriptor set

	//Insert current active sockets into fd stack
	fd_vec[STDIN_FD] = 0; // STDIN file descriptor is 0		
	fdInsertNode(fd_vec[STDIN_FD], "\0", 0);
	active_fd = fd_vec[STDIN_FD];

	//Main loop
	while(!end_flag){
		FD_ZERO(&rd_set);									// clear the descriptor set
		fdSetAllSelect(&rd_set);
		max_numbered_fd = fdMaxFdValue();
		if(select(max_numbered_fd+1, &rd_set, NULL, NULL, NULL) == -1){
			perror("select(): ");
			exit(-1);
		}
		active_fd = fdPollFd(&rd_set);
		end_flag = forwardHandler(active_fd);
	}

	//TODO close all sockets

	return 0;
}