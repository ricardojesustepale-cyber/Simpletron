#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TAM_MEMORIA 1000
#define MIN_VAL -99999
#define MAX_VAL 99999

// Opcodes SML
#define READ 10
#define WRITE 11
#define NEWLINE 12
#define READ_STRING 13
#define WRITE_STRING 14

#define LOAD 20
#define STORE 21

#define ADD 30
#define SUBTRACT 31
#define DIVIDE 32
#define MULTIPLY 33
#define MODULO 34
#define EXPONENT 35

#define BRANCH 40
#define BRANCHNEG 41
#define BRANCHZERO 42
#define HALT 43

// Opcodes Punto Flotante
#define READ_FLOAT 50
#define WRITE_FLOAT 51
#define ADD_FLOAT 52
#define SUB_FLOAT 53

// Prototipos
void inicializar(int memoria[], int *acumulador, int *contadorInstrucciones, int *registroInstrucciones, int *codigoOperacion, int *operando);
int cargarPrograma(int memoria[]);
void vaciadoMemoria(int memoria[], int acumulador, int contadorInstrucciones, int registroInstrucciones, int codigoOperacion, int operando);

int main() {
    int memoria[TAM_MEMORIA];
    int acumulador = 0;
    int contadorInstrucciones = 0;
    int registroInstrucciones = 0;
    int codigoOperacion = 0;
    int operando = 0;

    inicializar(memoria, &acumulador, &contadorInstrucciones, &registroInstrucciones, &codigoOperacion, &operando);

    printf("*** Bienvenido a Simpletron 1000 ***\n");
    if (!cargarPrograma(memoria)) {
        printf("*** Error o archivo inexistente. Inicie carga manual. ***\n");
        return 1;
    }

    printf("*** Carga de programa completada ***\n");
    printf("*** Iniciando ejecucion del programa ***\n\n");

    int ejecutando = 1;

    while (ejecutando && contadorInstrucciones < TAM_MEMORIA) {
        registroInstrucciones = memoria[contadorInstrucciones];
        codigoOperacion = registroInstrucciones / 1000;
        operando = registroInstrucciones % 1000;

        switch (codigoOperacion) {
            case READ: {
                int valor;
                printf("Ingrese un entero: ");
                scanf("%d", &valor);
                memoria[operando] = valor;
                contadorInstrucciones++;
                break;
            }
            case WRITE:
                printf("Salida: %d\n", memoria[operando]);
                contadorInstrucciones++;
                break;

            case NEWLINE:
                printf("\n");
                contadorInstrucciones++;
                break;

            case READ_STRING: {
                char str[100];
                printf("Ingrese una cadena: ");
                scanf(" %[^\n]", str);
                int len = strlen(str);
                
                // Guardar longitud
                memoria[operando] = len;
                
                // Guardar caracteres como ASCII de 3 dígitos
                for (int i = 0; i < len; i++) {
                    if (operando + 1 + i < TAM_MEMORIA) {
                        memoria[operando + 1 + i] = (int)str[i];
                    }
                }
                contadorInstrucciones++;
                break;
            }

            case WRITE_STRING: {
                int dirBase = operando;
                if (dirBase < 0 || dirBase >= TAM_MEMORIA) {
                    printf("\n*** ERROR: Dirección de cadena inválida ***\n");
                    ejecutando = 0;
                    break;
                }
                int len = memoria[dirBase];
                printf("Cadena: ");
                for (int i = 0; i < len; i++) {
                    if (dirBase + 1 + i < TAM_MEMORIA) {
                        printf("%c", (char)memoria[dirBase + 1 + i]);
                    }
                }
                printf("\n");
                contadorInstrucciones++;
                break;
            }

            case LOAD:
                acumulador = memoria[operando];
                contadorInstrucciones++;
                break;

            case STORE:
                memoria[operando] = acumulador;
                contadorInstrucciones++;
                break;

            case ADD:
                acumulador += memoria[operando];
                contadorInstrucciones++;
                break;

            case SUBTRACT:
                acumulador -= memoria[operando];
                contadorInstrucciones++;
                break;

            case DIVIDE:
                if (memoria[operando] == 0) {
                    printf("\n*** ERROR: Division entre cero ***\n");
                    ejecutando = 0;
                } else {
                    acumulador /= memoria[operando];
                    contadorInstrucciones++;
                }
                break;

            case MULTIPLY:
                acumulador *= memoria[operando];
                contadorInstrucciones++;
                break;

            case MODULO:
                if (memoria[operando] == 0) {
                    printf("\n*** ERROR: Modulo entre cero ***\n");
                    ejecutando = 0;
                } else {
                    acumulador %= memoria[operando];
                    contadorInstrucciones++;
                }
                break;

            case EXPONENT: {
                int exp = memoria[operando];
                if (exp < 0) {
                    printf("\n*** ERROR: Exponente negativo no soportado ***\n");
                    ejecutando = 0;
                } else {
                    acumulador = (int)pow(acumulador, exp);
                    contadorInstrucciones++;
                }
                break;
            }

            case BRANCH:
                contadorInstrucciones = operando;
                break;

            case BRANCHNEG:
                if (acumulador < 0) contadorInstrucciones = operando;
                else contadorInstrucciones++;
                break;

            case BRANCHZERO:
                if (acumulador == 0) contadorInstrucciones = operando;
                else contadorInstrucciones++;
                break;

            case HALT:
                printf("*** Ejecucion finalizada de Simpletron ***\n");
                ejecutando = 0;
                break;

            // Soporte Punto Flotante
            case READ_FLOAT: {
                float fval;
                printf("Ingrese un flotante: ");
                scanf("%f", &fval);
                int *ptr = (int*)&fval;
                memoria[operando] = *ptr;
                contadorInstrucciones++;
                break;
            }

            case WRITE_FLOAT: {
                int raw = memoria[operando];
                float *fptr = (float*)&raw;
                printf("Salida Flotante: %.2f\n", *fptr);
                contadorInstrucciones++;
                break;
            }

            case ADD_FLOAT: {
                int rawA = acumulador;
                int rawM = memoria[operando];
                float *fa = (float*)&rawA;
                float *fm = (float*)&rawM;
                float res = *fa + *fm;
                int *resPtr = (int*)&res;
                acumulador = *resPtr;
                contadorInstrucciones++;
                break;
            }

            case SUB_FLOAT: {
                int rawA = acumulador;
                int rawM = memoria[operando];
                float *fa = (float*)&rawA;
                float *fm = (float*)&rawM;
                float res = *fa - *fm;
                int *resPtr = (int*)&res;
                acumulador = *resPtr;
                contadorInstrucciones++;
                break;
            }

            default:
                printf("\n*** ERROR: Codigo de operacion invalido (%d) ***\n", codigoOperacion);
                ejecutando = 0;
                break;
        }
    }

    vaciadoMemoria(memoria, acumulador, contadorInstrucciones, registroInstrucciones, codigoOperacion, operando);
    return 0;
}

