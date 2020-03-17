#include "file_descriptors.h"

extern Fd_Node* fd_stack;

Fd_Node* newNode(int fd){
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

void deleteNode(Fd_Node* del_node){
    Fd_Node* prev;

    prev = del_node->prev;

   
    if(del_node == fd_stack){               //Se for a head
        fd_stack = del_node->next;
        if(fd_stack->prev != NULL){         //Se for a head e se não for o único elemento da lista
            fd_stack->prev = NULL;
        }
    }else if(del_node->next != NULL){       //Se for no meio da lista ou na cauda
        prev->next = fd_stack->next;
        if(fd_stack->next != NULL){         //Se não for a cauda
            fd_stack->next->prev = prev;
        }
    }

    free(del_node);

}

void delStack(){
    Fd_Node* aux;
    Fd_Node* next;

    for(aux = fd_stack; aux != NULL; aux = next){
        next = aux->next;
        free(aux);
    }
}