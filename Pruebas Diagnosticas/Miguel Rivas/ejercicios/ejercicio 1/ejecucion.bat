@echo off
echo ===================================================
echo [1/3] Compilando codigo C (ejercicio1.c)...
echo ===================================================
gcc ejercicio1.c -o ejercicio1.exe
if %errorlevel% neq 0 (
    echo Error al compilar C.
    pause
    exit /b %errorlevel%
)

echo.
echo ===================================================
echo [2/3] Ejecutando validacion y generando 'resultados_ej1.txt'...
echo ===================================================
ejercicio1.exe

echo.
echo ===================================================
echo [3/3] ¡Ejercicio 1 completado con exito!
echo Revisa tu archivo 'resultados_ej1.txt'.
echo ===================================================
pause