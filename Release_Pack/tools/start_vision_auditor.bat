@echo off
REM ========================================================================
REM FiberSight R1.3 - Iniciar Vision Auditor V2
REM Activa entorno virtual e inicia Vision Auditor en modo Observer
REM ========================================================================

echo.
echo ========================================================================
echo  FiberSight R1.3 - Vision Auditor V2
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
echo   - Modo: Observer (solo logging, no acciones)
echo.
echo Iniciando Vision Auditor V2...
echo.

REM Iniciar Vision Auditor
python vision_auditor_v2.py ^
    --api-key "%EMULE_API_KEY%" ^
    --anthropic-key "%ANTHROPIC_API_KEY%" ^
    --mode observer ^
    --interval 300

pause
