; mayor_n.sml - SELECCION dentro de REPETICION
; Lee n y despues n numeros; imprime el mayor (Deitel 7.18 c). Si n <= 0 termina.
; Instrucciones: 000-018   Datos: 900-903
; --- Zona de programa ---
+10900   ; 000  READ         900   lee n
+20900   ; 001  LOAD         900
+41018   ; 002  BRANCHNEG    018   n < 0 fin
+42018   ; 003  BRANCHZERO   018   n == 0 fin
+10902   ; 004  READ         902   mayor <- primer numero
+20900   ; 005  LOAD         900   [ciclo] acc <- n
+31903   ; 006  SUBTRACT     903   acc <- n - 1
+21900   ; 007  STORE        900   n <- acc
+42017   ; 008  BRANCHZERO   017   no quedan numeros
+10901   ; 009  READ         901   lee num
+20902   ; 010  LOAD         902   acc <- mayor
+31901   ; 011  SUBTRACT     901   acc <- mayor - num
+41014   ; 012  BRANCHNEG    014   mayor < num
+40005   ; 013  BRANCH       005   siguiente
+20901   ; 014  LOAD         901   acc <- num
+21902   ; 015  STORE        902   mayor <- num
+40005   ; 016  BRANCH       005   siguiente
+11902   ; 017  WRITE        902   imprime mayor
+43000   ; 018  HALT         000
; --- Zona de datos ---
@900
+00000   ; 900  n
+00000   ; 901  num
+00000   ; 902  mayor
+00001   ; 903  constante 1
