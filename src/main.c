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

int main(int argc, char const *argv[]) {
	int port;
	char ip[16];
	char command[MAX_LINE];
	int code = -1;
	int max_numbered_fd;
	char end_flag = 0;

	fd_set rd_set;			//read set

	read_arguments(argc, (char**) argv, &port, ip);

	FD_ZERO(&rd_set);									// clear the descriptor set

	fd_vec[LISTEN_FD] = initTcpSocket(ip, port);		//Setup tcp socket
	FD_SET(fd_vec[LISTEN_FD], &rd_set);					//set listen_fd in readset 
	
	FD_SET(fd_vec[STDIN_FD], &rd_set);					//set stdin in readset


	max_numbered_fd = maxValue(5, fd_vec[LISTEN_FD], 0, 0, 0, 0);			

	while(!end_flag){
		if(select(max_numbered_fd, &rd_set, NULL, NULL, NULL) == -1){
			perror("select(): ");
			exit(-1);
		}

		
	}


	//TODO close all sockets

	return 0;
}