/*
 * pila.h - Pila (LIFO) de enteros largos.
 * La usan el verificador de agrupacion (guarda caracteres), el convertidor
 * infijo -> postfijo (guarda indices de tokens), el evaluador (guarda valores)
 * y el compilador (guarda direcciones de memoria).
 */
#ifndef PILA_H
#define PILA_H

#include <stdbool.h>

#define MAX_PILA 256

typedef struct {
    long items[MAX_PILA];
    int top;
} Pila;

static inline void initPila(Pila *p) { p->top = -1; }
static inline bool isEmpty(const Pila *p) { return p->top == -1; }
static inline bool isFull(const Pila *p) { return p->top == MAX_PILA - 1; }
static inline int  tamanio(const Pila *p) { return p->top + 1; }

/* Devuelven false si la operacion no se puede hacer (pila llena / vacia). */
static inline bool push(Pila *p, long valor) {
    if (isFull(p)) return false;
    p->items[++p->top] = valor;
    return true;
}

static inline bool pop(Pila *p, long *valor) {
    if (isEmpty(p)) return false;
    *valor = p->items[p->top--];
    return true;
}

static inline bool peek(const Pila *p, long *valor) {
    if (isEmpty(p)) return false;
    *valor = p->items[p->top];
    return true;
}

#endif
