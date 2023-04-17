#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  

#include "dlist.h"
#include "dccthread.h"
#include "ucontext.h"


typedef struct dccthread {
    int id;
    char nome[DCCTHREAD_MAX_NAME_SIZE];
    ucontext_t contexto;
    bool cedido;

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
    gerente->cedido = false;

    principal = dccthread_create("principal", func, param);

    while(!dlist_empty(lista_prontos)){
		dccthread_t *temp = (dccthread_t*) malloc(sizeof(dccthread_t));
		temp = lista_prontos->head->data;

		swapcontext(&gerente->contexto, &temp->contexto);
		dlist_pop_left(lista_prontos);

        free(temp);
	}
    //-ga
}

//ga-
dccthread_t* dccthread_create(const char *name, void (*func)(int), int param){
    dccthread_t *nova_thread = (dccthread_t*) malloc(sizeof(dccthread_t));
    getcontext(&nova_thread->contexto);

    nova_thread->contexto.uc_link = &gerente->contexto;
	nova_thread->contexto.uc_stack.ss_sp = malloc(THREAD_STACK_SIZE);
	nova_thread->contexto.uc_stack.ss_size = THREAD_STACK_SIZE;
	nova_thread->contexto.uc_stack.ss_flags = 0;

	nova_thread->id = contador_thread; contador_thread++;
    *nova_thread->nome = name;

    dlist_push_right(lista_prontos, nova_thread);
    makecontext(&nova_thread->contexto, (void*) func, 1, param);

    return nova_thread;
}
//-ga

//gu-
void dccthread_yield(void){
    //obtem contexto da thread atual e coloca no final da lista
    dccthread_t* contexto_atual = dccthread_self();
    contexto_atual->cedido = true;
    dlist_push_right(lista_prontos, contexto_atual);

    //muda de contexto para thread gerente
    swapcontext(&contexto_atual->contexto, &gerente->contexto);
}
//-gu

//gu-
dccthread_t* dccthread_self(void){
    //retorna contexto da thread em execucao
    return lista_prontos->head->data;
}
//-gu


//gu-
const char* dccthread_name(dccthread_t *tid){
    //retorna contexto da thread em execucao
    return tid->nome;
}
//-gu

