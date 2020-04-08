#include "logic.h"
#include "connections.h"
#include "utils.h"
#include "io.h"


extern int fd_vec[NUM_FIXED_FD];
extern server_info serv_vec[];
extern int key_flag;
extern struct sockaddr_in udp_cli_addr;

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
    char send_message[UPD_RCV_SIZE], recv_msg[UPD_RCV_SIZE];
    char tcp_message[TCP_RCV_SIZE];
    cmd_struct cmd_recv;

    if (cmd->args_n < ENTRY_NUM_ARGS+1) {
        printf("The entry command needs 4 arguments\nUsage: entry <key> <key_2> <ip> <port>\n\n");
        return;
    }
    if (serv_vec[SELF].key != -1 || serv_vec[SUCC1].key != -1 || serv_vec[SUCC2].key != -1) {
        printf("[entry] ERROR: SERVER STILL BELONGS TO RING\n\n");
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
    if (cmd_recv.key != cmd->key) {
        printf("[entry] ERROR: RECEIVED KEY DOES NOT MATCH SENT KEY\n");
        return;
    }
    if ((fd_vec[SUCCESSOR_FD] = init_tcp_client(cmd_recv.ip, cmd_recv.port)) == -1)
        return; // it tcp connection fails

    sprintf(tcp_message, "NEW %d %s %d\n", cmd_recv.key, serv_vec[SELF].ip, serv_vec[SELF].port);
    if (write_n(fd_vec[SUCCESSOR_FD], tcp_message) == -1)
        return;

    serv_vec[SELF].key = cmd_recv.key;

    serv_vec[SUCC1].key = cmd_recv.key_2;
    serv_vec[SUCC1].port = cmd_recv.port;
    strcpy(serv_vec[SUCC1].ip, cmd_recv.ip); 
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
    char message[TCP_RCV_SIZE];

    if (cmd->args_n < FIND_NUM_ARGS+1) {
        printf("The find command needs 1 argument\nUsage: find <key>\n\n");
        return;
    }
    if(serv_vec[SUCC1].key == -1) {
        printf("[find] SERVER IS ALONE\n");
        return;
    }

    // if key is in sucessor
    if(key_in_succ(cmd->key) == 1){
        printf("KEY %d %d %s %d\n", cmd->key, serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
        return;
    }
    else {
        // start a search for the key in other nodes
        sprintf(message, "FND %d %d %s %d\n", cmd->key, serv_vec[SELF].key, serv_vec[SELF].ip, serv_vec[SELF].port);
        if(write_n(fd_vec[SUCCESSOR_FD], message) == -1)
            return;
        key_flag = KEY_FLAG_STDIN; // set flag for waiting for the response 
    }
    printf("[find] searching for the key\n");
    return;
}

// ------------------------------------------------------------------
// TCP functions ----------------------------------------------------
// ------------------------------------------------------------------

void tcpFnd(Fd_Node* active_node, int key, char* starting_ip, int starting_port, int starting_sv){
    char message[TCP_RCV_SIZE];
    int temp_fd;

    printf("RECEIVED FND %d %s %d\n", key, starting_ip, starting_port);

    if (serv_vec[SUCC1].key == -1) {
        printf("[tcpFnd] SERVER IS ALONE\n");
        return;
    }
    if(key_in_succ(key) == 1) {
        if ((temp_fd = init_tcp_client(starting_ip, starting_port)) == -1)
            return;
        sprintf(message, "KEY %d %d %s %d\n", key, serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
        write_n(temp_fd, message);
        return;
    }
    
    // if key is not in SUCC1, then resend the FND message to SUCC1
    sprintf(message, "FND %d %d %s %d\n", key, starting_sv, starting_ip, starting_port);
    if (write_n(fd_vec[SUCCESSOR_FD], message) == -1)
        return;
}

void tcpKey(Fd_Node* active_node, int key, char* owner_ip, int owner_port, int owner_of_key_sv){
    char message[UPD_RCV_SIZE];

    printf("RECEIVED KEY %d %d %s %d\n", key, owner_of_key_sv, owner_ip, owner_port);
    
    if (key_flag == KEY_FLAG_EMPTY) {
        printf("ERROR: RECEIVED KEY %d %d %s %d, BUT WAS NOT EXPECTING IT!\n", key, owner_of_key_sv, owner_ip, owner_port);
        return;
    }
    else if (key_flag == KEY_FLAG_STDIN){   // if key was called by find from stdin, print the result
        printf("[tcpKey] KEY %d %d %s %d\n", key, owner_of_key_sv, owner_ip, owner_port);
        key_flag = KEY_FLAG_EMPTY;
    }
    else if (key_flag == KEY_FLAG_UDP){     // if key was called by udp, send udp message back to client
        sprintf(message, "EKEY %d %d %s %d", key, owner_of_key_sv, owner_ip, owner_port);
        sendto(fd_vec[UDP_FD], (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &udp_cli_addr,  
                sizeof(udp_cli_addr));
        key_flag = KEY_FLAG_EMPTY;
    }
    else {
        printf("ERROR KEY_FLAG = %d IS A NON RECOGNIZED VALUE\n", key_flag);
    }
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

    printf("RECEIVED NEW %d %s %d\n", entry_key_sv, entry_ip, entry_port);

    // if node is alone in ring
    if (serv_vec[SUCC1].key == -1) { 
        printf("Node is alone in ring.\n");
        serv_vec[SUCC1].key = entry_key_sv;
        strcpy(serv_vec[SUCC1].ip, entry_ip);
        serv_vec[SUCC1].port = entry_port;

        fd_vec[PREDECESSOR_FD] = active_node->fd;
        if ((fd_vec[SUCCESSOR_FD] = init_tcp_client(entry_ip, entry_port)) == -1)
            return; //TODO CHECK WHAT TO DO IF THIS FAILS
        //send new succ1 SUCCCONF
        printf("sent new successor, a succconf\n");
        sprintf(message, "SUCCCONF\n");
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
        //send new succ1 SUCCCONF
        sprintf(message, "SUCCCONF\n");
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