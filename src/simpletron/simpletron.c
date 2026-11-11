/*
 * simpletron.c - Simulador de la computadora Simpletron (Fase 2: memoria extendida)
 *
 * Memoria de 1000 palabras decimales con signo (-99999 a +99999).
 * Formato de instruccion:  +CCOOO   CC = codigo de operacion, OOO = operando (000-999)
 *
 * Uso:
 *   simpletron                  carga programa.simp si existe; si no, carga interactiva
 *   simpletron archivo.sml      carga el programa desde el archivo indicado
 * Opciones:
 *   --traza      muestra cada instruccion antes de ejecutarla
 *   --sin-dump   no imprime el vaciado de memoria al terminar con HALT
 *   --eco        repite cada dato leido (util al redirigir la entrada desde un archivo)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "opcodes.h"

/* ---------------- Estado de la maquina ---------------- */
static int memory[TAM_MEMORIA];
static int accumulator = 0;          /* registro acumulador */
static int instructionCounter = 0;   /* direccion de la siguiente instruccion */
static int instructionRegister = 0;  /* instruccion en ejecucion */
static int operationCode = 0;        /* dos digitos izquierdos */
static int operand = 0;              /* tres digitos derechos */

static int traza = 0;
static int eco = 0;
static char mensajeError[200] = "";

/* ---------------- Prototipos ---------------- */
static void inicializar(void);
static int  cargarInteractivo(void);
static int  cargarArchivo(const char *nombre);
static int  ejecutar(void);
static int  cicloInstruccion(void);
static int  errorFatal(const char *mensaje);
static int  enRango(long long valor, const char *operacion);
static int  leerEntero(int *valor);
static void vaciadoMemoria(void);

/* ======================================================================= */

int main(int argc, char *argv[]) {
    const char *archivo = NULL;
    int dump = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--traza") == 0) traza = 1;
        else if (strcmp(argv[i], "--sin-dump") == 0) dump = 0;
        else if (strcmp(argv[i], "--eco") == 0) eco = 1;
        else if (argv[i][0] == '-') {
            fprintf(stderr, "Opcion desconocida: %s\n", argv[i]);
            fprintf(stderr, "Uso: simpletron [archivo.sml] [--traza] [--sin-dump] [--eco]\n");
            return 1;
        } else archivo = argv[i];
    }

    inicializar();

    if (archivo == NULL) {
        /* Sin argumento: se busca programa.simp (requerimiento 2.3) */
        FILE *prueba = fopen("programa.simp", "r");
        if (prueba != NULL) {
            fclose(prueba);
            archivo = "programa.simp";
        }
    }

    if (archivo != NULL) {
        if (!cargarArchivo(archivo)) {
            fprintf(stderr, "*** Error al cargar el programa: %s ***\n", mensajeError);
            return 1;
        }
        printf("*** Programa cargado desde %s ***\n", archivo);
    } else {
        cargarInteractivo();
    }

    int ok = ejecutar();
    if (dump || !ok) vaciadoMemoria();
    return ok ? 0 : 2;
}

static void inicializar(void) {
    for (int i = 0; i < TAM_MEMORIA; i++) memory[i] = 0;
    accumulator = instructionCounter = instructionRegister = 0;
    operationCode = operand = 0;
}

/* ======================================================================= */
/*                                 CARGA                                   */
/* ======================================================================= */

static int cargarInteractivo(void) {
    printf("*** Bienvenido a Simpletron 1000 ***\n");
    printf("*** Introduzca su programa una instruccion (o palabra de datos) ***\n");
    printf("*** a la vez. Se mostrara la direccion y un signo ?.           ***\n");
    printf("*** Teclee %d para terminar la carga.                     ***\n", CENTINELA);

    int direccion = 0;
    long long palabra;
    while (direccion < TAM_MEMORIA) {
        printf("%03d ? ", direccion);
        int leidos = scanf("%lld", &palabra);
        if (leidos == EOF) break;
        if (leidos != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("*** Entrada no numerica, intente de nuevo ***\n");
            continue;
        }
        if (eco) printf("%lld\n", palabra);
        if (palabra == CENTINELA) break;
        if (palabra < MIN_VAL || palabra > MAX_VAL) {
            printf("*** Palabra fuera de rango (%d a +%d), intente de nuevo ***\n", MIN_VAL, MAX_VAL);
            continue;
        }
        memory[direccion++] = (int)palabra;
    }
    printf("*** Carga del programa completada ***\n");
    return direccion;
}

