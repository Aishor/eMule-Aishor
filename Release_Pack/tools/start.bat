@echo off
REM ========================================
REM [Nombre]: start.bat
REM [Propósito]: Inicia el servidor MCP de eMule
REM [Uso]: Doble click o ejecutar desde consola
REM ========================================

echo Verificando entorno virtual...

if not exist "%~dp0..\_env\Scripts\activate.bat" (
    echo [ERROR] Entorno virtual no encontrado en ..\_env
    echo Por favor ejecuta primero: tools\install_mcp.bat
    pause
    exit /b 1
)

echo Activando entorno...
call "%~dp0..\_env\Scripts\activate.bat"

echo.
echo ========================================
echo  Iniciando eMule MCP Server
echo ========================================
echo.
echo Presiona Ctrl+C para detener el servidor
echo.

python "%~dp0emule_mcp_server.py"

if errorlevel 1 (
    echo.
    echo [ERROR] El servidor se detuvo con error
    pause
)

deactivate
