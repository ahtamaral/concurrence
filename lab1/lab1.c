/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 1 */
/* Codigo: "Hello World" usando threads em C */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

typedef struct {
   int idThread, indexToIncrement;
   int* vec;
} t_Args;

void *IncrementaVetor (void *arg) {
   // log da thread
   t_Args* args = (t_Args*) arg;

   args->vec[args->indexToIncrement] = args->vec[args->indexToIncrement] + 1;

   printf("Thread #%d incrementou v[%d]\n", args->idThread, args->indexToIncrement);

   free(arg);

   pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
   
   int nthreads;  // qtde de threads que serao criadas (recebida na linha de comando)
   int vectorLen; // tam vetor a ser manipulado
   t_Args *args;  // struct com argumentos passados para as threads

   if(argc<3) { 
      printf("--ERRO: informe a qtde de threads e o tamanho do vetor <%s> <nthreads> <vectorLen>\n", argv[0]); 
      return 1;
   }

   nthreads = atoi(argv[1]);
   vectorLen = atoi(argv[2]);

   int vec[vectorLen];

   pthread_t tid_sistema[nthreads]; 

   // Inicializa vetor em zeros e printa na tela.
   printf("Vetor de %d elementos a ser incrementado pelas threads:\n", vectorLen);

   for ( int i = 0; i < vectorLen; i++) {
      vec[i] = 0;
      printf("%d ", vec[i]);
   }
   printf("\n\n");

   // A lógica do balanceamento dos incrementos é a seguinte:
   // A primeira thread incrementa o elemento v[0], a segunda o elemento v[1], ...,
   // Ao alcançar o fim do vetor, a próxima thread, se ainda existir, incrementa o v[0], a seguinte incrementa o v[1], assim por diante.
   // Este comportamento foi alcançado através do uso do resto da divisão.
   // O contador i representa o id da thread e j  o índice do vetor que será incrementado.
   

   printf("Iniciando execução das threads:\n\n");
   int j = 0;
   for ( int i = 0; i < nthreads; i++) 
   {
      // printf("Thread #%d incrementa v[%d]\n", i, j);

      // Aqui, cria a thread, passand seu id e o respectivo índice do vetor que será incrementado por ela.
      args = malloc(sizeof(t_Args));

      if (args == NULL) {
         printf("--ERRO: malloc()\n"); 
         return 2;
      }

      args-> idThread = i;
      args->indexToIncrement = j;
      args->vec = vec;

      if (pthread_create(&tid_sistema[i], NULL, IncrementaVetor, (void*) args))
      {
         printf("--ERRO: pthread_create() #%d\n", i); 
         return 2;
      }

      j++;
      if ( j % vectorLen == 0) {
         j = 0;
      }
   }

   //espera todas as threads terminarem
   for (int i=0; i<nthreads; i++) {
      if (pthread_join(tid_sistema[i], NULL)) {
         printf("--ERRO: pthread_join() da thread %d\n", i); 
      } 
   }

   printf("\nVetor após interação das threads:\n");

   for ( int i = 0; i < vectorLen; i++) {
      vec[i] = 0;
      printf("%d ", vec[i]);
   }

   printf("\n\n");
   printf("--Thread principal terminou\n");

    return 0;
}
