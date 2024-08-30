/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 3 */
/* Codigo: Executa multiplicação de matrizes concorrentemente */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "include/timer.h"

typedef struct {
   int idThread;
   int m, n;
   float* A;
   float* B;
   float* C;
   int firstRow, lastRow;
} t_Args;

void *matrixMultiplication( void *arg )
{
    t_Args *args = (t_Args *) arg;

    #ifdef DEBUG
    fprintf( stdout, "Thread #%d - Linhas %d a %d.\n", args->idThread, args->firstRow, args->lastRow);
    #endif

    for ( int i = args->firstRow; i <= args->lastRow; i++ )
    {
        for ( int j = 0; j < args->n; j++ )
        {
            args->C[i * args->n + j] = 0;
            for ( int k = 0; k < args->n; k++ )
            {
                args->C[i * args->n + j] += args->A[i * args->n + k] * args->B[k * args->n + j];
            } 
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) 
{
    double start, finish;
    double initDuration, procDuration, finishingDuration;

    GET_TIME( start );

    int m, n; // Linhas, colunas
    float* A;
    float* B;
    float* C;
    FILE * fd; // File descriptor
    size_t ret; // Size returned by fread().
    int nthreads;
    t_Args *args;

    if(argc < 4) 
    {
        fprintf( stderr, "Uso: %s <arquivo entrada> <arquivo saida> <nthreads>\n", argv[0] );
        return 1;
    }

    nthreads = atoi( argv[3] );

    pthread_t tid_sistema[nthreads]; 

    fd = fopen( argv[1], "rb" );
    
    if ( !fd )
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    ret = fread( &m, sizeof(int), 1, fd );

    if ( !ret )
    {
        fprintf(stderr, " Erro de leitura das dimensoes da matriz no arquivo \n" );
        return 3;
    }

    ret = fread( &n, sizeof(int), 1, fd );

    if ( !ret )
    {
        fprintf(stderr, " Erro de leitura das dimensoes da matriz no arquivo \n" );
        return 3;
    }

    if ( m != n )
    {
        fprintf(stderr, " Por simplicidade, este programa só está aceitando matrizes quadradas. Tente novamente. \n" );
        return 7;
    }

    A = (float *) malloc( sizeof( float ) * m * n );
    B = (float *) malloc( sizeof( float ) * m * n );

    if ( !A || !B )
    {
        fprintf(stderr, "Erro de alocao de memoria para uma das matrizes\n");
        return 2;
    }

    ret = fread( A, sizeof( float ), m * n, fd );

    if ( ret < m * n )
    {
        fprintf(stderr, "Erro de leitura dos elementos da matriz A\n");
        return 4;
    }

    ret = fread( B, sizeof( float ), m * n, fd );

    if ( ret < m * n )
    {
        fprintf(stderr, "Erro de leitura dos elementos da matriz B\n");
        return 8;
    }

    if ( nthreads > m )
    {
        fprintf(stderr, "Não é permitido número de threads maior que as dimenões das matrizes.\n");
        return 8;
    }

    #ifdef DEBUG

    fprintf( stdout, "Matrix A:\n" );
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf( stdout, "%.6f ", A[ i * n + j] );
        }
        fprintf( stdout, "\n" );
    }

    fprintf( stdout, "\nMatrix B:\n" );
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf( stdout, "%.6f ", B[ i * n + j] );
        }
        fprintf( stdout, "\n" );
    }
    fprintf( stdout, "\n" );
    #endif

    C = (float *) malloc( sizeof( float ) * m * n );

    if ( !C )
    {
        fprintf( stderr, "Erro de alocao de memoria para matriz C\n" );
        return 2;
    }

    GET_TIME( finish );

    initDuration = finish - start;

    GET_TIME( start );

    int quotient = m / nthreads;
    int remainder = m % nthreads; 

    int nextAvailableRow = 0;

    // Divide a carga entre as threads e as executa.
    for ( int i = 0; i < nthreads; i++)
    {
        args = malloc(sizeof(t_Args));

        if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            return 2;
        }

        args->idThread = i;

        args->m = m;
        args->n = n;

        args->A = A;
        args->B = B;
        args->C = C;

        args->firstRow = nextAvailableRow;
        args->lastRow = nextAvailableRow + (quotient - 1);

        if ( remainder > 0 )
        {
            args->lastRow += 1;
            remainder--;
        }

        nextAvailableRow = args->lastRow + 1;

        if ( pthread_create( &tid_sistema[i], NULL, matrixMultiplication, (void*) args ) )
        {
            printf("--ERRO: pthread_create() #%d\n", i); 
            return 2;
        }
    }

    for ( int i=0; i < nthreads; i++ ) 
    {
        if ( pthread_join( tid_sistema[i], NULL ) ) 
        {
            printf( "--ERRO: pthread_join() da thread %d\n", i ); 
        } 
    }

    GET_TIME( finish );

    procDuration = finish - start;

    GET_TIME( start );

    fclose( fd ); // Fecha arquivo de entrada.

    fd = fopen( argv[2], "wb" ); // Abre arquivo de saída.

    if ( !fd )
    {
        fprintf( stderr, "Erro de abertura de arquivo.\n");
        return 3;
    }

    ret = fwrite( &m, sizeof(int), 1, fd);
    ret = fwrite( &n, sizeof(int), 1, fd);
    ret = fwrite( C, sizeof(float), m * n, fd);

    if ( ret < m * n )
    {
        fprintf( stderr, "Erro de escrita no  arquivo\n" );
        return 4;
    }

    fclose( fd ); // Fecha arquivo de saída.

    #ifdef DEBUG
    fprintf( stdout, "\nResulting matrix:\n" );
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf( stdout, "%.6f ", C[ i * n + j] );
        }
        fprintf( stdout, "\n" );
    }
    #endif

    free( A );
    free( B );
    free( C );

    GET_TIME( finish );

    finishingDuration = finish - start;

    fprintf(stdout, "Tempos de execução:\n\n");
    fprintf(stdout, "Inicialização: %f s\n", initDuration);
    fprintf(stdout, "Processamento: %f s\n", procDuration);
    fprintf(stdout, "Finalização: %f s\n\n", finishingDuration);

    return 0;
}