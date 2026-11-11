#!/usr/bin/env bash
# ejecutar_pruebas.sh - Pruebas automáticas del proyecto Simpletron
#
# Cada caso ejecuta un comando con una entrada de pruebas/entradas/ y compara
# la salida con la esperada en pruebas/salidas/. Lo obtenido se guarda en
# pruebas/obtenidas/ para revisarlo. En Windows se ejecuta desde Git Bash.
#
# Uso (desde la raíz del repositorio):
#   bash pruebas/ejecutar_pruebas.sh            ejecuta y compara
#   bash pruebas/ejecutar_pruebas.sh --generar  (re)genera las salidas esperadas

cd "$(dirname "$0")/.." || exit 1

SIM=bin/simpletron
EVAL=bin/evaluador
COMP=bin/compilador
SML=programas/sml
SIMPLE=programas/simple
ENT=pruebas/entradas
ESP=pruebas/salidas
OBT=pruebas/obtenidas

if [[ ! -x $SIM || ! -x $EVAL || ! -x $COMP ]]; then
    echo "Primero compile el proyecto con: make"
    exit 1
fi

GENERAR=0
[[ "$1" == "--generar" ]] && GENERAR=1
mkdir -p "$OBT" "$ESP"
PASAN=0
FALLAN=0

# caso NOMBRE ARCHIVO_ENTRADA COMANDO...
caso() {
    local nombre=$1 entrada=$2
    shift 2
    local obtenida="$OBT/$nombre.txt"
    if [[ -n $entrada ]]; then
        "$@" < "$ENT/$entrada" > "$obtenida" 2>&1
    else
        "$@" < /dev/null > "$obtenida" 2>&1
    fi
    echo "[codigo de salida: $?]" >> "$obtenida"

    if (( GENERAR )); then
        cp "$obtenida" "$ESP/$nombre.txt"
        printf "  generada  %s\n" "$nombre"
    elif diff -q "$ESP/$nombre.txt" "$obtenida" > /dev/null 2>&1; then
        printf "  PASA   %s\n" "$nombre"
        PASAN=$((PASAN + 1))
    else
        printf "  FALLA  %s   (diff %s %s)\n" "$nombre" "$ESP/$nombre.txt" "$obtenida"
        FALLAN=$((FALLAN + 1))
    fi
}

echo "== 1. Programas SML =="
caso sml_suma_dos            dos_numeros_15_27.txt  $SIM $SML/suma_dos.sml --eco
caso sml_mayor_dos_a         dos_numeros_27_15.txt  $SIM $SML/mayor_dos.sml --eco --sin-dump
caso sml_mayor_dos_b         dos_numeros_15_27.txt  $SIM $SML/mayor_dos.sml --eco --sin-dump
caso sml_suma_centinela      centinela.txt          $SIM $SML/suma_centinela.sml --eco --sin-dump
caso sml_promedio_siete      siete_numeros.txt      $SIM $SML/promedio_siete.sml --eco --sin-dump
caso sml_mayor_n             mayor_n.txt            $SIM $SML/mayor_n.sml --eco --sin-dump

echo "== 2. Simulador: carga, registros, traza y errores fatales =="
caso sim_carga_interactiva   carga_interactiva.txt  $SIM --eco
caso sim_traza               dos_numeros_27_15.txt  $SIM $SML/mayor_dos.sml --eco --traza --sin-dump
caso sim_read_fuera_rango    read_fuera_rango.txt   $SIM $SML/suma_dos.sml --eco --sin-dump
caso err_division_cero       division_cero.txt      $SIM $SML/errores/division_cero.sml --eco
caso err_opcode_invalido     ""                     $SIM $SML/errores/opcode_invalido.sml
caso err_desbordamiento      ""                     $SIM $SML/errores/desbordamiento.sml
caso err_direccion_invalida  ""                     $SIM $SML/errores/direccion_invalida.sml
caso err_sin_halt            ""                     $SIM $SML/errores/sin_halt.sml --sin-dump
caso err_read_sin_datos      ""                     $SIM $SML/suma_dos.sml --sin-dump

echo "== 3. Extensiones del simulador (rama feature/mejoras-simpletron) =="
caso ext_programa_simp       programa_simp.txt      bash -c "cd $SML && ../../$SIM --eco"
caso ext_archivo_inexistente ""                     $SIM $SML/no_existe.sml
caso ext_archivo_fuera_rango ""                     $SIM $SML/errores/fuera_rango.sml
caso ext_archivo_token       ""                     $SIM $SML/errores/token_invalido.sml
caso ext_modulo_potencia     modulo_potencia.txt    $SIM $SML/modulo_potencia.sml --eco
caso ext_modulo_cero         ""                     $SIM $SML/errores/modulo_cero.sml --sin-dump
caso ext_potencia_desborda   ""                     $SIM $SML/errores/potencia_desborda.sml --sin-dump
caso ext_exponente_negativo  ""                     $SIM $SML/errores/exponente_negativo.sml --sin-dump
caso ext_cadenas             nombre.txt             $SIM $SML/cadenas.sml --eco
caso ext_cadena_no_cabe      cadena_larga.txt       $SIM $SML/errores/cadena_no_cabe.sml --eco --sin-dump
caso ext_flotantes           flotantes.txt          $SIM $SML/flotantes.sml --eco --sin-dump

echo "== 4. Evaluacion de expresiones =="
caso expr_evaluador          ""                     $EVAL $ENT/expresiones.txt
caso expr_profundidad        ""                     $EVAL --profundidad 2 $ENT/profundidad.txt

echo "== 5 y 6. Lenguaje Simple y compilador (compilar + ejecutar) =="
compilar_y_ejecutar() {
    local prog=$1 entrada=$2 sufijo=${3:-}
    if [[ -z $sufijo ]]; then
        caso "comp_$prog" "" $COMP $SIMPLE/$prog.simple -o $OBT/$prog.sml --detalle
    fi
    caso "run_$prog$sufijo" "$entrada" $SIM $OBT/$prog.sml --eco --sin-dump
}
compilar_y_ejecutar programa    dos_numeros_15_27.txt
compilar_y_ejecutar suma        dos_numeros_15_27.txt
compilar_y_ejecutar mayor       dos_numeros_15_27.txt
compilar_y_ejecutar mayor       dos_numeros_27_15.txt _2
compilar_y_ejecutar cuadrados   cuadrados.txt
compilar_y_ejecutar factorial   factorial_5.txt
compilar_y_ejecutar factorial   factorial_8.txt _8
compilar_y_ejecutar factorial   factorial_negativo.txt _negativo
compilar_y_ejecutar promedio    promedio.txt
compilar_y_ejecutar fibonacci   fibonacci.txt
compilar_y_ejecutar expresiones expresiones_7_3.txt

caso comp_errores_sintaxis   ""   $COMP $SIMPLE/errores/errores_sintaxis.simple -o $OBT/errores_sintaxis.sml
caso comp_memoria_llena      ""   $COMP $SIMPLE/errores/memoria_llena.simple -o $OBT/memoria_llena.sml
caso comp_sin_end            ""   $COMP $SIMPLE/errores/sin_end.simple -o $OBT/sin_end.sml --tabla
caso run_sin_end             sin_end.txt  $SIM $OBT/sin_end.sml --eco --sin-dump

if (( ! GENERAR )); then
    echo
    echo "Resultado: $PASAN pruebas pasan, $FALLAN fallan."
    (( FALLAN == 0 ))
fi
