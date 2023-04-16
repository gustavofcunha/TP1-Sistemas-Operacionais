#include <stdio.h>
#include <stdlib.h>

#include "dccthread.h"
#include "ucontext.h"
#include "dlist.h"


typedef struct dccthread {
    int id;
    char nome[DCCTHREAD_MAX_NAME_SIZE];
    ucontext_t contexto;
} dccthread_t;

struct dlist *lista_prontos;
struct dlist *lista_espera;

dccthread_t *gerente;
dccthread_t *principal;


void dccthread_init(void (*func)(int), int param){
    //inicializacao das estruturas internas da biblioteca

    //ga-
    lista_prontos = dlist_create();
	lista_espera = dlist_create();

	gerente = (dccthread_t *) malloc(sizeof(dccthread_t));
	getcontext(&gerente->contexto);

    gerente->contexto.uc_link = NULL;
	gerente->contexto.uc_stack.ss_sp = malloc(THREAD_STACK_SIZE);
	gerente->contexto.uc_stack.ss_size = THREAD_STACK_SIZE;
	gerente->contexto.uc_stack.ss_flags = 0;

    gerente->id = -1;
    *gerente->nome = "gerente";

    //principal = dccthread_create("principal", func, param);

    while(!dlist_empty(lista_prontos)){
		dccthread_t *temp = (dccthread_t *) malloc(sizeof(dccthread_t));
		temp = lista_prontos->head->data;

		swapcontext(&gerente->contexto, &temp->contexto);
		dlist_pop_left(lista_prontos);

        free(temp);
	}
    //-ga

    //criacao thread gerente
    //dccthread_create("gerente", dccthread_yield, 0);

    //criacao de nova thread para executar func
    //dccthread_create("principal", func, param);
}


int main(){
    return 0;
}
