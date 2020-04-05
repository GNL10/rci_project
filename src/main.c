#include <stdio.h> 
#include <stdlib.h> 
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/select.h>
#include "io.h"
#include "connections.h"
#include "logic.h"
#include "utils.h"
#include "file_descriptors.h"

int fd_vec[NUM_FIXED_FD] = {0, 0, 0, 0, 0};
Fd_Node* fd_stack = NULL;

server_info serv_vec[SERVERS_NUM];

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

	init_serv_vec();	// initializes the vector's keys to -1

	read_arguments(argc, (char**) argv);

	FD_ZERO(&rd_set);									// clear the descriptor set

	fd_vec[LISTEN_FD] = initTcpServer();		//Setup tcp server
	fd_vec[UDP_FD] = set_udp_server();			//Setup udp server

	//Insert current active sockets into fd stack
	fdInsertNode(fd_vec[LISTEN_FD], "\0", 0);
	fdInsertNode(fd_vec[UDP_FD], "\0", 0);
	fdInsertNode(fd_vec[STDIN_FD], "\0", 0);
	active_fd = fd_vec[STDIN_FD];

	//Main loop
	while(!end_flag){
		FD_ZERO(&rd_set);									// clear the descriptor set
		fdSetAllSelect(&rd_set);
		max_numbered_fd = fdMaxFdValue();
		if(active_fd == fd_vec[STDIN_FD])
			printf("Enter a command:\n");
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