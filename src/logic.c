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
    if (serv_vec[SELF].key != -1 || serv_vec[SUCC1].key != -1 || serv_vec[SUCC2].key != -1) {
        printf("[new_stdin] ERROR: SERVER STILL BELONGS TO RING\n\n");
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
    snends <NEW i i.IP i.port\n >
    returns early if any error occurs
*/
void sentry (cmd_struct *cmd) {
    char message[TCP_RCV_SIZE];// TODO change the size

    if (cmd->args_n < SENTRY_NUM_ARGS+1) {
        printf("The sentry command needs 4 arguments\nUsage: sentry <key> <key_2> <ip> <port>\n\n");
        return;
    }
    if (serv_vec[SELF].key != -1 || serv_vec[SUCC1].key != -1 || serv_vec[SUCC2].key != -1) {
        printf("This server already belongs to a ring, must leave first!\n\n");
        return;
    }

    if ((fd_vec[SUCCESSOR_FD] = init_tcp_client(cmd->ip, cmd->port)) == -1)
        return;

    sprintf(message, "NEW %d %s %d\n", cmd->key, serv_vec[SELF].ip, serv_vec[SELF].port);
    if (write_n(fd_vec[SUCCESSOR_FD], message) == -1)
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
    if (serv_vec[SELF].key == -1) {
        printf("This server does not belong to a ring!\n\n");
        return;
    }
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

/*  tcpSuccconf
    predecessor must now be changed
*/
void tcpSuccconf(Fd_Node* active_node){
    printf("RECEIVED SUCCCONF\n");
    fd_vec[PREDECESSOR_FD] = active_node->fd;
}

void tcpSucc(Fd_Node* active_node, int new_succ_sv, char* new_succ_ip, int new_succ_port){
    printf("RECEIVED SUCC %d %s %d\n",new_succ_sv, new_succ_ip, new_succ_port);
    serv_vec[SUCC2].key = new_succ_sv;
    strcpy(serv_vec[SUCC2].ip, new_succ_ip);
    serv_vec[SUCC2].port = new_succ_port;
}

void tcpNew(Fd_Node* active_node, int entry_key_sv, char* entry_ip, int entry_port, int sender_fd){
    char message[TCP_RCV_SIZE];

    printf("NEW %d %s %d\n", entry_key_sv, entry_ip, entry_port);

    // if node is alone in ring
    if (serv_vec[SUCC1].key == -1) { 
        printf("Node is alone in ring.\n");
        serv_vec[SUCC1].key = entry_key_sv;
        strcpy(serv_vec[SUCC1].ip, entry_ip);
        serv_vec[SUCC1].port = entry_port;

        fd_vec[PREDECESSOR_FD] = active_node->fd;
        if ((fd_vec[SUCCESSOR_FD] = init_tcp_client(entry_ip, entry_port)) == -1)
            return; //TODO CHECK WHAT TO DO IF THIS FAILS
        //send new succ1 SUCCONF
        sprintf(message, "SUCCONF\n");
        if (write_n(fd_vec[SUCCESSOR_FD], message) == -1)
            return; //TODO

    }
    // if message was sent by successor
    else if (active_node->fd == fd_vec[SUCCESSOR_FD]) {
        //must close curr successor fd
        // connect to new node
        printf("RECEIVED MESSAGE FROM SUCC\n");
        // then change successor to new one and current successor becomes successor2
        serv_vec[SUCC2].key = serv_vec[SUCC1].key;
        strcpy(serv_vec[SUCC2].ip, serv_vec[SUCC1].ip);
        serv_vec[SUCC2].port = serv_vec[SUCC1].port;
        
        serv_vec[SUCC1].key = entry_key_sv;
        strcpy(serv_vec[SUCC1].ip, entry_ip);
        serv_vec[SUCC1].port = entry_port;
        fd_vec[SUCCESSOR_FD] = active_node->fd;

        //init connection with new node as succ1
        if ((fd_vec[SUCCESSOR_FD] = init_tcp_client(entry_ip, entry_port)) == -1)
            return; //TODO CHECK WHAT TO DO IF THIS FAILS
        //send new succ1 SUCCONF
        sprintf(message, "SUCCONF\n");
        if (write_n(fd_vec[SUCCESSOR_FD], message) == -1)
            return; //TODO
        //if predecessor exists
        if (fd_vec[PREDECESSOR_FD] != 0) {   //TODO CHECK IF IT EXISTS
            //send SUCC <new node info> to predecessor
            sprintf(message, "SUCC %d %s %d\n", entry_key_sv, entry_ip, entry_port);
            if (write_n(fd_vec[PREDECESSOR_FD], message) == -1)
                return;
        }
    }
    else {  // if message was sent by a new node, then new node becomes the predecessor
        printf("RECEIVED MESSAGE FROM NEW NODE\n");
        if (fd_vec[PREDECESSOR_FD] != 0) { // if a predecessor exists
            
            sprintf(message, "NEW %d %s %d\n", entry_key_sv, entry_ip, entry_port);
            printf("SEND %s TO CURRENT PREDECESSOR\n", message);
            if (write_n(fd_vec[PREDECESSOR_FD], message) == -1)
                return;
        }
        if (serv_vec[SUCC2].key == -1) {
            serv_vec[SUCC2].key = entry_key_sv;
            strcpy(serv_vec[SUCC2].ip, entry_ip);
            serv_vec[SUCC2].port = entry_port;
        }
        //must send predecessor message to update its successor
        fd_vec[PREDECESSOR_FD] = active_node->fd;
        if (serv_vec[SUCC1].key != -1){ // if it has a successor, then send new predecessor succ1 info
            sprintf(message, "SUCC %d %s %d\n", serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
            printf("Sending %s TO NEW PREDECESSOR\n", message);
            if (write_n(fd_vec[PREDECESSOR_FD], message) == -1)
                return;
        }
    }

}