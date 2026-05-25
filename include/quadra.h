#ifndef QUADRA_H
#define QUADRA_H

#include <stddef.h>

/*
   Uma quadra é representada por um retângulo com âncora no vértice sudeste, seguindo a orientação da roda dos ventos descrita no projeto.
   Possui um CEP alfanumérico que a identifica e diferencia das demais quadras existentes.
   Cada quadra possui 4 faces, nomeadas de acordo com o ponto cardeal correspondente (N, S, L ou O).
   É possível criar/deletar quadras or meio das funções criaQuadra e freeQuadra, respectivamente.
   As demais funções são para funcionamento interno (criação no SVG, outros módulos, etc).
*/

typedef void* Quadra;

Quadra criaQuadra(const char *cep, double x, double y, double l, double h, const char *corB, const char *corP, double espB);
/*
Cria uma quadra com âncora no vértice sudeste (x,y).
Primeiro parâmetro é o CEP alfanumérico da quadra.
Segundo e terceiro parâmetros são as coordenadas x e y da âncora.
Quarto parâmetro é a largura da quadra.
Quinto parâmetro é a altura da quadra.
Sexto parâmetro é a cor da borda.
Sétimo parâmetro é a cor de preenchimento.
Oitavo parâmetro é a espessura da borda.
Retorna um ponteiro para a quadra criada, ou NULL em caso de algum erro.
*/

const char* getCepQuadra(Quadra q);
/*
Retorna o CEP da quadra repassada pelo parâmetro
*/

double getXQuadra(Quadra q);
/*
Retorna a coordenada X da âncora da quadra repassada pelo parâmetro.
*/

double getYQuadra(Quadra q);
/*
Retorna a coordenada Y da âncora da quadra repassada pelo parâmetro.
*/

double getLQuadra(Quadra q);
/*
Retorna a largura da quadra repassada pelo parâmetro.
*/

double getHQuadra(Quadra q);
/*
Retorna a altura da quadra repassada pelo parâmetro.
*/

const char* getCorBQuadra(Quadra q);
/*
Retorna a cor da borda da quadra repassada pelo parâmetro.
*/

const char* getCorPQuadra(Quadra q);
/*
Retorna a cor de preenchimento da quadra repassada pelo parâmetro.
*/

double getEspBQuadra(Quadra q);
/*
Retorna a espessura da borda da quadra repassada pelo parâmetro.
*/
/*
Fazer a função calculaPoscicaoEndereco
*/

void getPontoEndQuadra(Quadra q, char face, double num, double *x, double *y);
/*
Calcula a posição (x,y) de um endereço na quadra.
Primeiro parâmetro é o ponteiro para a quadra.
Segundo parâmetro é a face.
Terceiro parâmetro é o número da casa (distância da âncora da face).
Quarto e quinto parâmetros são ponteiros para armazenar as coordenadas x e y calculadas.
*/

double areaQuadra(Quadra q);
/*
Calcula a área da quadra repassada pelo parâmetro.
Retorna a área (Largura x Altura).
*/

void setCorBQuadra(Quadra q, const char *novaCor);
/*
Altera a cor da borda da quadra.
Primeiro parâmetro é o ponteiro para a quadra.
Segundo parâmetro é a nova cor a ser colocada.
*/

void setCorPQuadra(Quadra q, const char *novaCor);
/*
Altera a cor de preenchimento da quadra.
Primeiro parâmetro é o ponteiro para a quadra.
Segundo parâmetro é a nova cor a ser colocada.
*/

void setEspBQuadra(Quadra q, double novaEspessura);
/*
Altera a espessura da borda da quadra.
Primeiro parâmetro é o ponteiro para a quadra.
Segundo parâmetro é a nova espessura a ser colocad.
*/

int validaFace(char face);
/*
Verifica se um caractere representa uma face válida, repassada pelo parâmetro.
Retorna 1 se válido, 0 caso contrário.
*/

int validaCor(const char *cor);
/*
Verifica se uma string está no formato válido de cor (ex: "#RRGGBB").
Primeiro parâmetro é a string a ser validada.
Retorna 1 se válido, 0 caso contrário.
*/

size_t tamSerialQuadra(void);
/*
Retorna o tamanho em bytes necessário para serializar uma quadra.
*/

int serialQuadra(Quadra q, void *buffer, size_t tamBuffer);
/*
Serializa a quadra em um buffer de bytes para armazenamento no HashFile.
Primeiro parâmetro é o ponteiro para a quadra.
Segundo parâmetro é o buffer de destino.
Terceiro parâmetro é o tamanho do buffer.
Retorna 1 caso sucesso, 0 caso contrário.
*/

Quadra desserialQuadra(void *buffer, size_t tamBuffer);
/*
Reconstrói uma quadra a partir de um buffer de bytes.
Primeiro parâmetro é o buffer contendo os dados serializados.
Segundo parâmetro é o tamanho do buffer.
Retorna um ponteiro para a quadra reconstruída, ou NULL em caso de erro.
*/

void freeQuadra(Quadra q);
/*
Libera toda a memória associada à quadra.
Primeiro parâmetro é o ponteiro para a quadra.
*/

#endif
