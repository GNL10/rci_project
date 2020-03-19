#include "file_descriptors.h"

extern Fd_Node* fd_stack;

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

//Inserts a new node in the head of the fd stack
void fdInsertNode(int fd){
    Fd_Node* new_node = NULL;

    new_node = (Fd_Node*)malloc(sizeof(Fd_Node));
    if(new_node == NULL){
        fprintf(stderr, "Error in memory allocation");
        exit(EXIT_FAILURE);
    }
    new_node->fd = fd;

    new_node->prev = NULL;
    new_node->next = fd_stack;

    fd_stack = new_node;
}

//Deletes the del_node in fd stack
void fdDeleteNode(Fd_Node* del_node){

    if(del_node == fd_stack){                   //Se for a head
        if(fd_stack->next != NULL){             //Se for a head e se não for o único elemento da lista
            fd_stack->next->prev = NULL;
        }
        fd_stack = del_node->next;
    }else if(del_node->next != NULL){           //Se for no meio da lista
        del_node->prev->next = del_node->next;
        del_node->next->prev = del_node->prev;
    }else if(del_node->next == NULL){           //Se for a cauda
        del_node->prev->next = NULL;
    }

    free(del_node);

}

//Deletes the fd stack
void fdDeleteStack(void){
    Fd_Node* aux;
    Fd_Node* next;

    for(aux = fd_stack; aux != NULL; aux = next){
        next = aux->next;
        free(aux);
    }
    fd_stack = NULL;
}

//Deletes the node which has del_fd fd
void fdDeleteFd(int del_fd){
    Fd_Node* aux;

    for(aux = fd_stack; aux != NULL; aux = aux->next){
        if(aux->fd == del_fd){
            fdDeleteNode(aux);
            return;
        }
    }   
}

//FD_SET All active file descriptors/sockets
void fdSetAllSelect(fd_set* rd_set){
    Fd_Node* aux;

    FD_ZERO(rd_set);									// clear the descriptor set
    for(aux = fd_stack; aux != NULL; aux = aux->next){
        FD_SET(aux->fd, rd_set);
    }
}

//Returns the max-valued fd of all nodes in fd stack
int fdMaxFdValue(void){
    Fd_Node* aux;
    int max = 0;

    //Determinar o fd max do stack fd
    for(aux = fd_stack; aux != NULL; aux = aux->next){
        max = MAX(max, aux->fd);
    }
    return max;
}

int fdPollFd(fd_set* _rd_set){
	Fd_Node* aux;

	for(aux = fd_stack; aux != NULL; aux = aux->next){
		if(FD_ISSET(aux->fd, _rd_set)){
			return aux->fd;
		}
	}
	fprintf(stderr, "No fd active");
	return -1;
}