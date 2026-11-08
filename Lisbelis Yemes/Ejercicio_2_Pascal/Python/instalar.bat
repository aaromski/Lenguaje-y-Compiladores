@echo off
setlocal
cd /d "%~dp0"

where python >nul 2>&1
if errorlevel 1 (
    echo Python no esta instalado.
    where winget >nul 2>&1
    if errorlevel 1 (
        echo Instala Python desde https://www.python.org/downloads/ y vuelve a ejecutar este archivo.
        pause
        exit /b 1
    )
    echo Instalando Python...
    winget install --id Python.Python.3.12 --exact --source winget --accept-source-agreements --accept-package-agreements
    if errorlevel 1 (
        echo No fue posible instalar Python.
        pause
        exit /b 1
    )
)

echo Verificando instalacion de Python...
python --version
if errorlevel 1 (
    echo Reinicia la terminal y vuelve a ejecutar este archivo.
    pause
    exit /b 1
)
echo.
echo Instalando dependencias del proyecto...
python -m pip install -r requerimientos.txt
if errorlevel 1 (
    echo No fue posible instalar las dependencias.
    pause
    exit /b 1
)
echo.
echo Entorno listo. Ejecutando el Ejercicio 2 en Python...
python ejercicio2.py
pause
endlocal