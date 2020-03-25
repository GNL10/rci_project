#include "logic.h"
#include "connections.h"
#include "file_descriptors.h"
#include "io.h"


extern int fd_vec[NUM_FIXED_FD];

void entry (int key, char *name, char *ip, int port) {
    int s_key, s_port;
    char message[8];
    char buffer[BUFFER_SIZE], s_name[PARAM_SIZE], s_ip[INET6_ADDRSTRLEN];

    sprintf(message, "%s %d", "EFND", key);
    
    udp_set_send_recv(ip, port, message, buffer);

    // must analyse message
    if (parse_and_validate(buffer, "EKEY", &s_key, s_name, s_ip, &s_port) == 4 + 1 ) {
        printf("Success: key: %d | name: %s | ip: %s | port %d\n", s_key, s_name, s_ip, s_port);
    }
    else {
        printf("Received wrong udp message\n");
    }
    
}

