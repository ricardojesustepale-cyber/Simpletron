; flotantes.sml - prueba de punto flotante (50-53), punto fijo x100
; Lee X y Y; imprime X + Y y X - Y con dos decimales
; --- Zona de programa ---
+50900   ; 000  READ_FLOAT   900   lee X
+50901   ; 001  READ_FLOAT   901   lee Y
+20900   ; 002  LOAD         900
+52901   ; 003  ADD_FLOAT    901   acc <- X + Y
+21902   ; 004  STORE        902
+51902   ; 005  WRITE_FLOAT  902   imprime X + Y
+20900   ; 006  LOAD         900
+53901   ; 007  SUB_FLOAT    901   acc <- X - Y
+21902   ; 008  STORE        902
+51902   ; 009  WRITE_FLOAT  902   imprime X - Y
+43000   ; 010  HALT         000
; --- Zona de datos ---
@900
+00000   ; 900  X
+00000   ; 901  Y
+00000   ; 902  resultado
