# Diseño y decisiones

## 1. Arquitectura

```
programa.simple ──► [compilador] ──► programa.sml ──► [simpletron] ──► salida
                         │
                         └── usa src/expresiones (tokens, infijo→postfijo, pila)
```

`src/simpletron/opcodes.h` es la única definición de las instrucciones. La usan el simulador y el compilador, así que nunca pueden quedar con formatos distintos. Antes sí pasaba: el compilador generaba `+1099` (4 dígitos) y el simulador de 1000 palabras esperaba `+10999`.

## 2. Simulador

**Registros:** `accumulator`, `instructionCounter`, `instructionRegister`, `operationCode` y `operand`.

**Ciclo de instrucción** (`cicloInstruccion`):

0. Si `instructionCounter` no está entre 0 y 999, es un error fatal de dirección inválida.
1. Traer: `instructionRegister = memory[instructionCounter]`.
2. Decodificar: `operationCode = IR / 1000` y `operand = IR % 1000`. Una palabra negativa o un opcode desconocido son un error fatal.
3. Ejecutar la instrucción con un `switch`.
4. Actualizar el contador: `instructionCounter + 1`, salvo en los saltos.

**Errores fatales:** división o módulo entre cero; opcode inválido (incluye `+00000` cuando falta HALT); desbordamiento del acumulador (el resultado se calcula en `long long` y se valida antes de asignarlo); dirección inválida; exponente negativo; cadenas que no caben en memoria; entrada agotada. En todos los casos se imprimen el mensaje con la dirección, el *memory dump*, y el programa termina con código 2.

**Valores fuera de rango:** en `READ` y en la carga interactiva se pide el dato de nuevo, como en Deitel. Un archivo con palabras fuera de rango o texto que no es número es un error de carga.

### Mejoras (rama `feature/mejoras-simpletron` y correcciones posteriores)

| Mejora | Decisión |
|---|---|
| Memoria de 1000 palabras | Formato `+CCOOO`: el operando necesita 3 dígitos. Las palabras van de −99999 a +99999. |
| Carga desde archivo | Sin argumentos se carga `programa.simp` (lo pide el requerimiento); con argumento, cualquier archivo. Se aceptan comentarios `;` y `#`, y la directiva `@NNN` coloca la zona de datos. |
| Centinela −999999 | −99999 ya es una palabra válida en Simpletron 1000, así que el centinela necesita un dígito más. |
| `MODULO` 34, `EXPONENT` 35 | Grupo aritmético 3x. Ambos validan división entre cero, exponente negativo y desbordamiento. |
| `NEWLINE` 12, `READ_STRING` 13, `WRITE_STRING` 14 | Cadena: `[d]` = longitud y `[d+1…]` = código ASCII. `WRITE_STRING` no imprime salto de línea, para poder combinarse con `NEWLINE`. |
| Punto flotante 50–53 | La primera versión guardaba los bits del `float` en un `int`, lo que daba palabras fuera de rango y un *dump* ilegible. Ahora se usa **punto fijo ×100**: `3.25` se guarda como `325`. Así sigue siendo una palabra válida y `ADD_FLOAT`/`SUB_FLOAT` detectan el desbordamiento. |
| *Memory dump* | Con 1000 celdas se omiten las filas que están completamente en cero. |

## 3. Expresiones

Las primeras versiones eran tres programas separados (`feature/pila-anidamiento`, `feature/conversion-postfijo` y `feature/evaluacion-postfijo`). Se unificaron en `expresiones.c` para que el compilador las use. También se corrigieron estas limitaciones:

| Antes | Ahora |
|---|---|
| La conversión solo aceptaba `( )` | Acepta `( ) [ ] { }` |
| La profundidad solo decía verdadero o falso | Calcula la profundidad máxima y puede aplicar un límite (`--profundidad N`) |
| Operandos de un carácter (`12+3` → `123+`) | Números de varios dígitos y variables de una letra |
| `^` asociativo a la izquierda (`2^3^2` = 64) | `^` asociativo a la derecha (`2^3^2` = 512) |
| La evaluación devolvía 0 ante un error | Mensaje de error con su posición |

