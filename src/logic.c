#include "logic.h"
#include "connections.h"

#include "io.h"


extern int fd_vec[NUM_FIXED_FD];
extern server_info serv_vec[];

/*  init_serv_vec
    intializes all of the keys in serv_vec to -1
*/
void init_serv_vec() {
    for (int i = 0; i < SERVERS_NUM; i++)
        serv_vec[i].key = -1;
}

/*  new_stdin
    creates a new ring with only the server key
*/
void new_stdin (cmd_struct *cmd) {
    if (cmd->args_n < NEW_STDIN_NUM_ARGS+1){
        printf("The entry command needs 1 argument\nUsage: new <key>\n\n");
        return;
    }

    if (serv_vec[SELF].key != -1 || serv_vec[SUCC1].key != -1 || serv_vec[SUCC2].key != -1) {
        printf("[new_stdin] ERROR: SERVER STILL BELONGS TO RING\n\n");
        return;
    }
    serv_vec[SELF].key = cmd->key;
    return;
}

void entry (cmd_struct *cmd) {
    char send_message[8], recv_msg[UPD_RCV_SIZE];
    cmd_struct cmd_recv;

    if (cmd->args_n < ENTRY_NUM_ARGS+1) {
        printf("The entry command needs 4 arguments\nUsage: entry <key> <key_2> <ip> <port>\n\n");
        return;
    }
    
    sprintf(send_message, "%s %d", "EFND", cmd->key);
    // sends and receives message
    if (udp_set_send_recv(cmd->ip, cmd->port, send_message, recv_msg) == -1)
        return; // if udp comms fail
    // must analyse message
    if (parse_command(recv_msg, &cmd_recv) == 4 + 1) {
        if ((strcmp("EKEY", cmd_recv.action) == 0) && (validate_parameters(&cmd_recv) < 5)){
            printf("[UDP] WRONG MESSAGE: %s\n", recv_msg);
            return;
        } 
    }
    else {
        printf("[UDP] WRONG MESSAGE: %s\n", recv_msg);
        return;
    }
    // TODO check values again ?
    // TODO connect TCP !
    // key was found successfully in ring
    serv_vec[SELF].key = cmd->key;

    serv_vec[SUCC1].port = cmd_recv.port;
    strcpy(serv_vec[SUCC1].ip, cmd_recv.ip); 
    serv_vec[SUCC1].key = cmd_recv.key;
    
    printf("serv_vec[SELF].key = %d SUCC_KEY %d SUCC_IP %s SUCC_PORT %d \n\n", serv_vec[SELF].key, serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
}

/*  sentry
    enters in a ring without performing the search for a key
    returns early if any error occurs
*/
void sentry (cmd_struct *cmd) {

    if (cmd->args_n < SENTRY_NUM_ARGS+1) {
        printf("The sentry command needs 4 arguments\nUsage: sentry <key> <key_2> <ip> <port>\n\n");
        return;
    }
    if (serv_vec[SELF].key != -1 || serv_vec[SUCC1].key != -1 || serv_vec[SUCC2].key != -1) {
        printf("This server already belongs to a ring, must leave first!\n\n");
        return;
    }

    if ((fd_vec[SUCCESSOR_FD] = init_tcp_client(cmd)) == -1)
        return;

    if (write_n(fd_vec[SUCCESSOR_FD], "SUCCONF\n") == -1)
        return;

    serv_vec[SELF].key = cmd->key;
    serv_vec[SUCC1].key = cmd->key_2;
    strcpy(serv_vec[SUCC1].ip, cmd->ip);
    serv_vec[SUCC1].port = cmd->port;
}

/*  leave
    closes the TCP sockets
    sets all keys to -1
*/
void leave() {
    // must disconnect TCP connections
    init_serv_vec();    // sets all keys to -1
}

/*  show
    prints the server's state (key, ip, port) for self and successors
*/
void show() {
    printf("SELF:  key=%d ip=%s port=%d\n", serv_vec[SELF].key, serv_vec[SELF].ip, serv_vec[SELF].port);
    printf("SUCC1: key=%d ip=%s port=%d\n", serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
    printf("SUCC2: key=%d ip=%s port=%d\n\n", serv_vec[SUCC2].key, serv_vec[SUCC2].ip, serv_vec[SUCC2].port);    
}

/*  find
    searches for the key in the ring
    after finding it, prints the key, ip and port of that server
*/
void find (cmd_struct *cmd) {
    if (cmd->args_n < FIND_NUM_ARGS+1) {
        printf("The find command needs 1 argument\nUsage: find <key>\n\n");
        return;
    }
    //TODO
}

// ------------------------------------------------------------------
// TCP functions ----------------------------------------------------
// ------------------------------------------------------------------

void tcpFnd(Fd_Node* active_node, int key, char* starting_ip, int starting_port, int starting_sv){

}

void tcpKey(Fd_Node* active_node, int key, char* owner_ip, int owner_port, int owner_of_key_sv){

}

void tcpSucconf(Fd_Node* active_node){

}

void tcpSucc(Fd_Node* active_node, int new_succ_sv, char* new_succ_ip, int new_succ_port){

}

void tcpNew(Fd_Node* active_node, int entry_key_sv, char* entry_ip, int entry_port, int sender_fd){

}