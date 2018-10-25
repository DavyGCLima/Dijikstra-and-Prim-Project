#include "grafo.h"

///Aloca o ponteiro do grafo, inicializa suas variaveis internas
Grafo* iniciaGrafo(int vertices, Grafo *G){
    //G = (Grafo*)malloc(sizeof(G));
    G->vertices = vertices;
    G->arcos = 0;
    G->matriz = iniciaMatriz(vertices, vertices, 0);
    return G;
}

///Preenche a matriz com valores inciais definios em valor
float **iniciaMatriz(int linhas, int colunas, float valor){
    int i, j;
    float **mat = malloc(linhas * sizeof(float*));
    for(i = 0; i < linhas; i++){
        mat[i] = malloc(colunas * sizeof(float));
    }
    for(i = 0; i < linhas; i++){
        for(j = 0; j < colunas; ++j)
            mat[i][j] = valor;
    }
    return mat;
}

void printaGrafo(Grafo *G){
    int vertices, w;
    for(vertices = 0; vertices < G->vertices; ++vertices){
        printf("%2d: ", vertices);
        for(w = 0; w < G->vertices; ++w){
            //if(G->matriz[vertices][w] != 0)
                printf(" %.f", G->matriz[vertices][w]);
        }
        printf("\n");
    }
}

///insere um valor na matriz, ou sejá, inserá um valor de um arco
void insereArco(Grafo *G, int vertice, int coluna, float valor){
    if(G->matriz[vertice][coluna] == 0){
        G->matriz[vertice][coluna] = valor;
        G->arcos++;
    }
}

void prim(Grafo *G, Grafo *GPrim){
    printf("\n\n\t ========= PRIM ========\n\n");
    iniciaGrafo(G->vertices, GPrim);
    int i, j;
	int visitados[G->vertices];///vetor de controle, marca os vertices já visitados
	float minimo = INFINITO;
	int u = 0, v = 0;///variaveis de contrle de linha e coluna respectivamente
	float total = 0;
    int rodando = 1;///variavel de controle do algoritimo

    ///marca todos os vertices como não visitados
    for (v = 0; v < G->vertices; v++)
        visitados[v] = NVISITADO;

    ///marca o vertice como lido/passado/inspecionado
    visitados[0] = 0;

    ///passa por todos os nos
	while (rodando != 0) {
	   	minimo = INFINITO;//reseta a variavel para percorrer a matriz e encotrar o menor arco
	   	///percorre todos os vertices não visitados e encontra o menor arco
	   	for (i = 0; i < G->vertices; i++){
            ///verifica se o vertice atual já foi visitado
	       	if (visitados[i] != NVISITADO){
	          	for (j = 0; j < G->vertices; j++){
                    ///verifica se o arco já foi analisado
	           		if (visitados[j] == NVISITADO && minimo > G->matriz[i][j] && G->matriz[i][j] != 0) {
	               		minimo = G->matriz[i][j];//salva o valor do arco de menor valor
	               		///marca o arco de mnor valor
	               		u = i;
	               		v = j;
	           		}
	          	}//for
	       	}//if
	   	}//for

	   	///controla a saida, caso as condições anteriores não tenham sido atingidas
	   	///o algoritimo já passou por todos os vertices e o laço while true deve ser finalizado
	   	if (minimo == INFINITO) {
	   		rodando = 0;
	   		break;
	   	}

	   	///marca o nó que acabou de ser visitado
	   	visitados[v] = u;
	   	printf("(%d, %d - %.2f) ", u, v, G->matriz[u][v]);
        GPrim->matriz[u][v] = G->matriz[u][v];

	   	///incrementa o custo
	   	total += minimo;
	}//while
	GPrim->custoMin = total;
	G->custoMin = total;
    printf("\n\t CUSTO %.2f\n", G->custoMin);
}

///Inicia variaveis utilizadas no Dijkstra, as variaveis são globais
///para facilitar a utilização, como elas são ponteiros especificos como os iteradores do java
///serem globais ou não, não faria diferença, apenas facilitou não passar todas elas por eferencia como *ponteiro
///e realziar as mudanças por (*variaveil) = valor
void inicializarVariaveis() {
    verticePartida = 0;
    verticeDestino = 0;
    verticeAtual = 0;
    verticeAnterior = 0;
    QTDVertices = 0;
    distanciaTotal = 0;
}

void dijkstra(Cidade* cidades, Grafo *grafo, int origemV, int destinoV, FILE *arq, int origem, int destino) {
    clock_t inic = clock();
    inicializarVariaveis();
    float** vetoresDeOrientacao;
    QTDVertices = grafo->vertices;

    iniciarVetoresDeOrientacao(grafo, &vetoresDeOrientacao);

    verticePartida = origemV;
    verticeDestino = destinoV;
    verticeAtual = verticePartida;

    vetoresDeOrientacao[ROTA][verticeAnterior] = verticeAtual;
    verticeAnterior++;

    ///Laço principal do algoritimo, suacondição de parada é atingir o objetivo, a cidade destino
     while(verticeAtual != verticeDestino){
        marcaPrecedentes(grafo, &vetoresDeOrientacao);
        menorEstimativa(&vetoresDeOrientacao);
    }
    clock_t fim = clock();
    ///Medição do tempo de execução
    double res = (double)(fim - inic)/CLOCKS_PER_SEC;

    exibirRota(&vetoresDeOrientacao, cidades, grafo->vertices, arq, origemV, destino, res);
}

