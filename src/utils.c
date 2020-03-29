#include "utils.h"
#include "file_descriptors.h"

int appendVector(char* src, char* dest, int dest_avai_index, int num_elements){
    int i;

    for(i = 0; i < num_elements && i+dest_avai_index < TCP_RCV_SIZE; i++){
        dest[i+dest_avai_index] = src[i];
    }

    return i+dest_avai_index;           //return new dest_avai_index
}