# eMule-Aishor Firewall Configuration Script
# Habilita los puertos por defecto para eMule en el Firewall de Windows

$TCP_PORT = 4662
$UDP_PORT = 4672
$WEB_PORT = 4711
$RuleName = "eMule-Aishor-R1.2"

# Verificar privilegios de Administrador
$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "[-] Se requieren privilegios de Administrador. Reiniciando como Admin..." -ForegroundColor Yellow
    Start-Process powershell.exe "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`"" -Verb RunAs
    exit
}

Write-Host "Configurando Firewall de Windows para $RuleName..." -ForegroundColor Cyan

# Eliminar reglas antiguas si existen
Remove-NetFirewallRule -DisplayName "$RuleName (TCP)" -ErrorAction SilentlyContinue
Remove-NetFirewallRule -DisplayName "$RuleName (UDP)" -ErrorAction SilentlyContinue
Remove-NetFirewallRule -DisplayName "$RuleName (Web)" -ErrorAction SilentlyContinue

# Crear nuevas reglas
try {
    New-NetFirewallRule -DisplayName "$RuleName (TCP)" -Direction Inbound -LocalPort $TCP_PORT -Protocol TCP -Action Allow -Description "Permite tráfico TCP eMule"
    Write-Host "[+] Regla TCP ($TCP_PORT) creada." -ForegroundColor Green

    New-NetFirewallRule -DisplayName "$RuleName (UDP)" -Direction Inbound -LocalPort $UDP_PORT -Protocol UDP -Action Allow -Description "Permite tráfico UDP eMule"
    Write-Host "[+] Regla UDP ($UDP_PORT) creada." -ForegroundColor Green

    New-NetFirewallRule -DisplayName "$RuleName (Web)" -Direction Inbound -LocalPort $WEB_PORT -Protocol TCP -Action Allow -Description "Permite interfaz web eMule"
    Write-Host "[+] Regla WebServer ($WEB_PORT) creada." -ForegroundColor Green
    
    Write-Host "`nConfiguracion completada exitosamente." -ForegroundColor Cyan
} catch {
    Write-Host "[-] Error al crear reglas: $_" -ForegroundColor Red
}

Write-Host "Presione cualquier tecla para salir..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
