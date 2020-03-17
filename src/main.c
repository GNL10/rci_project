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
int num_active_fd_vec = 0;
const void (*func_ptr[3])() = {NULL, udpHandler, stdinHandler};

int main(int argc, char const *argv[]) {
	int port;
	char ip[16];
	int max_numbered_fd;
	char end_flag = 0;
	int active_fd;
	fd_set rd_set;			//read set

	read_arguments(argc, (char**) argv, &port, ip);

	FD_ZERO(&rd_set);									// clear the descriptor set

	fd_vec[LISTEN_FD] = initTcpServer(ip, port);		//Setup tcp socket
	fd_vec[UDP_FD] = set_udp_server(ip, port);

	
	num_active_fd_vec = 3;



				

	while(!end_flag){
		FD_SET(fd_vec[LISTEN_FD], &rd_set);					//set listen_fd in readset 
		FD_SET(fd_vec[UDP_FD], &rd_set);					//set udp_fd in readset
		FD_SET(fd_vec[STDIN_FD], &rd_set);					//set stdin in readset
		max_numbered_fd = maxValue(3, fd_vec[LISTEN_FD], fd_vec[UDP_FD], fd_vec[STDIN_FD], 0, 0);
		
		if(select(max_numbered_fd+1, &rd_set, NULL, NULL, NULL) == -1){
			perror("select(): ");
			exit(-1);
		}
		active_fd = pollFd(&rd_set);
		func_ptr[active_fd]();
		printf("after func_ptr\n");
		
	}


	//TODO close all sockets

	return 0;
}