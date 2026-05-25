#ifndef LEITOR_H
#define LEITOR_H

#include <stdio.h>
#include <stdbool.h>

/*
   O leitor é responsável por processar arquivos de comando (.geo, .pm e .qry) que definem as operações do projeto.
   Ele coordena as operações evolvendo as quadras, pessoas, moradores, entre outros.
*/

void inicializarSistema(const char *nomeBase, const char *outputDir);
/*
   Inicializa todos os componentes do sistema. Deve ser chamado antes de processar qualquer arquivo.
   Primeiro parâmetro é o nome base para os hashfiles.
   Segundo parâmetro é o diretório de saída.
*/

void finalizarSistema(void);
/*
   Libera toda a memória alocada, fecha hashfiles e finaliza o sistema.
   Deve ser chamado ao término da execução.
*/

void processarArquivoGeo(const char *caminho, const char *inputDir, const char *nomeBase, const char *outputDir);
/*
   Processa o arquivo .geo, gerando o SVG inicial da cidade.
*/


//Editar dps
void processarArquivoPm(const char *caminho, const char *inputDir, const char *nomeBase);
/*
   Processa o arquivo .pm, cadastrando habitantes e moradores nos hashfiles.
*/



void processarArquivoQry(const char *caminho, const char *inputDir, const char *nomeBase, const char *outputDir);
/*
   Processa o arquivo .qry, gerando o SVG final e TXT com resultados das consultas.
*/


#endif
