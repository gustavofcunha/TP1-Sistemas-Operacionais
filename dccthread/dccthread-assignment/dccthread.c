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

dccthread_t *gerente;
dccthread_t *principal;

int contador_thread = 0;
int sleeptid = 1;

struct sigevent sleep;
struct sigaction acao_sleep;
struct sigevent sigev;
struct sigaction sact;

timer_t timerid;
struct itimerspec its;

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

static void sleep_catcher(int sig, siginfo_t *si, void *uc){
    sigprocmask(SIG_BLOCK, &sact.sa_mask, NULL);

	ucontext_t* ucp = (ucontext_t*) uc;
	dccthread_t* dummy = (dccthread_t*) malloc(sizeof(dccthread_t));
	dummy->stimerid = si->si_timerid;
   	dccthread_t* t_dependent = (dccthread_t*) dlist_find_remove(lista_espera, dummy, esta_esperando_sleep, NULL);

	if(t_dependent != NULL){
		t_dependent->na_lista_espera = false;
		t_dependent->na_lista_prontos = true;
		dlist_push_right(lista_prontos, t_dependent);
	}

	setcontext(ucp);
}

static void timer_catcher(int sig, siginfo_t *si, void *uc){
    dccthread_yield();
}

void dccthread_init(void (*func)(int), int param){
    sact.sa_flags = SA_SIGINFO;
	sact.sa_sigaction = timer_catcher;

	sigaction(SIGUSR1, &sact, NULL);

	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;

    sigemptyset(&sact.sa_mask);
	sigaddset(&sact.sa_mask, SIGUSR1);

	timer_create(CLOCK_PROCESS_CPUTIME_ID, &sigev, &timerid);

	its.it_value.tv_nsec = 10000000;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

    acao_sleep.sa_flags = SA_SIGINFO;
	acao_sleep.sa_sigaction = sleep_catcher;

	sigaction(SIGUSR2, &acao_sleep, NULL);
	sigemptyset(&acao_sleep.sa_mask);

	sleep.sigev_notify = SIGEV_SIGNAL;
	sleep.sigev_signo = SIGUSR2;

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

    sigprocmask(SIG_BLOCK, &sact.sa_mask, NULL);
    while(!dlist_empty(lista_prontos)){
		dccthread_t *temp = (dccthread_t*) malloc(sizeof(dccthread_t));
		temp = lista_prontos->head->data;

        timer_settime(timerid, 0, &its, NULL);

		swapcontext(&gerente->contexto, &temp->contexto);
		dlist_pop_left(lista_prontos);

        if(temp->cedido == false){
            temp->na_lista_prontos = false;
        }
	}

    exit(EXIT_SUCCESS);
}

/*ga-*/
dccthread_t* dccthread_create(const char *name, void (*func)(int), int param){
    dccthread_t *nova_thread;
	nova_thread = (dccthread_t*) malloc(sizeof(dccthread_t));
    getcontext(&nova_thread->contexto);

    sigprocmask(SIG_BLOCK, &sact.sa_mask, NULL);

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
    sigprocmask(SIG_UNBLOCK, &sact.sa_mask, NULL);

    return nova_thread;
}
/*-ga*/

/*gu-*/
void dccthread_yield(void){
    sigprocmask(SIG_BLOCK, &sact.sa_mask, NULL);
    dccthread_t* contexto_atual = dccthread_self(); /*obtem contexto da thread atual e coloca no final da lista*/

    contexto_atual->cedido = true;

    dlist_push_right(lista_prontos, contexto_atual);
    swapcontext(&contexto_atual->contexto, &gerente->contexto); /*muda de contexto para thread gerente*/
    sigprocmask(SIG_UNBLOCK, &sact.sa_mask, NULL);
}
/*-gu*/

/*gu-*/
dccthread_t* dccthread_self(void){
    return lista_prontos->head->data; /*retorna contexto da thread em execucao*/
}
/*-gu*/

/*gu-*/
const char* dccthread_name(dccthread_t *tid){
    return tid->nome; /*retorna nome da thread recebida como parametro*/
}
/*-gu*/

/*gu-*/
void dccthread_exit(void){
    dccthread_t* atual = dccthread_self();
    sigprocmask(SIG_BLOCK, &sact.sa_mask, NULL);

    dccthread_t* processo_em_espera =
    (dccthread_t *) dlist_find_remove(lista_espera, atual, esta_esperando_exit, NULL);

    if(processo_em_espera != NULL){
        processo_em_espera->na_lista_espera = false;
        processo_em_espera->na_lista_prontos = true;

        dlist_push_right(lista_prontos, processo_em_espera);
    }

    sigprocmask(SIG_UNBLOCK, &sact.sa_mask, NULL);
    setcontext(&gerente->contexto); /*muda contexto para thread gerente*/
}
/*-gu*/

/*gu-*/
void dccthread_wait(dccthread_t *tid){
    sigprocmask(SIG_BLOCK, &sact.sa_mask, NULL);
    dccthread_t* atual = dccthread_self();

    if(tid->na_lista_espera || tid->na_lista_prontos){
		atual->na_lista_prontos = false;
		atual->na_lista_espera = true;
		atual->esperando = tid;
		dlist_push_right(lista_espera, atual);

		swapcontext(&atual->contexto, &gerente->contexto);
	}

    sigprocmask(SIG_UNBLOCK, &sact.sa_mask, NULL);
}
/*-gu*/

/*gu-*/
void dccthread_sleep(struct timespec ts){
    sigprocmask(SIG_BLOCK, &sact.sa_mask, NULL);

	timer_t id_timer; //cria novo timer
	timer_create(CLOCK_REALTIME, &sleep, &id_timer);

    struct itimerspec its;
	its.it_value = ts;

	dccthread_t* atual = dccthread_self();
	atual->stimerid = sleeptid;
	sleeptid++;

	atual->na_lista_prontos = false;
	atual->na_lista_espera = true;
	dlist_push_right(lista_espera, atual);

	timer_settime(id_timer, 0, &its, NULL); //arma o timer criado acima (com id id_timer)

	swapcontext(&atual->contexto, &gerente->contexto);
    sigprocmask(SIG_UNBLOCK, &sact.sa_mask, NULL);
}
/*-gu*/
