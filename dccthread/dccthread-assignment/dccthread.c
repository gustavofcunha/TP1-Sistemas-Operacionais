#include <stdio.h>
#include <stdlib.h>

#include "dlist.h"
#include "dccthread.h"
#include "ucontext.h"


typedef struct dccthread {
    int id;
    char nome[DCCTHREAD_MAX_NAME_SIZE];
    ucontext_t contexto;
} dccthread_t;

struct dlist *lista_prontos;
struct dlist *lista_espera;

dccthread_t *gerente;
dccthread_t *principal;

int contador_thread = 0;

void dccthread_init(void (*func)(int), int param){
    //inicializacao das estruturas internas da biblioteca

    //ga-
    lista_prontos = dlist_create();
	lista_espera = dlist_create();

	gerente = (dccthread_t*) malloc(sizeof(dccthread_t));
	getcontext(&gerente->contexto);

    gerente->contexto.uc_link = NULL;
	gerente->contexto.uc_stack.ss_sp = malloc(THREAD_STACK_SIZE);
	gerente->contexto.uc_stack.ss_size = THREAD_STACK_SIZE;
	gerente->contexto.uc_stack.ss_flags = 0;

    gerente->id = -1;
    *gerente->nome = "gerente";

    principal = dccthread_create("principal", func, param);

    while(!dlist_empty(lista_prontos)){
		dccthread_t *temp = (dccthread_t*) malloc(sizeof(dccthread_t));
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

//ga
dccthread_t* dccthread_create(const char *name, void (*func)(int), int param){
    dccthread_t *nova_thread = (dccthread_t*) malloc(sizeof(dccthread_t));
    getcontext(&nova_thread->contexto);

    nova_thread->contexto.uc_link = &gerente->contexto;
	nova_thread->contexto.uc_stack.ss_sp = malloc(THREAD_STACK_SIZE);
	nova_thread->contexto.uc_stack.ss_size = THREAD_STACK_SIZE;
	nova_thread->contexto.uc_stack.ss_flags = 0;

	nova_thread->id = contador_thread; contador_thread++;
    *nova_thread->nome = "gerente";

    dlist_push_right(lista_prontos, nova_thread);
    makecontext(&nova_thread->contexto, (void*) func, 1, param);

    return nova_thread;
}
//-ga
