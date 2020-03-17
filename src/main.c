#include "io.h"
#include "connections.h"
#include "logic.h"



int main(int argc, char const *argv[]) {
	int port;
	char ip[16];
	char command[MAX_LINE];
	int code = -1;

	int predecessor_fd;
	int server_fd;
	int successor_fd;

	char end_flag = 0;

	read_arguments(argc, (char**) argv, &port, ip);

	
	while (1) {
		code = read_command(command);
		switch (code) {
			case 0:
				printf("new\n");
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
				break;
			case 1:
				printf("entry\n");
				entry(16, "boot??", ip, port);
				break;
			case 2:
				printf("sentry\n");
				break;
			case 3:
				printf("leave\n");
				break;
			case 4:
				printf("show\n");
				break;
			case 5:
				printf("find\n");
				break;			
			case -1:
				printf("Error: Command not recognized.\n");				
				break;
			case -2:
				printf("exit\n");
				break;
			default:
				break;
		}
	}
	return 0;
}