/*
 * Formato del archivo:
 *   - una palabra por linea, se cargan desde la direccion 000
 *   - ';' o '#' inician un comentario hasta el fin de la linea
 *   - @NNN cambia la direccion de carga (para colocar la zona de datos)
 *   - -999999 (opcional) termina la carga
 */
static int cargarArchivo(const char *nombre) {
    FILE *f = fopen(nombre, "r");
    if (f == NULL) {
        snprintf(mensajeError, sizeof mensajeError, "no se pudo abrir el archivo '%s'", nombre);
        return 0;
    }

    char linea[256];
    int numLinea = 0, direccion = 0, cargadas = 0;

    while (fgets(linea, sizeof linea, f) != NULL) {
        numLinea++;
        char *comentario = strpbrk(linea, ";#");
        if (comentario != NULL) *comentario = '\0';

        char token[64], sobra[64];
        int n = sscanf(linea, "%63s %63s", token, sobra);
        if (n <= 0) continue;                         /* linea vacia */
        if (token[0] == '@') {                        /* cambio de direccion */
            char *fin;
            long dir = strtol(token + 1, &fin, 10);
            if (token[1] == '\0' || *fin != '\0' || dir < 0 || dir >= TAM_MEMORIA) {
                snprintf(mensajeError, sizeof mensajeError,
                         "linea %d: direccion invalida '%s'", numLinea, token);
                fclose(f);
                return 0;
            }
            if (n == 2) {
                snprintf(mensajeError, sizeof mensajeError,
                         "linea %d: se esperaba una sola palabra, sobra '%s'", numLinea, sobra);
                fclose(f);
                return 0;
            }
            direccion = (int)dir;
            continue;
        }

        char *fin;
        long long palabra = strtoll(token, &fin, 10);
        if (*fin != '\0' || fin == token || strlen(token) > 8) {
            snprintf(mensajeError, sizeof mensajeError,
                     "linea %d: '%s' no es una palabra SML valida", numLinea, token);
            fclose(f);
            return 0;
        }
        if (n == 2) {
            snprintf(mensajeError, sizeof mensajeError,
                     "linea %d: se esperaba una sola palabra, sobra '%s'", numLinea, sobra);
            fclose(f);
            return 0;
        }
        if (palabra == CENTINELA) break;
        if (palabra < MIN_VAL || palabra > MAX_VAL) {
            snprintf(mensajeError, sizeof mensajeError,
                     "linea %d: la palabra %s esta fuera del rango permitido (%d a +%d)",
                     numLinea, token, MIN_VAL, MAX_VAL);
            fclose(f);
            return 0;
        }
        if (direccion >= TAM_MEMORIA) {
            snprintf(mensajeError, sizeof mensajeError,
                     "linea %d: el programa excede la memoria (%d palabras)", numLinea, TAM_MEMORIA);
            fclose(f);
            return 0;
        }
        memory[direccion++] = (int)palabra;
        cargadas++;
    }
    fclose(f);

    if (cargadas == 0) {
        snprintf(mensajeError, sizeof mensajeError, "el archivo '%s' no contiene instrucciones", nombre);
        return 0;
    }
    return 1;
}

/* ======================================================================= */
/*                               EJECUCION                                 */
/* ======================================================================= */

static int ejecutar(void) {
    instructionCounter = 0;
    accumulator = 0;
    mensajeError[0] = '\0';
    printf("*** Inicia la ejecucion del programa ***\n");

    while (cicloInstruccion()) { }

    if (mensajeError[0] != '\0') {
        printf("\n*** ERROR FATAL: %s ***\n", mensajeError);
        printf("*** La ejecucion de Simpletron termino en forma anormal ***\n");
        return 0;
    }
    printf("*** Ejecucion de Simpletron terminada (HALT) ***\n");
    return 1;
}

static int errorFatal(const char *mensaje) {
    snprintf(mensajeError, sizeof mensajeError, "%s (instruccion en la direccion %03d)",
             mensaje, instructionCounter);
    return 0;
}

