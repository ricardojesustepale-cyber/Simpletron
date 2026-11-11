# Requerimientos y cumplimiento de la rúbrica

En la columna de evidencia, los nombres como `sml_suma_dos` son casos de `pruebas/ejecutar_pruebas.sh`. La salida esperada de cada uno está en `pruebas/salidas/<caso>.txt`.

## 4.1 Programación en lenguaje de máquina SML (10 pts)

| Criterio | Evidencia |
|---|---|
| Uso correcto de instrucciones | `programas/sml/` usa READ, WRITE, LOAD, STORE, ADD, SUBTRACT, DIVIDE, MULTIPLY, BRANCH, BRANCHNEG, BRANCHZERO y HALT |
| Diseño: comentados y con código y datos separados | encabezado y comentario por instrucción; datos en `@900` |
| Secuencia, selección y repetición | `suma_dos`, `mayor_dos`, `suma_centinela`, `promedio_siete`, `mayor_n` |
| Pruebas | `sml_*` (6 casos) |

## 4.2 Simulador (15 pts)

| Criterio | Evidencia |
|---|---|
| Ciclo de ejecución | `cicloInstruccion()`; `sim_traza` |
| Instrucciones base | todas las pruebas `sml_*` |
| Registros | `accumulator`, `instructionCounter`, `instructionRegister`, `operationCode`, `operand` |
| Memory dump | `vaciadoMemoria()`; `sml_suma_dos` |
| Errores fatales | `err_division_cero`, `err_opcode_invalido`, `err_desbordamiento`, `err_direccion_invalida`, `err_sin_halt`, `err_read_sin_datos`, `sim_read_fuera_rango` |

## 4.3 Modificaciones al simulador (15 pts)

| Criterio | Evidencia |
|---|---|
| Carga desde archivo | `programa.simp` (`ext_programa_simp`), archivo inexistente, fuera de rango y texto inválido (`ext_archivo_*`) |
| Nuevas instrucciones documentadas | `opcodes.h` y la tabla del README; `ext_modulo_potencia` |
| Cadenas y salida extendida | `cadenas.sml`; `ext_cadenas` |
| Validaciones y límites | `ext_modulo_cero`, `ext_potencia_desborda`, `ext_exponente_negativo`, `ext_cadena_no_cabe` |
| Punto flotante | `flotantes.sml`; `ext_flotantes` |
| Rama aparte | `feature/mejoras-simpletron` |

## 4.4 Evaluación de expresiones

| Criterio | Evidencia |
|---|---|
| Agrupación `( ) [ ] { }` | `verificarAgrupacion()`; casos de error en `pruebas/entradas/expresiones.txt` |
| Profundidad de anidamiento | `profundidadMaxima` y `--profundidad`; `expr_profundidad` |
| Infijo → postfijo | `infijoAPostfijo()`; `2 ^ 3 ^ 2`, `10 - 4 - 3`, `6 + 2 * 3 ^ 2 - 8 / 4` |
| Evaluación del postfijo | `evaluarPostfijo()`; `expr_evaluador` |
| Varios dígitos | `125 * 40 - 999` |
| Integración con el compilador | `compilarExpresion()` en `compilador.c` |

## 4.5 Lenguaje Simple (10 pts)

| Criterio | Evidencia |
|---|---|
| Comandos | `rem`, `input`, `let`, `print`, `goto`, `if/goto`, `end` |
| Variables y constantes | `ubicacionVariable()`, `ubicacionConstante()` |
| Expresiones y condiciones | `expresiones.simple`; los 6 operadores relacionales en `mayor`, `factorial`, `promedio`, `fibonacci` y `cuadrados` |
| Reglas sintácticas | `comp_errores_sintaxis` |

## 4.6 Compilador (20 pts)

| Criterio | Evidencia |
|---|---|
| Primera pasada y tabla de símbolos | `--detalle` muestra la tabla y el SML preliminar |
| Referencias no resueltas | `flags[]` y las celdas `???` en `--detalle` |
| Segunda pasada | "celda NNN: salto a la linea …" en `--detalle` |
| `let` con postfijo y temporales | `comp_expresiones` |
| Programas ejecutables | pruebas `run_*` (11 ejecuciones) |
| Archivo SML documentado | `.sml` en `programas/simple/` |

## 4.7 Repositorio (10 pts)

| Criterio | Evidencia |
|---|---|
| Organización | estructura sugerida; `.gitignore` excluye ejecutables y `bin/` |
| README | compilación, ejecución, ejemplos e integrante |
| Historial y ramas | `git log --graph --all`; `evidencias/git-branch/` |
| Buenas prácticas | sin advertencias con `-Wall -Wextra -pedantic` |
