# PAGINADOR DE MEMÓRIA - RELATÓRIO

1. Termo de compromisso

Os membros do grupo afirmam que todo o código desenvolvido para este
trabalho é de autoria própria.  Exceto pelo material listado no item
3 deste relatório, os membros do grupo afirmam não ter copiado
material da Internet nem ter obtido código de terceiros.

2. Membros do grupo e alocação de esforço

Preencha as linhas abaixo com o nome e o e-mail dos integrantes do
grupo.  Substitua marcadores `XX` pela contribuição de cada membro
do grupo no desenvolvimento do trabalho (os valores devem somar
100%).

  * Nome gustavocunha@dcc.ufmg.br 50%
  * Nome gabrimartinsjuarez@ufmg.br 50%

3. Referências bibliográficas
FIGUEIREDO, Flavio. Slides da disciplina Sistemas Operacionais, 2023.
The Open Group. Ucontext. The Single UNIX ® Specification, Version 2, 1997. Disponível em: https://pubs.opengroup.org/onlinepubs/7908799/xsh/ucontext.h.html.
PetBBC. Signal, 2023. Disponível em: https://petbcc.ufscar.br/signal/.

4. Estruturas de dados

  1. Descreva e justifique as estruturas de dados utilizadas para
     gerência das threads de espaço do usuário (partes 1, 2 e 5).
  
    * struct dccthread: para a abstração de uma thread, utilizamos esta struct, que contém os atributos necessários para gerência no espaço do usuário. Optamos por utilizar a struct por ser uma estrutura de dados autocontida e orientada a objetos, atendendo bem os objetivos do trabalho.

    * struct dlist *lista_prontos: lista encadeada fornecida pelo professor, utilizada para armazenar as threads no estado "ready". Optamos por utilizar esta estrutura de dados por ser de fácil manipulação (adição e remoção de elementos).

    * struct dlist *lista_espera: lista encadeada fornecida pelo professor, utilizada para armazenar as threads no estado "waiting". Optamos por utilizar esta estrutura de dados por ser de fácil manipulação (adição e remoção de elementos).

    * struct sigevent sleep: esta é uma estrutura para notificação de rotinas assíncronas, utilizada para eventos de "sleep". Optamos por utilizá-la uma vez que já há uma biblioteca com todas as funcionalidades já implementadas.
    
    * struct sigaction acao_sleep: esta é uma estrutura que examina e muda ações de "sleep". Optamos por utilizá-la uma vez que já há uma biblioteca com todas as funcionalidades já implementadas.

    * struct sigevent sigev: esta é uma estrutura para notificação de rotinas assíncronas, utilizada para eventos de "signal". Optamos por utilizá-la uma vez que já há uma biblioteca com todas as funcionalidades já implementadas.

    * struct sigaction sact: esta é uma estrutura que examina e muda ações de "signal". Optamos por utilizá-la uma vez que já há uma biblioteca com todas as funcionalidades já implementadas.

  2. Descreva o mecanismo utilizado para sincronizar chamadas de
     dccthread_yield e disparos do temporizador (parte 4).
