@echo off
REM ========================================
REM [Nombre]: run_sse.bat
REM [Propósito]: Inicia el servidor MCP SSE (Acceso Externo)
REM ========================================

echo.
echo ========================================
echo  Iniciando eMule MCP SSE Server
echo ========================================
echo.
set EMULE_API_KEY=4386
echo URL de API eMule: %EMULE_API_URL%
echo Clave API configurada: ****
echo.

REM Intentar detectar entorno en Release_Pack
if exist "%~dp0..\Release_Pack\_env\Scripts\python.exe" (
    set "PYTHON_EXE=%~dp0..\Release_Pack\_env\Scripts\python.exe"
    goto :FOUND
)

REM Intentar detectar entorno en raiz
if exist "%~dp0..\_env\Scripts\python.exe" (
    set "PYTHON_EXE=%~dp0..\_env\Scripts\python.exe"
    goto :FOUND
)

echo [ERROR] No se encontro entorno virtual Python.
pause
exit /b 1

:FOUND
echo Usando Python: %PYTHON_EXE%
"%PYTHON_EXE%" "%~dp0emule_mcp_server.py"

if errorlevel 1 (
    echo.
    echo [ERROR] El servidor se detuvo con error
    pause
)
