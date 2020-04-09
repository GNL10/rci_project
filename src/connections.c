#include <sys/time.h>
#include <netdb.h>
#include "connections.h"
#include "logic.h"
#include "utils.h"

extern int fd_vec[NUM_FIXED_FD];
extern void (*forward_tcp_cmd[5])();

extern server_info serv_vec[];
extern int key_flag;

struct sockaddr_in udp_cli_addr;    // address of udp client



// ------------------------------------------------------------------
// HANDLERS ---------------------------------------------------------
// ------------------------------------------------------------------

/*  forwardHandler
    multiplexes the functions, depending on the active_fd
    returns: 0 if the program is to continue
             1 if the program is to end
*/
int forwardHandler(int active_fd){
    Fd_Node* found_node;

    if(active_fd == fd_vec[LISTEN_FD]){
        listenHandler();
    }else if(active_fd == fd_vec[UDP_FD]){
        udpHandler();
    }else if(active_fd == fd_vec[STDIN_FD]){
        return stdinHandler();
    }else{              //Generic TCP incoming message
        found_node = fdFindNode(active_fd);
        tcpHandler(active_fd, found_node);
    }
    return 0;   // TODO delete!    
}

/*  stdinHandler
    Handles all commands given by the standard input
    reads command, parses it and validates the parameters
    returns: 0 if the program is to continue
             1 if the program is to end
*/
int stdinHandler() {
    char command_line[BUFFER_SIZE];
    cmd_struct cmd;

    // resets the string, when only enter was pressed, it was using the previously assigned action
    strcpy(cmd.action, "\0"); 
    read_command_line(command_line);
    parse_command(command_line, &cmd);
    validate_parameters(&cmd); // args_num becomes number of valid parameters!
    switch(get_command_code(cmd.action)) {
        case NEW_STDIN:     // new
            new_stdin(&cmd);
            break;

        case ENTRY:     // entry
            entry(&cmd);
            break;

        case SENTRY:     // sentry
            sentry(&cmd);
            break;

        case LEAVE:     // leave
            leave();
            break;

        case SHOW:     // show
            show();
            break;

        case FIND:     // find
            find(&cmd);
            break;

        case EXIT:    // exit
            return 1;   // changes end flag to 1 when returned
            break;

        default :   // incorrect command
            printf("Command not recognized\n");
            printf("Available commands:\n");
            printf("\tnew <key>\n\tentry <key> <key_2> <ip> <port>\n");
            printf("\tsentry <key> <key_2> <ip> <port>\n\tleave\n\tshow\n\tfind <key>\n\texit\n\n\n");
            break;     
    }
    return 0;
}

