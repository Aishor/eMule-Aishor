@echo off
REM ========================================================================
REM FiberSight R1.3 - Activar Entorno Virtual Python
REM Activa el entorno virtual _env para usar los componentes Python
REM ========================================================================

if not exist "_env\Scripts\activate.bat" (
    echo [ERROR] Entorno virtual no encontrado en _env\
    echo.
    echo Por favor ejecuta primero: install_python_deps.bat
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================================================
echo  Activando Entorno Virtual FiberSight
echo ========================================================================
echo.

call _env\Scripts\activate.bat

echo.
echo [OK] Entorno virtual activado
echo.
echo Ahora puedes ejecutar:
echo   - python vision_auditor_v2.py
echo   - python emule_mcp_server.py
echo   - python orchestrator_agent.py
echo.
echo Para desactivar el entorno, escribe: deactivate
echo.
