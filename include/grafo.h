#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>

/*
   Um grafo é um tipo de Estrutura de Dados que permite organizar diversas informações entre si, não tendo a limitação da
   raiz das árvores, por exemplo.
*/

typedef void* Grafo;
typedef void* Vertice;
typedef void* Aresta;


Grafo criaGrafo(void);
/*
   Função que cria o grafo.
   Retorna um ponteiro para o grafo recém criado
*/

void deletaGrafo(Grafo g);
/*
   Função que deleta um grafo.
   Recebe como parâmetro o ponteiro do grafo a ser deletado
*/

Vertice adicionarVertice(Grafo g, const char* id, double x, double y);
/*
   Função que adiciona um vértice.
   Primeiro parâmetro é o grafo a ser inserido o vértice
   Segundo parâmetro é o ID que será associado ao vértice
   Terceiro e Quarto parâmetros são, respectivamente, a posição horizontal e vertical do vértice
   Retorna um ponteiro para este vértice recém criado
*/

Vertice buscarVertice(Grafo g, const char* id);
/*
   Função que busca um vértice.
   Primeiro parâmetro é o grafo a ser procurado o vértice
   Segundo parâmetro é o ID que está associado ao vértice
   Retorna um ponteiro para este vértice encontrado.
*/

Aresta buscarAresta(Grafo g, const char* origem, const char* destino);
/*
   Função que busca uma aresta.
   Primeiro parâmetro é o grafo a ser procurado a aresta
   Segundo e Terceiro parâmetros são o ponto de origem e destino da aresta
   Retorna um ponteiro para esta aresta encontrada.
*/

double getVerticeX(Vertice v);
/*
   Função que retorna a coordenada horizontal de um vértice.
   Recebe como parâmetro o vértice desejado.
   Retorna a coordenada X deste vértice.
*/

double getVerticeY(Vertice v);
/*
   Função que retorna a coordenada vertical de um vértice.
   Recebe como parâmetro o vértice desejado.
   Retorna a coordenada Y deste vértice.
*/

const char* getVerticeId(Vertice v);
/*
   Função que retorna o ID de um vértice.
   Recebe como parâmetro o vértice desejado.
   Retorna uma string contendo o ID deste vértice.
*/

int getIndiceVertice(Vertice v);
/*
   Função que retorna o índice numérico de um vértice.
   Recebe como parâmetro o vértice desejado.
   Retorna o índice do vértice ou -1 se inválido.
*/

Aresta adicionarAresta(Grafo g, const char* origem, const char* destino,
const char* ldir, const char* lesq, double comp, double velMedia, const char* nomeRua);
/*
   Função que adiciona uma aresta ao grafo.
   Primeiro parâmetro é o grafo onde a aresta será inserida.
   Segundo parâmetro é o ID do vértice de origem.
   Terceiro parâmetro é o ID do vértice de destino.
   Quarto parâmetro é o CEP da quadra à direita da aresta.
   Quinto parâmetro é o CEP da quadra à esquerda da aresta.
   Sexto parâmetro é o comprimento da aresta.
   Sétimo parâmetro é a velocidade média da aresta.
   Oitavo parâmetro é o nome da rua associada à aresta.
   Retorna um ponteiro para a aresta recém criada.
*/

void offAresta(Aresta a);
/*
   Função que desabilita uma aresta.
   Recebe como parâmetro a aresta a ser desabilitada.
*/

void onAresta(Aresta a);
/*
   Função que habilita uma aresta.
   Recebe como parâmetro a aresta a ser habilitada.
*/

bool arestaAtiva(Aresta a);
/*
   Função que verifica se uma aresta está ativa.
   Recebe como parâmetro a aresta desejada.
   Retorna 1 caso esteja ativa e 0 caso contrário.
*/

Vertice getOrigem(Aresta a);
/*
   Função que retorna o vértice de origem de uma aresta.
   Recebe como parâmetro a aresta desejada.
   Retorna o vértice de origem.
*/

Vertice getDestino(Aresta a);
/*
   Função que retorna o vértice de destino de uma aresta.
   Recebe como parâmetro a aresta desejada.
   Retorna o vértice de destino.
*/

double getComprimento(Aresta a);
/*
   Função que retorna o comprimento de uma aresta.
   Recebe como parâmetro a aresta desejada.
   Retorna o comprimento da aresta.
*/

double getVelocidadeMedia(Aresta a);
/*
   Função que retorna a velocidade média de uma aresta.
   Recebe como parâmetro a aresta desejada.
   Retorna a velocidade média da aresta.
*/

void setVelocidadeMedia(Aresta a, double novaVel);
/*
   Função que altera a velocidade média de uma aresta.
   Primeiro parâmetro é a aresta desejada.
   Segundo parâmetro é a nova velocidade média.
*/

const char* getNomeRua(Aresta a);
/*
   Função que retorna o nome da rua associado à aresta.
   Recebe como parâmetro a aresta desejada.
   Retorna uma string contendo o nome da rua.
*/

const char* getLdir(Aresta a);
/*
   Função que retorna o CEP da quadra à direita da aresta.
   Recebe como parâmetro a aresta desejada.
   Retorna uma string contendo o CEP.
*/

const char* getLesq(Aresta a);
/*
   Função que retorna o CEP da quadra à esquerda da aresta.
   Recebe como parâmetro a aresta desejada.
   Retorna uma string contendo o CEP.
*/

int quantVertices(Grafo g);
/*
   Função que retorna a quantidade de vértices do grafo.
   Recebe como parâmetro o grafo desejado.
   Retorna a quantidade de vértices.
*/

int quantArestas(Grafo g);
/*
   Função que retorna a quantidade de arestas do grafo.
   Recebe como parâmetro o grafo desejado.
   Retorna a quantidade de arestas.
*/

Vertice primeiroVertice(Grafo g);
/*
   Função que retorna o primeiro vértice do grafo.
   Recebe como parâmetro o grafo desejado.
   Retorna o primeiro vértice encontrado.
*/

Vertice proximoVertice(Grafo g, Vertice atual);
/*
   Função que retorna o próximo vértice do grafo.
   Primeiro parâmetro é o grafo desejado.
   Segundo parâmetro é o vértice atual.
   Retorna o próximo vértice após o atual.
*/

Aresta primeiraArestaAdj(Vertice v);
/*
   Função que retorna a primeira aresta adjacente de um vértice.
   Recebe como parâmetro o vértice desejado.
   Retorna a primeira aresta adjacente.
*/

Aresta proximaArestaAdj(Vertice v, Aresta atual);
/*
   Função que retorna a próxima aresta adjacente de um vértice.
   Primeiro parâmetro é o vértice desejado.
   Segundo parâmetro é a aresta atual.
   Retorna a próxima aresta adjacente após a atual.
*/

#endif
