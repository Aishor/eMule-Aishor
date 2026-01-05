$ErrorActionPreference = "Continue"

$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
$platform = "x64"
$config = "Release"

Write-Host ""
Write-Host "========================================"
Write-Host "  R1.1 TITANIUM FIBER - Build x64"  
Write-Host "========================================"
Write-Host ""

# Dependencias (Pre-compiladas en repo)
Write-Host "Nota: Las dependencias se asumen pre-compiladas en este repositorio." -ForegroundColor Gray

# Copy Libs (Setup environment)
Write-Host ""
Write-Host "[SETUP] Copiando librerias..." -ForegroundColor Cyan
cmd /c copy_libs.bat

# eMule Principal
Write-Host ""
Write-Host "[BUILD] eMule Principal..." -ForegroundColor Cyan
# Usar emule.vcxproj directamente para evitar dependencias fantasmas en el .sln
& $msbuild "srchybrid\emule.vcxproj" /p:Configuration=$config /p:Platform=$platform /m /v:minimal

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] eMule build failed" -ForegroundColor Red
    exit 1
} else {
    Write-Host "[SUCCESS] eMule build complete" -ForegroundColor Green
    Write-Host ""
    Write-Host "========================================"  -ForegroundColor Green
    Write-Host "  BUILD EXITOSO"  -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green  
    Write-Host "  Ejecutable: srchybrid\x64\Release\emule.exe" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "  BUILD FALLIDO"  -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "  Revisa: compilation_log_x64.txt" -ForegroundColor Red
}
