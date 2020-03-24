#include <arpa/inet.h>

#include "io.h"
#include "file_descriptors.h"

extern int fd_vec[NUM_FIXED_FD];

void read_arguments(int argc, char *argv[], int *port, char *ip) {

	// must have 2 arguments IP and PORT
	if (argc != 3) {
		printf("ERROR: WRONG ARGUMENTS!\n");
		printf("ARGUMENTS: [IP] [PORT]\n");
		exit(1);
	}

	strcpy(ip, argv[1]); // needs to be here, argv[1] is destroyed by the function validate_ip
	if (validate_ip(argv[1]) == 0) {
		printf("ERROR: IP ADDRESS IS NOT VALID!\n");
		exit(1);
	}

	sscanf(argv[2], "%d", port); // check for sscanf errors if needed
	if (validate_port(*port) == 0) {
		printf("ERROR: PORT IS NOT VALID!\n");
		exit(1);
	}
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

// copied from the internet!
int validate_ip(char *ip_in) { //check whether the IP is valid or not
	int dots = 0;
	char *ptr;
	char ip[INET6_ADDRSTRLEN];

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

int validate_port(int port) {
	// TODO: ask teacher about the upper limit !!
	if (port <= 1023 || port > 65535) // 2^16-1: 65535
		return 0;
	return 1;
}

int validate_key(int key) {
	if (key < 0 || key > 16)
		return 0;
	return 1;
}

int parse_and_validate (char *buffer, char *cmd_in, int *key, char *name, char *ip, int *port) {
    char cmd[BUFFER_SIZE];
	int n_args;

    n_args = parse_command(buffer, cmd, key, name, ip, port);
    if (strcmp(cmd, cmd_in)) {
        printf("Wrong command. UDP connection was expecting %s\n", cmd_in);
        printf("Received instead %s\n", buffer);
        exit(0);
    }
    if(!validate_ip(ip)) {
        printf("ERROR: IP ADDRESS IS NOT VALID!\n");
        exit(0);
    }
    if(!validate_port(*port)) {
        printf("ERROR: PORT IS NOT VALID!\n");
        exit(0);
    }
    if(!validate_key(*key)) {
        printf("ERROR: KEY IS NOT VALID\n");
        exit(0);
    }
	return n_args;
}

void read_command_line(char *command_line){
    fgets(command_line, BUFFER_SIZE,stdin);
}

int parse_command (char *str, char *command, int *key,  char *name, char *ip, int *port) {
    return sscanf(str, "%"PARAM_SIZE_STR"s %d %"PARAM_SIZE_STR"s %"PARAM_SIZE_STR"s %d", command, key, name, ip, port);
}

int get_command_code(char * command){
	if (!strcmp(command, "new"))
		return 0;
	else if (!strcmp(command, "entry"))
		return 1;
	else if (!strcmp(command, "sentry"))
		return 2;
	else if (!strcmp(command, "leave"))
		return 3;
	else if (!strcmp(command, "show"))
		return 4;
	else if (!strcmp(command, "find"))
		return 5;
	else if (!strcmp(command, "exit"))
		return -2;
	else 	// invalid command
		return -1;
}