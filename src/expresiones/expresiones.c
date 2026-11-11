/*
 * expresiones.c - Implementacion del modulo de expresiones
 */
#include "expresiones.h"
#include "pila.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int precedencia(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

bool asociativoDerecha(char op) { return op == '^'; }

bool esOperador(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
}

static bool esApertura(char c) { return c == '(' || c == '[' || c == '{'; }
static bool esCierre(char c) { return c == ')' || c == ']' || c == '}'; }

static bool esPareja(char apertura, char cierre) {
    return (apertura == '(' && cierre == ')') ||
           (apertura == '[' && cierre == ']') ||
           (apertura == '{' && cierre == '}');
}

static void ponerError(ErrorExpresion *err, int pos, const char *mensaje) {
    if (err == NULL) return;
    err->posicion = pos;
    snprintf(err->mensaje, sizeof err->mensaje, "%s", mensaje);
}

/* ======================================================================= */
/* 1. Agrupacion y profundidad de anidamiento                              */
/* ======================================================================= */
ResultadoAgrupacion verificarAgrupacion(const char *texto, int limite) {
    ResultadoAgrupacion r = { true, 0, -1, "" };
    Pila simbolos, posiciones;
    initPila(&simbolos);
    initPila(&posiciones);

    for (int i = 0; texto[i] != '\0'; i++) {
        char symb = texto[i];
        if (esApertura(symb)) {
            push(&simbolos, symb);
            push(&posiciones, i);
            if (tamanio(&simbolos) > r.profundidadMaxima) r.profundidadMaxima = tamanio(&simbolos);
            if (limite > 0 && tamanio(&simbolos) > limite) {
                r.valido = false;
                r.posicion = i;
                snprintf(r.mensaje, sizeof r.mensaje,
                         "la profundidad de anidamiento (%d) excede el limite de %d",
                         tamanio(&simbolos), limite);
                return r;
            }
        } else if (esCierre(symb)) {
            long abierto, posAbierto;
            if (!pop(&simbolos, &abierto)) {
                r.valido = false;
                r.posicion = i;
                snprintf(r.mensaje, sizeof r.mensaje, "'%c' sin simbolo de apertura", symb);
                return r;
            }
            pop(&posiciones, &posAbierto);
            if (!esPareja((char)abierto, symb)) {
                r.valido = false;
                r.posicion = i;
                snprintf(r.mensaje, sizeof r.mensaje,
                         "'%c' no corresponde con '%c' abierto en la posicion %ld",
                         symb, (char)abierto, posAbierto);
                return r;
            }
        }
    }
    if (!isEmpty(&simbolos)) {
        long abierto, posAbierto;
        pop(&simbolos, &abierto);
        pop(&posiciones, &posAbierto);
        r.valido = false;
        r.posicion = (int)posAbierto;
        snprintf(r.mensaje, sizeof r.mensaje, "'%c' sin cerrar", (char)abierto);
    }
    return r;
}

/* ======================================================================= */
/* 2. Tokenizacion y validacion de sintaxis                                */
/* ======================================================================= */
static int separar(const char *t, Token tokens[], int max, ErrorExpresion *err) {
    int n = 0;
    char msg[120];
    for (int i = 0; t[i] != '\0';) {
        char c = t[i];
        if (isspace((unsigned char)c)) { i++; continue; }
        if (n >= max) { ponerError(err, i, "expresion demasiado larga"); return -1; }

        Token tok;
        tok.posicion = i;
        tok.valor = 0;
        tok.simbolo = 0;
        if (isdigit((unsigned char)c)) {
            long v = 0;
            while (isdigit((unsigned char)t[i])) {
                v = v * 10 + (t[i] - '0');
                if (v > 999999999L) { ponerError(err, tok.posicion, "constante numerica demasiado grande"); return -1; }
                i++;
            }
            if (isalpha((unsigned char)t[i])) { ponerError(err, i, "numero seguido de una letra"); return -1; }
            tok.tipo = NUMERO;
            tok.valor = v;
        } else if (islower((unsigned char)c)) {
            if (isalpha((unsigned char)t[i + 1])) {
                ponerError(err, i, "las variables deben ser de una sola letra");
                return -1;
            }
            tok.tipo = VARIABLE;
            tok.simbolo = c;
            i++;
        } else if (isupper((unsigned char)c)) {
            snprintf(msg, sizeof msg, "'%c': solo se permiten letras minusculas", c);
            ponerError(err, i, msg);
            return -1;
        } else if (esOperador(c)) {
            tok.tipo = OPERADOR; tok.simbolo = c; i++;
        } else if (esApertura(c)) {
            tok.tipo = ABRE; tok.simbolo = c; i++;
        } else if (esCierre(c)) {
            tok.tipo = CIERRA; tok.simbolo = c; i++;
        } else {
            snprintf(msg, sizeof msg, "caracter invalido '%c'", c);
            ponerError(err, i, msg);
            return -1;
        }
        tokens[n++] = tok;
    }
    return n;
}

