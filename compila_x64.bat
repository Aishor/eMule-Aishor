@echo off
REM ========================================
REM PROYECTO R1.2 TITANIUM FIBER - BUILD x64
REM Compilacion automatizada x64 Release
REM ========================================

echo.
echo ========================================
echo   PROYECTO R1.2 "TITANIUM FIBER"
echo   Compilacion x64 Release Optimizada
echo ========================================
echo.

REM Establecer entorno
set PLATFORM=x64
set CONFIGURATION=Release
set BUILD_LOG=compilation_log_x64.txt

REM Verificar MSBuild con vswhere
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
    for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
        set "MSBUILD_PATH=%%i"
    )
)

if not defined MSBUILD_PATH (
    where msbuild >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        set "MSBUILD_PATH=msbuild"
    ) else (
        echo [ERROR] MSBuild no encontrado. Ejecuta desde Developer Command Prompt de VS2022 o instala Visual Studio.
        pause
        exit /b 1
    )
)

echo [INFO] MSBuild encontrado en: "%MSBUILD_PATH%"

REM Limpiar log anterior
if exist "%BUILD_LOG%" del /f /q "%BUILD_LOG%"

echo [INFO] Plataforma: %PLATFORM%
echo [INFO] Configuracion: %CONFIGURATION%
echo [INFO] Log: %BUILD_LOG%
echo.

REM ========================================
REM PASO 1: Compilar Dependencias (SALTADO - Usando Libs Precompiladas)
REM ========================================

echo ========================================
echo   PASO 1: Dependencias x64 (Pre-compiladas)
echo ========================================
echo.

REM call :BUILD_PROJECT "zlib\contrib\vstudio\vc17\zlibvc.sln" "zlibstat" "zlib"
REM if %ERRORLEVEL% NEQ 0 goto :ERROR

REM call :BUILD_PROJECT "mbedtls\visualc\VS2017\mbedTLS.sln" "mbedTLS" "mbedTLS"
REM if %ERRORLEVEL% NEQ 0 goto :ERROR

REM call :BUILD_PROJECT "cryptopp\cryptlib.vcxproj" "cryptlib" "CryptoPP"
REM if %ERRORLEVEL% NEQ 0 goto :ERROR

call :BUILD_PROJECT "libpng\projects\vstudio\libpng\libpng.vcxproj" "Rebuild" "libpng"
if %ERRORLEVEL% NEQ 0 goto :ERROR

call :BUILD_PROJECT "cximage\cximage.vcxproj" "Rebuild" "CxImage"
if %ERRORLEVEL% NEQ 0 goto :ERROR

REM call :BUILD_PROJECT "id3lib\libprj\id3lib.vcxproj" "id3lib" "id3lib"
REM if %ERRORLEVEL% NEQ 0 goto :ERROR

REM call :BUILD_PROJECT "resizablelib\ResizableLib\ResizableLib.vcxproj" "ResizableLib" "ResizableLib"
REM if %ERRORLEVEL% NEQ 0 goto :ERROR

REM call :BUILD_PROJECT "miniupnpc\msvc\miniupnpc.vcxproj" "miniupnpc" "miniupnpc"
REM if %ERRORLEVEL% NEQ 0 goto :ERROR

REM ========================================
REM PASO 2: Compilar Proyecto Principal
REM ========================================

echo.
echo ========================================
echo   PASO 2: Compilando eMule Principal
echo ========================================
echo.

call :BUILD_PROJECT "srchybrid\emule.sln" "emule" "eMule-Aishor"
if %ERRORLEVEL% NEQ 0 goto :ERROR

REM ========================================
REM PASO 3: Verificacion Final
REM ========================================

echo.
echo ========================================
echo   PASO 3: Verificacion Final
echo ========================================
echo.

if exist "srchybrid\x64\Release\emule.exe" (
    echo [OK] Ejecutable generado: srchybrid\x64\Release\emule.exe
    
    REM Obtener tamaño del archivo
    for %%A in ("srchybrid\x64\Release\emule.exe") do (
        set size=%%~zA
    )
    echo [INFO] Tamaño del ejecutable: !size! bytes
    
    REM Verificar arquitectura con dumpbin si esta disponible
    where dumpbin >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo [INFO] Verificando arquitectura...
        dumpbin /headers "srchybrid\x64\Release\emule.exe" | findstr "machine"
    )
) else (
    echo [ERROR] No se encontro el ejecutable final.
    goto :ERROR
)

echo.
echo ========================================
echo   BUILD COMPLETADO EXITOSAMENTE
echo ========================================
echo   Ejecutable: srchybrid\x64\Release\emule.exe
echo   Log completo: %BUILD_LOG%
echo ========================================
echo.
pause
exit /b 0

REM ========================================
REM Funciones Auxiliares
REM ========================================

:BUILD_PROJECT
setlocal
set "PROJECT_PATH=%~1"
set "PROJECT_NAME=%~2"
set "DISPLAY_NAME=%~3"

echo [BUILD] %DISPLAY_NAME%...
echo ----------------------------------------

"%MSBUILD_PATH%" "%PROJECT_PATH%" ^
    /p:Configuration=%CONFIGURATION% ^
    /p:Platform=%PLATFORM% ^
    /t:%PROJECT_NAME% ^
    /m ^
    /v:minimal ^
    /fl ^
    /flp:logfile=%BUILD_LOG%;append=true

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Fallo en compilacion de %DISPLAY_NAME%
    echo [ERROR] Revisa el log: %BUILD_LOG%
    exit /b 1
)

echo [OK] %DISPLAY_NAME% compilado correctamente
echo.
endlocal
exit /b 0

:ERROR
echo.
echo ========================================
echo   BUILD FALLIDO
echo ========================================
echo   Revisa el log: %BUILD_LOG%
echo ========================================
echo.
pause
exit /b 1
