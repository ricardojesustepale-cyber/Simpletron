; promedio_siete.sml - REPETICION controlada por contador
; Lee 7 numeros e imprime su promedio entero (Deitel 7.18 b)
; Instrucciones: 000-013   Datos: 900-905
; --- Zona de programa ---
+10900   ; 000  READ         900   lee num
+20901   ; 001  LOAD         901   acc <- suma
+30900   ; 002  ADD          900   acc <- suma + num
+21901   ; 003  STORE        901   suma <- acc
+20902   ; 004  LOAD         902   acc <- contador
+31903   ; 005  SUBTRACT     903   acc <- contador - 1
+21902   ; 006  STORE        902   contador <- acc
+42009   ; 007  BRANCHZERO   009   contador == 0 sale
+40000   ; 008  BRANCH       000   repite
+20901   ; 009  LOAD         901   acc <- suma
+32905   ; 010  DIVIDE       905   acc <- suma / 7
+21904   ; 011  STORE        904   promedio <- acc
+11904   ; 012  WRITE        904   imprime promedio
+43000   ; 013  HALT         000
; --- Zona de datos ---
@900
+00000   ; 900  num
+00000   ; 901  suma
+00007   ; 902  contador (inicia en 7)
+00001   ; 903  constante 1
+00000   ; 904  promedio
+00007   ; 905  constante 7
