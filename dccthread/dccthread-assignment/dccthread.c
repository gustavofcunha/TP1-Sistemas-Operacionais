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
    bool na_lista_espera;
    bool na_lista_prontos;
    dccthread_t* esperando;
    int stimerid;
} dccthread_t;

struct dlist *lista_prontos;
struct dlist *lista_espera;

//evento que indica quando o timer de sleep expirou
struct sigevent sleep;
//acao ao receber chamada de sistema do sleep
struct sigaction acao_sleep;

dccthread_t *gerente;
dccthread_t *principal;

int contador_thread = 0;
int sleeptid = 1;

/*ga*/
int esta_esperando_sleep(const void *e1, const void *e2, void *userdata){
	dccthread_t* e_list = (dccthread_t*) e1;
	dccthread_t* e_dummy = (dccthread_t*) e2;
	if(e_list->stimerid == e_dummy->stimerid)
		return 0;
	else
		return 1;
}
/*-ga*/

static void sleep_catcher(int sig, siginfo_t *si, void *uc){
	ucontext_t* ucp = (ucontext_t*) uc;
	dccthread_t* dummy = (dccthread_t*) malloc (sizeof(dccthread_t));
	dummy->stimerid = si->si_timerid;
   	dccthread_t* t_dependent = (dccthread_t*) dlist_find_remove(lista_espera, dummy, esta_esperando_sleep, NULL);

	if(t_dependent != NULL){
		t_dependent->na_lista_espera = 0;
		t_dependent->na_lista_prontos = 1;
		dlist_push_right(lista_prontos, t_dependent);
	}
	setcontext( ucp );
}

void dccthread_init(void (*func)(int), int param){
    acao_sleep.sa_flags = SA_SIGINFO;
	acao_sleep.sa_sigaction = sleep_catcher;
	sigaction(SIGUSR2, &acao_sleep, NULL);
	sigemptyset(&acao_sleep.sa_mask);
	//sigaddset(&sleep_act.sa_mask, SIGUSR2);
	sleep.sigev_notify = SIGEV_SIGNAL;
	sleep.sigev_signo = SIGUSR2;



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

    principal = dccthread_create("main", func, param);

    while(!dlist_empty(lista_prontos)){
		dccthread_t *temp = (dccthread_t*) malloc(sizeof(dccthread_t));
		temp = lista_prontos->head->data;

		swapcontext(&gerente->contexto, &temp->contexto);
		dlist_pop_left(lista_prontos);

        if(temp->cedido == true){
            temp->na_lista_prontos = 0;
        }
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

	nova_thread->id = contador_thread; 
    contador_thread++;
    strcpy(nova_thread->nome, name);

    nova_thread->na_lista_espera = false;
    nova_thread->na_lista_prontos = true;
    nova_thread->esperando = NULL;
    nova_thread->cedido = false;
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
int esta_esperando_exit(const void *e1, const void *e2, void *userdata){
	dccthread_t* e_list = (dccthread_t*) e1;
	dccthread_t* e_exit = (dccthread_t*) e2;

	if(e_exit == e_list->esperando){
		return 0;
    } else {
		return 1;
    }
}
/*-gu*/

/*gu-*/
void dccthread_exit(void){
    dccthread_t* atual = dccthread_self();

    dccthread_t* processo_em_espera =
    (dccthread_t *) dlist_find_remove(lista_espera, atual, esta_esperando_exit, NULL);

    if(processo_em_espera != NULL){
        processo_em_espera->na_lista_espera = false;
        processo_em_espera->na_lista_prontos = true;
        dlist_push_right(lista_prontos, processo_em_espera);
    }

    /*muda contexto para thread gerente*/
    setcontext(&gerente->contexto);
}
/*-gu*/

/*gu-*/
void dccthread_wait(dccthread_t *tid){
    dccthread_t* atual = dccthread_self();

    if(tid->na_lista_espera || tid->na_lista_prontos){
		atual->na_lista_prontos = 0;
		atual->na_lista_espera = 1;
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
	atual->stimerid = sleeptid;
	sleeptid++;

	atual->na_lista_prontos = 0;
	atual->na_lista_espera = 1;
	dlist_push_right(lista_espera, atual);

    //arma o timer criado acima (com id id_timer)
	timer_settime(id_timer, 0, &its, NULL);

	swapcontext(&atual->contexto, &gerente->contexto);
}
/*-gu*/
