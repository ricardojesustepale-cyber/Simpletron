#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define MAX 100

// Estructura de la Pila
typedef struct {
    char items[MAX];
    int top;
} Pila;

void initPila(Pila *p) {
    p->top = -1;
}

bool isEmpty(Pila *p) {
    return p->top == -1;
}

void push(Pila *p, char value) {
    if (p->top < MAX - 1) {
        p->items[++(p->top)] = value;
    }
}

char pop(Pila *p) {
    if (!isEmpty(p)) {
        return p->items[(p->top)--];
    }
    return '\0';
}

char peek(Pila *p) {
    if (!isEmpty(p)) {
        return p->items[p->top];
    }
    return '\0';
}

// Función para determinar la precedencia de los operadores
int precedencia(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}


void infijoAPostfijo(char *infijo, char *postfijo) {
    Pila p;
    initPila(&p);
    
    int i = 0, j = 0;
    char symb, temp;

    while (infijo[i] != '\0') {
        symb = infijo[i];

        // 1. Si es operando (letra o número)
        if (isalnum(symb)) {
            postfijo[j++] = symb;
        }
        // 2. Si es paréntesis de apertura
        else if (symb == '(') {
            push(&p, symb);
        }
        // 3. Si es paréntesis de cierre
        else if (symb == ')') {
            while (!isEmpty(&p) && peek(&p) != '(') {
                postfijo[j++] = pop(&p);
            }
            pop(&p); // Descartar el '('
        }
        // 4. Si es un operador
        else if (symb == '+' || symb == '-' || symb == '*' || symb == '/' || symb == '^') {
            while (!isEmpty(&p) && precedencia(peek(&p)) >= precedencia(symb)) {
                postfijo[j++] = pop(&p);
            }
            push(&p, symb);
        }
        i++;
    }

    // Vaciar los operadores restantes de la pila
    while (!isEmpty(&p)) {
        postfijo[j++] = pop(&p);
    }

    postfijo[j] = '\0'; // Carácter nulo de fin de cadena
}

int main() {
    char infijo[MAX];
    char postfijo[MAX];

    printf("=== Conversión de Infijo a Postfijo ===\n");
    printf("Ingrese expresión en infijo (ejemplo: (A+B)*C): ");
    scanf("%s", infijo);

    infijoAPostfijo(infijo, postfijo);

    printf("\nExpresión Postfija: %s\n", postfijo);

    return 0;
}