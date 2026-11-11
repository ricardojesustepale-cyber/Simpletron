; mayor_dos.sml - SELECCION: lee dos enteros e imprime el mayor
; Instrucciones: 000-008   Datos: 900-901
; --- Zona de programa ---
+10900   ; 000  READ         900   lee A
+10901   ; 001  READ         901   lee B
+20900   ; 002  LOAD         900   acc <- A
+31901   ; 003  SUBTRACT     901   acc <- A - B
+41007   ; 004  BRANCHNEG    007   si A < B imprime B
+11900   ; 005  WRITE        900   imprime A
+43000   ; 006  HALT         000
+11901   ; 007  WRITE        901   imprime B
+43000   ; 008  HALT         000
; --- Zona de datos ---
@900
+00000   ; 900  A
+00000   ; 901  B
