@echo off
REM ========================================
REM [Nombre]: activate_mcp_env.bat
REM [Propósito]: Activa el entorno virtual MCP para desarrollo/debug
REM [Uso]: activate_mcp_env.bat
REM ========================================

echo Activando entorno virtual MCP...

if not exist "%~dp0..\_env\Scripts\activate.bat" (
    echo [ERROR] Entorno virtual no encontrado en _env
    echo Por favor ejecuta primero: tools\install_mcp.bat
    pause
    exit /b 1
)

call "%~dp0..\_env\Scripts\activate.bat"
python emule_mcp_server.py
echo.
echo ========================================
echo  Entorno virtual MCP activado
echo ========================================
echo.
echo Python: %~dp0..\_env\Scripts\python.exe
echo.
echo Para ejecutar el MCP server:
echo   python emule_mcp_server.py
echo.
echo Para salir del entorno virtual:
echo   deactivate
echo.
