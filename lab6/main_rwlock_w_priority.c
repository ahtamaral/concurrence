/*

Programação concorrente 2024.2

Laboratório 6: Implementação de padrão leitor consumidos com prioridade para escrita

Ideia:

    Vamos modificar a função tarefa de forma que operações de escrita (inserção e remoção) 
tenham prioridade sobre operações de leitura. Para tal, iremos usar uma variável que conta 
quantos escritores estão aguardando para realizar suas operações. Quando um escritor solicita o acesso, 
ele impede que novos leitores iniciem suas operações, mas permite que leitores já ativos terminem. 
Essa abordagem garante que a escrita tenha prioridade.

Serão feitas duas modificações principais:

1- Adição de variável global para contar quantos escritores estão aguardando.
2- Bloquear novos leitores quando há escritores esperando.

Compilação:

$ gcc main_rwlock_w_priority.c cods-lab6/list_int.c -o main -lpthread

*/
#include <stdio.h>
#include <stdlib.h>
#include "cods-lab6/list_int.h"
#include <pthread.h>
#include "cods-lab6/timer.h"

#define QTDE_OPS 10000000 //quantidade de operacoes sobre a lista (insercao, remocao, consulta)
#define QTDE_INI 100 //quantidade de insercoes iniciais na lista
#define MAX_VALUE 100 //valor maximo a ser inserido

//lista compartilhada iniciada 
struct list_node_s* head_p = NULL; 
//qtde de threads no programa
int nthreads;

//rwlock de exclusao mutua
pthread_rwlock_t rwlock;

//Variável que conta o número de escritores esperando
int writers_waiting = 0;

//Mutex para proteger o contador de escritores
pthread_mutex_t writer_mutex = PTHREAD_MUTEX_INITIALIZER;

//tarefa das threads
void* tarefa(void* arg) {
   long int id = (long int) arg;
   int op;
   int in, out, read; 
   in = out = read = 0;

   //realiza operacoes de consulta (98%), insercao (1%) e remocao (1%)
   for(long int i = id; i < QTDE_OPS; i += nthreads) {
      op = rand() % 100;
      if(op < 98) { 
         // Operação de leitura (com prioridade para escrita)
         pthread_mutex_lock(&writer_mutex);
         while(writers_waiting > 0) {
            // Se houver escritores esperando, evitamos novas leituras
            pthread_mutex_unlock(&writer_mutex);
            // Busy-wait para permitir que escritores tenham prioridade
            pthread_mutex_lock(&writer_mutex);
         }
         pthread_mutex_unlock(&writer_mutex);

         // A leitura é permitida se não houver escritores esperando
         pthread_rwlock_rdlock(&rwlock); /* lock de LEITURA */    
         Member(i % MAX_VALUE, head_p);   /* Ignore return value */
         pthread_rwlock_unlock(&rwlock);     
         read++;
      } else if(98 <= op && op < 99) {
         // Operação de inserção (escrita)
         pthread_mutex_lock(&writer_mutex);
         writers_waiting++; // Um escritor está aguardando
         pthread_mutex_unlock(&writer_mutex);

         pthread_rwlock_wrlock(&rwlock); /* lock de ESCRITA */    
         Insert(i % MAX_VALUE, &head_p);  /* Ignore return value */
         pthread_rwlock_unlock(&rwlock);     

         pthread_mutex_lock(&writer_mutex);
         writers_waiting--; // Escritor terminou
         pthread_mutex_unlock(&writer_mutex);

         in++;
      } else if(op >= 99) {
         // Operação de remoção (escrita)
         pthread_mutex_lock(&writer_mutex);
         writers_waiting++; // Um escritor está aguardando
         pthread_mutex_unlock(&writer_mutex);

         pthread_rwlock_wrlock(&rwlock); /* lock de ESCRITA */     
         Delete(i % MAX_VALUE, &head_p);  /* Ignore return value */
         pthread_rwlock_unlock(&rwlock);     

         pthread_mutex_lock(&writer_mutex);
         writers_waiting--; // Escritor terminou
         pthread_mutex_unlock(&writer_mutex);

         out++;
      }
   }
   //registra a qtde de operacoes realizadas por tipo
   printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
   pthread_exit(NULL);
}

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   pthread_t *tid;
   double ini, fim, delta;
   
   //verifica se o numero de threads foi passado na linha de comando
   if(argc < 2) {
      printf("Digite: %s <numero de threads>\n", argv[0]); 
      return 1;
   }
   nthreads = atoi(argv[1]);

   //insere os primeiros elementos na lista
   for(int i = 0; i < QTDE_INI; i++) {
      Insert(i % MAX_VALUE, &head_p);  /* Ignore return value */
   }

   //aloca espaco de memoria para o vetor de identificadores de threads no sistema
   tid = malloc(sizeof(pthread_t) * nthreads);
   if(tid == NULL) {  
      printf("--ERRO: malloc()\n"); 
      return 2;
   }

   //tomada de tempo inicial
   GET_TIME(ini);
   //inicializa a variavel rwlock
   pthread_rwlock_init(&rwlock, NULL);
   
   //cria as threads
   for(long int i = 0; i < nthreads; i++) {
      if(pthread_create(tid + i, NULL, tarefa, (void*) i)) {
         printf("--ERRO: pthread_create()\n"); 
         return 3;
      }
   }
   
   //aguarda as threads terminarem
   for(int i = 0; i < nthreads; i++) {
      if(pthread_join(*(tid + i), NULL)) {
         printf("--ERRO: pthread_join()\n"); 
         return 4;
      }
   }

   //tomada de tempo final
   GET_TIME(fim);
   delta = fim - ini;
   printf("Tempo: %lf\n", delta);

   //libera o rwlock
   pthread_rwlock_destroy(&rwlock);
   //libera o espaco de memoria do vetor de threads
   free(tid);
   //libera o espaco de memoria da lista
   Free_list(&head_p);

   return 0;
}  /* main */