static void textoToken(const Token *t, char *dst, size_t tam) {
    if (t->tipo == NUMERO) snprintf(dst, tam, "%ld", t->valor);
    else snprintf(dst, tam, "%c", t->simbolo);
}

int tokenizar(const char *texto, Token tokens[], int max, ErrorExpresion *err) {
    ResultadoAgrupacion g = verificarAgrupacion(texto, 0);
    if (!g.valido) {
        char msg[160];
        snprintf(msg, sizeof msg, "agrupacion incorrecta: %s", g.mensaje);
        ponerError(err, g.posicion, msg);
        return -1;
    }

    Token crudos[MAX_TOKENS];
    int nc = separar(texto, crudos, MAX_TOKENS, err);
    if (nc < 0) return -1;
    if (nc == 0) { ponerError(err, 0, "expresion vacia"); return -1; }

    /* Maquina de dos estados: se espera un operando o se espera un operador */
    int n = 0;
    bool esperaOperando = true;
    char txt[24], msg[120];
    for (int i = 0; i < nc; i++) {
        Token tok = crudos[i];
        if (n + 2 > max) { ponerError(err, tok.posicion, "expresion demasiado larga"); return -1; }
        if (esperaOperando) {
            if (tok.tipo == NUMERO || tok.tipo == VARIABLE) {
                tokens[n++] = tok;
                esperaOperando = false;
            } else if (tok.tipo == ABRE) {
                tokens[n++] = tok;
            } else if (tok.tipo == OPERADOR && tok.simbolo == '+') {
                /* mas unario: se ignora */
            } else if (tok.tipo == OPERADOR && tok.simbolo == '-') {
                /* menos unario */
                bool numeroSimple = i + 1 < nc && crudos[i + 1].tipo == NUMERO &&
                                    !(i + 2 < nc && crudos[i + 2].tipo == OPERADOR &&
                                      crudos[i + 2].simbolo == '^');
                if (numeroSimple) {                  /* -5 -> constante negativa */
                    Token neg = crudos[++i];
                    neg.valor = -neg.valor;
                    neg.posicion = tok.posicion;
                    tokens[n++] = neg;
                    esperaOperando = false;
                } else {                             /* -x -> -1 * x */
                    Token menosUno = { NUMERO, -1, 0, tok.posicion };
                    Token por = { OPERADOR, 0, '*', tok.posicion };
                    tokens[n++] = menosUno;
                    tokens[n++] = por;
                }
            } else {
                textoToken(&tok, txt, sizeof txt);
                snprintf(msg, sizeof msg, "falta un operando antes de '%s'", txt);
                ponerError(err, tok.posicion, msg);
                return -1;
            }
        } else {
            if (tok.tipo == OPERADOR) {
                tokens[n++] = tok;
                esperaOperando = true;
            } else if (tok.tipo == CIERRA) {
                tokens[n++] = tok;
            } else {
                textoToken(&tok, txt, sizeof txt);
                snprintf(msg, sizeof msg, "falta un operador antes de '%s'", txt);
                ponerError(err, tok.posicion, msg);
                return -1;
            }
        }
    }
    if (esperaOperando) {
        ponerError(err, (int)strlen(texto), "la expresion termina sin operando");
        return -1;
    }
    return n;
}

