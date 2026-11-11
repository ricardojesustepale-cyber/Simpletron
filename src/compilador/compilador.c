/*
 * compilador.c - Compilador de dos pasadas del lenguaje Simple a SML (Simpletron 1000)
 *
 * Primera pasada: lee cada linea, registra lineas/variables/constantes en la tabla
 *   de simbolos, genera SML y marca en flags[] los saltos a lineas que aun no se
 *   conocen (referencias hacia adelante).
 * Segunda pasada: recorre flags[], busca cada linea pendiente en la tabla y
 *   completa el operando de la instruccion de salto.
 *
 * Uso:
 *   compilador programa.simple               genera programa.sml
 *   compilador programa.simple -o otro.sml   elige el archivo de salida
 *   compilador                               pregunta el nombre del archivo
 * Opciones:
 *   --tabla     imprime la tabla de simbolos final
 *   --detalle   muestra la tabla, el SML preliminar y flags[] de cada pasada
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "../simpletron/opcodes.h"
#include "../expresiones/expresiones.h"
#include "../expresiones/pila.h"

/* ---------------- Tabla de simbolos (lista ligada) ---------------- */
typedef struct TableEntry {
    int symbol;     /* numero de linea, codigo ASCII de la variable o valor de la constante */
    char type;      /* 'L' = linea, 'V' = variable, 'C' = constante */
    int location;   /* direccion en la memoria de Simpletron (000-999) */
    struct TableEntry *next;
} TableEntry;

static TableEntry *symbolTable = NULL;
static TableEntry *ultimoSimbolo = NULL;

/* ---------------- Memoria que se genera ---------------- */
static int SML[TAM_MEMORIA];
static int flags[TAM_MEMORIA];          /* -1 = sin referencia pendiente */
static int lineaOrigen[TAM_MEMORIA];    /* linea Simple que genero la instruccion */
static char tipoCelda[TAM_MEMORIA];     /* 'I' instruccion, 'V', 'C', 'T' (temporal) */
static char descripcion[TAM_MEMORIA][80];

static int instructionCounter = 0;       /* siguiente celda para instrucciones (sube) */
static int memoryCounter = TAM_MEMORIA - 1; /* siguiente celda para datos (baja) */
static bool memoriaAgotada = false;

static int temporales[TAM_MEMORIA];     /* celdas temporales reutilizables */
static int numTemporales = 0;
static int temporalesUsados = 0;        /* cuantas lleva la instruccion actual */
static int enAcumulador = -1;           /* celda cuyo valor ya esta en el acumulador */

/* ---------------- Estado de la linea actual ---------------- */
static const char *nombreFuente = "";
static char textoLinea[256];
static int numLinea = 0;                /* numero de linea Simple */
static int renglon = 0;                 /* renglon del archivo */
static int ultimaLinea = -1;
static bool hayEnd = false;
static bool primeraDeLinea = false;
static int errores = 0;

/* ======================================================================= */
/*                          Utilidades                                     */
/* ======================================================================= */

static void error(const char *formato, const char *arg) {
    char msg[300];
    snprintf(msg, sizeof msg, formato, arg ? arg : "");
    if (renglon > 0)
        fprintf(stderr, "%s:%d (linea %d): error: %s\n", nombreFuente, renglon, numLinea, msg);
    else
        fprintf(stderr, "%s (linea %d): error: %s\n", nombreFuente, numLinea, msg);
    if (textoLinea[0] != '\0') fprintf(stderr, "    | %s\n", textoLinea);
    errores++;
}

