/*
 * evaluador.c - Programa de prueba del modulo de expresiones
 *
 * Uso:
 *   evaluador                         lee expresiones del teclado
 *   evaluador archivo.txt             lee expresiones de un archivo
 *   evaluador --profundidad N ...     limita el anidamiento a N niveles
 *
 * Cada linea:  expresion              ej.  (2 + 3) * 4 ^ 2
 *              expresion ; a=1 b=2    ej.  {a + [b * 2]} - c ; a=1 b=2 c=3
 * Las lineas vacias o que empiezan con '#' se ignoran.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expresiones.h"

static void marcar(const char *expresion, int posicion) {
    printf("             %s\n             %*s^\n", expresion, posicion, "");
}

static void procesar(char *linea, int limite) {
    long valores[26] = {0};
    bool definida[26] = {false};
    bool hayVariables = false;

    char *pc = strchr(linea, ';');
    if (pc != NULL) {
        *pc = '\0';
        char *asig = strtok(pc + 1, " \t\r\n");
        while (asig != NULL) {
            char letra;
            long valor;
            if (sscanf(asig, "%c=%ld", &letra, &valor) == 2 && islower((unsigned char)letra)) {
                valores[letra - 'a'] = valor;
                definida[letra - 'a'] = true;
                hayVariables = true;
            } else {
                printf("  (se ignora la asignacion invalida '%s')\n", asig);
            }
            asig = strtok(NULL, " \t\r\n");
        }
    }
    /* quitar espacios finales */
    size_t len = strlen(linea);
    while (len > 0 && isspace((unsigned char)linea[len - 1])) linea[--len] = '\0';

    printf("Expresion  : %s\n", linea);

    ResultadoAgrupacion g = verificarAgrupacion(linea, limite);
    if (!g.valido) {
        printf("Agrupacion : INCORRECTA - %s\n", g.mensaje);
        marcar(linea, g.posicion);
        printf("\n");
        return;
    }
    printf("Agrupacion : correcta (profundidad maxima %d)\n", g.profundidadMaxima);

    Token infijo[MAX_TOKENS], postfijo[MAX_TOKENS];
    ErrorExpresion err = { "", -1 };
    int n = tokenizar(linea, infijo, MAX_TOKENS, &err);
    if (n >= 0) n = infijoAPostfijo(infijo, n, postfijo, &err);
    if (n < 0) {
        printf("Sintaxis   : ERROR - %s\n", err.mensaje);
        if (err.posicion >= 0) marcar(linea, err.posicion);
        printf("\n");
        return;
    }

    char texto[1024];
    tokensATexto(postfijo, n, texto, sizeof texto);
    printf("Postfijo   : %s\n", texto);
    if (hayVariables) {
        printf("Variables  :");
        for (int i = 0; i < 26; i++)
            if (definida[i]) printf(" %c=%ld", 'a' + i, valores[i]);
        printf("\n");
    }
    long resultado;
    if (evaluarPostfijo(postfijo, n, valores, definida, &resultado, &err))
        printf("Resultado  : %ld\n\n", resultado);
    else
        printf("Resultado  : no se puede evaluar (%s)\n\n", err.mensaje);
}

int main(int argc, char *argv[]) {
    int limite = 0;
    const char *archivo = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--profundidad") == 0 && i + 1 < argc) limite = atoi(argv[++i]);
        else archivo = argv[i];
    }

    FILE *in = stdin;
    if (archivo != NULL) {
        in = fopen(archivo, "r");
        if (in == NULL) {
            fprintf(stderr, "No se pudo abrir '%s'\n", archivo);
            return 1;
        }
    } else {
        printf("=== Evaluador de expresiones ===\n"
               "Escriba una expresion por linea (Ctrl+Z / Ctrl+D para salir)\n\n");
    }
    if (limite > 0) printf("(Limite de anidamiento: %d niveles)\n\n", limite);

    char linea[512];
    while (fgets(linea, sizeof linea, in) != NULL) {
        linea[strcspn(linea, "\r\n")] = '\0';
        char *inicio = linea;
        while (isspace((unsigned char)*inicio)) inicio++;
        if (*inicio == '\0' || *inicio == '#') continue;
        procesar(inicio, limite);
    }
    if (in != stdin) fclose(in);
    return 0;
}
