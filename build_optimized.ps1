$ErrorActionPreference = "Continue"

$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
$platform = "x64"
$config = "Release"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Compilacion Optimizada (4 hilos)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

if (-not (Test-Path $msbuild)) {
    Write-Host "ERROR: MSBuild no encontrado" -ForegroundColor Red
    exit 1
}

Write-Host "INFO: Compilando con 4 hilos paralelos..." -ForegroundColor Yellow
Write-Host ""

$logFile = "build_log.txt"
$errorLog = "build_errors.txt"

# Usar /m:4 para aprovechar CPU sin congelar el sistema
& $msbuild "srchybrid\emule.vcxproj" /p:Configuration=$config /p:Platform=$platform /m:4 /v:minimal /fl "/flp:LogFile=$logFile;Verbosity=minimal" "/flp1:LogFile=$errorLog;Verbosity=minimal;errorsonly"

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: Compilacion fallida" -ForegroundColor Red
    if (Test-Path $errorLog) {
        Write-Host ""
        Write-Host "Ultimos errores:" -ForegroundColor Yellow
        Get-Content $errorLog | Select-Object -Last 50
    }
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  COMPILACION EXITOSA" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

$exePath = "srchybrid\$platform\$config\emule.exe"
if (Test-Path $exePath) {
    $exeFile = Get-Item $exePath
    $sizeM = [math]::Round($exeFile.Length / 1MB, 2)
    Write-Host "  Ejecutable: $exePath" -ForegroundColor White
    Write-Host "  Tamano: $sizeM MB" -ForegroundColor White
    Write-Host "  Fecha: $($exeFile.LastWriteTime)" -ForegroundColor White
}

Write-Host "========================================" -ForegroundColor Green
Write-Host ""
