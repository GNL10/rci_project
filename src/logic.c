#include "logic.h"
#include "connections.h"
#include "utils.h"


extern int fd_vec[NUM_FIXED_FD];
extern server_info serv_vec[];
extern int key_flag;
extern struct sockaddr_in udp_cli_addr;
extern struct timeval find_timeout;

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
    // Initiate tcp and udp servers
    fd_vec[LISTEN_FD] = initTcpServer();		//Setup tcp server
	fd_vec[UDP_FD] = set_udp_server();			//Setup udp server
	fdInsertNode(fd_vec[LISTEN_FD], "\0", 0);
	fdInsertNode(fd_vec[UDP_FD], "\0", 0);
    serv_vec[SELF].key = cmd->key;
    return;
}

/*  entry
    sends UDP message to a node in the ring
    receives the UDP message
    initiates the tcp client
    sends TCP message NEW key ip port
    updates SELF key and SUCC1 info
    returns early if any error occurs
*/
void entry (cmd_struct *cmd) {
    char send_message[UPD_RCV_SIZE], recv_msg[UPD_RCV_SIZE];
    char tcp_message[TCP_RCV_SIZE];
    cmd_struct cmd_recv;
    int flag = 0;

    if (cmd->args_n < ENTRY_NUM_ARGS+1) {
        printf("The entry command needs 4 arguments\nUsage: entry <key> <key_2> <ip> <port>\n\n");
        return;
    }
    if (serv_vec[SELF].key != -1 || serv_vec[SUCC1].key != -1 || serv_vec[SUCC2].key != -1) {
        printf("[entry] ERROR: SERVER STILL BELONGS TO RING\n\n");
        return;
    }
    
    sprintf(send_message, "%s %d", "EFND", cmd->key);
    
    for (int i = 0; i < N_UDP_TRIES; i++) {
        // sends and receives message
        if (udp_set_send_recv(cmd->ip, cmd->port, send_message, recv_msg) != -1) {
            flag = 1;
            break;
        }
    }
    if (flag == 0) {
        printf("[entry] ERROR: %d CONSECUTIVE TRIES FAILED\n", N_UDP_TRIES);
        return;
    }
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
    if (cmd_recv.key == cmd_recv.key_2) {
        printf("[entry ERROR: KEY1 and KEY2 MATCH!\n]");
        return;
    }
    if (cmd_recv.key != cmd->key) {
        printf("[entry] ERROR: RECEIVED KEY DOES NOT MATCH SENT KEY\n");
        return;
    }
    if ((fd_vec[SUCCESSOR_FD] = init_tcp_client(cmd_recv.ip, cmd_recv.port)) == -1)
        return; // it tcp connection fails

    sprintf(tcp_message, "NEW %d %s %d\n", cmd_recv.key, serv_vec[SELF].ip, serv_vec[SELF].port);
    if (write_n(fd_vec[SUCCESSOR_FD], tcp_message) == -1) {
        return;
    }

    // Initiate tcp and udp servers
    fd_vec[LISTEN_FD] = initTcpServer();		//Setup tcp server
	fd_vec[UDP_FD] = set_udp_server();			//Setup udp server
	fdInsertNode(fd_vec[LISTEN_FD], "\0", 0);
	fdInsertNode(fd_vec[UDP_FD], "\0", 0);

    serv_vec[SELF].key = cmd_recv.key;
    serv_vec[SUCC1].key = cmd_recv.key_2;
    serv_vec[SUCC1].port = cmd_recv.port;
    strcpy(serv_vec[SUCC1].ip, cmd_recv.ip); 
}

