#include "utils.h"
#include "file_descriptors.h"
#include "connections.h"

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

//Appends sr vector into dest vector, returns the new first index avaiable in dest vector
int appendVector(char* src, char* dest, int dest_avai_index, int num_elements){
    int i;

    for(i = 0; i < num_elements && i+dest_avai_index < TCP_RCV_SIZE; i++){
        dest[i+dest_avai_index] = src[i];
    }

    return i+dest_avai_index;           //return new dest_avai_index
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



/*  parse_command
	parses the given string str and divides it according to the expect command order
	returns: the number of read arguments
*/
int parse_command (char *str, cmd_struct *cmd) {
    cmd->args_n = sscanf(str, "%"PARAM_SIZE_STR"s %d %d %"INET_ADDRSTRLEN_STR"s %d", cmd->action, &cmd->key, &cmd->key_2, cmd->ip, &cmd->port);
	return cmd->args_n;
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

/*  validate_parameters
    validates parameters if displayed in the following order command key key2 ip port
    returns: 1 if first key not valid
             2 if first key valid and second key not valid
             3 if both keys valid valid and ip not valid
             4 if both keys + ip valid and port not valid
             5 if every parameter is valid
*/
int validate_parameters(cmd_struct *cmd) {
	if(cmd->args_n >= 2) {	// command + key = 2
		if(!validate_key(cmd->key)) {
			printf("ERROR: KEY IS NOT VALID\n");
			return (cmd->args_n = 1);
		}
    }
	else
		return (cmd->args_n = 1);

	if(cmd->args_n >= 3) {	// command + key + key_2 = 3
        if(!validate_key(cmd->key_2)){
			printf("ERROR: KEY_2 IS NOT VALID\n");
        	return (cmd->args_n = 2);
		}
    }
	else
		return (cmd->args_n = 2);
	
	if(cmd->args_n >= 4) { // command + key + key_2 + ip = 4
		if(!validate_ip(cmd->ip)){
        	printf("ERROR: IP ADDRESS IS NOT VALID!\n");
        	return (cmd->args_n = 3);
		}
    }
	else
		return (cmd->args_n = 3);
		
    if(cmd->args_n >= 5) { // command + key + key_2 + ip + port = 5
    	if(!validate_port(cmd->port)) {
			printf("ERROR: PORT IS NOT VALID!\n");
       		return (cmd->args_n = 4);
		}
    }
	else
		return (cmd->args_n = 4);	
	return (cmd->args_n = 5);
}

/*  key_in_succ
    checks if the key is in the next node
    returns: 0 if key is not in next node
             1 if key is in next node
*/
int key_in_succ (int key) {
    // if between self and succ1 the keys go over 15 (example. self.key = 13 and succ1.key = 2) 
    if (serv_vec[SELF].key > serv_vec[SUCC1].key){
        if (key > serv_vec[SELF].key || key <= serv_vec[SUCC1].key) // key in next node
            return 1;
    }
    else if (key > serv_vec[SELF].key && key <= serv_vec[SUCC1].key)// key in next node
        return 1;
    return 0;   // if key is not in next node
}

void shiftArray(char* array, int start_idx, int vec_size){
	int i;

	for(i = start_idx; i < vec_size; i++){
		array[i-start_idx] = array[i];
		array[i] = '0';
	}

}