void inicializar(int memoria[], int *acumulador, int *contadorInstrucciones, int *registroInstrucciones, int *codigoOperacion, int *operando) {
    for (int i = 0; i < TAM_MEMORIA; i++) memoria[i] = 0;
    *acumulador = 0;
    *contadorInstrucciones = 0;
    *registroInstrucciones = 0;
    *codigoOperacion = 0;
    *operando = 0;
}

int cargarPrograma(int memoria[]) {
    FILE *archivo = fopen("programa.simp", "r");
    if (archivo == NULL) {
        printf("El archivo 'programa.simp' no existe. Pasando a entrada interactiva...\n");
        int inst, i = 0;
        while (i < TAM_MEMORIA) {
            printf("%03d ? ", i);
            scanf("%d", &inst);
            if (inst == -99999) break;
            memoria[i++] = inst;
        }
        return 1;
    }

    int i = 0, inst;
    while (fscanf(archivo, "%d", &inst) != EOF && i < TAM_MEMORIA) {
        memoria[i++] = inst;
    }
    fclose(archivo);
    return 1;
}

void vaciadoMemoria(int memoria[], int acumulador, int contadorInstrucciones, int registroInstrucciones, int codigoOperacion, int operando) {
    printf("\nREGISTROS:\n");
    printf("Acumulador:           %+06d\n", acumulador);
    printf("ContadorInstrucciones:   %03d\n", contadorInstrucciones);
    printf("RegistroInstrucciones: %+06d\n", registroInstrucciones);
    printf("CodigoOperacion:        %02d\n", codigoOperacion);
    printf("Operando:              %03d\n\n", operando);

    printf("MEMORIA (Primeras 100 posiciones):\n  ");
    for (int i = 0; i < 10; i++) printf("%7d", i);
    printf("\n");

    for (int i = 0; i < 100; i += 10) {
        printf("%03d ", i);
        for (int j = 0; j < 10; j++) {
            printf("%+06d ", memoria[i + j]);
        }
        printf("\n");
    }
}