#include <stdio.h>
#include "dccthread.h"
#include "ucontext.h"

typedef struct dccthread{
    int id;
    char nome[DCCTHREAD_MAX_NAME_SIZE];
    ucontext_t contexto;
    

}dccthread_t;

void dccthread_init(void (*func)(int), int param){
    //inicializacao das estruturas internas da biblioteca
    

    //criacao thread gerente
    dccthread_create("gerente", dccthread_yield, 0);

    //criacao de nova thread para executar func
    dccthread_create("principal", func, param);
}


int main(){
    return 0;
}