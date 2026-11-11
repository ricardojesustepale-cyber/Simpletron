; direccion_invalida.sml - ERROR FATAL esperado: salta a la celda 999 y el contador llega a 1000
; --- Zona de programa ---
+40999   ; 000  BRANCH       999
; --- Zona de datos ---
@999
+11900   ; 999  999  WRITE 900 -> siguiente seria la 1000
