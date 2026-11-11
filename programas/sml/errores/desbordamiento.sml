; desbordamiento.sml - ERROR FATAL esperado: 90000 + 90000 desborda el acumulador
; --- Zona de programa ---
+20900   ; 000  LOAD         900
+30900   ; 001  ADD          900
+43000   ; 002  HALT         000
; --- Zona de datos ---
@900
+90000   ; 900  constante 90000
