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
cmd /c copy_libs.bat

# Paso 2: Compilar eMule
Write-Host ""
Write-Host "[2/4] Compilando emule.exe..." -ForegroundColor Cyan
& $msbuild "srchybrid\emule.vcxproj" /p:Configuration=$config /p:Platform=$platform /m /v:minimal

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "[ERROR] Compilación fallida" -ForegroundColor Red
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

# Copiar archivos de documentación (actualizar siempre)
Copy-Item "srchybrid\scripts\configure_firewall.ps1" "$releaseDir\Configurar_Firewall.ps1" -Force -ErrorAction SilentlyContinue
Copy-Item "docs\RELEASE_v0.70b-Build26-R1.2-X64.md" "$releaseDir\NOTAS_RELEASE.md" -Force -ErrorAction SilentlyContinue
Write-Host "  + Documentación actualizada" -ForegroundColor Gray

# Paso 4: Generar ZIP (Opcional - por ahora solo preparamos la carpeta)
Write-Host ""
Write-Host "[4/4] Paquete portable listo en: $releaseDir\" -ForegroundColor Green

# Información del ejecutable
$exeFile = Get-Item "$releaseDir\emule.exe"
$sizeM = [math]::Round($exeFile.Length / 1MB, 2)

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  BUILD EXITOSO" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Ejecutable: $releaseDir\emule.exe" -ForegroundColor White
Write-Host "  Tamaño: $sizeM MB" -ForegroundColor White
Write-Host "  Fecha: $($exeFile.LastWriteTime)" -ForegroundColor White
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Para crear el ZIP de distribución, ejecuta:" -ForegroundColor Yellow
Write-Host '  Compress-Archive -Path Release_Pack\* -DestinationPath eMule-Aishor-R1.2-Portable.zip -Force' -ForegroundColor Cyan
Write-Host ""
