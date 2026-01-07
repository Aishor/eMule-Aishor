function Add-Include($path, $include, $afterInclude) {
    if (-not (Test-Path $path)) { Write-Host "Skipping $path"; return }
    $lines = @(Get-Content $path)
    $list = [System.Collections.ArrayList]::new($lines)
    
    # Check if already included
    foreach($l in $list) { if ($l.Trim() -eq "#include `"$include`"") { return } }
    
    $insertIdx = 0
    if ($afterInclude) {
        for($i=0; $i -lt $list.Count; $i++) {
            if ($list[$i].Trim() -eq "#include `"$afterInclude`"") {
                $insertIdx = $i + 1
                break
            }
        }
    } else {
        # Insert after last include or at top
        for($i=0; $i -lt $list.Count; $i++) {
            if ($list[$i].Trim().StartsWith("#include")) { $insertIdx = $i + 1 }
        }
    }
    
    $list.Insert($insertIdx, "#include `"$include`"")
    Set-Content -Path $path -Value $list
    Write-Host "Added $include to $path"
}

Add-Include "c:\Fragua\eMule-Aishor\srchybrid\AsyncSocketEx.cpp" "Preferences.h" "stdafx.h"
Add-Include "c:\Fragua\eMule-Aishor\srchybrid\Friend.h" "OtherFunctions.h" ""
Add-Include "c:\Fragua\eMule-Aishor\srchybrid\DownloadQueue.h" "OtherFunctions.h" ""
Add-Include "c:\Fragua\eMule-Aishor\srchybrid\MediaInfo.h" "OtherFunctions.h" ""
Add-Include "c:\Fragua\eMule-Aishor\srchybrid\StatisticsDlg.h" "resource.h" ""

# Specifically for MediaInfo.h, ensure OtherFunctions is before usage if possible, but includes usually go at top.
# Also MediaInfo.h had a resource.h error, so add that too if missing.
Add-Include "c:\Fragua\eMule-Aishor\srchybrid\MediaInfo.h" "resource.h" "OtherFunctions.h"
