/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 2 */
/* Codigo: usando threads em C com passagem e retorno de um argumento */

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#define MAX 1000 //valor maximo de um elemento do vetor
#define TEXTO 

int main(int argc, char * argv[]) 
{
    long int N;
    float *v1;
    float *v2;
    int fator=1; //fator multiplicador para gerar números negativos
    double soma1 = 0;
    double soma2 = 0;
    float elem; //valor gerado para incluir nos vetores
    FILE * descritorArquivo; //descritor do arquivo de saida
    size_t ret; //retorno da funcao de escrita no arquivo de saida
    double produtoInterno = 0;

    //verifica se o argumento 'qtde de threads' foi passado e armazena seu valor
    if(argc <3) {
        printf("--ERRO: informe o tamanho dos vetores de entrada e o nome do arquivo de saída <%s> <N> <nomeArquivo>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);

    //aloca memoria para o vetor1
    v1 = (float*) malloc(sizeof(float) * N);
    if(!v1) {
        fprintf(stderr, "Erro de alocao da memoria do vetor 1.\n");
        return 2;
    }
    
    //aloca memoria para o vetor1
    v2 = (float*) malloc(sizeof(float) * N);
    if(!v2) {
        fprintf(stderr, "Erro de alocao da memoria do vetor 2.\n");
        return 2;
    }

    //preenche o vetor 1 com valores float aleatorios
    srand(time(NULL));
    
    for(long int i=0; i< N; i++) {
        elem = (rand() % MAX)/3.0 * fator;
        v1[i] = elem;
        soma1 += elem; //acumula o elemento na soma total
        fator*=-1;
    }

    //preenche o vetor 2 com valores float aleatorios
    for( long int i = 0; i < N; i++ ) {
        elem = (rand() % MAX)/3.0 * fator;
        v2[i] = elem;
        soma2 += elem; //acumula o elemento na soma total
        fator*=-1;
    }

    //imprimir na saida padrao o vetor gerado
    #ifdef TEXTO
    fprintf(stdout, "N = %ld\n\n", N);
    fprintf(stdout, "vetor 1:\n");
    for(long int i=0; i<N; i++) {
        fprintf(stdout, "%.2f ",v1[i]);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "soma=%.2lf\n\n", soma1);

    fprintf(stdout, "vetor 2:\n");
    for(long int i=0; i<N; i++) {
        fprintf(stdout, "%.2f ",v2[i]);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "soma=%.2lf\n\n", soma2);
    #endif

    //escreve o vetor no arquivo
   //abre o arquivo para escrita binaria
   descritorArquivo = fopen(argv[2], "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

    //escreve a dimensao
    ret = fwrite(&N, sizeof(long int), 1, descritorArquivo);
    //escreve os elementos do vetor
    ret = fwrite(v1, sizeof(float), N, descritorArquivo);

    ret = fwrite(v2, sizeof(float), N, descritorArquivo);

    if(ret < N) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }
    //escreve o somatorio
    // ret = fwrite(&soma1, sizeof(double), 1, descritorArquivo);

    for( long int i = 0; i < N; i++ ) {
        produtoInterno += v1[i] * v2[i];
    }

    #ifdef TEXTO
    fprintf(stdout, "P.I.: %.2lf\n\n", produtoInterno);
    #endif

    ret = fwrite(&produtoInterno, sizeof(double), 1, descritorArquivo);

    //finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(v1);
    free(v2);

    return 0;
}