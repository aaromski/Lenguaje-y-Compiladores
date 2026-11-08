@echo off
echo ===================================================
echo [1/3] Compilando codigo C (ejercicio3.c)...
echo ===================================================
gcc ejercicio3.c -o ejercicio3.exe
if %errorlevel% neq 0 (
    echo Error al compilar C.
    pause
    exit /b %errorlevel%
)

echo.
echo ===================================================
echo [2/3] Ejecutando traduccion y generando 'resultados_ej3.txt'...
echo ===================================================
ejercicio3.exe

echo.
echo ===================================================
echo [3/3] ¡Ejercicio 3 completado con exito!
echo Revisa tu archivo 'resultados_ej3.txt'.
echo ===================================================
pause