/* Verifica que el resultado quepa en una palabra antes de guardarlo en el acumulador. */
static int enRango(long long valor, const char *operacion) {
    if (valor < MIN_VAL || valor > MAX_VAL) {
        char msg[120];
        snprintf(msg, sizeof msg, "desbordamiento del acumulador en %s (resultado %lld)", operacion, valor);
        return errorFatal(msg);
    }
    accumulator = (int)valor;
    return 1;
}

static int leerEntero(int *valor) {
    long long v;
    while (1) {
        printf("? ");
        int leidos = scanf("%lld", &v);
        if (leidos == 1) {
            if (eco) printf("%lld\n", v);
            if (v >= MIN_VAL && v <= MAX_VAL) {
                *valor = (int)v;
                return 1;
            }
            printf("*** Valor fuera de rango (%d a +%d), intente de nuevo ***\n", MIN_VAL, MAX_VAL);
            continue;
        }
        if (leidos == EOF) return errorFatal("READ sin datos: se termino la entrada");
        char basura[100];
        if (scanf("%99s", basura) == 1 && eco) printf("%s\n", basura);
        printf("*** Entrada no numerica, intente de nuevo ***\n");
    }
}

/* Ciclo de instruccion. Devuelve 1 si la ejecucion debe continuar. */
static int cicloInstruccion(void) {
    /* 0. Validar el contador de instrucciones */
    if (instructionCounter < 0 || instructionCounter >= TAM_MEMORIA) {
        snprintf(mensajeError, sizeof mensajeError,
                 "direccion invalida %d: el programa salio de la memoria sin HALT",
                 instructionCounter);
        return 0;
    }

    /* 1. Traer la instruccion */
    instructionRegister = memory[instructionCounter];

    /* 2. Decodificar */
    operationCode = instructionRegister / 1000;
    operand = instructionRegister % 1000;
    if (instructionRegister < 0 || nombreOpcode(operationCode) == NULL) {
        char msg[80];
        snprintf(msg, sizeof msg, "codigo de operacion invalido %+06d", instructionRegister);
        return errorFatal(msg);
    }

    if (traza)
        printf("  [traza] %03d: %+06d  %-12s %03d   acc=%+06d\n", instructionCounter,
               instructionRegister, nombreOpcode(operationCode), operand, accumulator);

    /* 3. Ejecutar. Por defecto se avanza a la siguiente direccion. */
    int siguiente = instructionCounter + 1;
    switch (operationCode) {
        case READ: {
            int valor;
            if (!leerEntero(&valor)) return 0;
            memory[operand] = valor;
            break;
        }
        case WRITE:
            printf("%d\n", memory[operand]);
            break;

        case NEWLINE:
            printf("\n");
            break;

        case READ_STRING: {
            /* memoria[op] = longitud, memoria[op+1 ...] = codigo ASCII de cada caracter */
            char texto[256];
            printf("? ");
            if (scanf(" %255[^\n]", texto) != 1)
                return errorFatal("READ_STRING sin datos: se termino la entrada");
            if (eco) printf("%s\n", texto);
            int len = (int)strlen(texto);
            if (operand + len >= TAM_MEMORIA) {
                char msg[100];
                snprintf(msg, sizeof msg, "READ_STRING: la cadena de %d caracteres no cabe desde la direccion %03d",
                         len, operand);
                return errorFatal(msg);
            }
            memory[operand] = len;
            for (int i = 0; i < len; i++)
                memory[operand + 1 + i] = (unsigned char)texto[i];
            break;
        }
        case WRITE_STRING: {
            int len = memory[operand];
            if (len < 0 || operand + len >= TAM_MEMORIA)
                return errorFatal("WRITE_STRING: no hay una cadena valida en esa direccion");
            for (int i = 1; i <= len; i++) {
                int c = memory[operand + i];
                if (c < 0 || c > 255)
                    return errorFatal("WRITE_STRING: la cadena contiene un caracter invalido");
                putchar(c);
            }
            break;                                  /* sin salto de linea: usar NEWLINE */
        }

        case LOAD:
            accumulator = memory[operand];
            break;
        case STORE:
            memory[operand] = accumulator;
            break;

        case ADD:
            if (!enRango((long long)accumulator + memory[operand], "ADD")) return 0;
            break;
        case SUBTRACT:
            if (!enRango((long long)accumulator - memory[operand], "SUBTRACT")) return 0;
            break;
        case MULTIPLY:
            if (!enRango((long long)accumulator * memory[operand], "MULTIPLY")) return 0;
            break;
        case DIVIDE:
            if (memory[operand] == 0) return errorFatal("intento de division entre cero");
            accumulator /= memory[operand];
            break;
        case MODULO:
            if (memory[operand] == 0) return errorFatal("intento de modulo entre cero");
            accumulator %= memory[operand];
            break;
        case EXPONENT: {
            int exp = memory[operand];
            if (exp < 0) return errorFatal("exponente negativo no soportado");
            long long r = 1;
            for (int i = 0; i < exp; i++) {
                r *= accumulator;
                if (r < MIN_VAL || r > MAX_VAL) {
                    char msg[100];
                    snprintf(msg, sizeof msg, "desbordamiento del acumulador en EXPONENT (%d^%d)",
                             accumulator, exp);
                    return errorFatal(msg);
                }
                if (r == 0 || r == 1) break;        /* base 0 o 1: ya no cambia */
            }
            if (exp > 0 && accumulator == -1) r = (exp % 2) ? -1 : 1;
            accumulator = (int)r;
            break;
        }

        case BRANCH:
            siguiente = operand;
            break;
        case BRANCHNEG:
            if (accumulator < 0) siguiente = operand;
            break;
        case BRANCHZERO:
            if (accumulator == 0) siguiente = operand;
            break;
        case HALT:
            return 0;

        /* Punto flotante en punto fijo: se guarda el valor x 100 (2 decimales).
           Asi el numero sigue siendo una palabra valida de -99999 a +99999
           (de -999.99 a +999.99). */
        case READ_FLOAT: {
            double x;
            printf("? ");
            if (scanf("%lf", &x) != 1) return errorFatal("READ_FLOAT sin datos validos");
            if (eco) printf("%.2f\n", x);
            long long escalado = llround(x * ESCALA_FLOTANTE);
            if (escalado < MIN_VAL || escalado > MAX_VAL)
                return errorFatal("READ_FLOAT: valor fuera de rango (-999.99 a 999.99)");
            memory[operand] = (int)escalado;
            break;
        }
        case WRITE_FLOAT:
            printf("%.2f\n", memory[operand] / (double)ESCALA_FLOTANTE);
            break;
        case ADD_FLOAT:
            if (!enRango((long long)accumulator + memory[operand], "ADD_FLOAT")) return 0;
            break;
        case SUB_FLOAT:
            if (!enRango((long long)accumulator - memory[operand], "SUB_FLOAT")) return 0;
            break;
    }

    /* 4. Actualizar el contador de instrucciones */
    instructionCounter = siguiente;
    return 1;
}

