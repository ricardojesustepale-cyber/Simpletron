; cadenas.sml - prueba de READ_STRING (13), WRITE_STRING (14) y NEWLINE (12)
; Formato: memoria[d] = longitud, memoria[d+1..] = codigo ASCII de cada caracter
; Lee un nombre en 500 y saluda: Hola, <nombre>!
; --- Zona de programa ---
+13500   ; 000  READ_STRING  500   lee el nombre
+14900   ; 001  WRITE_STRING 900   "Hola, "
+14500   ; 002  WRITE_STRING 500   nombre
+14907   ; 003  WRITE_STRING 907   "!"
+12000   ; 004  NEWLINE      000
+43000   ; 005  HALT         000
; --- Zona de datos ---
@900
+00006   ; 900  "Hola, " longitud 6
+00072   ; 901  'H'
+00111   ; 902  'o'
+00108   ; 903  'l'
+00097   ; 904  'a'
+00044   ; 905  ','
+00032   ; 906  ' '
+00001   ; 907  "!" longitud 1
+00033   ; 908  '!'
