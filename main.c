#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

// Definición de las operaciones SML (Simpletron Machine Language)
#define READ        10
#define WRITE       11
#define LOAD        20
#define STORE       21
#define ADD         30
#define SUBTRACT    31
#define DIVIDE      32
#define MULTIPLY    33
#define BRANCH      40
#define BRANCHNEG   41
#define BRANCHZERO  42
#define HALT        43

// Prototipo de la función para el vaciado de memoria 
void dump(int accumulator, int instructionCounter, int instructionRegister, 
          int operationCode, int operand, const int memory[]);

int main(void) {

    SetConsoleOutputCP(CP_UTF8);

    int memory[100] = {0};
    int accumulator = 0;
    int instructionCounter = 0;
    int instructionRegister = 0;
    int operationCode = 0;
    int operand = 0;

    int inputVal = 0;
    bool fatalError = false;

    // 1. Mensaje de bienvenida
    printf("*** ¡Bienvenido a Simpletron! ***\n");
    printf("*** Introduzca su programa una instrucción ***\n");
    printf("*** (o palabra de datos) a la vez en la línea ***\n");
    printf("*** de texto de entrada. Yo indicaré el número ***\n");
    printf("*** de posición y una interrogación(?). Usted ***\n");
    printf("*** tecleará entonces una palabra para esa ***\n");
    printf("*** posición. Capture el número 9999 para ***\n");
    printf("*** terminar de introducir su programa. ***\n\n");

    // 2. Carga del programa en la memoria
    while (instructionCounter < 100) {
        printf("%02d ? ", instructionCounter);
        scanf("%d", &inputVal);

        if (inputVal == 9999) {
            break;
        }

        // Validación de rango de entrada (-9999 a +9998)
        if (inputVal < -9999 || inputVal > 9998) {
            printf("*** Número inválido. Intente nuevamente. ***\n");
            continue;
        }

        memory[instructionCounter] = inputVal;
        instructionCounter++;
    }

    printf("*** Se terminó de cargar el programa. ***\n");
    printf("*** Comienza la ejecución del programa. ***\n\n");

    // Reiniciar el contador de instrucciones para iniciar la ejecución desde la posición 00
    instructionCounter = 0;

    
    while (instructionCounter < 100 && !fatalError) {
        
        instructionRegister = memory[instructionCounter];

        
        operationCode = instructionRegister / 100;
        operand = instructionRegister % 100;

        bool branched = false;

        
        switch (operationCode) {
            case READ:
                printf("Teclee un entero: ");
                scanf("%d", &memory[operand]);
                break;

            case WRITE:
                printf("Salida: %d\n", memory[operand]);
                break;

            case LOAD:
                accumulator = memory[operand];
                break;

            case STORE:
                memory[operand] = accumulator;
                break;

            case ADD:
                accumulator += memory[operand];
                if (accumulator > 9999 || accumulator < -9999) {
                    printf("\n*** Error: Desbordamiento del acumulador ***\n");
                    fatalError = true;
                }
                break;

            case SUBTRACT:
                accumulator -= memory[operand];
                if (accumulator > 9999 || accumulator < -9999) {
                    printf("\n*** Error: Desbordamiento del acumulador ***\n");
                    fatalError = true;
                }
                break;

            case DIVIDE:
                if (memory[operand] == 0) {
                    printf("\n*** Intento de dividir entre cero ***\n");
                    fatalError = true;
                } else {
                    accumulator /= memory[operand];
                }
                break;

            case MULTIPLY:
                accumulator *= memory[operand];
                if (accumulator > 9999 || accumulator < -9999) {
                    printf("\n*** Error: Desbordamiento del acumulador ***\n");
                    fatalError = true;
                }
                break;

            case BRANCH:
                instructionCounter = operand;
                branched = true;
                break;

            case BRANCHNEG:
                if (accumulator < 0) {
                    instructionCounter = operand;
                    branched = true;
                }
                break;

            case BRANCHZERO:
                if (accumulator == 0) {
                    instructionCounter = operand;
                    branched = true;
                }
                break;

            case HALT:
                printf("*** La ejecución de Simpletron terminó normalmente ***\n\n");
                dump(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory);
                return 0;

            default:
                printf("\n*** Código de operación no válido ***\n");
                fatalError = true;
                break;
        }

        
        if (fatalError) {
            printf("*** La ejecución de Simpletron terminó anormalmente ***\n\n");
            dump(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory);
            return 1;
        }
        
        if (!branched) {
            instructionCounter++;
        }
    }

    return 0;
}

// Función para imprimir registros y el estado de la memoria
void dump(int accumulator, int instructionCounter, int instructionRegister, 
          int operationCode, int operand, const int memory[]) {
    printf("Registros:\n");
    printf("acumulador:          %+05d\n", accumulator);
    printf("instructionCounter:     %02d\n", instructionCounter);
    printf("instructionRegister: %+05d\n", instructionRegister);
    printf("operationcode:          %02d\n", operationCode);
    printf("operand:                %02d\n\n", operand);

    printf("MEMORIA:\n");
    printf("%8d%6d%6d%6d%6d%6d%6d%6d%6d%6d\n", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    for (int row = 0; row < 10; row++) {
        printf("%2d ", row * 10);
        for (int col = 0; col < 10; col++) {
            printf("%+05d ", memory[row * 10 + col]);
        }
        printf("\n");
    }
}