; =============================================================
; Programa SML generado por el compilador Simple -> SML
; Fuente        : fibonacci.simple
; Instrucciones : 000-017  (18 palabras)
; Datos         : 994-999  (6 palabras)
; =============================================================
; --- Zona de programa ---
+10999   ; 000  READ         999   | 20 input l
+20997   ; 001  LOAD         997   | 30 let a = 0
+21998   ; 002  STORE        998
+20995   ; 003  LOAD         995   | 40 let b = 1
+21996   ; 004  STORE        996
+20999   ; 005  LOAD         999   | 50 if a > l goto 99
+31998   ; 006  SUBTRACT     998
+41017   ; 007  BRANCHNEG    017
+11998   ; 008  WRITE        998   | 60 print a
+20998   ; 009  LOAD         998   | 70 let t = a + b
+30996   ; 010  ADD          996
+21994   ; 011  STORE        994
+20996   ; 012  LOAD         996   | 80 let a = b
+21998   ; 013  STORE        998
+20994   ; 014  LOAD         994   | 90 let b = t
+21996   ; 015  STORE        996
+40005   ; 016  BRANCH       005   | 95 goto 50
+43000   ; 017  HALT         000   | 99 end
; --- Zona de datos ---
@994
+00000   ; 994  variable t
+00001   ; 995  constante 1
+00000   ; 996  variable b
+00000   ; 997  constante 0
+00000   ; 998  variable a
+00000   ; 999  variable l
