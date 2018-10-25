#include "grafo.h"

void zerarString(char *s){
    int i;
    for(i = 0; i < sizeof(s); i++){
        s[i] = '\0';
    }
}

Grafo* lerArquivo(Cidade **vetor, Grafo *g){
    clock_t inic = clock();
    FILE *arquivo = fopen("entrada.txt", "r");
    if(!arquivo){
        printf("\n\t Erro ao ler o arquivo");
    }else{
        char* linha = (char*)malloc(10 * sizeof(char));
        //char linha[100000];
        zerarString(linha);
        char* conteudo = fgets(linha, 10, arquivo);

        ///captura a quantidade de cidades na primeira linha
        int qtdCidades = atoi((char*)strtok(conteudo, ";"));
        *vetor = (Cidade*)malloc(qtdCidades * sizeof(Cidade));
        int tam = (7 * qtdCidades) + 1;
        free(linha);
        linha = (char*)malloc(tam * sizeof(char));
        //linha = (char*)realloc(linha, tam * sizeof(char));

        if(conteudo != NULL){
            int i;
            for(i = 0; i < qtdCidades; i++){
                conteudo = fgets(linha, tam , arquivo);
                char *resStr = (char*)strtok(conteudo, ";");
                (*vetor)[i].numero = atoi(resStr);
                char *nom = (char *)malloc(10 * sizeof(char));
                char *nomAux;
                nomAux = strtok(NULL, ";");
                strcpy(nom, nomAux);
                (*vetor)[i].nome = nom;
                (*vetor)[i].populacao = atoi(strtok(NULL, ";"));
                //insere
            }
            printf("Vetor construido \n");
            ///preencher a matriz

            ///inicia o grafo
            iniciaGrafo(qtdCidades, g);
            printf("Grafo construido \n");
            ///preenche o grafo
            for(i = 0; i < qtdCidades; i++){
            ///percore a linha
                ///escreve a primeira coluna, pois strktok precisa receber null no primeiro paramentro
                ///para continuar "andando" na linha
                conteudo = fgets(linha, tam, arquivo);
                int j = 0;
                char *resStr = (char*)strtok(conteudo, ";");
                float val = atof(resStr);
                insereArco(g, i, j, val);
                ///percorre a coluna
                for(j = 1; j < qtdCidades; j++){
                    char *resStr = (char*)strtok(NULL, ";");
                    float val = atof(resStr);
                    insereArco(g, i, j, val);
                }//for j, colunas
            }//for i, linhas
            printaGrafo(g);
        }//FIM IF
    }//FIM ELSE
    clock_t fim = clock();
        printf("\n\t Arquivo lido!");
        double res = (double)(fim - inic)/CLOCKS_PER_SEC;
        printf("\n\t tempo: %f \n",res);
    fclose(arquivo);
    return g;
}

void geraArquivoSaida(Grafo *GPrim, Cidade **vetor, float tempoAGM){
    int i,j;

    FILE *arquivo = fopen("viasAsfaltadas.txt", "w");
    if(!arquivo){
        printf("\n\t Erro ao ler o arquivo");
    }else{
        printf("\n\n\t ====== AGM ===== \n");
        //percorre todos os arcos que possuem valor diferente de 0 e carrega no arquivo
        for(i = 0; i < GPrim->vertices; i++){
            for(j = 0; j < GPrim->vertices; j++){
                if(GPrim->matriz[i][j] != 0){
                    fprintf(arquivo, "%s,%s,%f\n", (*vetor)[i].nome, (*vetor)[j].nome, GPrim->matriz[i][j]);
                    printf("%s,%s,%f\n", (*vetor)[i].nome, (*vetor)[j].nome, GPrim->matriz[i][j]);
                }
            }//for j, coluna
        }//fpr i, linha
        fprintf(arquivo, "%f;%f", GPrim->custoMin, tempoAGM);
        printf("\n\n\t TEMPO %f", tempoAGM);
    }//else da arbertura do arquivo
    fclose(arquivo);
}

void lerArquivoPesquisa(Grafo *grafoOriginal, Grafo *grafoAGM, Cidade *vetCidades){
    printf("\n\n\t\t Pesquisa ============= ");
    FILE *arquivo = fopen("pesquisas.txt", "r");
    FILE *arqSaida = fopen("melhorescaminhos.txt", "w");
    if(!arquivo && !arqSaida){
        printf("\n\t Erro ao ler o arquivo de pesquisas");
    }else{
        while(!feof(arquivo)){
            char linha[100];
            zerarString(linha);
            char* conteudo = fgets(linha, sizeof(linha), arquivo);
            if(conteudo != NULL){
                strtok(conteudo, ";");
                char* origem = strtok(NULL, ";");
                int ori = atoi(origem);
                char* destino = strtok(NULL, ";");
                int dest = atoi(destino);

                dijkstra(vetCidades, grafoOriginal, ori, dest, arqSaida, ori, dest);
                dijkstra(vetCidades, grafoAGM, ori, dest, arqSaida, ori, dest);
            }
        }
    }
    fclose(arquivo);
    fclose(arqSaida);
}

int main(){
    /**
    Essa versão do algoritimo de dijstra aaprentemente funciona com o grafo gerado pelo prim
    o incoveniente agora é que em 100 cidades ele funciona, a partir de 200 ele da erro de invasão de memoria
    creio que as variaveis verticeAtual e verticeAnterior estejam sendo manipuladas, não incorretamente, mas
    de alguma forma acessando indices incorretos, deve haver algum furo na forma de implementação, algo com
    relação a cosntrução dos vetores, o que é utilizado para marcar como um vertice nulo/infinito. Muito provavelmente
    em algum ponto o tratamento ou teste de uma posição que deveria ser desconsiderada não está sendo, algum teste está falho

    PS: eu entendo que a utilziação de variaveis globais deve ser evitada por motivos de manipulação incorreta, mas
    para esse caso, como são muitas e são ponteiros não vi problema, alem disso os valores são setados nelas através
    dos vetores, ou sejá, há alguma verificação incorreta.
    No caso atual a variavel verticeAtual está recebendo um valor de -1 do vetor de precedentes (matriz de controle [PRECEDENTE])
    e por este motivo a variavel verticeAnterior está acessando um endereço ilegal com indice -1 e está recupernado
    um valor irracional
    **/

    Cidade *vet;
    Grafo G, GPrim;
    lerArquivo(&vet, &G);

    clock_t inic = clock();
    prim(&G, &GPrim);
    clock_t fim = clock();
    printaGrafo(&GPrim);
    double res = (double)(fim - inic)/CLOCKS_PER_SEC;

    lerArquivoPesquisa(&G, &GPrim, vet);

    geraArquivoSaida(&GPrim, &vet, res);

    printf("\n\tFim da execucao");
    return 0;
}
