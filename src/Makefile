# Nome do executável
PROJ_NAME = ted
ALUNO = Rafael Matui Cuenya

# Compilador
CC = gcc

# Flags
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS = -O0

# Bibliotecas
LIBS = -lm

# Diretórios
SRC_DIR = .
INC_DIR = ../include
TST_DIR = ../test
UNITY_DIR = ../unity

# Objetos do projeto
OBJETOS = main.o leitor.o trataNomeArquivo.o hashfile.o quadra.o \
          grafo.o dijkstra.o conexos.o disjoint.o agm.o svg.o

# Objetos dos testes 
TEST_OBJETOS = 

# Regra principal
$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)
	@echo "Compilação bem sucedida."

%.o: %.c
	$(CC) -c $(CFLAGS) -I$(INC_DIR) $< -o $@



# Módulo principal
main.o: main.c $(INC_DIR)/leitor.h $(INC_DIR)/trataNomeArquivo.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) main.c -o $@

# Módulo leitor
leitor.o: leitor.c $(INC_DIR)/leitor.h $(INC_DIR)/quadra.h \
          $(INC_DIR)/hashfile.h $(INC_DIR)/grafo.h $(INC_DIR)/dijkstra.h \
          $(INC_DIR)/conexos.h $(INC_DIR)/agm.h $(INC_DIR)/svg.h \
          $(INC_DIR)/trataNomeArquivo.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) leitor.c -o $@

# Módulo de tratamento de nomes
trataNomeArquivo.o: trataNomeArquivo.c $(INC_DIR)/trataNomeArquivo.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) trataNomeArquivo.c -o $@



# HashFile
hashfile.o: hashfile.c $(INC_DIR)/hashfile.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) hashfile.c -o $@

# Quadra
quadra.o: quadra.c $(INC_DIR)/quadra.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) quadra.c -o $@

# Lista
lista.o: lista.c $(INC_DIR)/lista.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) lista.c -o $@

# Grafo
grafo.o: grafo.c $(INC_DIR)/grafo.h $(INC_DIR)/lista.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) grafo.c -o $@

# Dijkstra
dijkstra.o: dijkstra.c $(INC_DIR)/dijkstra.h $(INC_DIR)/grafo.h $(INC_DIR)/lista.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) dijkstra.c -o $@

# Conexos
conexos.o: conexos.c $(INC_DIR)/conexos.h $(INC_DIR)/grafo.h $(INC_DIR)/lista.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) conexos.c -o $@

# Disjoint (Union-Find)
disjoint.o: disjoint.c $(INC_DIR)/disjoint.h $(INC_DIR)/grafo.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) disjoint.c -o $@

# AGM (Árvore Geradora Mínima)
agm.o: agm.c $(INC_DIR)/agm.h $(INC_DIR)/grafo.h $(INC_DIR)/disjoint.h $(INC_DIR)/lista.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) agm.c -o $@

# SVG
svg.o: svg.c $(INC_DIR)/svg.h $(INC_DIR)/quadra.h $(INC_DIR)/hashfile.h $(INC_DIR)/grafo.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) svg.c -o $@



# Unity
unity.o: $(UNITY_DIR)/unity.c $(UNITY_DIR)/unity.h $(UNITY_DIR)/unity_internals.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(UNITY_DIR)/unity.c -o $@

# Teste da Lista
t_lista: unity.o lista.o t_lista.o
	$(CC) -o $@ $(LDFLAGS) unity.o lista.o t_lista.o $(LIBS)
	./t_lista

t_lista.o: $(TST_DIR)/t_lista.c $(INC_DIR)/lista.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_lista.c -o $@

# Teste do Grafo
t_grafo: unity.o lista.o grafo.o t_grafo.o
	$(CC) -o $@ $(LDFLAGS) unity.o lista.o grafo.o t_grafo.o $(LIBS)
	./t_grafo

