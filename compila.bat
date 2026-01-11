@echo off
REM ============================================================
REM Menu Principal de Compilacion - eMule-Aishor R1.2
REM ============================================================

:MENU
cls
echo.
echo ========================================
echo   eMule-Aishor R1.2 - Menu Principal
echo ========================================
echo.
echo   1. Descargar dependencias
echo   2. Compilar eMule (x64 Release)
echo   3. Generar paquete portable ZIP
echo   4. TODO (1+2+3)
echo   5. Limpiar build artifacts
echo   6. Salir
echo.
echo ========================================
echo.

set /p OPCION="Selecciona opcion (1-6): "

if "%OPCION%"=="1" goto DOWNLOAD_DEPS
if "%OPCION%"=="2" goto BUILD
if "%OPCION%"=="3" goto PACKAGE
if "%OPCION%"=="4" goto ALL
if "%OPCION%"=="5" goto CLEAN
if "%OPCION%"=="6" goto EXIT

echo.
echo [ERROR] Opcion invalida
timeout /t 2 >nul
goto MENU

REM ============================================================
REM Opcion 1: Descargar Dependencias
REM ============================================================
:DOWNLOAD_DEPS
cls
echo.
echo ========================================
echo   Descargando Dependencias
echo ========================================
echo.
call download_deps.bat
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Descarga fallida
    pause
    goto MENU
)
echo.
echo [OK] Dependencias listas
pause
goto MENU

REM ============================================================
REM Opcion 2: Compilar
REM ============================================================
:BUILD
cls
echo.
echo ========================================
echo   Compilando eMule x64 Release
echo ========================================
echo.
powershell -ExecutionPolicy Bypass -File build_x64.ps1
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Compilacion fallida
    pause
    goto MENU
)
echo.
echo [OK] Compilacion exitosa
pause
goto MENU

REM ============================================================
REM Opcion 3: Generar Paquete Portable
REM ============================================================
:PACKAGE
cls
echo.
echo ========================================
echo   Generando Paquete Portable
echo ========================================
echo.

if not exist "Release_Pack\emule.exe" (
    echo [ERROR] No existe Release_Pack\emule.exe
    echo Primero debes compilar (opcion 2^)
    pause
    goto MENU
)

echo Comprimiendo Release_Pack...
powershell -Command "Compress-Archive -Path Release_Pack\* -DestinationPath eMule-Aishor-R1.2-Portable.zip -Force"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] No se pudo crear el ZIP
    pause
    goto MENU
)

REM Calcular SHA256
echo.
echo Calculando SHA256...
powershell -Command "(Get-FileHash eMule-Aishor-R1.2-Portable.zip -Algorithm SHA256).Hash" > sha256.tmp
set /p SHA256=<sha256.tmp
del sha256.tmp

echo.
echo ========================================
echo   PAQUETE GENERADO
echo ========================================
echo   Archivo: eMule-Aishor-R1.2-Portable.zip
echo   SHA256:  %SHA256%
echo ========================================
echo.
pause
goto MENU

REM ============================================================
REM Opcion 4: TODO
REM ============================================================
:ALL
cls
echo.
echo ========================================
echo   Proceso Completo
echo ========================================
echo.
echo Ejecutando: Descargar + Compilar + Empaquetar
echo.
pause

REM Paso 1: Descargar
echo.
echo [PASO 1/3] Descargando dependencias...
call download_deps.bat
if %ERRORLEVEL% NEQ 0 goto ERROR_ALL

REM Paso 2: Compilar
echo.
echo [PASO 2/3] Compilando...
powershell -ExecutionPolicy Bypass -File build_x64.ps1
if %ERRORLEVEL% NEQ 0 goto ERROR_ALL

REM Paso 3: Empaquetar
echo.
echo [PASO 3/3] Empaquetando...
powershell -Command "Compress-Archive -Path Release_Pack\* -DestinationPath eMule-Aishor-R1.2-Portable.zip -Force"
if %ERRORLEVEL% NEQ 0 goto ERROR_ALL

echo.
echo ========================================
echo   PROCESO COMPLETO EXITOSO
echo ========================================
pause
goto MENU

:ERROR_ALL
echo.
echo [ERROR] El proceso fallo
pause
goto MENU

REM ============================================================
REM Opcion 5: Limpiar
REM ============================================================
:CLEAN
cls
echo.
echo ========================================
echo   Limpiando Build Artifacts
echo ========================================
echo.

if exist "srchybrid\x64\Release" (
    echo Eliminando srchybrid\x64\Release...
    rmdir /s /q srchybrid\x64\Release
)

if exist "Release_Pack\emule.exe" (
    echo Eliminando Release_Pack\emule.exe...
    del /f /q Release_Pack\emule.exe
)

if exist "*.zip" (
    echo Eliminando archivos ZIP...
    del /f /q *.zip
)

echo.
echo [OK] Limpieza completada
pause
goto MENU

REM ============================================================
REM Salir
REM ============================================================
:EXIT
echo.
echo Saliendo...
exit /b 0