/*  sentry
    enters in a ring without performing the search for a key
    sends <NEW i i.IP i.port\n >
    returns early if any error occurs
*/
void sentry (cmd_struct *cmd) {
    char message[TCP_RCV_SIZE];

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

    // Initiate tcp and udp servers
    fd_vec[LISTEN_FD] = initTcpServer();		//Setup tcp server
	fd_vec[UDP_FD] = set_udp_server();			//Setup udp server
	fdInsertNode(fd_vec[LISTEN_FD], "\0", 0);
	fdInsertNode(fd_vec[UDP_FD], "\0", 0);

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
    int i;
    if (serv_vec[SELF].key == -1) {
        printf("This server does not belong to a ring!\n\n");
        return;
    }
    // set the fd_vec to -1 ?
    // must disconnect TCP connections
    serv_vec[SELF].key = -1;
    serv_vec[SUCC1].key = -1;

    serv_vec[SUCC2].key = -1;   

    for(i = 0; i < NUM_FIXED_FD; i++){
        if(fd_vec[i] != -1 && i != STDIN_FD){
            fdDeleteFd(fd_vec[i]);
            fd_vec[i] = -1;
        }
    }
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
    if (serv_vec[SELF].key == -1) {
        printf("This server does not belong to a ring\n");
        return;
    }
    if(serv_vec[SUCC1].key == -1) { // alone in ring
        printf("KEY %d %d %s %d\n", cmd->key, serv_vec[SELF].key, serv_vec[SELF].ip, serv_vec[SELF].port);
        return;
    }

    // if key is in sucessor
    if(key_in_succ(cmd->key) == 1){
        printf("KEY %d %d %s %d\n", cmd->key, serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
        return;
    }
    else {
        // start a search for the key in other nodes
        if (key_flag != KEY_FLAG_EMPTY) {
            printf("[find] FIND CANNOT BE INITIATED, STILL WAITING FOR A DIFFERENT FIND TO FINISH\n");
            return;
        }
        sprintf(message, "FND %d %d %s %d\n", cmd->key, serv_vec[SELF].key, serv_vec[SELF].ip, serv_vec[SELF].port);
        if(write_n(fd_vec[SUCCESSOR_FD], message) == -1)
            return;
        key_flag = KEY_FLAG_STDIN; // set flag for waiting for the response 
        find_timeout.tv_sec = FIND_TIMEOUT;
    }
    return;
}

// ------------------------------------------------------------------
// TCP functions ----------------------------------------------------
// ------------------------------------------------------------------

/*  tcpFnd
    checks if key is in next node
    if it is, connects to server that originated the find and sends message with succ info
    if not, resends the received message to successor
*/
void tcpFnd(Fd_Node* active_node, int key, char* starting_ip, int starting_port, int starting_sv){
    char message[TCP_RCV_SIZE];
    int temp_fd;

    if (serv_vec[SUCC1].key == -1) {
        printf("[tcpFnd] SERVER IS ALONE\n");
        return;
    }
    if(key_in_succ(key) == 1) {
        if ((temp_fd = init_tcp_client(starting_ip, starting_port)) == -1)
            return;
        sprintf(message, "KEY %d %d %s %d\n", key, serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
        if (write_n(temp_fd, message) == -1) {
            fdDeleteFd(temp_fd);
            return;
        }
        fdDeleteFd(temp_fd);
        return;
    }
    else {
        // if key is not in SUCC1, then resend the FND message to SUCC1
        sprintf(message, "FND %d %d %s %d\n", key, starting_sv, starting_ip, starting_port);
        if (write_n(fd_vec[SUCCESSOR_FD], message) == -1)
            return;
        find_timeout.tv_sec = FIND_TIMEOUT;
    }
}

/*  tcpKey
    if was not expecting key message, prints error
    if find was initiated by stdin, prints the info on stdin
    if find was initiated by a udp message, replies info with udp message
*/
void tcpKey(Fd_Node* active_node, int key, char* owner_ip, int owner_port, int owner_of_key_sv){
    char message[UPD_RCV_SIZE];
    
    if (key_flag == KEY_FLAG_EMPTY) {
        printf("ERROR: RECEIVED KEY %d %d %s %d, BUT WAS NOT EXPECTING IT!\n", key, owner_of_key_sv, owner_ip, owner_port);
        return;
    }
    else if (key_flag == KEY_FLAG_STDIN){   // if key was called by find from stdin, print the result
        printf("KEY %d %d %s %d\n", key, owner_of_key_sv, owner_ip, owner_port);
        key_flag = KEY_FLAG_EMPTY;
        find_timeout.tv_sec = 0;
    }
    else if (key_flag == KEY_FLAG_UDP){     // if key was called by udp, send udp message back to client
        sprintf(message, "EKEY %d %d %s %d", key, owner_of_key_sv, owner_ip, owner_port);
        if(sendto(fd_vec[UDP_FD], (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &udp_cli_addr,  
                sizeof(udp_cli_addr)) == -1) {
            perror("ERROR:sendto");
            key_flag = KEY_FLAG_EMPTY;
            find_timeout.tv_sec = 0;
            return;
        }
        if(DEBUG_MODE) printf("[UDP] Sent: %s\n", message);
        key_flag = KEY_FLAG_EMPTY;
        find_timeout.tv_sec = 0;
    }
    else {
        printf("[ERROR] KEY_FLAG = %d IS A NON RECOGNIZED VALUE\n", key_flag);
    }
}


/*  tcpSuccconf
    changes predecessor to the active_node->fd
*/
void tcpSuccconf(Fd_Node* active_node){
    if (fd_vec[PREDECESSOR_FD] != -1) {
        if(close(fd_vec[PREDECESSOR_FD]) < 0) 
            perror("Close:");
    } 
    fd_vec[PREDECESSOR_FD] = active_node->fd;
}

/*  tcpSucc
    updates the info on the second successor
*/
void tcpSucc(Fd_Node* active_node, int new_succ_sv, char* new_succ_ip, int new_succ_port){
    if(new_succ_sv == serv_vec[SELF].key) {
        if(DEBUG_MODE) printf("RECEIVED SUCC WITH OWN INFO\n");
        serv_vec[SUCC2].key = -1;
        return;
    }
    serv_vec[SUCC2].key = new_succ_sv;
    strcpy(serv_vec[SUCC2].ip, new_succ_ip);
    serv_vec[SUCC2].port = new_succ_port;
}

/*  tcpNew
    if node is alone in ring, then the node that sent the message becomes successor
    if message was sent by successor
        connect to new node and send it succconf
        if predecessor exists, send it a succ with info for the predecessors new SUCC2
    if message was sent by a new node, integrates new node in ring
*/
void tcpNew(Fd_Node* active_node, int entry_key_sv, char* entry_ip, int entry_port, int sender_fd){
    char message[TCP_RCV_SIZE];

    // if node is alone in ring
    if (serv_vec[SUCC1].key == -1) { 
        serv_vec[SUCC1].key = entry_key_sv;
        strcpy(serv_vec[SUCC1].ip, entry_ip);
        serv_vec[SUCC1].port = entry_port;

        fd_vec[PREDECESSOR_FD] = active_node->fd;
        if ((fd_vec[SUCCESSOR_FD] = init_tcp_client(entry_ip, entry_port)) == -1)
            return; //TODO CHECK WHAT TO DO IF THIS FAILS
        //send new succ1 SUCCCONF
        sprintf(message, "SUCCCONF\n");
        if (write_n(fd_vec[SUCCESSOR_FD], message) == -1)
            return; //TODO

    }
    // if message was sent by successor
    else if (active_node->fd == fd_vec[SUCCESSOR_FD]) {
        //must close curr successor fd
        // connect to new node
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
        if (fd_vec[PREDECESSOR_FD] != -1) {   //TODO CHECK IF IT EXISTS
            //send SUCC <new node info> to predecessor
            sprintf(message, "SUCC %d %s %d\n", entry_key_sv, entry_ip, entry_port);
            if (write_n(fd_vec[PREDECESSOR_FD], message) == -1)
                return;
        }
    }
    else {  // if message was sent by a new node, then new node becomes the predecessor
        if (fd_vec[PREDECESSOR_FD] != -1) { // if a predecessor exists
            
            sprintf(message, "NEW %d %s %d\n", entry_key_sv, entry_ip, entry_port);
            if (write_n(fd_vec[PREDECESSOR_FD], message) == -1)
                return;
            fdDeleteFd(fd_vec[PREDECESSOR_FD]); // close the current predecessor
            fd_vec[PREDECESSOR_FD] = -1;
        }
        if (serv_vec[SUCC2].key == -1) {    // if new is the third node in the ring it becomes the succ2
            serv_vec[SUCC2].key = entry_key_sv;
            strcpy(serv_vec[SUCC2].ip, entry_ip);
            serv_vec[SUCC2].port = entry_port;
        }
        //must send predecessor message to update its successor
        fd_vec[PREDECESSOR_FD] = active_node->fd;
        if (serv_vec[SUCC1].key != -1){ // if it has a successor, then send new predecessor succ1 info
            sprintf(message, "SUCC %d %s %d\n", serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
            if (write_n(fd_vec[PREDECESSOR_FD], message) == -1)
                return;
        }
    }

}