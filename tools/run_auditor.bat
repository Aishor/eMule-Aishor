@echo off
REM Script de ejemplo para ejecutar Vision Auditor en Windows
REM eMule-Aishor R1.3 - Vision Verification

echo ========================================
echo Vision Auditor - eMule-Aishor R1.3
echo ========================================
echo.

REM Configuración
set EMULE_API_KEY=YOUR_EMULE_API_KEY_HERE
set ANTHROPIC_KEY=YOUR_ANTHROPIC_API_KEY_HERE
set INTERVAL=300

REM Verificar que FFmpeg esté instalado
where ffmpeg >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: FFmpeg no encontrado en PATH
    echo.
    echo Instalar FFmpeg:
    echo   choco install ffmpeg
    echo   O descargar desde https://ffmpeg.org/download.html
    pause
    exit /b 1
)

echo FFmpeg encontrado: OK
echo.

REM Verificar que Python esté instalado
where python >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Python no encontrado en PATH
    pause
    exit /b 1
)

echo Python encontrado: OK
echo.

REM Instalar dependencias si es necesario
echo Verificando dependencias Python...
python -c "import requests" 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Instalando dependencias...
    pip install -r requirements.txt
)

echo.
echo Iniciando Vision Auditor...
echo Intervalo: %INTERVAL% segundos
echo.
echo Presiona Ctrl+C para detener
echo.

REM Ejecutar auditor
python vision_auditor.py ^
  --api-key "%EMULE_API_KEY%" ^
  --anthropic-key "%ANTHROPIC_KEY%" ^
  --interval %INTERVAL%

pause
