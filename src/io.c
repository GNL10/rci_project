#include <arpa/inet.h>

#include "io.h"
#include "file_descriptors.h"
#include "utils.h"

extern int fd_vec[NUM_FIXED_FD];
extern server_info serv_vec[];

void read_arguments(int argc, char *argv[]) {
	int port_temp;

	// must have 2 arguments IP and PORT
	if (argc != 3) {
		printf("ERROR: WRONG ARGUMENTS!\n");
		printf("ARGUMENTS: [IP] [PORT]\n");
		exit(1);
	}
	if (validate_ip(argv[1]) == 0) {
		printf("ERROR: IP ADDRESS IS NOT VALID!\n");
		exit(1);
	}
	// argv[1] max size is tested in validate_ip
	strcpy(serv_vec[SELF].ip, argv[1]);
	if (sscanf(argv[2], "%d", &port_temp) == -1){ // check for sscanf errors if needed
		printf("ERROR: SSCANF FOR PORT FAILED!\n");
		exit(1);
	}
	if (validate_port(port_temp) == 0) {
		printf("ERROR: PORT IS NOT VALID!\n");
		exit(1);
	}
	serv_vec[SELF].port = port_temp;
}

// copied from the internet!
int validate_number(char *str) {
	while (*str) {
		if(!isdigit(*str)) //if the character is not a number, return false
			return 0;
		str++; //point to next character
	}
   return 1;
}

/* 	validate_ip
	returns: 0 if ip is invalid
			 1 if ip is valid
*/
int validate_ip(char *ip_in) { //check whether the IP is valid or not
	int dots = 0;
	char *ptr;
	char ip[INET_ADDRSTRLEN];

	if (strlen(ip_in) >= INET_ADDRSTRLEN)
		return 0;

	strcpy(ip, ip_in);
	
	if (ip == NULL)
		return 0;
	ptr = strtok(ip, "."); //cut the string using dor delimiter
	if (ptr == NULL)
		return 0;
	while (ptr) {
		int num;
		if (!validate_number(ptr)) //check whether the sub string is holding only number or not
			return 0;
		num = atoi(ptr); //convert substring to number
		if (num >= 0 && num <= 255) {
			ptr = strtok(NULL, "."); //cut the next part of the string
			if (ptr != NULL)
				dots++; //increase the dot count
		} else
			return 0;
	}
	if (dots != 3) //if the number of dots are not 3, return false
		return 0;
	return 1;
}

/* 	validate_port
	returns: 0 if port is invalid
			 1 if port is valid
*/
int validate_port(int port) {
	if (port <= 1023 || port > 65535) // 2^16-1: 65535
		return 0;
	return 1;
}

/* 	validate_key
	returns: 0 if key is invalid
			 1 if key is valid
*/
int validate_key(int key) {
	if (key < 0 || key > 15)
		return 0;
	return 1;
}



void read_command_line(char *command_line){
    fgets(command_line, BUFFER_SIZE,stdin);
}


int get_command_code(char * command){
	if (!strcmp(command, "new"))
		return NEW_STDIN;
	else if (!strcmp(command, "entry"))
		return ENTRY;
	else if (!strcmp(command, "sentry"))
		return SENTRY;
	else if (!strcmp(command, "leave"))
		return LEAVE;
	else if (!strcmp(command, "show"))
		return SHOW;
	else if (!strcmp(command, "find"))
		return FIND;
	else if (!strcmp(command, "exit"))
		return EXIT;
	else 	// invalid command
		return -1;
}