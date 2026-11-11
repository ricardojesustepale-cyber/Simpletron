; =============================================================
; Programa SML generado por el compilador Simple -> SML
; Fuente        : cuadrados.simple
; Instrucciones : 000-009  (10 palabras)
; Datos         : 997-999  (3 palabras)
; =============================================================
; --- Zona de programa ---
+10999   ; 000  READ         999   | 20 input j
+20999   ; 001  LOAD         999   | 30 if j == 0 goto 99
+31998   ; 002  SUBTRACT     998
+42009   ; 003  BRANCHZERO   009
+20999   ; 004  LOAD         999   | 40 let k = j * j
+33999   ; 005  MULTIPLY     999
+21997   ; 006  STORE        997
+11997   ; 007  WRITE        997   | 50 print k
+40000   ; 008  BRANCH       000   | 60 goto 20
+43000   ; 009  HALT         000   | 99 end
; --- Zona de datos ---
@997
+00000   ; 997  variable k
+00000   ; 998  constante 0
+00000   ; 999  variable j
