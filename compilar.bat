@echo off
REM compilar.bat - compila el proyecto en Windows (MinGW / MSYS2)
if not exist bin mkdir bin
gcc -std=c99 -Wall -o bin\simpletron.exe src\simpletron\simpletron.c -lm
gcc -std=c99 -Wall -o bin\evaluador.exe src\expresiones\evaluador.c src\expresiones\expresiones.c
gcc -std=c99 -Wall -o bin\compilador.exe src\compilador\compilador.c src\expresiones\expresiones.c
echo Listo: bin\simpletron.exe, bin\evaluador.exe y bin\compilador.exe
