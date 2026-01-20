@echo off
REM ========================================================================
REM FiberSight R1.3 - Instalador de Dependencias Python
REM ========================================================================
echo.
echo ========================================================================
echo  FiberSight R1.3 - Instalador de Dependencias Python
echo ========================================================================
echo.
REM Verificar que Python est instalado
python --version >nul 2>&1
if errorlevel 1 (
    echo [INFO] Python no encontrado en PATH. Intentando instalar automaticamente...
    echo.
    winget install -e --id Python.Python.3.11
    if errorlevel 1 (
         echo [ERROR] Fallo la instalacion automatica.
         pause
         exit /b 1
    )
    call refreshenv.cmd >nul 2>&1
)
echo [OK] Python encontrado
if not exist "_env" (
    echo [1/4] Creando entorno virtual en _env...
    python -m venv _env
    if errorlevel 1 exit /b 1
)
call _env\Scripts\activate.bat
python -m pip install --upgrade pip --quiet
echo [4/4] Instalando dependencias...
pip install -r requirements.txt --quiet
pip install -r requirements_mcp.txt --quiet
pip install httpx anthropic --quiet
echo.
echo INSTALACION COMPLETADA
pause
