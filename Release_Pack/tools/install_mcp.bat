@echo off
REM ========================================
REM [Nombre]: install_mcp.bat
REM [Propósito]: Instala dependencias Python MCP en entorno virtual aislado
REM [Uso]: install_mcp.bat
REM [Requisitos]: Python 3.11+ y pip instalados
REM ========================================

echo ========================================
echo  eMule-Aishor MCP Server - Instalacion
echo ========================================
echo.

REM Verificar que Python está instalado
python --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python no esta instalado o no esta en el PATH
    echo Por favor instala Python 3.11+ desde python.org
    pause
    exit /b 1
)

echo [OK] Python encontrado:
python --version
echo.

REM Verificar que pip está instalado
pip --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] pip no esta instalado
    echo Por favor instala pip
    pause
    exit /b 1
)

echo [OK] pip encontrado:
pip --version
echo.

REM Crear entorno virtual si no existe
if not exist "%~dp0..\_env" (
    echo Creando entorno virtual en _env...
    python -m venv "%~dp0..\_env"
    if errorlevel 1 (
        echo [ERROR] Fallo al crear entorno virtual
        pause
        exit /b 1
    )
    echo [OK] Entorno virtual creado
    echo.
) else (
    echo [OK] Entorno virtual ya existe
    echo.
)

REM Activar entorno virtual
echo Activando entorno virtual...
call "%~dp0..\_env\Scripts\activate.bat"
if errorlevel 1 (
    echo [ERROR] Fallo al activar entorno virtual
    pause
    exit /b 1
)

echo [OK] Entorno virtual activado
echo.

REM Actualizar pip en el entorno virtual
echo Actualizando pip en entorno virtual...
python -m pip install --upgrade pip

echo.
echo Instalando dependencias MCP...
echo.
pip install -r "%~dp0requirements_mcp.txt"

if errorlevel 1 (
    echo.
    echo [ERROR] Fallo la instalacion de dependencias
    pause
    exit /b 1
)

echo.
echo ========================================
echo  Instalacion completada exitosamente!
echo ========================================
echo.
echo IMPORTANTE: Las dependencias MCP estan instaladas en el entorno virtual:
echo   %~dp0..\_env
echo.
echo Proximos pasos:
echo 1. Asegurate de que eMule esta ejecutandose con LLMApiServer activo (puerto 4711)
echo 2. Para ejecutar el MCP server, usa:
echo    _env\Scripts\python.exe tools\emule_mcp_server.py
echo 3. Para configurar CHAMAN, consulta: tools\CHAMAN_MCP_CONFIG.md
echo.
pause
