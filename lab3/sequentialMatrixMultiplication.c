/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 3 */
/* Codigo: Executa multiplicação de matrizes sequencialmente */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/timer.h"

void matrixMultiplication( int m, int n, float *A, float *B, float *result )
{
    for ( int i = 0; i < m; i++ )
    {
        for ( int j = 0; j < n; j++ )
        {
            result[i * n + j] = 0;  // Initialize the result element to 0
            for ( int k = 0; k < n; k++) 
            {
                result[i * n + j] += A[i * n + k] * B[k * n + j];
            }
        }
    }
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

    if(argc < 3) {
        fprintf( stderr, "Uso: %s <arquivo entrada> <arquivo saida>\n", argv[0] );
        return 1;
    }

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
        return 4;
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

    matrixMultiplication( m, n, A, B, C );

    GET_TIME( finish );

    procDuration = finish - start; 

    GET_TIME( start );  

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

    fclose( fd );

    free( A );
    free( B );

    GET_TIME( finish );

    finishingDuration = finish - start;

    fprintf(stdout, "Tempos de execução:\n\n");
    fprintf(stdout, "Inicialização: %f s\n", initDuration);
    fprintf(stdout, "Processamento: %f s\n", procDuration);
    fprintf(stdout, "Finalização: %f s\n\n", finishingDuration);

    return 0;
}