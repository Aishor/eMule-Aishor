function Fix-PCH($path) {
    if (-not (Test-Path $path)) { Write-Host "File not found: $path"; return }
    $lines = @(Get-Content $path)
    $list = [System.Collections.ArrayList]::new($lines)
    
    $idxStdafx = -1
    for($i=0; $i -lt $list.Count; $i++) {
        if ($list[$i].Trim() -eq '#include "stdafx.h"') {
            $idxStdafx = $i
            break
        }
    }
    
    if ($idxStdafx -ge 0) {
        $stdafxLine = $list[$idxStdafx]
        $list.RemoveAt($idxStdafx)
        
        # Find insertion point (after comments)
        $insertIdx = 0
        for($i=0; $i -lt $list.Count; $i++) {
            $l = $list[$i].Trim()
            if ($l -ne "" -and -not $l.StartsWith("//")) {
                $insertIdx = $i
                break
            }
        }
        $list.Insert($insertIdx, $stdafxLine)
        Set-Content -Path $path -Value $list
        Write-Host "Fixed PCH for $path"
    } else {
        Write-Host "stdafx.h not found in $path"
    }
}

Fix-PCH "c:\Fragua\eMule-Aishor\srchybrid\MediaInfo.cpp"
Fix-PCH "c:\Fragua\eMule-Aishor\srchybrid\AsyncSocketEx.cpp"

# Fix Preferences.h
$prefH = "c:\Fragua\eMule-Aishor\srchybrid\Preferences.h"
$lines = @(Get-Content $prefH)
$list = [System.Collections.ArrayList]::new($lines)
$hasTcp = $false
foreach($line in $list) { if ($line -match "m_uTcpWindowSize") { $hasTcp = $true; break } }

if (-not $hasTcp) {
    $idxMax = -1
    for($i=0; $i -lt $list.Count; $i++) {
        if ($list[$i] -match "static UINT\s+maxconnections;") {
            $idxMax = $i
            break
        }
    }
    if ($idxMax -ge 0) {
        $list.Insert($idxMax+1, "	static uint32	m_uTcpWindowSize;")
        Set-Content -Path $prefH -Value $list
        Write-Host "Added m_uTcpWindowSize to Preferences.h"
    } else {
        Write-Host "Could not find maxconnections in Preferences.h"
    }
} else {
    Write-Host "Preferences.h already has m_uTcpWindowSize"
}
