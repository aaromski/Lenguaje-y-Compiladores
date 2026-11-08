@echo off
echo ===================================================
echo [1/4] Compilando codigo C (ejercicio2.c)...
echo ===================================================
gcc ejercicio2.c -o ejercicio2.exe
if %errorlevel% neq 0 (
    echo Error al compilar C.
    pause
    exit /b %errorlevel%
)

echo.
echo ===================================================
echo [2/4] Parametros de entrada (Se aplican a C y Python)
echo ===================================================
set /p n_val="Ingrese el grado del binomio (n >= 0): "
set /p x_val="Ingrese el valor de x para evaluar f(x): "

echo.
echo ===================================================
echo [3/4] Ejecutando version en C...
echo ===================================================
ejercicio2.exe %n_val% %x_val%

echo.
echo ===================================================
echo [4/4] Ejecutando version en Python...
echo ===================================================
python ejercicio2.py %n_val% %x_val%

echo.
echo ===================================================
echo ¡Ejercicio 2 completado con exito!
echo Revisa el archivo 'tiempos_ej2.txt' con los benchmarks.
echo ===================================================
pause