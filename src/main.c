#include "io.h"
#include "connections.h"
#include "logic.h"
// melo TCP
// goncalo UDP

int main(int argc, char const *argv[]) {
	int port;
	char ip[16];

	int predecessor_fd;
	int server_fd;
	int successor_fd;

	char end_flag = 0;

	read_arguments(argc, (char**) argv, &port, ip);

	server_fd = initTcpSocket(ip, port);	//Setup tcp socket

	while(!end_flag){
		printf("Ready to accept connections\n");	
		/*if((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &size_addr)) == -1){	//Verficiar se não houve erro a fazer accept
			perror("accept");
			exit(-1);
		}*/
	}

	close(server_fd);
	//TODO fechar todas as sockets

	return 0;
}