static char *recortar(char *s) {
    while (isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    return s;
}

static bool esVariable(const char *s) {
    return strlen(s) == 1 && islower((unsigned char)s[0]);
}

static bool esEnteroSinSigno(const char *s) {
    if (*s == '\0' || strlen(s) > 9) return false;
    for (; *s; s++) if (!isdigit((unsigned char)*s)) return false;
    return true;
}

/* ---------------- Tabla de simbolos ---------------- */
static int buscarSimbolo(int symbol, char type) {
    for (TableEntry *curr = symbolTable; curr != NULL; curr = curr->next)
        if (curr->symbol == symbol && curr->type == type) return curr->location;
    return -1;
}

static void agregarSimbolo(int symbol, char type, int location) {
    TableEntry *nuevo = (TableEntry *)malloc(sizeof(TableEntry));
    if (nuevo == NULL) { fprintf(stderr, "Sin memoria\n"); exit(1); }
    nuevo->symbol = symbol;
    nuevo->type = type;
    nuevo->location = location;
    nuevo->next = NULL;
    if (ultimoSimbolo == NULL) symbolTable = nuevo;   /* se agrega al final: */
    else ultimoSimbolo->next = nuevo;                 /* la tabla queda en orden */
    ultimoSimbolo = nuevo;
}

static void liberarTabla(void) {
    while (symbolTable != NULL) {
        TableEntry *sig = symbolTable->next;
        free(symbolTable);
        symbolTable = sig;
    }
    ultimoSimbolo = NULL;
}

static void imprimirTabla(FILE *out) {
    fprintf(out, "TABLA DE SIMBOLOS\n");
    fprintf(out, "  Simbolo   Tipo   Ubicacion   Descripcion\n");
    fprintf(out, "  -------   ----   ---------   -----------\n");
    for (TableEntry *e = symbolTable; e != NULL; e = e->next) {
        fprintf(out, "  %7d   %4c         %03d   ", e->symbol, e->type, e->location);
        if (e->type == 'L') fprintf(out, "linea %d\n", e->symbol);
        else if (e->type == 'V') fprintf(out, "variable '%c'\n", (char)e->symbol);
        else fprintf(out, "constante %d\n", e->symbol);
    }
}

/* ======================================================================= */
/*                    Generacion de codigo y memoria                       */
/* ======================================================================= */

static void emitir(int opcode, int operando) {
    if (memoriaAgotada) return;
    if (instructionCounter > memoryCounter) {
        memoriaAgotada = true;
        error("memoria insuficiente: el programa no cabe en Simpletron%s", NULL);
        return;
    }
    int celda = instructionCounter++;
    SML[celda] = opcode * FACTOR_OPCODE + operando;
    tipoCelda[celda] = 'I';
    lineaOrigen[celda] = numLinea;
    if (primeraDeLinea) {
        snprintf(descripcion[celda], sizeof descripcion[celda], "%.79s", textoLinea);
        primeraDeLinea = false;
    }
}

/* LOAD que se omite si el valor ya esta en el acumulador */
static void emitirCarga(int direccion) {
    if (direccion != enAcumulador) emitir(LOAD, direccion);
}

static void emitirSalto(int opcode, int lineaDestino) {
    int ubicacion = buscarSimbolo(lineaDestino, 'L');
    if (ubicacion >= 0) {
        emitir(opcode, ubicacion);                 /* salto hacia atras: ya se conoce */
    } else {
        if (!memoriaAgotada && instructionCounter <= memoryCounter)
            flags[instructionCounter] = lineaDestino;  /* referencia hacia adelante */
        emitir(opcode, 0);                         /* operando incompleto */
    }
}

static int reservarDato(char tipo, const char *texto, int valorInicial) {
    if (memoriaAgotada) return 0;
    if (memoryCounter < instructionCounter) {
        memoriaAgotada = true;
        error("memoria insuficiente: no hay celdas libres para datos%s", NULL);
        return 0;
    }
    int celda = memoryCounter--;
    SML[celda] = valorInicial;
    tipoCelda[celda] = tipo;
    snprintf(descripcion[celda], sizeof descripcion[celda], "%.79s", texto);
    return celda;
}

static int ubicacionVariable(char v) {
    int loc = buscarSimbolo(v, 'V');
    if (loc < 0) {
        char texto[32];
        snprintf(texto, sizeof texto, "variable %c", v);
        loc = reservarDato('V', texto, 0);
        if (!memoriaAgotada) agregarSimbolo(v, 'V', loc);
    }
    return loc;
}

static int ubicacionConstante(int valor) {
    int loc = buscarSimbolo(valor, 'C');
    if (loc < 0) {
        char texto[32];
        snprintf(texto, sizeof texto, "constante %d", valor);
        loc = reservarDato('C', texto, valor);
        if (!memoriaAgotada) agregarSimbolo(valor, 'C', loc);
    }
    return loc;
}

/* Las temporales se reutilizan: cada instruccion Simple empieza desde la primera */
static int temporal(void) {
    if (temporalesUsados < numTemporales) return temporales[temporalesUsados++];
    char texto[32];
    snprintf(texto, sizeof texto, "temporal t%d", numTemporales + 1);
    int celda = reservarDato('T', texto, 0);
    if (!memoriaAgotada) {
        temporales[numTemporales++] = celda;
        temporalesUsados++;
    }
    return celda;
}

static int opcodeDe(char operador) {
    switch (operador) {
        case '+': return ADD;
        case '-': return SUBTRACT;
        case '*': return MULTIPLY;
        case '/': return DIVIDE;
        case '%': return MODULO;
        case '^': return EXPONENT;
    }
    return -1;
}

/*
 * Compila una expresion: infijo -> postfijo -> SML con una pila de direcciones.
 *   operando -> se apila su direccion
 *   operador -> y = pop, x = pop;  LOAD x, OP y, STORE t;  push t
 * Si destino >= 0, la ultima operacion guarda directo en destino.
 * Devuelve la direccion del resultado o -1 si hay error.
 */
static int compilarExpresion(const char *texto, int destino) {
    Token infijo[MAX_TOKENS], postfijo[MAX_TOKENS];
    ErrorExpresion err = { "", -1 };
    int n = tokenizar(texto, infijo, MAX_TOKENS, &err);
    if (n >= 0) n = infijoAPostfijo(infijo, n, postfijo, &err);
    if (n < 0) {
        char msg[260];
        snprintf(msg, sizeof msg, "expresion invalida '%s': %s", texto, err.mensaje);
        error("%s", msg);
        return -1;
    }

    Pila direcciones;
    initPila(&direcciones);
    for (int i = 0; i < n; i++) {
        Token *tok = &postfijo[i];
        if (tok->tipo == NUMERO) {
            if (tok->valor < MIN_VAL || tok->valor > MAX_VAL) {
                char msg[120];
                snprintf(msg, sizeof msg, "la constante %ld esta fuera del rango de Simpletron (%d a %d)",
                         tok->valor, MIN_VAL, MAX_VAL);
                error("%s", msg);
                return -1;
            }
            push(&direcciones, ubicacionConstante((int)tok->valor));
        } else if (tok->tipo == VARIABLE) {
            push(&direcciones, ubicacionVariable(tok->simbolo));
        } else {
            long x = 0, y = 0;
            pop(&direcciones, &y);
            pop(&direcciones, &x);
            emitirCarga((int)x);
            emitir(opcodeDe(tok->simbolo), (int)y);
            int t = (i == n - 1 && destino >= 0) ? destino : temporal();
            emitir(STORE, t);
            enAcumulador = t;
            push(&direcciones, t);
        }
        if (memoriaAgotada) return -1;
    }
    long resultado = 0;
    pop(&direcciones, &resultado);
    return (int)resultado;
}

/* ======================================================================= */
/*                              Comandos                                   */
/* ======================================================================= */

/* input a   |   input a, b, c */
static void comandoInput(char *args) {
    if (*args == '\0') { error("input requiere al menos una variable%s", NULL); return; }
    for (char *item = strtok(args, ","); item != NULL; item = strtok(NULL, ",")) {
        item = recortar(item);
        if (!esVariable(item)) {
            error("'%s' no es una variable valida (una sola letra minuscula)", item);
            return;
        }
        emitir(READ, ubicacionVariable(item[0]));
    }
}

/* print a   |   print a, b   |   print a * 2 + 1 */
static void comandoPrint(char *args) {
    if (*args == '\0') { error("print requiere una variable o expresion%s", NULL); return; }
    char copia[256];
    snprintf(copia, sizeof copia, "%s", args);
    char *resto = copia;
    while (resto != NULL) {
        char *coma = strchr(resto, ',');
        if (coma != NULL) *coma = '\0';
        char *item = recortar(resto);
        if (*item == '\0') { error("elemento vacio en la lista de print%s", NULL); return; }
        int dir = compilarExpresion(item, -1);
        if (dir < 0) return;
        emitir(WRITE, dir);
        resto = coma ? coma + 1 : NULL;
    }
}

/* let x = expresion */
static void comandoLet(char *args) {
    char *igual = strchr(args, '=');
    if (igual == NULL) { error("let requiere el formato  let x = expresion%s", NULL); return; }
    *igual = '\0';
    char *destino = recortar(args);
    char *expresion = recortar(igual + 1);
    if (!esVariable(destino)) {
        error("'%s' no es una variable valida (una sola letra minuscula)", destino);
        return;
    }
    if (*expresion == '\0') { error("falta la expresion despues de '='%s", NULL); return; }
    if (*expresion == '=') { error("se esperaba '=' y se encontro '=='%s", NULL); return; }

    int variable = ubicacionVariable(destino[0]);
    int resultado = compilarExpresion(expresion, variable);
    if (resultado < 0 || resultado == variable) return;
    emitirCarga(resultado);                 /* let x = y   o   let x = 5 */
    emitir(STORE, variable);
    enAcumulador = variable;
}

/* goto 50 */
static void comandoGoto(char *args) {
    if (!esEnteroSinSigno(args)) { error("goto requiere un numero de linea%s", NULL); return; }
    emitirSalto(BRANCH, atoi(args));
}

/* if a op b goto 50      op: == != < <= > >= */
static void comandoIf(char *args) {
    char *pos = NULL;
    for (char *p = strstr(args, "goto"); p != NULL; p = strstr(p + 1, "goto")) pos = p;
    if (pos == NULL) { error("if requiere el formato  if condicion goto linea%s", NULL); return; }
    *pos = '\0';
    char *destinoTxt = recortar(pos + 4);
    if (!esEnteroSinSigno(destinoTxt)) { error("falta un numero de linea valido despues de goto%s", NULL); return; }
    int destino = atoi(destinoTxt);
    char *condicion = recortar(args);

    /* buscar el operador relacional */
    char op[3] = "";
    int k;
    for (k = 0; condicion[k] != '\0'; k++) {
        if (strncmp(condicion + k, "<=", 2) == 0 || strncmp(condicion + k, ">=", 2) == 0 ||
            strncmp(condicion + k, "==", 2) == 0 || strncmp(condicion + k, "!=", 2) == 0) {
            strncpy(op, condicion + k, 2);
            op[2] = '\0';
            break;
        }
        if (condicion[k] == '<' || condicion[k] == '>') { op[0] = condicion[k]; op[1] = '\0'; break; }
        if (condicion[k] == '=') { error("para comparar use '==' (no '=')%s", NULL); return; }
        if (condicion[k] == '!') { error("operador relacional invalido; use '!='%s", NULL); return; }
    }
    if (op[0] == '\0') { error("falta el operador relacional (==, !=, <, <=, >, >=)%s", NULL); return; }

    char izqTxt[128], derTxt[128];
    snprintf(izqTxt, sizeof izqTxt, "%.*s", k, condicion);
    snprintf(derTxt, sizeof derTxt, "%s", condicion + k + strlen(op));
    char *izq = recortar(izqTxt), *der = recortar(derTxt);
    if (*izq == '\0' || *der == '\0') { error("a la condicion le falta un operando%s", NULL); return; }
    if (strpbrk(der, "<>=!") != NULL) { error("la condicion solo puede tener un operador relacional%s", NULL); return; }

    int a = compilarExpresion(izq, -1);
    if (a < 0) return;
    int b = compilarExpresion(der, -1);
    if (b < 0) return;

    if (strcmp(op, "==") == 0) {            /* a - b == 0 */
        emitirCarga(a); emitir(SUBTRACT, b);
        emitirSalto(BRANCHZERO, destino);
    } else if (strcmp(op, "!=") == 0) {     /* si a - b == 0 se brinca el salto */
        emitirCarga(a); emitir(SUBTRACT, b);
        emitir(BRANCHZERO, instructionCounter + 2);
        emitirSalto(BRANCH, destino);
    } else if (strcmp(op, "<") == 0) {      /* a - b < 0 */
        emitirCarga(a); emitir(SUBTRACT, b);
        emitirSalto(BRANCHNEG, destino);
    } else if (strcmp(op, ">") == 0) {      /* b - a < 0 */
        emitirCarga(b); emitir(SUBTRACT, a);
        emitirSalto(BRANCHNEG, destino);
    } else if (strcmp(op, "<=") == 0) {     /* a - b < 0  o  a - b == 0 */
        emitirCarga(a); emitir(SUBTRACT, b);
        emitirSalto(BRANCHNEG, destino);
        emitirSalto(BRANCHZERO, destino);
    } else {                                /* >= :  b - a < 0  o  b - a == 0 */
        emitirCarga(b); emitir(SUBTRACT, a);
        emitirSalto(BRANCHNEG, destino);
        emitirSalto(BRANCHZERO, destino);
    }
    enAcumulador = -1;
}

static void comandoEnd(char *args) {
    if (*args != '\0') { error("end no lleva argumentos%s", NULL); return; }
    emitir(HALT, 0);
    hayEnd = true;
}

/* ======================================================================= */
/*                            Primera pasada                               */
/* ======================================================================= */

static void compilarLinea(char *cruda) {
    cruda[strcspn(cruda, "\r\n")] = '\0';
    char *linea = recortar(cruda);
    snprintf(textoLinea, sizeof textoLinea, "%s", linea);
    numLinea = 0;
    if (*linea == '\0') return;                          /* renglon en blanco */

    /* 1. Numero de linea */
    int i = 0;
    while (isdigit((unsigned char)linea[i])) i++;
    if (i == 0) { error("cada instruccion debe iniciar con un numero de linea%s", NULL); return; }
    if (i > 5) { error("numero de linea demasiado grande (maximo 99999)%s", NULL); return; }
    numLinea = atoi(linea);
    if (numLinea <= ultimaLinea) {
        char msg[100];
        snprintf(msg, sizeof msg, "los numeros de linea deben ir en orden ascendente (la anterior fue %d)", ultimaLinea);
        error("%s", msg);
        return;
    }
    if (linea[i] != '\0' && !isspace((unsigned char)linea[i])) {
        error("falta un espacio despues del numero de linea%s", NULL);
        return;
    }
    ultimaLinea = numLinea;

    /* 2. Comando y argumentos */
    char *resto = recortar(linea + i);
    char comando[16];
    int j = 0;
    while (isalpha((unsigned char)resto[j]) && j < 15) { comando[j] = resto[j]; j++; }
    comando[j] = '\0';
    char *args = recortar(resto + j);

    /* 3. Registrar la linea: apunta a la primera instruccion que genere */
    agregarSimbolo(numLinea, 'L', instructionCounter);
    primeraDeLinea = true;
    enAcumulador = -1;            /* la linea puede ser destino de un salto */
    temporalesUsados = 0;

    if (strcmp(comando, "rem") == 0) return;             /* comentario: sin codigo */

    /* 4. Restriccion de minusculas (excepto en rem) */
    for (char *p = resto; *p; p++)
        if (isupper((unsigned char)*p)) {
            error("solo se permiten letras minusculas (fuera de 'rem')%s", NULL);
            return;
        }

    if (comando[0] == '\0')               error("falta el comando despues del numero de linea%s", NULL);
    else if (strcmp(comando, "input") == 0) comandoInput(args);
    else if (strcmp(comando, "print") == 0) comandoPrint(args);
    else if (strcmp(comando, "let") == 0)   comandoLet(args);
    else if (strcmp(comando, "goto") == 0)  comandoGoto(args);
    else if (strcmp(comando, "if") == 0)    comandoIf(args);
    else if (strcmp(comando, "end") == 0)   comandoEnd(args);
    else error("comando desconocido '%s'", comando);
}

static void imprimirSML(FILE *out, bool marcarPendientes) {
    for (int i = 0; i < instructionCounter; i++) {
        int op = SML[i] / FACTOR_OPCODE;
        const char *nombre = nombreOpcode(op);
        bool pendiente = marcarPendientes && flags[i] != -1;
        if (pendiente)
            fprintf(out, "+%02d???   ; %03d  %-12s ???   -> linea %d (pendiente)\n",
                    op, i, nombre ? nombre : "?", flags[i]);
        else {
            fprintf(out, "%+06d   ; %03d  %-12s %03d", SML[i], i, nombre ? nombre : "?",
                    SML[i] % FACTOR_OPCODE);
            if (descripcion[i][0] != '\0') fprintf(out, "   | %s", descripcion[i]);
            fprintf(out, "\n");
        }
    }
}

static void primeraPasada(FILE *archivo, bool detalle) {
    char linea[256];
    while (fgets(linea, sizeof linea, archivo) != NULL) {
        renglon++;
        compilarLinea(linea);
    }
    if (!hayEnd && instructionCounter > 0) {
        printf("%s: advertencia: el programa no tiene 'end'; se agrego HALT al final\n", nombreFuente);
        numLinea = ultimaLinea;
        primeraDeLinea = true;
        snprintf(textoLinea, sizeof textoLinea, "(HALT agregado por el compilador)");
        emitir(HALT, 0);
    }
    if (ultimaLinea < 0 && errores == 0) {
        textoLinea[0] = '\0';
        error("el programa fuente esta vacio%s", NULL);
    }

    if (detalle) {
        printf("=== PRIMERA PASADA ===\n\n");
        imprimirTabla(stdout);
        printf("\nSML preliminar (??? = referencia hacia adelante sin resolver):\n");
        imprimirSML(stdout, true);
        printf("\nArreglo flags (solo entradas distintas de -1):\n");
        bool alguna = false;
        for (int i = 0; i < TAM_MEMORIA; i++)
            if (flags[i] != -1) { printf("  flags[%03d] = %d\n", i, flags[i]); alguna = true; }
        if (!alguna) printf("  (ninguna referencia pendiente)\n");
    }
}

/* ======================================================================= */
/*                            Segunda pasada                               */
/* ======================================================================= */

static void segundaPasada(bool detalle) {
    if (detalle) printf("\n=== SEGUNDA PASADA ===\n\n");
    bool alguna = false;
    textoLinea[0] = '\0';
    for (int i = 0; i < instructionCounter; i++) {
        if (flags[i] == -1) continue;
        alguna = true;
        int destino = flags[i];
        int ubicacion = buscarSimbolo(destino, 'L');
        numLinea = lineaOrigen[i];
        renglon = 0;
        char msg[120];
        if (ubicacion < 0) {
            snprintf(msg, sizeof msg, "goto a la linea %d, que no existe", destino);
            error("%s", msg);
            continue;
        }
        if (ubicacion >= instructionCounter) {
            snprintf(msg, sizeof msg, "la linea %d no genera instrucciones (esta despues del final)", destino);
            error("%s", msg);
            continue;
        }
        SML[i] += ubicacion;                               /* completa el operando */
        if (detalle)
            printf("  celda %03d: salto a la linea %d -> ubicacion %03d  =>  %+06d\n",
                   i, destino, ubicacion, SML[i]);
    }
    if (detalle && !alguna) printf("  No habia referencias hacia adelante.\n");
}

/* ======================================================================= */
/*                              Salida                                     */
/* ======================================================================= */

static bool exportarSML(const char *nombreSalida) {
    FILE *out = fopen(nombreSalida, "w");
    if (out == NULL) {
        fprintf(stderr, "Error: no se pudo crear el archivo %s\n", nombreSalida);
        return false;
    }
    int primeraDato = memoryCounter + 1;
    fprintf(out, "; =============================================================\n");
    fprintf(out, "; Programa SML generado por el compilador Simple -> SML\n");
    fprintf(out, "; Fuente        : %s\n", nombreFuente);
    fprintf(out, "; Instrucciones : 000-%03d  (%d palabras)\n", instructionCounter - 1, instructionCounter);
    if (primeraDato < TAM_MEMORIA)
        fprintf(out, "; Datos         : %03d-%03d  (%d palabras)\n", primeraDato, TAM_MEMORIA - 1,
                TAM_MEMORIA - primeraDato);
    fprintf(out, "; =============================================================\n");
    fprintf(out, "; --- Zona de programa ---\n");
    imprimirSML(out, false);
    if (primeraDato < TAM_MEMORIA) {
        fprintf(out, "; --- Zona de datos ---\n@%03d\n", primeraDato);
        for (int i = primeraDato; i < TAM_MEMORIA; i++)
            fprintf(out, "%+06d   ; %03d  %s\n", SML[i], i, descripcion[i]);
    }
    fclose(out);
    return true;
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    const char *fuente = NULL;
    const char *salida = NULL;
    bool tabla = false, detalle = false;
    char nombreLeido[260];

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) salida = argv[++i];
        else if (strcmp(argv[i], "--tabla") == 0) tabla = true;
        else if (strcmp(argv[i], "--detalle") == 0) detalle = true;
        else if (argv[i][0] == '-') {
            fprintf(stderr, "Opcion desconocida: %s\n", argv[i]);
            fprintf(stderr, "Uso: compilador programa.simple [-o salida.sml] [--tabla] [--detalle]\n");
            return 1;
        } else fuente = argv[i];
    }
    if (fuente == NULL) {
        printf("Ingrese el nombre del archivo de programa Simple (ej. programa.simple): ");
        if (scanf("%259s", nombreLeido) != 1) return 1;
        fuente = nombreLeido;
    }

    /* nombre de salida: mismo nombre con extension .sml */
    char nombreSalida[300];
    if (salida == NULL) {
        snprintf(nombreSalida, sizeof nombreSalida, "%s", fuente);
        char *punto = strrchr(nombreSalida, '.');
        char *barra = strrchr(nombreSalida, '/');
        char *barraW = strrchr(nombreSalida, '\\');
        if (barraW > barra) barra = barraW;
        if (punto != NULL && (barra == NULL || punto > barra)) *punto = '\0';
        strncat(nombreSalida, ".sml", sizeof nombreSalida - strlen(nombreSalida) - 1);
        salida = nombreSalida;
    }

    FILE *archivo = fopen(fuente, "r");
    if (archivo == NULL) {
        fprintf(stderr, "*** No se pudo abrir el archivo fuente '%s' ***\n", fuente);
        return 1;
    }
    const char *barra = strrchr(fuente, '/');
    const char *barraW = strrchr(fuente, '\\');
    if (barraW > barra) barra = barraW;
    nombreFuente = barra ? barra + 1 : fuente;

    for (int i = 0; i < TAM_MEMORIA; i++) { SML[i] = 0; flags[i] = -1; }

    printf("Compilando %s ...\n", nombreFuente);
    primeraPasada(archivo, detalle);           /* 1. primera pasada */
    fclose(archivo);
    segundaPasada(detalle);                    /* 2. segunda pasada */

    if (tabla && !detalle) { printf("\n"); imprimirTabla(stdout); }

    if (errores > 0) {
        fprintf(stderr, "*** Compilacion fallida: %d error(es). No se genero %s ***\n", errores, salida);
        liberarTabla();
        return 1;
    }
    if (!exportarSML(salida)) { liberarTabla(); return 1; }    /* 3. archivo final */
    printf("\nCompilacion exitosa: %d instrucciones, %d celdas de datos -> %s\n",
           instructionCounter, TAM_MEMORIA - 1 - memoryCounter, salida);
    liberarTabla();
    return 0;
}
