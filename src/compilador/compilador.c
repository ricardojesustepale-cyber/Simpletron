#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h>

#define MAX_MEMORIA 100

// Estructura de la Tabla de Símbolos (Lista Ligada)
typedef struct TableEntry {
    int symbol;     // Carácter ASCII (variables/constantes) o número de línea
    char type;      // 'C' = Constante, 'L' = Línea, 'V' = Variable
    int location;   // Posición en la memoria de Simpletron (00-99)
    struct TableEntry *next;
} TableEntry;

TableEntry *symbolTable = NULL;

// Arreglos globales del compilador
int SML[MAX_MEMORIA];       // Memoria de instrucciones/datos SML
int flags[MAX_MEMORIA];     // Arreglo de banderas para referencias inconclusas (-1 si no hay)

int instructionCounter = 0; // Posiciones para instrucciones SML 
int memoryCounter = 99;      // Posiciones para variables/constantes 

void initCompilador() {
    for (int i = 0; i < MAX_MEMORIA; i++) {
        SML[i] = 0;
        flags[i] = -1;
    }
}

int agregarOBuscarSimbolo(int symbol, char type) {
    TableEntry *curr = symbolTable;
    
    while (curr != NULL) {
        if (curr->symbol == symbol && curr->type == type) {
            return curr->location;
        }
        curr = curr->next;
    }

    TableEntry *newEntry = (TableEntry *)malloc(sizeof(TableEntry));
    newEntry->symbol = symbol;
    newEntry->type = type;

    if (type == 'L') {
        newEntry->location = instructionCounter;
    } else if (type == 'V' || type == 'C') {
        newEntry->location = memoryCounter--;
    }

    newEntry->next = symbolTable;
    symbolTable = newEntry;

    return newEntry->location;
}

// PRIMERA PASADA: Análisis léxico, tokenización y generación temprana
void primeraPasada(FILE *archivo) {
    char linea[100];

    printf("=== INICIANDO PRIMERA PASADA ===\n\n");

    while (fgets(linea, sizeof(linea), archivo)) {
        if (linea[0] == '\n' || linea[0] == '\r') continue;

        char *tokenNumeroLinea = strtok(linea, " \t\r\n");
        if (tokenNumeroLinea == NULL) continue;

        int numLinea = atoi(tokenNumeroLinea);
        agregarOBuscarSimbolo(numLinea, 'L');

        char *comando = strtok(NULL, " \t\r\n");
        if (comando == NULL) continue;

        if (strcmp(comando, "input") == 0) {
            char *varToken = strtok(NULL, " \t\r\n");
            int locVar = agregarOBuscarSimbolo(varToken[0], 'V');
            SML[instructionCounter++] = 1000 + locVar; // READ (10)
            printf("Línea %02d [input %s] -> SML[%02d] = %+05d\n", numLinea, varToken, instructionCounter - 1, SML[instructionCounter - 1]);
        }
        else if (strcmp(comando, "print") == 0) {
            char *varToken = strtok(NULL, " \t\r\n");
            int locVar = agregarOBuscarSimbolo(varToken[0], 'V');
            SML[instructionCounter++] = 1100 + locVar; // WRITE (11)
            printf("Línea %02d [print %s] -> SML[%02d] = %+05d\n", numLinea, varToken, instructionCounter - 1, SML[instructionCounter - 1]);
        }
        else if (strcmp(comando, "goto") == 0) {
            char *destinoToken = strtok(NULL, " \t\r\n");
            int lineaDestino = atoi(destinoToken);

            TableEntry *curr = symbolTable;
            int locDestino = -1;
            while (curr != NULL) {
                if (curr->symbol == lineaDestino && curr->type == 'L') {
                    locDestino = curr->location;
                    break;
                }
                curr = curr->next;
            }

            if (locDestino != -1) {
                SML[instructionCounter++] = 4000 + locDestino; // BRANCH (40)
            } else {
                SML[instructionCounter] = 4000; // Asume operando 00
                flags[instructionCounter] = lineaDestino;
                printf("Línea %02d [goto %d] -> SML[%02d] = %+05d (INCONCLUSA: Banderas[%02d] = %d)\n", 
                       numLinea, lineaDestino, instructionCounter, SML[instructionCounter], instructionCounter, lineaDestino);
                instructionCounter++;
            }
        }
        else if (strcmp(comando, "end") == 0) {
            SML[instructionCounter++] = 4300; // HALT (43)
            printf("Línea %02d [end] -> SML[%02d] = %+05d\n", numLinea, instructionCounter - 1, SML[instructionCounter - 1]);
        }
    }
}

