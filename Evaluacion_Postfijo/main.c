#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#define MAX 100

// Pila de números flotantes para la evaluación
typedef struct {
    float items[MAX];
    int top;
} PilaFloat;

void initPila(PilaFloat *p) {
    p->top = -1;
}

bool isEmpty(PilaFloat *p) {
    return p->top == -1;
}

void push(PilaFloat *p, float value) {
    if (p->top < MAX - 1) {
        p->items[++(p->top)] = value;
    }
}

float pop(PilaFloat *p) {
    if (!isEmpty(p)) {
        return p->items[(p->top)--];
    }
    return 0.0f;
}


float evaluar(char expresion[]) {
    PilaFloat pila;
    initPila(&pila);

    char symb;
    int i = 0;
    float opnd1, opnd2, value;

    // Recorrer toda la cadena
    while (expresion[i] != '\0') {
        symb = expresion[i];

        // Si symb es un operando (dígito numérico)
        if (isdigit(symb)) {
            push(&pila, (float)(symb - '0')); // Convertir '0'-'9' a su valor numérico float
        } 
        // Si es un operador
        else if (symb == '+' || symb == '-' || symb == '*' || symb == '/' || symb == '^') {
            opnd2 = pop(&pila);
            opnd1 = pop(&pila);

            switch (symb) {
                case '+': value = opnd1 + opnd2; break;
                case '-': value = opnd1 - opnd2; break;
                case '*': value = opnd1 * opnd2; break;
                case '/': 
                    if (opnd2 != 0) {
                        value = opnd1 / opnd2; 
                    } else {
                        printf("\nError: Division entre cero.\n");
                        return 0.0f;
                    }
                    break;
                case '^': value = pow(opnd1, opnd2); break;
            }
            push(&pila, value);
        }
        i++;
    }

    return pop(&pila);
}

int main() {
    char expresion[MAX];

    printf("=== Evaluacion de Expresiones Postfijas ===\n");
    printf("Ingrese la expresion en postfijo (ejemplo 23+4*): ");
    scanf("%s", expresion);

    float resultado = evaluar(expresion);

    printf("\nResultado de la evaluacion: %.2f\n", resultado);

    return 0;
}