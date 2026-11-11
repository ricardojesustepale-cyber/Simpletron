; =============================================================
; Programa SML generado por el compilador Simple -> SML
; Fuente        : expresiones.simple
; Instrucciones : 000-062  (63 palabras)
; Datos         : 984-999  (16 palabras)
; =============================================================
; --- Zona de programa ---
+10999   ; 000  READ         999   | 20 input a, b
+10998   ; 001  READ         998
+20999   ; 002  LOAD         999   | 30 let c = (a + b) * (a - b)
+30998   ; 003  ADD          998
+21996   ; 004  STORE        996
+20999   ; 005  LOAD         999
+31998   ; 006  SUBTRACT     998
+21995   ; 007  STORE        995
+20996   ; 008  LOAD         996
+33995   ; 009  MULTIPLY     995
+21997   ; 010  STORE        997
+20999   ; 011  LOAD         999   | 40 let d = a ^ 2 - b ^ 2
+35993   ; 012  EXPONENT     993
+21996   ; 013  STORE        996
+20998   ; 014  LOAD         998
+35993   ; 015  EXPONENT     993
+21995   ; 016  STORE        995
+20996   ; 017  LOAD         996
+31995   ; 018  SUBTRACT     995
+21994   ; 019  STORE        994
+20999   ; 020  LOAD         999   | 50 let e = {a + [b * (a - 1)]} / 2
+31991   ; 021  SUBTRACT     991
+21996   ; 022  STORE        996
+20998   ; 023  LOAD         998
+33996   ; 024  MULTIPLY     996
+21995   ; 025  STORE        995
+20999   ; 026  LOAD         999
+30995   ; 027  ADD          995
+21990   ; 028  STORE        990
+32993   ; 029  DIVIDE       993
+21992   ; 030  STORE        992
+20999   ; 031  LOAD         999   | 60 let r = a % b
+34998   ; 032  MODULO       998
+21989   ; 033  STORE        989
+11997   ; 034  WRITE        997   | 70 print c, d, e, r
+11994   ; 035  WRITE        994
+11992   ; 036  WRITE        992
+11989   ; 037  WRITE        989
+20988   ; 038  LOAD         988   | 80 print 2 ^ 3 ^ 2 - 500
+35993   ; 039  EXPONENT     993
+21996   ; 040  STORE        996
+20993   ; 041  LOAD         993
+35996   ; 042  EXPONENT     996
+21995   ; 043  STORE        995
+31987   ; 044  SUBTRACT     987
+21990   ; 045  STORE        990
+11990   ; 046  WRITE        990
+20986   ; 047  LOAD         986   | 90 print -a + 3 * (b - 1) ^ 2
+33999   ; 048  MULTIPLY     999
+21996   ; 049  STORE        996
+20998   ; 050  LOAD         998
+31991   ; 051  SUBTRACT     991
+21995   ; 052  STORE        995
+35993   ; 053  EXPONENT     993
+21990   ; 054  STORE        990
+20988   ; 055  LOAD         988
+33990   ; 056  MULTIPLY     990
+21985   ; 057  STORE        985
+20996   ; 058  LOAD         996
+30985   ; 059  ADD          985
+21984   ; 060  STORE        984
+11984   ; 061  WRITE        984
+43000   ; 062  HALT         000   | 99 end
; --- Zona de datos ---
@984
+00000   ; 984  temporal t5
+00000   ; 985  temporal t4
-00001   ; 986  constante -1
+00500   ; 987  constante 500
+00003   ; 988  constante 3
+00000   ; 989  variable r
+00000   ; 990  temporal t3
+00001   ; 991  constante 1
+00000   ; 992  variable e
+00002   ; 993  constante 2
+00000   ; 994  variable d
+00000   ; 995  temporal t2
+00000   ; 996  temporal t1
+00000   ; 997  variable c
+00000   ; 998  variable b
+00000   ; 999  variable a
