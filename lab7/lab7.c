/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Artur Amaral */
/* Codigo: */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define NTHREADS 3
#define N        10

// Variaveis globais
sem_t podeEscreverNoBuffer, podeLerBuffer;      //semaforos para coordenar a ordem de execucao das threads
int done = 0;

char buffer1[N + 1];  // +1 to store the null terminator for the string

//funcao executada pela thread 1
void *leitor (void *arg) {
   printf("[ t1 ] Leitor esta executando...\n");
   
    FILE *file;
    size_t bytesRead;

    // Open the file in read mode
    file = fopen( "in.txt", "r" );
    if ( file == NULL ) 
    {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    while ( 1 ) 
    {
        sem_wait( &podeEscreverNoBuffer ); // Se puder escrever no buffer1, prossegue.
        printf("[ t1 ] Leitor pode escrever no buffer1.\n");

        // Clear the buffer before reading (set to all zeros)
        memset(buffer1, 0, sizeof(buffer1));
        // Read the file in chunks of N characters
        bytesRead = fread(buffer1, sizeof(char), N, file);

        printf("[ t1 ] buffer1: %s\n", buffer1);

        if ( feof(file) ) 
        {
            printf("DONE\n");
            done = 1;  // Set the done flag when reading is finished
            sem_post(&podeLerBuffer);  // Notify processador for final check
            break;
        }

        printf("[ t1 ] Processador pode consumir buffer1.\n");
        sem_post( &podeLerBuffer );    // Avisa que o buffer1 foi preenchido e pode ser lido.   
    }

    fclose(file);

    printf("[ t1 ] Leitor terminou execucao!\n");
    pthread_exit(NULL);
}

void *processador (void *arg) {

    char myBuffer1[N + 1]; 

    printf("[ t2 ] Processador esta executando...\n");

    while( 1 )
    {
        printf("[ t2 ] Processador está aguardando liberação para consumir buffer1.\n");
        sem_wait( &podeLerBuffer ); // Quando o processador puder ler o buffer 1, prossegue

        if ( done )
        {
            printf("[ t2 ] Leitor terminou. Processador finalizando...\n");
            break;
        }

        printf("[ t2 ] Processador LIBERADO para consumir buffer1.\n");
        // printf("%s\n", buffer1);
        // printf(" len: %ld \n", strlen(buffer1));
        strcpy(myBuffer1, buffer1);

        printf("[ t2 ] Processador criou cópia local do buffer1: %s\n", myBuffer1);
        printf("[ t2 ] Leitor pode sobrescrever buffer1.\n");
        sem_post( &podeEscreverNoBuffer); // Quando o processador já tiver lido o buffer e o leitor puder sobrescrevê-lo, emite este sinal.
        
    }

    printf("[ t2 ] Processador terminou execucao!\n");
    pthread_exit(NULL);
}

void *escritor (void *arg) {
//    printf("Thread : 1 esta executando...\n");
   
//    printf("Thread : 1 mudou estado!\n");
//    //faz qq coisa ate mudar de estado
//    sem_post(&estado1); 
   
//    printf("Thread : 1 terminou!\n");
//    pthread_exit(NULL);
    // printf("Escritor esta executando...\n");


    // printf("Escritor terminou execucao!\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];

  //inicia os semaforos
  sem_init(&podeEscreverNoBuffer, 0, 1);
  sem_init(&podeLerBuffer, 0, 0);

  //cria as tres threads
  if (pthread_create(&tid[2], NULL, leitor, NULL)) { 
	  printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[1], NULL, processador, NULL)) { 
	  printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[0], NULL, escritor, NULL)) { 
	  printf("--ERRO: pthread_create()\n"); exit(-1); }

  //--espera todas as threads terminarem
  for (int t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  } 

  return 0;
}