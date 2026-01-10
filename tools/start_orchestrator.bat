@echo off
REM ========================================================================
REM FiberSight R1.3 - Iniciar Orchestrator Agent
REM Activa entorno virtual e inicia el agente orquestador
REM ========================================================================

echo.
echo ========================================================================
echo  FiberSight R1.3 - Orchestrator Agent
echo ========================================================================
echo.

REM Verificar entorno virtual
if not exist "_env\Scripts\activate.bat" (
    echo [ERROR] Entorno virtual no encontrado
    echo.
    echo Por favor ejecuta primero: install_python_deps.bat
    echo.
    pause
    exit /b 1
)

REM Activar entorno virtual
call _env\Scripts\activate.bat

REM Verificar variables de entorno
if "%EMULE_API_KEY%"=="" (
    echo [WARNING] Variable EMULE_API_KEY no configurada
    echo.
    set /p EMULE_API_KEY="Ingresa tu eMule API Key: "
)

if "%ANTHROPIC_API_KEY%"=="" (
    echo [WARNING] Variable ANTHROPIC_API_KEY no configurada
    echo.
    set /p ANTHROPIC_API_KEY="Ingresa tu Anthropic API Key: "
)

echo.
echo [INFO] Configuracion:
echo   - eMule API: %EMULE_API_KEY:~0,8%...
echo   - Anthropic API: %ANTHROPIC_API_KEY:~0,8%...
echo   - Base de datos: orchestrator.db
echo.
echo Iniciando Orchestrator Agent...
echo.

REM Exportar variables
set EMULE_API_URL=http://localhost:4711/api/v1

REM Iniciar Orchestrator
python orchestrator_agent.py

pause