| Operador | Precedencia | Asociatividad |
|---|:---:|---|
| `^` | 3 | derecha |
| `* / %` | 2 | izquierda |
| `+ -` | 1 | izquierda |

**Menos unario:** `-5` se convierte en la constante −5, y `-x` en `-1 * x`.

## 4. Compilador

**Memoria:** las instrucciones se colocan desde la 000 hacia arriba (`instructionCounter`) y las variables, constantes y temporales desde la 999 hacia abajo (`memoryCounter`). Si se cruzan, se informa "memoria insuficiente".

**Tabla de símbolos:** es una lista ligada (el diseño original), ahora con inserción al final para que se imprima en orden. Tipos: `L` (línea → primera instrucción), `V` (variable → celda), `C` (constante → celda con el valor ya cargado).

**Primera pasada:**

| Simple | SML |
|---|---|
| `input x` | `READ x` |
| `print e` | código de `e` + `WRITE` |
| `let x = e` | código de `e`; la última operación hace `STORE x` |
| `goto n` | `BRANCH n` |
| `if a == b goto n` | `LOAD a`, `SUBTRACT b`, `BRANCHZERO n` |
| `if a != b goto n` | `LOAD a`, `SUBTRACT b`, `BRANCHZERO +2`, `BRANCH n` |
| `if a < b goto n` | `LOAD a`, `SUBTRACT b`, `BRANCHNEG n` |
| `if a > b goto n` | `LOAD b`, `SUBTRACT a`, `BRANCHNEG n` |
| `if a <= b goto n` | `LOAD a`, `SUBTRACT b`, `BRANCHNEG n`, `BRANCHZERO n` |
| `if a >= b goto n` | `LOAD b`, `SUBTRACT a`, `BRANCHNEG n`, `BRANCHZERO n` |
| `end` | `HALT` |

**Expresiones en `let`:** se convierten a postfijo con el módulo de expresiones y se recorren con una pila de *direcciones*. Cada operando apila su dirección. Cada operador saca `y` y `x`, genera `LOAD x`, `OP y` y `STORE t`, y apila `t`. Las temporales se reutilizan en cada línea.

**Referencias hacia adelante:** si la línea destino de un salto todavía no existe, se genera el salto con operando `000` y se anota `flags[celda] = línea`.

**Segunda pasada:** para cada `flags[i] != -1` se busca la línea en la tabla y se suma su ubicación a la instrucción. Si la línea no existe, es un error de compilación.

**Errores:** línea sin número, números de línea no ascendentes, mayúsculas fuera de `rem`, comando desconocido, variable inválida, expresión mal formada, `=` en lugar de `==`, constante fuera de rango, `goto` a una línea inexistente y memoria insuficiente. Si hay errores no se genera el `.sml`. Un programa sin `end` solo produce una advertencia y se le agrega `HALT`.

## 5. Bitácora de decisiones

| Fecha | Decisión |
|---|---|
| 2026-09-17 | Simulador V1 de 100 palabras en C (`main.c`). |
| 2026-09-18 | Fase 2 en una rama aparte: memoria de 1000, opcodes 12–14, 34–35 y 50–53, y `programa.simp`. |
| 2026-09-30 | Pila, verificación de anidamiento, conversión y evaluación del postfijo en ramas separadas. |
| 2026-10-05 | Primera y segunda pasada del compilador con lista ligada y `flags[]`. |
| 2026-11-10 | Revisión antes de la entrega: compilador y simulador en el mismo formato (`opcodes.h`); se restauraron los errores fatales; flotante en punto fijo; expresiones unificadas y corregidas; `let` e `if/goto` en el compilador; pruebas y documentación. |
