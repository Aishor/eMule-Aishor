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

set /a TOTAL=7
set /a CURRENT=0

REM ResizableLib
set /a CURRENT+=1
if not exist "resizablelib\ResizableLib\ResizableDialog.h" (
    echo [%CURRENT%/%TOTAL%] Descargando ResizableLib...
    if exist "resizablelib" rmdir /s /q resizablelib 2>nul
    git clone --depth 1 https://github.com/ppescher/resizablelib.git resizablelib_tmp >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo   [ERROR] Fallo en descarga
    ) else (
        mkdir resizablelib 2>nul
        xcopy resizablelib_tmp\* resizablelib\ /E /I /Y /Q >nul
        rmdir /s /q resizablelib_tmp
        echo   + OK
    )
) else (
    echo [%CURRENT%/%TOTAL%] ResizableLib - ya existe
)

REM Crypto++
set /a CURRENT+=1
if not exist "cryptopp\cryptlib.h" (
    echo [%CURRENT%/%TOTAL%] Descargando Crypto++...
    if exist "cryptopp" rmdir /s /q cryptopp 2>nul
    git clone --depth 1 https://github.com/weidai11/cryptopp.git cryptopp_tmp >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo   [ERROR] Fallo en descarga
    ) else (
        mkdir cryptopp 2>nul
        xcopy cryptopp_tmp\* cryptopp\ /E /I /Y /Q >nul
        rmdir /s /q cryptopp_tmp
        echo   + OK
    )
) else (
    echo [%CURRENT%/%TOTAL%] Crypto++ - ya existe
)

REM zlib
set /a CURRENT+=1
if not exist "zlib\zlib.h" (
    echo [%CURRENT%/%TOTAL%] Descargando zlib...
    if exist "zlib" rmdir /s /q zlib 2>nul
    git clone --depth 1 https://github.com/madler/zlib.git zlib_tmp >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo   [ERROR] Fallo en descarga
    ) else (
        mkdir zlib 2>nul
        xcopy zlib_tmp\* zlib\ /E /I /Y /Q >nul
        rmdir /s /q zlib_tmp
        echo   + OK
    )
) else (
    echo [%CURRENT%/%TOTAL%] zlib - ya existe
)

REM libpng
set /a CURRENT+=1
if not exist "libpng\png.h" (
    echo [%CURRENT%/%TOTAL%] Descargando libpng...
    if exist "libpng" rmdir /s /q libpng 2>nul
    git clone --depth 1 https://github.com/glennrp/libpng.git libpng_tmp >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo   [ERROR] Fallo en descarga
    ) else (
        mkdir libpng 2>nul
        xcopy libpng_tmp\* libpng\ /E /I /Y /Q >nul
        rmdir /s /q libpng_tmp
        echo   + OK
    )
) else (
    echo [%CURRENT%/%TOTAL%] libpng - ya existe
)

REM mbedTLS
set /a CURRENT+=1
if not exist "mbedtls\include\mbedtls\ssl.h" (
    echo [%CURRENT%/%TOTAL%] Descargando mbedTLS...
    if exist "mbedtls" rmdir /s /q mbedtls 2>nul
    git clone --depth 1 --branch v2.28.9 https://github.com/Mbed-TLS/mbedtls.git mbedtls_tmp >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo   [ERROR] Fallo en descarga
    ) else (
        mkdir mbedtls 2>nul
        xcopy mbedtls_tmp\* mbedtls\ /E /I /Y /Q >nul
        rmdir /s /q mbedtls_tmp
        echo   + OK
    )
) else (
    echo [%CURRENT%/%TOTAL%] mbedTLS - ya existe
)

REM miniupnpc
set /a CURRENT+=1
if not exist "miniupnpc\miniupnpc.h" (
    echo [%CURRENT%/%TOTAL%] Descargando miniupnpc...
    if exist "miniupnpc" rmdir /s /q miniupnpc 2>nul
    git clone --depth 1 https://github.com/miniupnp/miniupnp.git miniupnp_tmp >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo   [ERROR] Fallo en descarga
    ) else (
        mkdir miniupnpc 2>nul
        xcopy miniupnp_tmp\miniupnpc\* miniupnpc\ /E /I /Y /Q >nul
        rmdir /s /q miniupnp_tmp
        echo   + OK
    )
) else (
    echo [%CURRENT%/%TOTAL%] miniupnpc - ya existe
)

REM cximage (mantener estructura existente)
set /a CURRENT+=1
if not exist "cximage\ximage.h" (
    echo [%CURRENT%/%TOTAL%] cximage - incluida en repo
) else (
    echo [%CURRENT%/%TOTAL%] cximage - ya existe  
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
