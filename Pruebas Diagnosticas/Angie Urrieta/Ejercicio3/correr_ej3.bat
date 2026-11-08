@echo off
where gcc >nul 2>nul
if errorlevel 1 set "PATH=C:\Program Files\CodeBlocks\MinGW\bin;%PATH%"
where gcc >nul 2>nul
if errorlevel 1 ( echo No se encontro gcc. Instala Code::Blocks o agrega su carpeta bin al PATH. & pause & exit /b )
cd /d "%~dp0Ejercicio3"
gcc ejercicio3.c -o "%TEMP%\ej3_angie.exe"
if not exist "%TEMP%\ej3_angie.exe" ( echo Error al compilar ejercicio3.c & pause & exit /b )
"%TEMP%\ej3_angie.exe"
echo.
pause