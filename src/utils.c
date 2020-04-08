#include "utils.h"
#include "file_descriptors.h"
#include "connections.h"
#include "io.h"

extern server_info serv_vec[];
//Appends sr vector into dest vector, returns the new first index avaiable in dest vector
int appendVector(char* src, char* dest, int dest_avai_index, int num_elements){
    int i;

    for(i = 0; i < num_elements && i+dest_avai_index < TCP_RCV_SIZE; i++){
        dest[i+dest_avai_index] = src[i];
    }

    return i+dest_avai_index;           //return new dest_avai_index
}

/*  parse_command
	parses the given string str and divides it according to the expect command order
	returns: the number of read arguments
*/
int parse_command (char *str, cmd_struct *cmd) {
    cmd->args_n = sscanf(str, "%"PARAM_SIZE_STR"s %d %d %"PARAM_SIZE_STR"s %d", cmd->action, &cmd->key, &cmd->key_2, cmd->ip, &cmd->port);
	return cmd->args_n;
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
	if (cmd->port == serv_vec[SELF].port && (strcmp(serv_vec[SELF].ip, cmd->ip) == 0)) { // avoid sending a message to itself
        printf("ERROR: IP and PORT MATCH THE ONES FROM THIS PROCESS!\n");
        return (cmd->args_n = 2);
    }
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