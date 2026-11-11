; =============================================================
; Programa SML generado por el compilador Simple -> SML
; Fuente        : mayor.simple
; Instrucciones : 000-009  (10 palabras)
; Datos         : 998-999  (2 palabras)
; =============================================================
; --- Zona de programa ---
+10999   ; 000  READ         999   | 20 input s
+10998   ; 001  READ         998   | 30 input t
+20998   ; 002  LOAD         998   | 40 if s >= t goto 90
+31999   ; 003  SUBTRACT     999
+41008   ; 004  BRANCHNEG    008
+42008   ; 005  BRANCHZERO   008
+11998   ; 006  WRITE        998   | 60 print t
+40009   ; 007  BRANCH       009   | 70 goto 99
+11999   ; 008  WRITE        999   | 90 print s
+43000   ; 009  HALT         000   | 99 end
; --- Zona de datos ---
@998
+00000   ; 998  variable t
+00000   ; 999  variable s
