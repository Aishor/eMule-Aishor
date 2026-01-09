$ErrorActionPreference = "Continue"

$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
$platform = "x64"
$config = "Release"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Compilación Single-Thread" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar MSBuild
if (-not (Test-Path $msbuild)) {
    Write-Host "[ERROR] MSBuild no encontrado en: $msbuild" -ForegroundColor Red
    exit 1
}

Write-Host "[INFO] Compilando con 1 hilo (sin paralelización)..." -ForegroundColor Yellow
Write-Host ""

$logFile = "build_log.txt"
$errorLog = "build_errors.txt"

# Compilar SIN /m (sin paralelización) para evitar congelamiento
& $msbuild "srchybrid\emule.vcxproj" /p:Configuration=$config /p:Platform=$platform /m:1 /v:minimal /fl "/flp:LogFile=$logFile;Verbosity=minimal" "/flp1:LogFile=$errorLog;Verbosity=minimal;errorsonly"

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "[ERROR] Compilación fallida" -ForegroundColor Red
    if (Test-Path $errorLog) {
        Write-Host ""
        Write-Host "Últimos errores:" -ForegroundColor Yellow
        Get-Content $errorLog | Select-Object -Last 50
        Write-Host ""
        Write-Host "Log completo en: $errorLog" -ForegroundColor Gray
    }
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  COMPILACIÓN EXITOSA" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

# Verificar ejecutable
$exePath = "srchybrid\$platform\$config\emule.exe"
if (Test-Path $exePath) {
    $exeFile = Get-Item $exePath
    $sizeM = [math]::Round($exeFile.Length / 1MB, 2)
    Write-Host "  Ejecutable: $exePath" -ForegroundColor White
    Write-Host "  Tamaño: $sizeM MB" -ForegroundColor White
    Write-Host "  Fecha: $($exeFile.LastWriteTime)" -ForegroundColor White
}
else {
    Write-Host "[WARNING] No se encontro el ejecutable en $exePath" -ForegroundColor Yellow
}

Write-Host "========================================" -ForegroundColor Green
Write-Host ""