/*  udpHandler
    Handles incoming udp messages
    validates incoming message (expects: EFND key)
    searches for the key
    answers back by UDP with key (EKEY key key2 ip port)
*/
void udpHandler(void) {
    char message[UPD_RCV_SIZE];
    int n;
    socklen_t len;
    cmd_struct recv_cmd;

    memset(&udp_cli_addr, 0, sizeof(udp_cli_addr));
    len = sizeof(udp_cli_addr);

    if ((n = recvfrom(fd_vec[UDP_FD], (char *)message, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &udp_cli_addr, &len)) <= 0) {
        perror("[udpHandler] ERROR: recvfrom");
        return;
    }
    message[n] = '\0';
    if (parse_command(message, &recv_cmd) <= 1){
        printf("[UDP] WRONG MESSAGE: %s\n", message);
        return;
    }
    if ((validate_parameters(&recv_cmd) != 2) || strcmp(recv_cmd.action, "EFND") != 0) {
        printf("[UDP] WRONG MESSAGE: %s\n", message);
        return;
    }
    printf("[UDP] Read: %s\n", message);  // EFND key 

    if (serv_vec[SUCC1].key == -1) {
        sprintf(message, "EKEY %d %d %s %d", recv_cmd.key, serv_vec[SELF].key, serv_vec[SELF].ip, serv_vec[SELF].port);
        if (sendto(fd_vec[UDP_FD], (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &udp_cli_addr,  
                sizeof(udp_cli_addr)) == -1) {
            perror("ERROR:sendto");
            return;
        }
        printf("[UDP] Sent: %s\n", message);
    }
    else if (key_in_succ(recv_cmd.key) == 1) {   // if key is in successor answer right away
        sprintf(message, "EKEY %d %d %s %d", recv_cmd.key, serv_vec[SUCC1].key, serv_vec[SUCC1].ip, serv_vec[SUCC1].port);
        if (sendto(fd_vec[UDP_FD], (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &udp_cli_addr,  
                sizeof(udp_cli_addr)) == -1) {
            perror("ERROR:sendto");
            return;
        }
        printf("[UDP] Sent: %s\n", message);
    }
    else {
        key_flag = KEY_FLAG_UDP;
        // key is not in successor... Initiating find process
        sprintf(message, "FND %d %d %s %d\n", recv_cmd.key, serv_vec[SELF].key, serv_vec[SELF].ip, serv_vec[SELF].port);
        if (write_n(fd_vec[SUCCESSOR_FD], message) == -1)
            return;
    }
}

/*  tcpHandler
    reads incoming tcp messages
    parses the command and forwards to the corresponding tcp function
*/
void tcpHandler(int sock_fd, Fd_Node* active_node){
    int cmd_code, first_int, second_int, port;
    char ip[INET_ADDRSTRLEN], command[PARAM_SIZE];
    char read_buff[TCP_RCV_SIZE];
    int read_bytes = 0;

    if((read_bytes = read(sock_fd, read_buff, TCP_RCV_SIZE) )<= 0){
        printf("Client %d disconnected abruptly\n", sock_fd);
        //TODO send message to another node that needs to be disconnected
        fdDeleteFd(sock_fd);
        close(sock_fd);
        return;
    }

    //Interpret and extract the command and its arguments
    if((cmd_code = parseCommandTcp(active_node, read_buff, read_bytes, command, &first_int, &second_int, ip, &port)) < 0){
        if(cmd_code == ERR_INCOMP_MSG_TCP){
            printf("Incomplete TCP message. Storing this partial message\n");
        }else{
            printf("Error in TCP message arguments. Message discarded\n");
        }
        return;
    }

    //Forward to the corresponding command handler function
    forward_tcp_cmd[cmd_code](active_node, first_int, ip, port, second_int);
}

/*  listenHandler
    accepts a tcp client and adds it to the fd_stack
*/
void listenHandler(void){
    int new_fd;
    struct sockaddr_in new_addr;
    socklen_t size_addr = 0;

    if((new_fd = accept(fd_vec[LISTEN_FD], (struct sockaddr*)&new_addr, &size_addr)) == -1){
        perror("accept");
        exit(-1);
	}

    fdInsertNode(new_fd, inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
}

// ------------------------------------------------------------------
// UDP functions ----------------------------------------------------
// ------------------------------------------------------------------


/*  set_udp_server
    initiates the udp server
    becomes ready to receive udp messages on sockfd
*/
int set_udp_server() {
	int sockfd; 
    struct sockaddr_in servaddr; 

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    
      
    // Filling server information 
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, serv_vec[SELF].ip, &(servaddr.sin_addr));
    servaddr.sin_port = htons(serv_vec[SELF].port);
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    printf("UDP socket created and binded.\n");
    return sockfd; 
}  

/*  udp_set_send_recv
    Sets udp client
    Sends msg_in to server
    Receives msg_out from server
    Recv times out after RECV_TIMEOUT seconds
    returns: n received bytes if successful
             -1 in case of error
*/
int udp_set_send_recv (char* ip, int port, char *msg_in, char *msg_out) {
    int sockfd, n;
    struct addrinfo hints, *res;
    struct timeval timeout={RECV_TIMEOUT,0}; //set timeout
    char port_str[PORT_STR_SIZE];
    struct sockaddr_in serv_addr;
    socklen_t addr_len;

    // Creating socket file descriptor 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) { 
        perror("socket creation failed"); 
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_INET;    // IPv4
    hints.ai_socktype=SOCK_DGRAM;  // UDP socket
    sprintf(port_str, "%d", port);
    if(getaddrinfo(ip, port_str, &hints, &res)) {
        printf("ERROR: getaddrinfo failed\n");
        return -1;
    }

    // setting timeout
    if (setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval)) == -1) {  
        perror("setsockopt");
        close(sockfd);
        return -1;
    }

    if (sendto(sockfd, (const char *)msg_in, strlen(msg_in), MSG_CONFIRM, 
        (const struct sockaddr *) res->ai_addr, res->ai_addrlen) == -1) {
        perror("ERROR:sendto");
        close(sockfd);
        return -1;
    }
    printf("[UDP] Sent: %s\n", msg_in);

    if ( (n = recvfrom(sockfd, (char *)msg_out, UPD_RCV_SIZE, MSG_WAITALL, 
              (struct sockaddr *) &serv_addr, &addr_len)) == -1) { 
        perror("ERROR: recvfrom");
        close(sockfd);
        return -1;
    }
    msg_out[n] = '\0';
    printf("[UDP] Read: %s\n", msg_out);
    close(sockfd);  
    return n;
}