///Esta função inicia a matriz (tratado como cada linha um vetor) que controlará o algoritimo de Dijkstra
void iniciarVetoresDeOrientacao(Grafo *grafo, float*** vetoresDeOrientacao) {
    int linha, coluna;

    ///Aloca a matriz
    *vetoresDeOrientacao = (float**) malloc(4 * sizeof (float*));
    for (linha = 0; linha < 4; linha++) {
        for (coluna = 0; coluna < QTDVertices; coluna++)
            (*vetoresDeOrientacao)[linha] = (float*) malloc(QTDVertices * sizeof (float));
    }


    ///preenche os vetores de distancias e precedentes
    /// * talvez a alteração que causa falha ao executar com o grafo AGM deva ser tratada nesta inicialização
    for (linha = 0; linha < 2; linha++) {
        for (coluna = 0; coluna < QTDVertices; coluna++)
            (*vetoresDeOrientacao)[linha][coluna] = 0;
    }

    /// Preenche os vetores:
    /// Controle (status) se é possivvel ou não passar por determiando vertice
    /// Rota, que armazenará a rota final
    for (coluna = 0; coluna < QTDVertices; coluna++) {
        (*vetoresDeOrientacao)[STATUS][coluna] = ABERTO;
        (*vetoresDeOrientacao)[ROTA][coluna] = -1;
    }
}

///Marca os precedentes
void marcaPrecedentes(Grafo *grafo, float*** vetoresDeOrientacao) {
    int i;
    (*vetoresDeOrientacao)[STATUS][verticeAtual] = FECHADO;

    for (i = 0; i < QTDVertices; i++) {
        if ((*vetoresDeOrientacao)[STATUS][i] == ABERTO) {
            (*vetoresDeOrientacao)[DISTANCIA][i] = grafo->matriz[verticeAtual][i] /*+ (*vetoresDeOrientacao)[DISTANCIA][i]*/;
            if ((*vetoresDeOrientacao)[DISTANCIA][i] > 0)
                (*vetoresDeOrientacao)[PRECEDENTE][i] = verticeAtual;
        }
    }
}

///Encontra o proximo vertice a servaliado e verifica a distancia
void menorEstimativa(float*** vetoresDeOrientacao) {
    int i,
        ehConexo = 0,
        primeiroLoop = 1,
        verticeMaisProximo = 0;

    ///procura por um vertice que possui um arco
    while (!ehConexo){
        ///percrre os vertices
        for (i = 0; i < QTDVertices; i++) {
            ///Se o vertice não foi visitado, o observará
            if ((*vetoresDeOrientacao)[STATUS][i] == ABERTO && (*vetoresDeOrientacao)[DISTANCIA][i] > 0) {
                ehConexo = 1;
                if (primeiroLoop) {
                    verticeMaisProximo = i;
                    primeiroLoop = 0;
                }else{
                    ///encontra o vertice mais proximo verificando a distancia maxima
                    if ((*vetoresDeOrientacao)[DISTANCIA][i] < (*vetoresDeOrientacao)[DISTANCIA][verticeMaisProximo] + (*vetoresDeOrientacao)[DISTANCIA][i]){
                        verticeMaisProximo = i;
                    }
                }//else
            }//if
        }//for
        ///Se o vertice não tem coneção com outro deve haver um rollback para o vertice anterior
        ///e o laço deve ser reiniciado
        if (!ehConexo) {
            distanciaTotal -= (*vetoresDeOrientacao)[DISTANCIA][verticeAtual];
            verticeAtual = (*vetoresDeOrientacao)[PRECEDENTE][verticeAtual];
            (*vetoresDeOrientacao)[STATUS][verticeAtual] = ABERTO;
            (*vetoresDeOrientacao)[ROTA][verticeAnterior] = -1;
            verticeAnterior--;
            break;
        }//if
    }//while
    ///se o vertice é conexo, então deverá entrar na rota como a melhor possibildiade
    ///avança o ponteiro do vertice atual
    if (ehConexo) {
        verticeAtual = verticeMaisProximo;
        (*vetoresDeOrientacao)[ROTA][verticeAnterior] = verticeAtual;
        distanciaTotal += (*vetoresDeOrientacao)[DISTANCIA][verticeAtual];
        verticeAnterior++;
    }
}

///Função que exibe na tela e grava noa rquivo de saida o resultado do Dijkstra
void exibirRota(float*** vetoresDeOrientacao, Cidade* cidades, int tam, FILE *arqSaida, int vertInic, int vertDest, double temp) {
    int i; ///variavel de controle
    fprintf(arqSaida, "Menor caminho entre as cidades %s e %s : %f\n", cidades[vertInic].nome, cidades[vertDest].nome, distanciaTotal);
    printf("\n\n\t ==== Distancia total: %f ==== ", distanciaTotal);
    printf("\n\t === Rota === \n\t ");
    ///passa por todos os vertices conexos
    for(i = 1; i < tam; i++) {
        if((*vetoresDeOrientacao)[ROTA][i] > -1){
            ///posição vertice atual (cidade de origem)
            int posicao = (*vetoresDeOrientacao)[ROTA][i];
            ///posição do vertice anterior (cidade de destino)
            int anterior = (*vetoresDeOrientacao)[PRECEDENTE][posicao];
            printf(" %s ", cidades[posicao].nome);
            printf(" -> ");
                ///grava a cidade de origem do arco
                fprintf(arqSaida, "%s;", cidades[anterior].nome);
                ///grava a cidade de destino do arco e o valor do arco
                fprintf(arqSaida, "%s;%f\n", cidades[posicao].nome, (*vetoresDeOrientacao)[DISTANCIA][posicao]);
        }
    }//for
    fprintf(arqSaida, "Tempo: %f\n", temp);
}






