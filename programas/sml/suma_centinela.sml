; suma_centinela.sml - REPETICION con centinela
; Suma numeros hasta que se teclea uno negativo (Deitel 7.18 a)
; Instrucciones: 000-007   Datos: 900-901
; --- Zona de programa ---
+10900   ; 000  READ         900   lee num
+20900   ; 001  LOAD         900   acc <- num
+41006   ; 002  BRANCHNEG    006   num < 0 termina
+30901   ; 003  ADD          901   acc <- num + suma
+21901   ; 004  STORE        901   suma <- acc
+40000   ; 005  BRANCH       000   repite
+11901   ; 006  WRITE        901   imprime suma
+43000   ; 007  HALT         000
; --- Zona de datos ---
@900
+00000   ; 900  num
+00000   ; 901  suma