/* ======================================================================= */
/* 3. Infijo -> postfijo (pila de indices de tokens)                        */
/* ======================================================================= */
int infijoAPostfijo(const Token infijo[], int n, Token postfijo[], ErrorExpresion *err) {
    Pila p;
    initPila(&p);
    int j = 0;
    long cima;

    for (int i = 0; i < n; i++) {
        const Token *symb = &infijo[i];
        switch (symb->tipo) {
            case NUMERO:
            case VARIABLE:
                postfijo[j++] = *symb;                       /* operandos pasan directo */
                break;
            case ABRE:
                push(&p, i);
                break;
            case OPERADOR:
                /* sacar operadores de mayor precedencia, o de igual precedencia
                   si el nuevo es asociativo a la izquierda */
                while (peek(&p, &cima) && infijo[cima].tipo == OPERADOR) {
                    char top = infijo[cima].simbolo;
                    bool sacar = precedencia(top) > precedencia(symb->simbolo) ||
                                 (precedencia(top) == precedencia(symb->simbolo) &&
                                  !asociativoDerecha(symb->simbolo));
                    if (!sacar) break;
                    pop(&p, &cima);
                    postfijo[j++] = infijo[cima];
                }
                if (!push(&p, i)) { ponerError(err, symb->posicion, "expresion demasiado larga"); return -1; }
                break;
            case CIERRA:
                while (peek(&p, &cima) && infijo[cima].tipo != ABRE) {
                    pop(&p, &cima);
                    postfijo[j++] = infijo[cima];
                }
                if (!pop(&p, &cima)) {                       /* descarta la apertura */
                    ponerError(err, symb->posicion, "simbolo de cierre sin apertura");
                    return -1;
                }
                break;
        }
    }
    while (pop(&p, &cima)) {
        if (infijo[cima].tipo == ABRE) {
            ponerError(err, infijo[cima].posicion, "simbolo de apertura sin cerrar");
            return -1;
        }
        postfijo[j++] = infijo[cima];
    }
    return j;
}

/* ======================================================================= */
/* 4. Evaluacion del postfijo                                              */
/* ======================================================================= */
#define LIMITE_EVAL 1000000000000000000.0   /* para detectar desbordamiento */

static bool aplicar(char op, long a, long b, long *r, ErrorExpresion *err) {
    switch (op) {
        case '+': *r = a + b; return true;
        case '-': *r = a - b; return true;
        case '*':
            if ((double)a * (double)b > LIMITE_EVAL || (double)a * (double)b < -LIMITE_EVAL) {
                ponerError(err, -1, "desbordamiento en la multiplicacion");
                return false;
            }
            *r = a * b; return true;
        case '/':
            if (b == 0) { ponerError(err, -1, "division entre cero"); return false; }
            *r = a / b; return true;
        case '%':
            if (b == 0) { ponerError(err, -1, "residuo entre cero"); return false; }
            *r = a % b; return true;
        case '^':
            if (b < 0) { ponerError(err, -1, "exponente negativo"); return false; }
            if (a == 0) { *r = (b == 0) ? 1 : 0; return true; }
            if (a == 1) { *r = 1; return true; }
            if (a == -1) { *r = (b % 2) ? -1 : 1; return true; }
            *r = 1;
            for (long i = 0; i < b; i++) {
                if ((double)*r * (double)a > LIMITE_EVAL || (double)*r * (double)a < -LIMITE_EVAL) {
                    ponerError(err, -1, "desbordamiento en la potencia");
                    return false;
                }
                *r *= a;
            }
            return true;
    }
    ponerError(err, -1, "operador desconocido");
    return false;
}

bool evaluarPostfijo(const Token postfijo[], int n, const long valores[26],
                     const bool definida[26], long *resultado, ErrorExpresion *err) {
    Pila p;
    initPila(&p);
    char msg[80];

    for (int i = 0; i < n; i++) {
        const Token *symb = &postfijo[i];
        if (symb->tipo == NUMERO) {
            push(&p, symb->valor);
        } else if (symb->tipo == VARIABLE) {
            int k = symb->simbolo - 'a';
            if (valores == NULL || definida == NULL || !definida[k]) {
                snprintf(msg, sizeof msg, "la variable '%c' no tiene valor", symb->simbolo);
                ponerError(err, symb->posicion, msg);
                return false;
            }
            push(&p, valores[k]);
        } else if (symb->tipo == OPERADOR) {
            long opnd1, opnd2, valor;
            if (!pop(&p, &opnd2) || !pop(&p, &opnd1)) {    /* el tope es el 2o operando */
                ponerError(err, symb->posicion, "faltan operandos");
                return false;
            }
            if (!aplicar(symb->simbolo, opnd1, opnd2, &valor, err)) return false;
            push(&p, valor);
        } else {
            ponerError(err, symb->posicion, "el postfijo contiene simbolos de agrupacion");
            return false;
        }
    }
    if (tamanio(&p) != 1) {
        ponerError(err, -1, "expresion postfija mal formada");
        return false;
    }
    pop(&p, resultado);
    return true;
}

void tokensATexto(const Token tokens[], int n, char *destino, size_t tam) {
    size_t usado = 0;
    destino[0] = '\0';
    for (int i = 0; i < n && usado < tam; i++) {
        char t[24];
        textoToken(&tokens[i], t, sizeof t);
        int w = snprintf(destino + usado, tam - usado, "%s%s", i ? " " : "", t);
        if (w < 0) break;
        usado += (size_t)w;
    }
}
