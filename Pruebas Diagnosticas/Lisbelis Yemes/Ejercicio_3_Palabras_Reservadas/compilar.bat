@echo off
setlocal
cd /d "%~dp0"

where gcc >nul 2>&1
if errorlevel 1 (
    echo No se encontro GCC en el PATH.
    where winget >nul 2>&1
    if errorlevel 1 (
        echo.
        echo No se encontro winget para instalar GCC automaticamente.
        echo Instala MSYS2 desde https://www.msys2.org/ y vuelve a ejecutar este archivo.
        echo Despues, desde MSYS2 UCRT64, ejecuta:
        echo pacman -S --needed mingw-w64-ucrt-x86_64-gcc
        echo y agrega C:\msys64\ucrt64\bin al PATH de Windows.
        pause
        exit /b 1
    )

    echo Instalando MSYS2 mediante winget...
    winget install --id MSYS2.MSYS2 --exact --source winget --accept-source-agreements --accept-package-agreements
    if errorlevel 1 (
        echo No fue posible instalar MSYS2. Ejecuta este archivo nuevamente despues de instalarlo.
        pause
        exit /b 1
    )
)

if exist "C:\msys64\usr\bin\bash.exe" (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
    where gcc >nul 2>&1
    if errorlevel 1 (
        echo Instalando GCC dentro de MSYS2...
        "C:\msys64\usr\bin\bash.exe" -lc "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc"
        set "PATH=C:\msys64\ucrt64\bin;%PATH%"
    )
)

where gcc >nul 2>&1
if errorlevel 1 (
    echo GCC sigue sin estar disponible. Reinicia la terminal y vuelve a ejecutar este archivo.
    pause
    exit /b 1
)

echo Compilando Ejercicio 3 (Lenguaje C)...
gcc ejercicio3.c -o ejercicio3.exe
if errorlevel 1 (
    echo.
    echo Error en la compilacion.
    pause
    exit /b 1
)
echo.
if exist ejercicio3.exe (
    echo Compilacion exitosa. Ejecutando programa...
    echo.
    ejercicio3.exe
) else (
    echo Error en la compilacion.
)
pause
endlocal