function Add-Include($path, $include) {
    if (-not (Test-Path $path)) { return }
    $lines = @(Get-Content $path)
    $list = [System.Collections.ArrayList]::new($lines)
    foreach($l in $list) { if ($l.Trim() -eq "#include `"$include`"") { return } }
    
    $idx = -1
    for($i=0; $i -lt $list.Count; $i++) {
        if ($list[$i].Trim() -eq "#pragma once") { $idx = $i; break }
    }
    if ($idx -ge 0) {
        $list.Insert($idx + 1, "#include `"$include`"")
        Set-Content -Path $path -Value $list -Encoding UTF8
        Write-Host "Added $include to $path"
    }
}

function Add-Method($path, $pattern, $newMethod) {
    if (-not (Test-Path $path)) { return }
    $lines = @(Get-Content $path)
    $list = [System.Collections.ArrayList]::new($lines)
    foreach($l in $list) { if ($l.Trim() -eq $newMethod.Trim()) { return } }
    
    $idx = -1
    for($i=0; $i -lt $list.Count; $i++) {
        if ($list[$i] -match $pattern) { $idx = $i; break }
    }
    if ($idx -ge 0) {
        $list.Insert($idx + 1, $newMethod)
        Set-Content -Path $path -Value $list -Encoding UTF8
        Write-Host "Added method to $path"
    }
}

# Fix Preferences.h
Add-Method "c:\Fragua\eMule-Aishor\srchybrid\Preferences.h" "static UINT\s+GetMaxConnections\(\)" "	static uint32	GetTcpWindowSize()					{ return m_uTcpWindowSize; }"
Add-Method "c:\Fragua\eMule-Aishor\srchybrid\Preferences.h" "static void\s+SetMaxConnections\(UINT in\)" "	static void		SetTcpWindowSize(uint32 in)			{ m_uTcpWindowSize = in; }"

# Fix Headers
Add-Include "c:\Fragua\eMule-Aishor\srchybrid\MediaInfo.h" "resource.h"
Add-Include "c:\Fragua\eMule-Aishor\srchybrid\MediaInfo.h" "OtherFunctions.h"
Add-Include "c:\Fragua\eMule-Aishor\srchybrid\StatisticsDlg.h" "resource.h"
