; suma_dos.sml - SECUENCIA: lee dos enteros e imprime su suma
; Instrucciones: 000-006   Datos: 900-902
; --- Zona de programa ---
+10900   ; 000  READ         900   lee A
+10901   ; 001  READ         901   lee B
+20900   ; 002  LOAD         900   acc <- A
+30901   ; 003  ADD          901   acc <- A + B
+21902   ; 004  STORE        902   C <- acc
+11902   ; 005  WRITE        902   imprime C
+43000   ; 006  HALT         000
; --- Zona de datos ---
@900
+00000   ; 900  A
+00000   ; 901  B
+00000   ; 902  C (suma)
