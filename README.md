# Simulador Simpletron (SML) - Fase 2 (Mejorado)

Esta versión extendida del simulador de la computadora **Simpletron** amplía la arquitectura de memoria a 1000 posiciones e integra nuevas capacidades avanzadas de procesamiento.

---

## 🚀 Nuevas Características (Fase 2)

1. **Memoria de 1000 Posiciones:** Direccionamiento ampliado de `000` a `999`.
2. **Carga desde Archivo:** Lee automáticamente las instrucciones del archivo `programa.simp`. Si no existe, conmuta a modo interactivo por teclado.
3. **Residuo / Módulo (`Opcode 34`):** Calcula el resto de la división entera 
4. **Exponenciación (`Opcode 35`):** Eleva el acumulador a la potencia indicada 
5. **Salida de Nueva Línea (`Opcode 12`):** Imprime un salto de línea (`\n`) en consola.
6. **Entrada de Cadenas ASCII (`Opcode 13`):** Lee texto y lo almacena codificado en ASCII a partir de la dirección base.
7. **Salida de Cadenas ASCII (`Opcode 14`):** Decodifica e imprime caracteres ASCII almacenados en memoria.
8. **Punto Flotante (`Opcodes 50-53`):** Soporte para lectura, escritura y operaciones con números reales.

---

## 📋 Tabla Actualizada de Instrucciones SML

| Opcode | Instrucción | Descripción |
| :---: | :--- | :--- |
| **10** | `READ` | Lee un entero desde el teclado y lo guarda en la dirección. |
| **11** | `WRITE` | Imprime el entero almacenado en la dirección. |
| **12** | `NEWLINE` | Imprime un salto de línea en consola. |
| **13** | `READ_STRING` | Lee una cadena y guarda longitud y valores ASCII. |
| **14** | `WRITE_STRING` | Imprime la cadena almacenada desde la dirección base. |
| **20** | `LOAD` | Carga el valor de memoria en el Acumulador. |
| **21** | `STORE` | Almacena el valor del Acumulador en memoria. |
| **30** | `ADD` | Suma el valor de memoria al Acumulador. |
| **31** | `SUBTRACT` | Resta el valor de memoria al Acumulador. |
| **32** | `DIVIDE` | Divide el Acumulador entre el valor de memoria. |
| **33** | `MULTIPLY` | Multiplica el Acumulador por el valor de memoria. |
| **34** | `MODULO` | Calcula el módulo entre el Acumulador y la memoria. |
| **35** | `EXPONENT` | Eleva el Acumulador a la potencia indicada en memoria. |
| **40** | `BRANCH` | Salto incondicional a la dirección especificada. |
| **41** | `BRANCHNEG` | Salta a la dirección si el Acumulador es negativo. |
| **42** | `BRANCHZERO` | Salta a la dirección si el Acumulador es cero. |
| **43** | `HALT` | Detiene la ejecución del simulador. |
| **50** | `READ_FLOAT` | Lee un número decimal (flotante) desde teclado. |
| **51** | `WRITE_FLOAT` | Imprime un número decimal almacenado en memoria. |
| **52** | `ADD_FLOAT` | Suma de punto flotante en el Acumulador. |
| **53** | `SUB_FLOAT` | Resta de punto flotante en el Acumulador. |

---

## 🛠️ Compilación y Ejecución