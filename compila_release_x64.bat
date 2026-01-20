@echo off
setlocal
echo ============================================================
echo   Compilacion Completa de eMule-Aishor (Release x64)
echo ============================================================
echo.

set MSBUILD="C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"

IF NOT EXIST %MSBUILD% (
    echo [ERROR] MSBuild no encontrado en: %MSBUILD%
    
    exit /b 1
)

echo [1/4] Compilando mbedTLS...
call compila_mbedtls.bat
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Fallo en mbedTLS
    
    exit /b 1
)

echo.
echo [2/4] Compilando ResizableLib...
if exist "resizablelib\ResizableLib\x64\Release\bin\ResizableLib.lib" del "resizablelib\ResizableLib\x64\Release\bin\ResizableLib.lib"
if exist "resizablelib\x64\Release\resizablelib.lib" del "resizablelib\x64\Release\resizablelib.lib"
%MSBUILD% "resizablelib\ResizableLib\ResizableLib.vcxproj" /p:Configuration=Release /p:Platform=x64 /t:Rebuild /m /v:minimal
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Fallo en ResizableLib
    
    exit /b 1
)

echo [INFO] Copiando ResizableLib.lib a la ruta esperada...
if not exist "resizablelib\x64\Release" mkdir "resizablelib\x64\Release"
copy "resizablelib\ResizableLib\x64\Release\bin\ResizableLib.lib" "resizablelib\x64\Release\resizablelib.lib" /Y
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Fallo al copiar ResizableLib.lib
    exit /b 1
)

echo.
echo [3/4] Verificando otras librerias estaticas...
if not exist "zlib\contrib\vstudio\vc\x64\Release\zlib.lib" (
    echo [WARNING] zlib.lib no encontrada en zlib\contrib\vstudio\vc\x64\Release\zlib.lib
    echo Asegurate de que zlib este compilada.
) else (
    echo   + zlib.lib encontrada.
)

if not exist "cryptopp\x64\Release\cryptlib.lib" (
    echo [WARNING] cryptlib.lib no encontrada en cryptopp\x64\Release\cryptlib.lib
    echo Asegurate de que cryptopp este compilada.
) else (
    echo   + cryptlib.lib encontrada.
)

echo.
echo [4/4] Compilando eMule...
taskkill /F /IM mspdbsrv.exe >nul 2>&1
if exist "srchybrid\x64\Release" rd /s /q "srchybrid\x64\Release"
%MSBUILD% "srchybrid\emule.vcxproj" /p:Configuration=Release /p:Platform=x64 /t:Rebuild /m /v:minimal /fl /flp:LogFile=build_log.txt;Verbosity=normal
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Fallo al compilar eMule. Revisa build_log.txt
    
    exit /b 1
)

echo.
echo ============================================================
echo   COMPILACION EXITOSA
echo ============================================================
echo.
pause
