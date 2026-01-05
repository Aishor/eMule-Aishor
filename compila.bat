@echo off
REM ============================================================
REM Script de Compilación - eMule v0.70b-Build26-R1.0
REM ============================================================
REM Este script compila el proyecto eMule usando MSBuild
REM Asegúrate de tener Visual Studio 2022 instalado
REM ============================================================

setlocal EnableDelayedExpansion

echo.
echo ========================================
echo   eMule v0.70b-Build26-R1.0
echo   Script de Compilacion
echo ========================================
echo.

REM Configuración
set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
set "PROJECT_FILE=srchybrid\emule.vcxproj"
set "CONFIGURATION=Release"
set "PLATFORM=Win32"
set "SDK_VERSION=10.0.26100.0"

REM Verificar que existe MSBuild
if not exist "%MSBUILD_PATH%" (
    echo ERROR: No se encuentra MSBuild.exe
    echo Ruta esperada: %MSBUILD_PATH%
    echo.
    echo Verifica que Visual Studio 2022 este instalado correctamente.
    pause
    exit /b 1
)

REM Verificar que existe el proyecto
if not exist "%PROJECT_FILE%" (
    echo ERROR: No se encuentra el archivo del proyecto
    echo Ruta esperada: %PROJECT_FILE%
    echo.
    echo Asegurate de ejecutar este script desde la raiz del repositorio.
    pause
    exit /b 1
)

echo [1/3] Verificando entorno...
echo   - MSBuild: OK
echo   - Proyecto: OK
echo.

REM Preguntar tipo de compilación
echo [2/3] Selecciona el tipo de compilacion:
echo   1. Build (Compilacion incremental - rapida)
echo   2. Rebuild (Compilacion completa - recomendada)
echo   3. Clean (Limpiar archivos de compilacion)
echo.
set /p "BUILD_TYPE=Opcion (1-3): "

if "%BUILD_TYPE%"=="1" (
    set "BUILD_TARGET=Build"
    echo.
    echo Ejecutando compilacion incremental...
) else if "%BUILD_TYPE%"=="2" (
    set "BUILD_TARGET=Rebuild"
    echo.
    echo Ejecutando compilacion completa...
) else if "%BUILD_TYPE%"=="3" (
    set "BUILD_TARGET=Clean"
    echo.
    echo Limpiando archivos de compilacion...
) else (
    echo.
    echo Opcion invalida. Usando Build por defecto.
    set "BUILD_TARGET=Build"
)

echo.
echo [3/3] Compilando...
echo.

REM Ejecutar MSBuild
"%MSBUILD_PATH%" "%PROJECT_FILE%" ^
  /t:%BUILD_TARGET% ^
  /p:Configuration=%CONFIGURATION% ^
  /p:Platform=%PLATFORM% ^
  /p:WindowsTargetPlatformVersion=%SDK_VERSION% ^
  /v:minimal

REM Verificar resultado
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   COMPILACION EXITOSA
    echo ========================================
    echo.
    if exist "srchybrid\Win32\Release\emule.exe" (
        echo Ejecutable generado:
        echo   srchybrid\Win32\Release\emule.exe
        echo.
        for %%F in ("srchybrid\Win32\Release\emule.exe") do (
            echo Tamano: %%~zF bytes
            echo Fecha: %%~tF
        )
    )
    echo.
    echo Presiona cualquier tecla para salir...
    pause >nul
    exit /b 0
) else (
    echo.
    echo ========================================
    echo   ERROR EN LA COMPILACION
    echo ========================================
    echo.
    echo Codigo de error: %ERRORLEVEL%
    echo.
    echo Revisa los mensajes de error arriba para mas detalles.
    echo.
    echo Presiona cualquier tecla para salir...
    pause >nul
    exit /b %ERRORLEVEL%
)
