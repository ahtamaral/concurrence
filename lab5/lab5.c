#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0; //variavel compartilhada entre as threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
pthread_cond_t cond;   //variavel de condicao para sincronizacao

int printed_multiples = 0; // Counter for printed multiples of 10

// Funcao executada pelas threads
void *ExecutaTarefa(void *arg) {
    long int id = (long int) arg;
    printf("Thread : %ld esta executando...\n", id);

    for (int i = 0; i < 100000; i++) {
        // Entrada na SC (Seção Crítica)
        pthread_mutex_lock(&mutex);
        
        // Incrementa a variavel compartilhada 
        soma++;

        // Se for múltiplo de 10, sinaliza a thread extra e espera
        if (soma % 10 == 0) {
            pthread_cond_signal(&cond);  // Sinaliza a thread extra
            pthread_cond_wait(&cond, &mutex); // Espera até a thread extra imprimir o valor
        }

        // Saida da SC
        pthread_mutex_unlock(&mutex);
    }
    printf("Thread : %ld terminou!\n", id);
    pthread_exit(NULL);
}

// Funcao executada pela thread de log
void *extra(void *args) {
    printf("Extra : esta executando...\n");

    while (printed_multiples < 20) {  // Queremos apenas os primeiros 20 múltiplos de 10
        pthread_mutex_lock(&mutex);
        
        // Espera até um múltiplo de 10 ser alcançado
        while (soma % 10 != 0) {
            pthread_cond_wait(&cond, &mutex);  // Aguarda sinal da thread ExecutaTarefa
        }

        // Imprime o valor de 'soma' se for múltiplo de 10
        if (printed_multiples < 20) {
            printf("soma = %ld\n", soma);
            printed_multiples++;
        }

        // Sinaliza para as threads ExecutaTarefa que podem continuar
        pthread_cond_signal(&cond);
        
        pthread_mutex_unlock(&mutex);
    }

    printf("Extra : terminou!\n");
    pthread_exit(NULL);
}

// Fluxo principal
int main(int argc, char *argv[]) {
    pthread_t *tid;  // Identificadores das threads no sistema
    int nthreads;    // Quantidade de threads (passada por linha de comando)

    // Leitura e avaliacao dos parametros de entrada
    if (argc < 2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    // Aloca as estruturas
    tid = (pthread_t *) malloc(sizeof(pthread_t) * (nthreads + 1));
    if (tid == NULL) {
        puts("ERRO--malloc");
        return 2;
    }

    // Inicializa o mutex e a variavel de condicao
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Cria as threads
    for (long int t = 0; t < nthreads; t++) {
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *) t)) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    // Cria a thread de log
    if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }

    // Espera todas as threads terminarem
    for (int t = 0; t < nthreads + 1; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    // Finaliza o mutex e a variavel de condicao
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    printf("Valor final de 'soma' = %ld\n", soma);
    return 0;
}