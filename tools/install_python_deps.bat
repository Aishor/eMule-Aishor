@echo off
REM ========================================================================
REM FiberSight R1.3 - Instalador de Dependencias Python
REM Crea entorno virtual e instala todas las dependencias necesarias
REM ========================================================================

echo.
echo ========================================================================
echo  FiberSight R1.3 - Instalador de Dependencias Python
echo ========================================================================
echo.

REM Verificar que Python está instalado
python --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python no encontrado en PATH
    echo.
    echo Por favor instala Python 3.8 o superior desde:
    echo https://www.python.org/downloads/
    echo.
    pause
    exit /b 1
)

echo [OK] Python encontrado
python --version
echo.

REM Crear directorio para entorno virtual si no existe
if not exist "_env" (
    echo [1/4] Creando entorno virtual en _env...
    python -m venv _env
    if errorlevel 1 (
        echo [ERROR] No se pudo crear el entorno virtual
        echo.
        echo Asegurate de tener el modulo venv instalado:
        echo python -m pip install --upgrade pip
        pause
        exit /b 1
    )
    echo [OK] Entorno virtual creado
) else (
    echo [INFO] Entorno virtual ya existe en _env
)
echo.

REM Activar entorno virtual
echo [2/4] Activando entorno virtual...
call _env\Scripts\activate.bat
if errorlevel 1 (
    echo [ERROR] No se pudo activar el entorno virtual
    pause
    exit /b 1
)
echo [OK] Entorno virtual activado
echo.

REM Actualizar pip
echo [3/4] Actualizando pip...
python -m pip install --upgrade pip --quiet
echo [OK] pip actualizado
echo.

REM Instalar dependencias
echo [4/4] Instalando dependencias...
echo.

echo  - Instalando Vision Auditor dependencies...
pip install -r requirements.txt --quiet
if errorlevel 1 (
    echo [ERROR] Fallo instalando Vision Auditor dependencies
    pause
    exit /b 1
)
echo    [OK] Vision Auditor dependencies instaladas

echo  - Instalando MCP Server dependencies...
pip install -r requirements_mcp.txt --quiet
if errorlevel 1 (
    echo [ERROR] Fallo instalando MCP Server dependencies
    pause
    exit /b 1
)
echo    [OK] MCP Server dependencies instaladas

echo  - Instalando Orchestrator dependencies...
pip install httpx anthropic --quiet
if errorlevel 1 (
    echo [ERROR] Fallo instalando Orchestrator dependencies
    pause
    exit /b 1
)
echo    [OK] Orchestrator dependencies instaladas

echo.
echo ========================================================================
echo  INSTALACION COMPLETADA EXITOSAMENTE
echo ========================================================================
echo.
echo Entorno virtual creado en: _env\
echo.
echo Para activar el entorno virtual en el futuro, ejecuta:
echo   _env\Scripts\activate.bat
echo.
echo O usa los scripts de inicio:
echo   - start_vision_auditor.bat
echo   - start_orchestrator.bat
echo.
echo Dependencias instaladas:
echo   - requests, anthropic, httpx
echo   - mcp, mcp-cli
echo   - Todas las dependencias de Vision Auditor
echo.
pause
