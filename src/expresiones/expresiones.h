/*
 * expresiones.h - Evaluacion de expresiones (base del compilador Simple)
 *
 *   1. verificarAgrupacion : balance de ( ) [ ] { } y profundidad de anidamiento
 *   2. tokenizar           : numeros de varios digitos, variables de una letra,
 *                            operadores + - * / % ^ y simbolos de agrupacion
 *   3. infijoAPostfijo     : respeta precedencia y asociatividad
 *   4. evaluarPostfijo     : evaluacion con pila de valores
 */
#ifndef EXPRESIONES_H
#define EXPRESIONES_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_TOKENS 256

typedef enum { NUMERO, VARIABLE, OPERADOR, ABRE, CIERRA } TipoToken;

typedef struct {
    TipoToken tipo;
    long valor;      /* NUMERO */
    char simbolo;    /* VARIABLE, OPERADOR, ABRE, CIERRA */
    int posicion;    /* posicion en el texto original */
} Token;

typedef struct {
    bool valido;
    int profundidadMaxima;
    int posicion;          /* donde esta el error (-1 si no hay) */
    char mensaje[120];
} ResultadoAgrupacion;

/* Mensaje y posicion del ultimo error de tokenizar / convertir / evaluar */
typedef struct {
    char mensaje[160];
    int posicion;
} ErrorExpresion;

int  precedencia(char op);         /* ^ = 3, * / % = 2, + - = 1 */
bool asociativoDerecha(char op);   /* solo ^ */
bool esOperador(char c);

/* limite > 0: tambien falla si el anidamiento lo excede */
ResultadoAgrupacion verificarAgrupacion(const char *texto, int limite);

/* Devuelve el numero de tokens o -1 si hay error.
   El menos unario se convierte en constante negativa (-5) o en "-1 *". */
int tokenizar(const char *texto, Token tokens[], int max, ErrorExpresion *err);

/* Devuelve el numero de tokens en postfijo o -1 si hay error. */
int infijoAPostfijo(const Token infijo[], int n, Token postfijo[], ErrorExpresion *err);

/* valores['a'-'a'..'z'-'a'] con su bandera definida[]; devuelve false si hay error */
bool evaluarPostfijo(const Token postfijo[], int n, const long valores[26],
                     const bool definida[26], long *resultado, ErrorExpresion *err);

/* Escribe los tokens separados por espacios en 'destino' */
void tokensATexto(const Token tokens[], int n, char *destino, size_t tam);

#endif