// ------------------------------------------------------------------
// TCP functions ----------------------------------------------------
// ------------------------------------------------------------------

/*  initTcpServer
    initiates the TCP server
    returns: server_fd
*/
int initTcpServer(){
    struct sockaddr_in local_addr;
    int server_fd;
	
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){										//Verificar se não houve erro a criar a socket
		perror("socket: ");
		exit(-1);
	}

	local_addr.sin_family = AF_INET;
    inet_pton(AF_INET, serv_vec[SELF].ip, &(local_addr.sin_addr));                         
    local_addr.sin_port = htons(serv_vec[SELF].port); 

	if(bind(server_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0){					//Verificar se não houve erro a fazer bind
		perror("bind");
		exit(-1);
	}
	printf("TCP socket created and binded.\n");

	if(listen(server_fd, 2) == -1){																//Verificar se não houve erro a fazer listen
		perror("listen");
		exit(-1);
	}
	return server_fd;
}

/*  tcp_client
    connects a tcp client so the server given on the command
    adds the socket to fd_vec[index]
    returns: -1 if error occurred
              sockfd if successful
*/
int init_tcp_client(char ip[], int port) {
    struct addrinfo hints,*res;
    int sockfd;
    char port_str[PORT_STR_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);    // TCP socket
    if (sockfd == -1) { 
        printf("[init_tcp_client] ERROR: SOCKET CREATION FAILED\n"); 
        return -1;
    }
    
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket

    sprintf(port_str, "%d", port);
    if (getaddrinfo(ip, port_str, &hints, &res) != 0){
        printf("[init_tcp_client] ERROR: GETADDRINFO FAILED\n");
        return -1;
    }
    if (connect(sockfd,res->ai_addr,res->ai_addrlen) == -1){
        printf("[init_tcp_client] ERROR: CONNECT FAILED\n");
        return -1;
    }
    fdInsertNode(sockfd, ip, port);
    printf("[init_tcp_client] Client successfully connected\n");
    return sockfd;
}

/*  write_n
    writes to socket the input message
    returns: 0 in case of success
             -1 in case of error
*/ 
int write_n (int fd, char *message) {
    char *ptr;
    ssize_t n_left = strlen(message), n_written;

    if (n_left > BUFFER_SIZE) {
        return -1;
    }
    ptr = message;

    while (n_left > 0) {
        n_written = write(fd, message, n_left);
        if (n_written == -1) {
            printf("[write_n] ERROR: WRITE FAILED\n");
            return -1;
        }
        n_left -= n_written;
        ptr += n_written;
    }
    printf("[TCP] Sent: %s", message);
    return 0;
}

