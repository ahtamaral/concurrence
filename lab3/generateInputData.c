/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 3 */
/* Codigo: Gera dados de entrada para a prática */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) 
{
    int m, n; // Linhas, colunas
    float* A;
    float* B;
    FILE * fd; // File descriptor
    size_t ret; // Size returned by fwrite().

    if( argc < 4 ) 
    {
        fprintf(stderr, "Uso: %s <linhas> <colunas> <arquivo saida>\n", argv[0]);
        return 1;
    }

    m = atoi( argv[1] );
    n = atoi( argv[2] );
    
    A = (float *) malloc( sizeof( float ) * m * n );
    B = (float *) malloc( sizeof( float ) * m * n );

    if ( !A || !B )
    {
        fprintf(stderr, "Erro de alocao de memoria para uma das matrizes\n");
        return 2;
    }

    srand(time(NULL));

    for ( long long int i = 0; i < m * n; i++)
    {
        *( A + i ) = ( rand() % 1000 ) * 0.3;
        *( B + i ) = ( rand() % 1000 ) * 0.3;
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

    fd = fopen( argv[3], "wb" );

    if ( !fd )
    {
        fprintf( stderr, "Erro de abertura de arquivo.\n");
        return 3;
    }

    ret = fwrite( &m, sizeof(int), 1, fd);
    ret = fwrite( &n, sizeof(int), 1, fd);

    ret = fwrite( A, sizeof(float), m * n, fd);
    ret = fwrite( B, sizeof(float), m * n, fd);

    if ( ret < m * n )
    {
        fprintf( stderr, "Erro de escrita no  arquivo\n" );
        return 4;
    }

    fclose( fd );

    free( A );
    free( B );

    return 0;
}