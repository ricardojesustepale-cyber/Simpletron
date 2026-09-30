#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX 100

// Estructura de la Pila 
typedef struct {
    char items[MAX];
    int top;
} Pila;

// Funciones de la Pila
void initPila(Pila *p) {
    p->top = -1;
}

bool isEmpty(Pila *p) {
    return p->top == -1;
}

bool isFull(Pila *p) {
    return p->top == MAX - 1;
}

void push(Pila *p, char value) {
    if (!isFull(p)) {
        p->top++;
        p->items[p->top] = value;
    }
}

char pop(Pila *p) {
    if (!isEmpty(p)) {
        char val = p->items[p->top];
        p->top--;
        return val;
    }
    return '\0';
}


bool esPareja(char apertura, char cierre) {
    if (apertura == '(' && cierre == ')') return true;
    if (apertura == '[' && cierre == ']') return true;
    if (apertura == '{' && cierre == '}') return true;
    return false;
}

// Algoritmo de la diapositiva: Profundidad de Anidamiento
bool profundidad(char *expresion) {
    Pila p;
    initPila(&p);

    char symb;
    char temp;
    int i = 0;

   
    while (expresion[i] != '\0') {
        symb = expresion[i]; 

        // Si symb es '(', '[' o '{'
        if (symb == '(' || symb == '[' || symb == '{') {
            push(&p, symb);
        }
        // Si symb es ')', ']' o '}'
        else if (symb == ')' || symb == ']' || symb == '}') {
            if (isEmpty(&p)) {
                return false; // Error: cierre sin apertura previa
            }
            temp = pop(&p);
            if (!esPareja(temp, symb)) {
                return false; // Error: los símbolos no coinciden
            }
        }
        i++; // Avanzar al siguiente carácter
    }

    // Si la pila NO está vacía al terminar, faltaron cierres
    if (!isEmpty(&p)) {
        return false;
    }

    return true; // Expresión balanceada correctamente
}

int main() {
    char expresion[200];

    printf("=== Verificador de Profundidad de Anidamiento ===\n");
    printf("Ingrese la expresion a evaluar: ");
    scanf(" %[^\n]", expresion);

    if (profundidad(expresion)) {
        printf("\nResultado: VERDADERO (1) -> La expresion esta correctamente anidada.\n");
    } else {
        printf("\nResultado: FALSO (0) -> La expresion NO esta correctamente anidada.\n");
    }

    return 0;
}