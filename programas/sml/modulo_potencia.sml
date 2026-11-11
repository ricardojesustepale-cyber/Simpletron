; modulo_potencia.sml - prueba de MODULO (34) y EXPONENT (35)
; Lee A y B; imprime A % B y A ^ B
; --- Zona de programa ---
+10900   ; 000  READ         900   lee A
+10901   ; 001  READ         901   lee B
+20900   ; 002  LOAD         900
+34901   ; 003  MODULO       901   acc <- A % B
+21902   ; 004  STORE        902
+11902   ; 005  WRITE        902   imprime A % B
+20900   ; 006  LOAD         900
+35901   ; 007  EXPONENT     901   acc <- A ^ B
+21902   ; 008  STORE        902
+11902   ; 009  WRITE        902   imprime A ^ B
+43000   ; 010  HALT         000
; --- Zona de datos ---
@900
+00000   ; 900  A
+00000   ; 901  B
+00000   ; 902  resultado
