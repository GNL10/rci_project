#include "utils.h"
#include "file_descriptors.h"
#include "connections.h"
#include "io.h"

//Appends sr vector into dest vector, returns the new first index avaiable in dest vector
int appendVector(char* src, char* dest, int dest_avai_index, int num_elements){
    int i;

    for(i = 0; i < num_elements && i+dest_avai_index < TCP_RCV_SIZE; i++){
        dest[i+dest_avai_index] = src[i];
    }

    return i+dest_avai_index;           //return new dest_avai_index
}

//Extracts the Ip from a string <some_integer>.ip
int getIpFromArg(char* arg, char* ip){
    int i;
    unsigned char flag = 0;
    int j = 0;

    for(i = 0; arg[i] != '0' && i < strlen(arg); i++){
        if(arg[i] == '.'){
            flag = 1;
        }
        if(flag){
            if(j >= INET_ADDRSTRLEN){
                return ERR_ARGS_TCP;
            }
            ip[j] = arg[i];
            j++;
        }
    }
    ip[j] = '\0';
    return 1;
}

//Extracts the port from a string <some_integer>.port
void getPortFromArg(char* arg, int* port){
    int i;
    unsigned char flag = 0;
    int j = 0;
    char aux[PARAM_SIZE];

    for(i = 0; arg[i] != '0' && i < strlen(arg); i++){
        if(arg[i] == '.'){
            flag = 1;
        }
        if(flag){
            aux[j] = arg[i];
            j++;
        }
    }
    aux[j] = '\0';
    *port = atoi(aux);
}