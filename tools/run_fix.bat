@echo off
echo Attempting to fix encodings using local python...
if exist "%~dp0..\_env\Scripts\python.exe" (
    "%~dp0..\_env\Scripts\python.exe" "%~dp0fix_encoding.py"
) else (
    echo [ERROR] Python not found in ..\_env\Scripts\python.exe
    echo Please run tools\install_mcp.bat first
)
pause
