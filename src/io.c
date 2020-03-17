#include "io.h"

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
int validate_ip(char *ip) { //check whether the IP is valid or not
	int num, dots = 0;
	char *ptr;
	
	if (ip == NULL)
		return 0;
	ptr = strtok(ip, "."); //cut the string using dor delimiter
	if (ptr == NULL)
		return 0;
	while (ptr) {
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
	if (port <= 1023 || port > 65535) {	// 2^16-1: 65535
		return 0;
	}
	return 1;
}

int read_command(char *command) {
	char buffer[MAX_LINE];

    printf("Enter a command:\n");
    fgets(buffer, sizeof(buffer),stdin);
    if (sscanf(buffer, "%s", command) == 1) {
        if (!strcmp(command, "new")) {
            return 0;
        }
        else if (!strcmp(command, "entry")) {
        	return 1;
        }
        else if (!strcmp(command, "sentry")) {
            return 2;
        }
        else if (!strcmp(command, "leave")) {
            return 3;
        }
        else if (!strcmp(command, "show")) {
            return 4;
        }
        else if (!strcmp(command, "find")) {
            return 5;
        }
        else if (!strcmp(command, "exit")) {
            return -2;
        }
        else {
            return -1;
        }
    }
    return -1;
}