/*  parseCommandTcp
    if the message is incomplete appends it into a message vector
    if complete parses the TCP message received
    returns: command code if the action correponds to a valid one
             ERR_INCOMP_MSG_TCP if message is incomplete
             ERR_ARGS_TCP if there is an error in the TCP arguments             
*/
int parseCommandTcp(Fd_Node* active_node, char* read_buff, int read_bytes, char *command, int *first_int,  int* second_int, char *ip, int *port){
    int num_args = 0;
    char args[5][PARAM_SIZE];
    char* working_buff;
    int i;

    //Find the end of message char (\n)
    for(i = 0; i < TCP_RCV_SIZE && i < read_bytes; i++){
        if(read_buff[i] == '\n'){
            read_buff[i] = '\0';                           //sscanf ignores everything after \n
            i = -1;
            break;
        }
    }
    printf("[TCP] Read: %s\n", read_buff);

    //If a \n hasn't been found
    if(i != -1){
        active_node->buff_avai_index = appendVector(read_buff, active_node->buff, active_node->buff_avai_index, read_bytes);        //Store the message in fd buffer
        return ERR_INCOMP_MSG_TCP;
    }

    //If there's part of a message in the active_node->buffer, append the newly recieved partial message to it
    if(active_node->buff_avai_index){
        active_node->buff_avai_index = appendVector(read_buff, active_node->buff, active_node->buff_avai_index, read_bytes);        //Store the message in fd buffer
        if(active_node->buff_avai_index == TCP_RCV_SIZE){           //If buffer is full
            active_node->buff_avai_index = 0;
            return ERR_ARGS_TCP;                                     //The message is too big, hence is invalid
        }
        //Here it is certain that we have a full message to work on, so we can clear the buffer associated with the active_node of the fd stack
        active_node->buff_avai_index = 0;
        working_buff = active_node->buff;       //The working_buff is this one because this buffer has the complete message
    }else{  //If the newly message recieved is completed, then the working_buff is the read_buff
        working_buff = read_buff;
    }

    //Read the message's arguments and detects if there are more than the maximum allowed arguments
    if((num_args = sscanf(working_buff, "%"PARAM_SIZE_STR"s %"PARAM_SIZE_STR"s %"PARAM_SIZE_STR"s %"PARAM_SIZE_STR"s %"PARAM_SIZE_STR"s", args[0], args[1], args[2], args[3], args[4])) == -1){
        printf("TCP stream recieved is faulty\n");
        return ERR_ARGS_TCP;
    }
    //Interpret the arguments and return the cmd_code or error
    return getTcpCommandArgs(working_buff, args[0], num_args, first_int, second_int, ip, port);
}

/*  getTcpCommandArgs
    finds the command code corresponding to the action
    returns: command code if successful
             ERR_ARGS_TCP if there was an error
*/
int getTcpCommandArgs(char message[], char action[], int num_args, int *first_int,  int* second_int, char *ip, int *port){
    int cmd_code = ERR_ARGS_TCP;
    cmd_struct cmd;

    //Poll cmd and assign arguments
	if(!strcmp(action, "FND")){
        if(num_args != FND_NUM_ARGS+1){
            return ERR_ARGS_TCP;
        }
        cmd_code = FND;
    }else if(!strcmp(action, "KEY")){
        if(num_args != KEY_NUM_ARGS+1){
            return ERR_ARGS_TCP;
        }
        cmd_code = KEY;
    }else if(!strcmp(action, "SUCCCONF")){
        if(num_args != SUCCCONF_NUM_ARGS+1){
            return ERR_ARGS_TCP;
        }
        cmd_code = SUCCCONF;
    }else if(!strcmp(action, "SUCC")){
        if(num_args != SUCC_NUM_ARGS+1){
            return ERR_ARGS_TCP;
        }
        cmd_code = SUCC;
    }else if(!strcmp(action, "NEW")){
        if(num_args != NEW_NUM_ARGS+1){
            return ERR_ARGS_TCP;
        }
        cmd_code = NEW;
    }else{
        return 0;
    }
    // SUCC and NEW command have a different command order
    // 2nd parameter is ip instead of key2
    if (cmd_code != SUCC && cmd_code != NEW) { 
        parse_command(message, &cmd);
        validate_parameters(&cmd);
        *first_int = cmd.key;
        *second_int = cmd.key_2;
        strcpy(ip, cmd.ip);
        *port = cmd.port;
    }
    else {  // SUCC and NEW commands
        if (sscanf(message, "%"PARAM_SIZE_STR"s %d %"INET_ADDRSTRLEN_STR"s %d", cmd.action, &cmd.key, cmd.ip, &cmd.port) == -1) {
            printf("ERROR: SSCANF FOR SUCC FAILED\n");
            return ERR_ARGS_TCP;
        }
        cmd.key_2 = 0;  // assign a valid key value, so validate parameters works
        validate_parameters(&cmd);
        *first_int = cmd.key;
        strcpy(ip, cmd.ip);
        *port = cmd.port;
    }
    return cmd_code;
}