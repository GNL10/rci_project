#include "logic.h"
#include "connections.h"
#include "file_descriptors.h"
#include "io.h"


extern int fd_vec[NUM_FIXED_FD];
extern int PORT;
extern char IP[];

void entry (int key, char *name, char *ip, int port) {
    int s_key, s_port;
    char send_message[8];
    char recv_msg[BUFFER_SIZE], command[PARAM_SIZE], s_name[PARAM_SIZE], s_ip[INET6_ADDRSTRLEN];

    sprintf(send_message, "%s %d", "EFND", key);
    if (udp_set_send_recv(ip, port, send_message, recv_msg) == -1)
        return; // if udp comms fail

    // must analyse message
    if (parse_command(recv_msg, command, &s_key, s_name, s_ip, &s_port) == 4 + 1) {
        if ((strcmp("EKEY", command) == 0) && (validate_n_parameters(5, s_key, s_ip, s_port) < 5)){
            printf("[UDP] WRONG MESSAGE: %s\n", recv_msg);
            return;
        } 
    }
    else {
        printf("[UDP] WRONG MESSAGE: %s\n", recv_msg);
        return;
    }
}

void tcpFnd(int key, char* starting_ip, int starting_port, int starting_sv){

}

void tcpKey(int key, char* owner_ip, int owner_port, int owner_of_key_sv){

}

void tcpSucconf(void){

}

void tcpSucc(int new_succ_sv, char* new_succ_ip, int new_succ_port){

}

void tcpNew(int entry_key_sv, char* entry_ip, int entry_port, int sender_fd){

}