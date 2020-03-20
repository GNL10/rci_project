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

// Quando um processo esta a tentar entrar no anel e dps a msg que recebe esta errada, ele deve sair do processo ou voltar ao menu?

int fd_vec[NUM_FIXED_FD] = {0, 0, 0, 0, 0};
Fd_Node* fd_stack = NULL;

int main(int argc, char const *argv[]){
	int port;
	char ip[INET6_ADDRSTRLEN];
	int max_numbered_fd;
	char end_flag = 0;
	int active_fd;
	fd_set rd_set;			//read set

	read_arguments(argc, (char**) argv, &port, ip);

	FD_ZERO(&rd_set);									// clear the descriptor set

	fd_vec[LISTEN_FD] = initTcpServer(ip, port);		//Setup tcp server
	fd_vec[UDP_FD] = set_udp_server(ip, port);			//Setup udp server

	//Insert current active sockets into fd stack
	fdInsertNode(fd_vec[LISTEN_FD]);
	fdInsertNode(fd_vec[UDP_FD]);
	fdInsertNode(fd_vec[STDIN_FD]);

	while(!end_flag){
		printf("Enter a command:\n\n");
		fdSetAllSelect(&rd_set);
		max_numbered_fd = fdMaxFdValue();
		if(select(max_numbered_fd+1, &rd_set, NULL, NULL, NULL) == -1){
			perror("select(): ");
			exit(-1);
		}

		active_fd = fdPollFd(&rd_set);
		
		forwardHandler(active_fd);
	}


	//TODO close all sockets

	return 0;
}