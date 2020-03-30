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

// Verificar retorno de todas as system calls
// fix bind error
// atencao ao tamanho dos buffers a enviar
// ciclo apos leitura de mensagens
// voltar ao menu 
// ask what the succ (name) means, and define UPD_RCV_SIZE accordingly
//Temos de verificar cenas que recebemos? tipo o key ser mt grande
//É preciso verificar a dimensão da porta recebida?

int fd_vec[NUM_FIXED_FD] = {0, 0, 0, 0, 0};
Fd_Node* fd_stack = NULL;
int PORT;
char IP[16];

void (*forward_tcp_cmd[5])() = {tcpFnd, tcpKey, tcpSucconf, tcpSucc, tcpNew};

int main(int argc, char const *argv[]){
	int max_numbered_fd;
	char end_flag = 0;
	int active_fd;
	fd_set rd_set;			//read set

	//Ignore SIGPIPE signal which is generated when program tries to write to a recently disconnected socket
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR){	
		fprintf(stderr, "Couldn't define signal handler\n");
		exit(EXIT_FAILURE);
	}

	read_arguments(argc, (char**) argv);

	FD_ZERO(&rd_set);									// clear the descriptor set

	fd_vec[LISTEN_FD] = initTcpServer();		//Setup tcp server
	fd_vec[UDP_FD] = set_udp_server();			//Setup udp server

	//Insert current active sockets into fd stack
	fdInsertNode(fd_vec[LISTEN_FD]);
	fdInsertNode(fd_vec[UDP_FD]);
	fdInsertNode(fd_vec[STDIN_FD]);
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
		if(active_fd == fd_vec[STDIN_FD])
			printf("Enter a command:\n");

		end_flag = forwardHandler(active_fd);
	}

	//TODO close all sockets

	return 0;
}