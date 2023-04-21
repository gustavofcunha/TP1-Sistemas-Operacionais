#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <ucontext.h>
#include "dlist.h"
#include "dccthread.h"


typedef struct dccthread {
    int id;
    char nome[DCCTHREAD_MAX_NAME_SIZE];
    ucontext_t contexto;
    bool cedido;
    bool esta_na_lista_espera;
    bool esta_na_lista_prontos;
    dccthread_t* esperando;
    int id_timer;

} dccthread_t;

struct dlist *lista_prontos;
struct dlist *lista_espera;

//evento que indica quando o timer de sleep expirou
struct sigevent sleep;

dccthread_t *gerente;
dccthread_t *principal;

int contador_thread = 0;
int sleeptid = 1;

void dccthread_init(void (*func)(int), int param){
    /*ga-*/
    lista_prontos = dlist_create();
	lista_espera = dlist_create();

	gerente = (dccthread_t*) malloc(sizeof(dccthread_t));
	getcontext(&gerente->contexto);

    gerente->contexto.uc_link = NULL;
	gerente->contexto.uc_stack.ss_sp = malloc(THREAD_STACK_SIZE);
	gerente->contexto.uc_stack.ss_size = THREAD_STACK_SIZE;
	gerente->contexto.uc_stack.ss_flags = 0;

    gerente->id = -1;
    strcpy(gerente->nome, "gerente");
    gerente->cedido = false;
    gerente->esperando = NULL;

    principal = dccthread_create("principal", func, param);

    while(!dlist_empty(lista_prontos)){
		dccthread_t *temp = (dccthread_t*) malloc(sizeof(dccthread_t));
		temp = lista_prontos->head->data;

		swapcontext(&gerente->contexto, &temp->contexto);
		dlist_pop_left(lista_prontos);

        if(temp->cedido == 1){
            temp->esta_na_lista_prontos = 0;
        }
        //free(temp);
	}
    exit(EXIT_SUCCESS);
    /*-ga*/
}

/*ga-*/
dccthread_t* dccthread_create(const char *name, void (*func)(int), int param){
    dccthread_t *nova_thread = (dccthread_t*) malloc(sizeof(dccthread_t));
    getcontext(&nova_thread->contexto);

    nova_thread->contexto.uc_link = &gerente->contexto;
	nova_thread->contexto.uc_stack.ss_sp = malloc(THREAD_STACK_SIZE);
	nova_thread->contexto.uc_stack.ss_size = THREAD_STACK_SIZE;
	nova_thread->contexto.uc_stack.ss_flags = 0;

	nova_thread->id = contador_thread; contador_thread++;
    strcpy(nova_thread->nome, name);
    nova_thread->esta_na_lista_espera = false;
    nova_thread->esta_na_lista_prontos = true;
    nova_thread->esperando = NULL;

    dlist_push_right(lista_prontos, nova_thread);
    makecontext(&nova_thread->contexto, (void*) func, 1, param);

    return nova_thread;
}
/*-ga*/

/*gu-*/
void dccthread_yield(void){
    /*obtem contexto da thread atual e coloca no final da lista*/
    dccthread_t* contexto_atual = dccthread_self();
    contexto_atual->cedido = true;
    dlist_push_right(lista_prontos, contexto_atual);

    /*muda de contexto para thread gerente*/
    swapcontext(&contexto_atual->contexto, &gerente->contexto);
}
/*-gu*/

/*gu-*/
dccthread_t* dccthread_self(void){
    /*retorna contexto da thread em execucao*/
    return lista_prontos->head->data;
}
/*-gu*/


/*gu-*/
/*retorna nome da thread recebida como parametro*/
const char* dccthread_name(dccthread_t *tid){
    return tid->nome;
}
/*-gu*/

/*gu-*/
/*verifica se e1 esta esperando e2*/
int esta_esperando(const void *e1, const void *e2, void *userdata){
	dccthread_t* e_list = (dccthread_t*) e1;
	dccthread_t* e_exit = (dccthread_t*) e2;

	if(e_exit == e_list->esperando){
		return 0;
    }
	else{
		return 1;
    }
}
/*-gu*/

/*gu-*/
void dccthread_exit(void){
    dccthread_t* atual = dccthread_self();

    dccthread_t* processo_em_espera =
    (dccthread_t *) dlist_find_remove(lista_espera, atual, esta_esperando, NULL);

    if(processo_em_espera != NULL){
        processo_em_espera->esta_na_lista_espera = false;
        processo_em_espera->esta_na_lista_prontos = true;
        dlist_push_right(lista_prontos, processo_em_espera);
    }

    /*muda contexto para thread gerente*/
    setcontext(&gerente->contexto);
}
/*-gu*/

/*gu-*/
void dccthread_wait(dccthread_t *tid){
    dccthread_t* atual = dccthread_self();

    if(tid->esta_na_lista_espera || tid->esta_na_lista_prontos){
		atual->esta_na_lista_prontos = 0;
		atual->esta_na_lista_espera = 1;
		atual->esperando = tid;
		dlist_push_right(lista_espera, atual);

		swapcontext(&atual->contexto, &gerente->contexto);
	}

}
/*-gu*/

/*gu-*/
void dccthread_sleep(struct timespec ts){
    //cria novo timer
	timer_t id_timer;
	timer_create(CLOCK_REALTIME, &sleep, &id_timer);

    struct itimerspec its;
	its.it_value = ts;

	dccthread_t* atual = dccthread_self();
	atual->id_timer = sleeptid;
	sleeptid++;

	atual->esta_na_lista_prontos = 0;
	atual->esta_na_lista_espera = 1;
	dlist_push_right(lista_espera, atual);

    //arma o timer criado acima (com id id_timer)
	timer_settime(id_timer, 0, &its, NULL);

	swapcontext(&atual->contexto, &gerente->contexto);
}
/*-gu*/