// SEGUNDA PASADA: Resolver referencias inconclusas con la Tabla de Símbolos y Banderas
void segundaPasada() {
    printf("\n=== INICIANDO SEGUNDA PASADA (RESOLUCIÓN DE BANDERAS) ===\n\n");

    for (int i = 0; i < instructionCounter; i++) {
        if (flags[i] != -1) { // 1. Escanear
            int lineaBuscada = flags[i];
            int locReal = -1;

            // 2. Cruzar con la Tabla de Símbolos
            TableEntry *curr = symbolTable;
            while (curr != NULL) {
                if (curr->symbol == lineaBuscada && curr->type == 'L') {
                    locReal = curr->location;
                    break;
                }
                curr = curr->next;
            }

            if (locReal != -1) {
                int instruccionAnterior = SML[i];
                SML[i] += locReal; // 3. Parchar la dirección SML
                printf("Bandera resuelta en SML[%02d]: Instrucción inicial %+05d -> Parchada a %+05d (Línea %d = Posición %02d)\n",
                       i, instruccionAnterior, SML[i], lineaBuscada, locReal);
            } else {
                printf("Error de compilación: La línea destino %d no existe en el programa.\n", lineaBuscada);
            }
        }
    }
}

// EXPORTACIÓN A DISCO: Generar el archivo final .sml
void exportarSML(const char *nombreArchivoSalida) {
    FILE *archivoSML = fopen(nombreArchivoSalida, "w");
    if (!archivoSML) {
        printf("Error: No se pudo crear el archivo objeto %s\n", nombreArchivoSalida);
        return;
    }

    // Exporta todas las celdas de memoria utilizadas
    for (int i = 0; i < MAX_MEMORIA; i++) {
        fprintf(archivoSML, "%+05d\n", SML[i]);
    }

    fclose(archivoSML);
    printf("\n[ÉXITO] Programa objeto exportado correctamente a: %s\n", nombreArchivoSalida);
}

void mostrarEstadoFinal() {
    printf("\n=== TABLA DE SÍMBOLOS FINAL ===\n");
    printf("Símbolo\tTipo\tUbicación SML\n");
    TableEntry *curr = symbolTable;
    while (curr != NULL) {
        if (curr->type == 'L') {
            printf("%d\t%c\t%02d\n", curr->symbol, curr->type, curr->location);
        } else {
            printf("%c\t%c\t%02d\n", (char)curr->symbol, curr->type, curr->location);
        }
        curr = curr->next;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "es_ES.UTF-8");

    initCompilador();

    char nombreArchivo[50];
    printf("Ingrese el nombre del archivo de programa Simple (ej. programa.simple): ");
    scanf("%s", nombreArchivo);

    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        printf("Error: No se pudo abrir el archivo %s\n", nombreArchivo);
        return 1;
    }

    // 1. Ejecutar Primera Pasada
    primeraPasada(archivo);
    fclose(archivo);

    // 2. Ejecutar Segunda Pasada
    segundaPasada();

    // 3. Mostrar resumen final
    mostrarEstadoFinal();

    // 4. Exportar archivo .sml
    exportarSML("programa.sml");

    return 0;
}