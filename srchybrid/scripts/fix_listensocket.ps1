function Move-IncludeToTop($path, $includeName) {
    if (-not (Test-Path $path)) { Write-Host "File not found: $path"; return }
    $lines = @(Get-Content $path)
    $list = [System.Collections.ArrayList]::new($lines)
    
    $idxSync = -1
    for($i=0; $i -lt $list.Count; $i++) {
        if ($list[$i].Trim() -eq "#include `"$includeName`"") {
            $idxSync = $i
            break
        }
    }
    
    if ($idxSync -ge 0) {
        $line = $list[$idxSync]
        $list.RemoveAt($idxSync)
        # Find insertion (after comments)
        $insertIdx = 0
        for($i=0; $i -lt $list.Count; $i++) {
            $l = $list[$i].Trim()
            if ($l -ne "" -and -not $l.StartsWith("//")) {
                $insertIdx = $i
                break
            }
        }
        $list.Insert($insertIdx, $line)
        Set-Content -Path $path -Value $list
        Write-Host "Moved $includeName to top in $path"
    } else {
        Write-Host "$includeName not found in $path, inserting at top"
        # Insert at top
        $insertIdx = 0
        for($i=0; $i -lt $list.Count; $i++) {
            $l = $list[$i].Trim()
            if ($l -ne "" -and -not $l.StartsWith("//")) {
                $insertIdx = $i
                break
            }
        }
        $list.Insert($insertIdx, "#include `"$includeName`"")
        Set-Content -Path $path -Value $list
    }
}

function Move-ResourceBeforeMediaInfo($path) {
    if (-not (Test-Path $path)) { return }
    $lines = @(Get-Content $path)
    $list = [System.Collections.ArrayList]::new($lines)
    
    $idxRes = -1
    $idxMed = -1
    
    for($i=0; $i -lt $list.Count; $i++) {
        if ($list[$i].Trim() -eq '#include "resource.h"') { $idxRes = $i }
        if ($list[$i].Trim() -eq '#include "MediaInfo.h"') { $idxMed = $i }
    }
    
    if ($idxRes -ge 0 -and $idxMed -ge 0 -and $idxRes -gt $idxMed) {
        $resLine = $list[$idxRes]
        $list.RemoveAt($idxRes)
        # idxMed might override if idxRes was before it? No, idxRes > idxMed.
        # So idxMed is still at same index.
        $list.Insert($idxMed, $resLine)
        Set-Content -Path $path -Value $list
        Write-Host "Moved resource.h before MediaInfo.h in $path"
    }
}

Move-IncludeToTop "c:\Fragua\eMule-Aishor\srchybrid\ListenSocket.cpp" "stdafx.h"
Move-ResourceBeforeMediaInfo "c:\Fragua\eMule-Aishor\srchybrid\MediaInfo.cpp"
