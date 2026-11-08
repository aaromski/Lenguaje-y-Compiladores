@echo off
where gcc >nul 2>nul
if errorlevel 1 set "PATH=C:\Program Files\CodeBlocks\MinGW\bin;%PATH%"
where gcc >nul 2>nul
if errorlevel 1 ( echo No se encontro gcc. Instala Code::Blocks o agrega su carpeta bin al PATH. & pause & exit /b )
cd /d "%~dp0Ejercicio2"
gcc ejercicio2.c -o "%TEMP%\ej2_angie.exe"
if not exist "%TEMP%\ej2_angie.exe" ( echo Error al compilar ejercicio2.c & pause & exit /b )
echo (cuando pregunte: n=5, x=2, luego ENTER vacio para el benchmark)
"%TEMP%\ej2_angie.exe"
echo.
pause