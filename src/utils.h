#ifndef _UTILS_H_
#define _UTILS_H_

#include<stdlib.h>
#include<stdio.h>
#include <arpa/inet.h>

int appendVector(char* src, char* dest, int dest_avai_index, int num_elements);
int getIpFromArg(char* arg, char* ip);
void getPortFromArg(char* arg, int* port);

#endif