; =============================================================
; Programa SML generado por el compilador Simple -> SML
; Fuente        : factorial.simple
; Instrucciones : 000-021  (22 palabras)
; Datos         : 994-999  (6 palabras)
; =============================================================
; --- Zona de programa ---
+10999   ; 000  READ         999   | 20 input n
+20999   ; 001  LOAD         999   | 30 if n < 0 goto 90
+31998   ; 002  SUBTRACT     998
+41020   ; 003  BRANCHNEG    020
+20996   ; 004  LOAD         996   | 40 let f = 1
+21997   ; 005  STORE        997
+20996   ; 006  LOAD         996   | 50 let i = 1
+21995   ; 007  STORE        995
+20999   ; 008  LOAD         999   | 60 if i > n goto 80
+31995   ; 009  SUBTRACT     995
+41018   ; 010  BRANCHNEG    018
+20997   ; 011  LOAD         997   | 65 let f = f * i
+33995   ; 012  MULTIPLY     995
+21997   ; 013  STORE        997
+20995   ; 014  LOAD         995   | 70 let i = i + 1
+30996   ; 015  ADD          996
+21995   ; 016  STORE        995
+40008   ; 017  BRANCH       008   | 75 goto 60
+11997   ; 018  WRITE        997   | 80 print f
+40021   ; 019  BRANCH       021   | 85 goto 99
+11994   ; 020  WRITE        994   | 90 print -1
+43000   ; 021  HALT         000   | 99 end
; --- Zona de datos ---
@994
-00001   ; 994  constante -1
+00000   ; 995  variable i
+00001   ; 996  constante 1
+00000   ; 997  variable f
+00000   ; 998  constante 0
+00000   ; 999  variable n
