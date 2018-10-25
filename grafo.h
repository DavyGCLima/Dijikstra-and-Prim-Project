#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define INFINITO 99999
#define NVISITADO -1

#define DISTANCIA  0
#define PRECEDENTE 1
#define STATUS     2
#define ROTA       3
#define ABERTO     0
#define FECHADO    1
int   verticePartida,
      verticeDestino,
      verticeAtual,
      verticeAnterior,
      QTDVertices;
float distanciaTotal,
      totalArcos;

typedef struct cidade{
    char *nome;
    int numero;
    int populacao;
}Cidade;

typedef struct grafo{
    int vertices;
    int arcos;
    float **matriz;
    float custoMin;
}Grafo;
//PRIM
Grafo* iniciaGrafo(int vertices, Grafo *G);
float **iniciaMatriz(int linhas, int colunas, float valor);
void printaGrafo(Grafo *G);
void insereArco(Grafo *G, int vertice, int coluna, float valor);
void prim(Grafo *G, Grafo *GPrim);
//DIJKSTRA
void dijkstra(Cidade* cidades, Grafo *grafo, int origemV, int destinoV, FILE *arq, int origem, int destino);
void inicializarVariaveis();
void iniciarVetoresDeOrientacao(Grafo *grafo, float*** vetoresDeOrientacao);
void marcaPrecedentes(Grafo *grafo, float*** vetoresDeOrientacao);
void menorEstimativa(float*** vetoresDeOrientacao);
void exibirRota(float*** vetoresDeOrientacao, Cidade* cidades, int tam, FILE *arqSaida, int vertInic, int vertDest, double temp);