t_grafo.o: $(TST_DIR)/t_grafo.c $(INC_DIR)/grafo.h $(INC_DIR)/lista.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_grafo.c -o $@

# Teste do Dijkstra
t_dijkstra: unity.o lista.o grafo.o dijkstra.o t_dijkstra.o
	$(CC) -o $@ $(LDFLAGS) unity.o lista.o grafo.o dijkstra.o t_dijkstra.o $(LIBS)
	./t_dijkstra

t_dijkstra.o: $(TST_DIR)/t_dijkstra.c $(INC_DIR)/dijkstra.h $(INC_DIR)/grafo.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_dijkstra.c -o $@

# Teste dos Componentes Conexos
t_conexos: unity.o lista.o grafo.o conexos.o t_conexos.o
	$(CC) -o $@ $(LDFLAGS) unity.o lista.o grafo.o conexos.o t_conexos.o $(LIBS)
	./t_conexos

t_conexos.o: $(TST_DIR)/t_conexos.c $(INC_DIR)/conexos.h $(INC_DIR)/grafo.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_conexos.c -o $@

# Teste do Disjoint
t_disjoint: unity.o lista.o grafo.o disjoint.o t_disjoint.o
	$(CC) -o $@ $(LDFLAGS) unity.o lista.o grafo.o disjoint.o t_disjoint.o $(LIBS)
	./t_disjoint

t_disjoint.o: $(TST_DIR)/t_disjoint.c $(INC_DIR)/disjoint.h $(INC_DIR)/grafo.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_disjoint.c -o $@

# Teste da AGM
t_agm: unity.o lista.o grafo.o disjoint.o agm.o t_agm.o
	$(CC) -o $@ $(LDFLAGS) unity.o lista.o grafo.o disjoint.o agm.o t_agm.o $(LIBS)
	./t_agm

t_agm.o: $(TST_DIR)/t_agm.c $(INC_DIR)/agm.h $(INC_DIR)/grafo.h $(INC_DIR)/disjoint.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_agm.c -o $@

# Teste da Quadra
t_quadra: unity.o quadra.o t_quadra.o
	$(CC) -o $@ $(LDFLAGS) unity.o quadra.o t_quadra.o $(LIBS)
	./t_quadra

t_quadra.o: $(TST_DIR)/t_quadra.c $(INC_DIR)/quadra.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_quadra.c -o $@

# Teste do HashFile
t_hashfile: unity.o hashfile.o t_hashfile.o
	$(CC) -o $@ $(LDFLAGS) unity.o hashfile.o t_hashfile.o $(LIBS)
	./t_hashfile

t_hashfile.o: $(TST_DIR)/t_hashfile.c $(INC_DIR)/hashfile.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_hashfile.c -o $@

# Teste do SVG
t_svg: unity.o quadra.o hashfile.o svg.o t_svg.o
	$(CC) -o $@ $(LDFLAGS) unity.o quadra.o hashfile.o svg.o t_svg.o $(LIBS)
	./t_svg

t_svg.o: $(TST_DIR)/t_svg.c $(INC_DIR)/svg.h $(INC_DIR)/quadra.h $(INC_DIR)/hashfile.h $(UNITY_DIR)/unity.h
	$(CC) -c $(CFLAGS) -I$(INC_DIR) -I$(UNITY_DIR) $(TST_DIR)/t_svg.c -o $@

# Executa todos os testes
tstall: t_lista t_grafo t_dijkstra t_conexos t_disjoint t_agm t_quadra t_hashfile t_svg
	@echo "Todos os testes executados com sucesso!"



clean:
	rm -f *.o $(PROJ_NAME) t_lista t_grafo t_dijkstra t_conexos t_disjoint t_agm t_quadra t_hashfile t_svg
	@echo "Limpeza realizada"

valgrind: $(PROJ_NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(PROJ_NAME) -f teste.geo -v teste.via -q teste.qry -o ./saida

.PHONY: clean valgrind tstall
