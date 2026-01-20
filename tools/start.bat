@echo off
REM ========================================
REM [Nombre]: start.bat
REM [Propósito]: Inicia el servidor MCP de eMule (Modo SSE)
REM [Uso]: Doble click o ejecutar desde consola
REM ========================================

echo.
echo ========================================
echo  Iniciando eMule MCP Server (SSE)
echo ========================================
echo.

REM Configuración de autenticación
set EMULE_API_KEY=4386
echo Clave API configurada: ****
echo.

REM Intentar detectar entorno en Release_Pack (Prioritario)
if exist "%~dp0..\Release_Pack\_env\Scripts\python.exe" (
    set "PYTHON_EXE=%~dp0..\Release_Pack\_env\Scripts\python.exe"
    goto :FOUND
)

REM Intentar detectar entorno en raiz (Legacy)
if exist "%~dp0..\_env\Scripts\activate.bat" (
    echo Activando entorno legacy...
    call "%~dp0..\_env\Scripts\activate.bat"
    python "%~dp0emule_mcp_server.py"
    goto :END
)

echo [ERROR] No se encontro entorno virtual Python.
echo Buscado en: ..\Release_Pack\_env y ..\_env
pause
exit /b 1

:FOUND
echo Usando Python: %PYTHON_EXE%
"%PYTHON_EXE%" "%~dp0emule_mcp_server.py"

:END
if errorlevel 1 (
    echo.
    echo [ERROR] El servidor se detuvo con error
    pause
)
