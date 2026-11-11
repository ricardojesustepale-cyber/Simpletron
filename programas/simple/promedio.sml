; =============================================================
; Programa SML generado por el compilador Simple -> SML
; Fuente        : promedio.simple
; Instrucciones : 000-024  (25 palabras)
; Datos         : 993-999  (7 palabras)
; =============================================================
; --- Zona de programa ---
+10999   ; 000  READ         999   | 20 input n
+20999   ; 001  LOAD         999   | 30 if n <= 0 goto 99
+31998   ; 002  SUBTRACT     998
+41024   ; 003  BRANCHNEG    024
+42024   ; 004  BRANCHZERO   024
+20998   ; 005  LOAD         998   | 40 let s = 0
+21997   ; 006  STORE        997
+20998   ; 007  LOAD         998   | 50 let c = 0
+21996   ; 008  STORE        996
+10995   ; 009  READ         995   | 60 input x
+20997   ; 010  LOAD         997   | 70 let s = s + x
+30995   ; 011  ADD          995
+21997   ; 012  STORE        997
+20996   ; 013  LOAD         996   | 80 let c = c + 1
+30994   ; 014  ADD          994
+21996   ; 015  STORE        996
+20996   ; 016  LOAD         996   | 90 if c != n goto 60
+31999   ; 017  SUBTRACT     999
+42020   ; 018  BRANCHZERO   020
+40009   ; 019  BRANCH       009
+20997   ; 020  LOAD         997   | 95 print s / n
+32999   ; 021  DIVIDE       999
+21993   ; 022  STORE        993
+11993   ; 023  WRITE        993
+43000   ; 024  HALT         000   | 99 end
; --- Zona de datos ---
@993
+00000   ; 993  temporal t1
+00001   ; 994  constante 1
+00000   ; 995  variable x
+00000   ; 996  variable c
+00000   ; 997  variable s
+00000   ; 998  constante 0
+00000   ; 999  variable n
