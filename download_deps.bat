@echo off
REM ============================================================
REM Script de Descarga de Dependencias - eMule-Aishor R1.2
REM ============================================================
REM Descarga el código fuente de librerías externas necesarias
REM para compilar eMule-Aishor desde cero.
REM
REM NOTA: Las librerías precompiladas (.lib) YA están incluidas
REM       en el repositorio. Este script solo descarga el código
REM       fuente si necesitas recompilar las dependencias.
REM ============================================================

echo.
echo ========================================
echo   eMule-Aishor - Descarga Dependencias
echo ========================================
echo.

REM Verificar que Git está disponible
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Git no encontrado. Instala Git para Windows.
    pause
    exit /b 1
)

REM ResizableLib
if not exist "resizablelib\ResizableLib\ResizableDialog.h" (
    echo [1/1] Descargando ResizableLib...
    
    REM Si existe directorio parcial, eliminarlo primero
    if exist "resizablelib\ResizableLib" (
        echo   Limpiando estructura parcial...
        rmdir /s /q resizablelib 2>nul
    )
    
    git clone --depth 1 --branch master https://github.com/ppescher/resizablelib.git resizablelib_tmp
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] No se pudo descargar ResizableLib
        pause
        exit /b 1
    )
    
    REM Crear estructura y mover solo lo necesario
    mkdir resizablelib 2>nul
    xcopy resizablelib_tmp\* resizablelib\ /E /I /Y /Q
    rmdir /s /q resizablelib_tmp
    
    echo   + ResizableLib descargada
) else (
    echo [1/1] ResizableLib ya existe (omitiendo)
)

echo.
echo ========================================
echo   DEPENDENCIAS LISTAS
echo ========================================
echo.
echo NOTA: Las librerias compiladas (.lib) ya
echo       estan incluidas en el repositorio.
echo.
echo Para compilar eMule ahora ejecuta:
echo   build_x64.ps1
echo.
pause
