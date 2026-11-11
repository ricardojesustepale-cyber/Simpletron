# Makefile - Proyecto Simpletron
#   make           compila bin/simpletron, bin/evaluador y bin/compilador
#   make pruebas   compila y ejecuta las pruebas automaticas
#   make clean     borra bin/ y los resultados de las pruebas

CC     ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2
BIN    := bin

all: $(BIN)/simpletron $(BIN)/evaluador $(BIN)/compilador

$(BIN):
	mkdir -p $(BIN)

$(BIN)/simpletron: src/simpletron/simpletron.c src/simpletron/opcodes.h | $(BIN)
	$(CC) $(CFLAGS) -o $@ src/simpletron/simpletron.c -lm

$(BIN)/evaluador: src/expresiones/evaluador.c src/expresiones/expresiones.c src/expresiones/*.h | $(BIN)
	$(CC) $(CFLAGS) -o $@ src/expresiones/evaluador.c src/expresiones/expresiones.c

$(BIN)/compilador: src/compilador/compilador.c src/expresiones/expresiones.c src/expresiones/*.h src/simpletron/opcodes.h | $(BIN)
	$(CC) $(CFLAGS) -o $@ src/compilador/compilador.c src/expresiones/expresiones.c

pruebas: all
	bash pruebas/ejecutar_pruebas.sh

clean:
	rm -rf $(BIN) pruebas/obtenidas

.PHONY: all pruebas clean
