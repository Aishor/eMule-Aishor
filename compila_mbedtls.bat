@echo off
REM ============================================================
REM Compilar mbedTLS para eMule-Aishor
REM ============================================================

set MSBUILD="C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
set PLATFORM=x64
set CONFIG=Release
set PROJECT=mbedtls\visualc\VS2017\mbedTLS.vcxproj

echo.
echo ========================================
echo   Compilando mbedTLS x64 Release
echo ========================================
echo.

REM Verificar MSBuild
if not exist %MSBUILD% (
    echo [ERROR] MSBuild no encontrado
    echo Ruta esperada: %MSBUILD%
    exit /b 1
)

REM Verificar proyecto
if not exist "%PROJECT%" (
    echo [ERROR] Proyecto no encontrado: %PROJECT%
    exit /b 1
)

echo [INFO] Compilando: %PROJECT%
echo [INFO] Plataforma: %PLATFORM%
echo [INFO] Configuracion: %CONFIG%
echo.

%MSBUILD% "%PROJECT%" /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /m /v:minimal

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Compilacion de mbedTLS fallida
    exit /b 1
)

echo.
echo ========================================
echo   mbedTLS compilado exitosamente
echo ========================================
echo   Salida: mbedtls\visualc\VS2017\x64\Release\mbedTLS.lib
echo ========================================
echo.
