/*
 * opcodes.h - Definicion unica del lenguaje de maquina SML (Simpletron 1000).
 * La comparten el simulador (src/simpletron) y el compilador (src/compilador).
 *
 * Instruccion:  +CCOOO   CC = codigo de operacion, OOO = operando (000-999)
 */
#ifndef OPCODES_H
#define OPCODES_H

#include <stddef.h>

#define TAM_MEMORIA      1000
#define MIN_VAL         -99999
#define MAX_VAL          99999
#define CENTINELA       -999999   /* fin de la carga interactiva o del archivo */
#define FACTOR_OPCODE    1000     /* instruccion = opcode * 1000 + operando */
#define ESCALA_FLOTANTE  100      /* flotantes en punto fijo: 2 decimales */

/* Entrada / salida */
#define READ          10
#define WRITE         11
#define NEWLINE       12   /* extension: salto de linea */
#define READ_STRING   13   /* extension: lee una cadena */
#define WRITE_STRING  14   /* extension: imprime una cadena */

/* Carga y almacenamiento */
#define LOAD          20
#define STORE         21

/* Aritmetica */
#define ADD           30
#define SUBTRACT      31
#define DIVIDE        32
#define MULTIPLY      33
#define MODULO        34   /* extension: residuo */
#define EXPONENT      35   /* extension: potencia */

/* Control */
#define BRANCH        40
#define BRANCHNEG     41
#define BRANCHZERO    42
#define HALT          43

/* Punto flotante (extension) */
#define READ_FLOAT    50
#define WRITE_FLOAT   51
#define ADD_FLOAT     52
#define SUB_FLOAT     53

/* Nombre mnemonico del opcode, o NULL si no existe. */
static inline const char *nombreOpcode(int opcode) {
    switch (opcode) {
        case READ:         return "READ";
        case WRITE:        return "WRITE";
        case NEWLINE:      return "NEWLINE";
        case READ_STRING:  return "READ_STRING";
        case WRITE_STRING: return "WRITE_STRING";
        case LOAD:         return "LOAD";
        case STORE:        return "STORE";
        case ADD:          return "ADD";
        case SUBTRACT:     return "SUBTRACT";
        case DIVIDE:       return "DIVIDE";
        case MULTIPLY:     return "MULTIPLY";
        case MODULO:       return "MODULO";
        case EXPONENT:     return "EXPONENT";
        case BRANCH:       return "BRANCH";
        case BRANCHNEG:    return "BRANCHNEG";
        case BRANCHZERO:   return "BRANCHZERO";
        case HALT:         return "HALT";
        case READ_FLOAT:   return "READ_FLOAT";
        case WRITE_FLOAT:  return "WRITE_FLOAT";
        case ADD_FLOAT:    return "ADD_FLOAT";
        case SUB_FLOAT:    return "SUB_FLOAT";
        default:           return NULL;
    }
}

#endif
