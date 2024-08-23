/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 2 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <math.h>

typedef struct {
    int idThread, nThreads;
    int vBegin, vEnd;
    float *v1;
    float *v2;
} t_Args;

//cria a estrutura de dados de retorno da thread
typedef struct {
   int idThread;
   double sum;
} t_Ret;

void *produtoInterno(void *arg)
{
    t_Args *args = (t_Args *) arg;
    t_Ret *ret; //estrutura de retorno
    
    fprintf( stdout, "Calling thread #%d", args->idThread);
    fprintf( stdout, " -> v[%d,%d]\n", args->vBegin, args->vEnd);

    double sum = 0;

    for (int i = args->vBegin; i <= args->vEnd; i++) {
        sum += args->v1[i] * args->v2[i];
    }

    fprintf( stdout, " Sum #%d: %lf\n", args->idThread, sum);

    // free(arg); //libera a memoria que foi alocada na main

    ret = malloc(sizeof(t_Ret));
    if (ret==NULL) {
        printf("--ERRO: malloc() thread\n");
        pthread_exit(NULL);
    }

    ret->idThread = args->idThread;
    ret->sum = sum;

    pthread_exit((void*) ret);
}

//--funcao principal do programa
int main(int argc, char * argv[]) {
    
    int nthreads; //qtde de threads que serao criadas (recebida na linha de comando)
    FILE * descritorArquivo; //descritor do arquivo de saida 
    t_Ret  *retorno;
    size_t ret;
    t_Args *args;  // struct com argumentos passados para a thread.

    // Dados que serão lidos no arquivo binário.
    long int N;
    float *v1;
    float *v2;
    double produtoInternoSequencial;

    int chunkSize = 0;

    double produtoInternoConcorrente = 0;

    pthread_t *tid_sistema; //vetor de identificadores das threads no sistema

    fprintf( stdout, "Programa concorrente para realizar o produto interno de dois vetores.\n\n");

    //verifica se o argumento 'qtde de threads' foi passado e armazena seu valor
    if(argc<2) {
        printf("--ERRO: informe a qtde de threads e o nome do arquivo de entrada <%s> <nthreads> <nomeArquivo>\n", argv[0]);
        return 1;
    }

    nthreads = atoi(argv[1]);

    // Limita o nº de threads ao tamanho do vetor.
    if( nthreads > N ) nthreads = N;

    //aloca espaco para o vetor de identificadores das threads no sistema
    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid_sistema==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }

    fprintf( stdout, "Threads: %d\n", nthreads);

    descritorArquivo = fopen(argv[2], "rb");
    if(!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }

    // Lê N
    ret = fread(&N, sizeof(long int), 1, descritorArquivo);

    if ( ret != 1 ) {
        fprintf(stderr, "Erro de leitura do arquivo\n");
    } 

    fprintf( stdout, "N: %ld\n", N );
    
    // Lê v1

    v1 = malloc (sizeof(float) * N);
    if( v1==NULL ) { printf("--ERRO: malloc()\n"); exit(-1); }

    ret = fread(v1, sizeof(float), N, descritorArquivo);

    if ( !ret ) { fprintf(stderr, "Erro de leitura dos elementos do vetor1\n"); return 4; }

    for ( int i = 0; i < N; i++ ) {
        fprintf( stdout, "%lf ", v1[i] );
    }

    fprintf( stdout, "\n" );

    // Lê v2

    v2 = malloc (sizeof(float) * N);
    if( v2==NULL ) { printf("--ERRO: malloc()\n"); exit(-1); }

    ret = fread(v2, sizeof(float), N, descritorArquivo);

    if ( !ret ) { fprintf(stderr, "Erro de leitura dos elementos do vetor3\n"); return 4; }

    for ( int i = 0; i < N; i++ ) {
        fprintf( stdout, "%lf ", v2[i] );
    }

    fprintf( stdout, "\n" );

    // Lê produto interno calculado no programa sequencial

    ret = fread(&produtoInternoSequencial, sizeof(double), 1, descritorArquivo);

    if ( !ret ) { fprintf(stderr, "Erro de leitura do produto interno sequencial.\n"); return 4; }

    fprintf( stdout, " PI seq: %lf\n", produtoInternoSequencial);

    fprintf( stdout, "\n" );

    chunkSize = N / nthreads;

    // if ( chunkSize == 0 ) {
    //     chunkSize = 1;
    // }

    fprintf( stdout, "chunkSize: %d\n", chunkSize );

    printf("Iniciando execução das threads:\n\n");
    int j = 0;
    for ( int i = 0; i < nthreads; i++) 
    {
        args = malloc(sizeof(t_Args));

        if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            return 2;
        }

        args-> idThread = i;
        args->v1 = v1;
        args->v2 = v2;
        
        args->vBegin = i*chunkSize;
        if (i > 0) args->vBegin += 1;

        if (i == nthreads - 1) {
            args->vEnd = N - 1;
        }
        else{
            args->vEnd = (i+1)*chunkSize;
        }

        if (pthread_create(&tid_sistema[i], NULL, produtoInterno, (void*) args))
        {
            printf("--ERRO: pthread_create() #%d\n", i); 
            return 2;
        }
    }


    //espera todas as threads terminarem
    for (int i=0; i<nthreads; i++) {
        if (pthread_join(tid_sistema[i], (void**) &retorno)) {
            printf("--ERRO: pthread_join() da thread %d\n", i); 
        }
        printf("Thread #%d retornou %lf \n", i, retorno->sum);
        produtoInternoConcorrente += retorno->sum;
    }

    printf( "\nProduto interno concorrente: %lf\n", produtoInternoConcorrente);

    double varRelativa = abs( ( produtoInternoSequencial - produtoInternoConcorrente ) / produtoInternoSequencial);

    printf("Variação relativa: %lf\n", varRelativa);

    printf("\n\n");
    printf("--Thread principal terminou\n");



    return 0;
}