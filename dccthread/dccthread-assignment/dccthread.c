#include <stdio.h>
#include "dccthread.h"
#include "ucontext.h"

typedef struct dccthread {
    int id;
    char nome[DCCTHREAD_MAX_NAME_SIZE];
    ucontext_t contexto;
} dccthread_t;

void dccthread_init(void (*func)(int), int param){
    //inicializacao das estruturas internas da biblioteca

    //ga-
    ready_list = dlist_create();
	waiting_list = dlist_create();

	gerente = (dccthread_t *) malloc (sizeof(dccthread_t));
	getcontext(&gerente->contexto);

    gerente->contexto.uc_link = NULL;
	gerente->contexto.uc_stack.ss_sp = malloc (THREAD_STACK_SIZE);
	gerente->contexto.uc_stack.ss_size = THREAD_STACK_SIZE;
	gerente->contexto.uc_stack.ss_flags = 0;

    gerente->id = -1;
    gerente->nome = "gerente";

    //-ga

    //criacao thread gerente
    //dccthread_create("gerente", dccthread_yield, 0);

    //criacao de nova thread para executar func
    //dccthread_create("principal", func, param);
}


int main(){
    return 0;
}