/* ======================================================================= */
/*                          VACIADO DE MEMORIA                             */
/* ======================================================================= */

static void vaciadoMemoria(void) {
    printf("\nREGISTROS:\n");
    printf("accumulator          %+06d\n", accumulator);
    printf("instructionCounter      %03d\n", instructionCounter);
    printf("instructionRegister  %+06d\n", instructionRegister);
    printf("operationCode            %02d\n", operationCode);
    printf("operand                 %03d\n", operand);

    printf("\nMEMORIA (se omiten las filas que estan completamente en cero):\n     ");
    for (int c = 0; c < 10; c++) printf("%7d", c);
    printf("\n");

    int omitidas = 0;
    for (int fila = 0; fila < TAM_MEMORIA; fila += 10) {
        int vacia = 1;
        for (int c = 0; c < 10; c++)
            if (memory[fila + c] != 0) vacia = 0;
        if (vacia && fila != 0) { omitidas++; continue; }
        if (omitidas > 0) { printf("  ... (%d filas en cero)\n", omitidas); omitidas = 0; }
        printf("%03d  ", fila);
        for (int c = 0; c < 10; c++) printf(" %+06d", memory[fila + c]);
        printf("\n");
    }
    if (omitidas > 0) printf("  ... (%d filas en cero)\n", omitidas);
}
