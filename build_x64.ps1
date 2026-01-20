$ErrorActionPreference = "Continue"

$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
$platform = "x64"
$config = "Release"
$releaseDir = "Release_Pack"

Write-Host ""
Write-Host "========================================"
Write-Host "  eMule-Aishor R1.2 - Build & Package"  
Write-Host "========================================"
Write-Host ""

# Verificar MSBuild
if (-not (Test-Path $msbuild)) {
    Write-Host "[ERROR] MSBuild no encontrado en: $msbuild" -ForegroundColor Red
    Write-Host "Instala Visual Studio 2022 o ajusta la ruta." -ForegroundColor Yellow
    exit 1
}

# Paso 1: Copy Libs (Setup environment)
Write-Host "[1/4] Preparando dependencias..." -ForegroundColor Cyan
# cmd /c copy_libs.bat

# Paso 2: Compilar eMule (Rebuild explícito)
Write-Host ""
Write-Host "[2/4] Compilando emule.exe (Clean + Build)..." -ForegroundColor Cyan

$logFile = "build_log.txt"
$errorLog = "build_errors.txt"

# Usamos /t:Rebuild para asegurar limpieza total a nivel de compilador
& $msbuild "srchybrid\emule.vcxproj" /t:Rebuild /p:Configuration=$config /p:Platform=$platform /m /v:minimal /fl "/flp:LogFile=$logFile;Verbosity=minimal" "/flp1:LogFile=$errorLog;Verbosity=minimal;errorsonly"

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "[ERROR] Compilaci├│n fallida" -ForegroundColor Red
    if (Test-Path $errorLog) {
        Write-Host ""
        Write-Host "Errores encontrados:" -ForegroundColor Yellow
        Get-Content $errorLog | Select-Object -First 50
        Write-Host ""
        Write-Host "Log completo en: $errorLog" -ForegroundColor Gray
    }
    exit 1
}

Write-Host "[SUCCESS] emule.exe compilado" -ForegroundColor Green

# Paso 3: Preparar Release_Pack
Write-Host ""
Write-Host "[3/4] Preparando paquete portable..." -ForegroundColor Cyan

# Crear estructura si no existe
if (-not (Test-Path $releaseDir)) {
    New-Item -ItemType Directory -Path $releaseDir | Out-Null
}

# Copiar ejecutable compilado
Copy-Item "srchybrid\$platform\$config\emule.exe" "$releaseDir\emule.exe" -Force
Write-Host "  + emule.exe copiado" -ForegroundColor Gray

# Copiar archivos de idioma (solo si no existen)
if (-not (Test-Path "$releaseDir\lang")) {
    xcopy "srchybrid\lang" "$releaseDir\lang\" /E /I /Y /Q | Out-Null
    Write-Host "  + lang/ copiado (131 archivos)" -ForegroundColor Gray
}

# Copiar webinterface (solo si no existen)
if (-not (Test-Path "$releaseDir\webinterface")) {
    xcopy "srchybrid\webinterface" "$releaseDir\webinterface\" /E /I /Y /Q | Out-Null
    Write-Host "  + webinterface/ copiado" -ForegroundColor Gray
}

# Copiar archivos de documentaci├│n (actualizar siempre)
Copy-Item "srchybrid\scripts\configure_firewall.ps1" "$releaseDir\Configurar_Firewall.ps1" -Force -ErrorAction SilentlyContinue
Copy-Item "docs\RELEASE_v0.70b-Build26-R1.2-X64.md" "$releaseDir\NOTAS_RELEASE.md" -Force -ErrorAction SilentlyContinue
Write-Host "  + Documentaci├│n actualizada" -ForegroundColor Gray

# Paso 4: Generar ZIP (Opcional - por ahora solo preparamos la carpeta)
Write-Host ""
Write-Host "[4/4] Paquete portable listo en: $releaseDir\" -ForegroundColor Green

# Informaci├│n del ejecutable
$exeFile = Get-Item "$releaseDir\emule.exe"
$sizeM = [math]::Round($exeFile.Length / 1MB, 2)

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  BUILD EXITOSO" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Ejecutable: $releaseDir\emule.exe" -ForegroundColor White
Write-Host "  Tama├▒o: $sizeM MB" -ForegroundColor White
Write-Host "  Fecha: $($exeFile.LastWriteTime)" -ForegroundColor White
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Para crear el ZIP de distribuci├│n, ejecuta:" -ForegroundColor Yellow
Write-Host '  Compress-Archive -Path Release_Pack\* -DestinationPath eMule-Aishor-R1.2-Portable.zip -Force' -ForegroundColor Cyan
Write-Host ""
