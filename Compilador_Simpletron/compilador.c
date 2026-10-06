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
    int symbol;   
    char type;      
    int location;   
    struct TableEntry *next;
} TableEntry;

TableEntry *symbolTable = NULL;

// Arreglos globales del compilador
int SML[MAX_MEMORIA];       // Memoria de instrucciones SML
int flags[MAX_MEMORIA];     // Arreglo de banderas para referencias inconclusas (-1 si no hay bandera)

int instructionCounter = 0; // Posiciones para instrucciones SML (00 en adelante)
int memoryCounter = 99;      // Posiciones para variables/constantes (99 hacia abajo)

// Inicializar arreglos y variables
void initCompilador() {
    for (int i = 0; i < MAX_MEMORIA; i++) {
        SML[i] = 0;
        flags[i] = -1; 
    }
}


int agregarOBuscarSimbolo(int symbol, char type) {
    TableEntry *curr = symbolTable;
    
    // Buscar si ya existe en la lista ligada
    while (curr != NULL) {
        if (curr->symbol == symbol && curr->type == type) {
            return curr->location;
        }
        curr = curr->next;
    }

    // Si no existe, crear nueva entrada
    TableEntry *newEntry = (TableEntry *)malloc(sizeof(TableEntry));
    newEntry->symbol = symbol;
    newEntry->type = type;

    if (type == 'L') {
        newEntry->location = instructionCounter; // La línea apunta a la posición actual de instrucción
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
        // Ignorar líneas vacías o saltos de línea
        if (linea[0] == '\n' || linea[0] == '\r') continue;

        // Tokenización con strtok()
        char *tokenNumeroLinea = strtok(linea, " \t\r\n");
        if (tokenNumeroLinea == NULL) continue;

        int numLinea = atoi(tokenNumeroLinea);
        // Registrar el número de línea ('L') en la Tabla de Símbolos
        agregarOBuscarSimbolo(numLinea, 'L');

        char *comando = strtok(NULL, " \t\r\n");
        if (comando == NULL) continue;

        // 1. Comando input
        if (strcmp(comando, "input") == 0) {
            char *varToken = strtok(NULL, " \t\r\n");
            int locVar = agregarOBuscarSimbolo(varToken[0], 'V');
            SML[instructionCounter++] = 1000 + locVar; // READ (10)
            printf("Línea %02d [input %s] -> SML[%02d] = %+05d\n", numLinea, varToken, instructionCounter - 1, SML[instructionCounter - 1]);
        }
        // 2. Comando print
        else if (strcmp(comando, "print") == 0) {
            char *varToken = strtok(NULL, " \t\r\n");
            int locVar = agregarOBuscarSimbolo(varToken[0], 'V');
            SML[instructionCounter++] = 1100 + locVar; // WRITE (11)
            printf("Línea %02d [print %s] -> SML[%02d] = %+05d\n", numLinea, varToken, instructionCounter - 1, SML[instructionCounter - 1]);
        }
        // 3. Comando goto (Manejo de referencias inconclusas con banderas)
        else if (strcmp(comando, "goto") == 0) {
            char *destinoToken = strtok(NULL, " \t\r\n");
            int lineaDestino = atoi(destinoToken);

            // Verificar si la línea de destino ya está registrada en la Tabla de Símbolos
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
                // Referencia resuelta inmediatamente
                SML[instructionCounter++] = 4000 + locDestino; // BRANCH (40)
            } else {
                // REFERENCIA INCONCLUSA (Salto hacia adelante)
                SML[instructionCounter] = 4000; // Asume operando 00 temporal
                flags[instructionCounter] = lineaDestino; // Registra la línea esperada en el arreglo de banderas
                printf("Línea %02d [goto %d] -> SML[%02d] = %+05d (INCONCLUSA: Banderas[%02d] = %d)\n", 
                       numLinea, lineaDestino, instructionCounter, SML[instructionCounter], instructionCounter, lineaDestino);
                instructionCounter++;
            }
        }
        // 4. Comando end
        else if (strcmp(comando, "end") == 0) {
            SML[instructionCounter++] = 4300; // HALT (43)
            printf("Línea %02d [end] -> SML[%02d] = %+05d\n", numLinea, instructionCounter - 1, SML[instructionCounter - 1]);
        }
    }
}

// Función para mostrar la Tabla de Símbolos y las Banderas
void mostrarEstadoPrimeraPasada() {
    printf("\n=== TABLA DE SÍMBOLOS GENERADA ===\n");
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

    printf("\n=== ARREGLO DE BANDERAS (FLAGS) ===\n");
    bool hayBanderas = false;
    for (int i = 0; i < instructionCounter; i++) {
        if (flags[i] != -1) {
            printf("flags[%02d] = %d (Espera resolver línea %d)\n", i, flags[i], flags[i]);
            hayBanderas = true;
        }
    }
    if (!hayBanderas) {
        printf("No hay saltos pendientes por resolver.\n");
    }
}

int main() {
    // Configuración de la consola para soporte de acentos/UTF-8 en Windows
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

    primeraPasada(archivo);
    fclose(archivo);

    mostrarEstadoPrimeraPasada();

    return 0;
}