/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 1 */
/* Codigo: "Hello World" usando threads em C */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

typedef struct {
   int idThread, indexToIncrement;
   int *vec;
} t_Args;

void *IncrementaVetor (void *arg) {
   // log da thread
   t_Args args = *(t_Args*) arg;
   printf("IncrementaVetor #%d\n", args.idThread);

   pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
   
   int nthreads; //qtde de threads que serao criadas (recebida na linha de comando)
   int vectorLen; // tam vetor
   t_Args *args;


   if(argc<2) { 
      printf("--ERRO: informe a qtde de threads e o tamanho do vetor <%s> <nthreads> <vectorLen>\n", argv[0]); 
      return 1;
   }

   nthreads = atoi(argv[1]);
   vectorLen = atoi(argv[2]);

   int vec[vectorLen];

   pthread_t tid_sistema[nthreads]; 

   for(int i=0; i<nthreads; i++) 
   {
      args = malloc(sizeof(t_Args));
      // printf("--Cria a thread %d\n", i);

      if (args == NULL) {
         printf("--ERRO: malloc()\n"); 
         return 2;
      }

      args->idThread = i;
      args->indexToIncrement = i;
      args->vec = vec;

      if (pthread_create(&tid_sistema[i], NULL, IncrementaVetor, (void*) args))
      {
         printf("--ERRO: pthread_create() #%d\n", i); 
         return 2;
      }
   }

    return 0;
}
