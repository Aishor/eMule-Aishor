function Patch-Header($path, $includes) {
    if (-not (Test-Path $path)) { return }
    $content = [System.IO.File]::ReadAllText($path)
    
    foreach ($inc in $includes) {
        if ($content -contains "#include `"$inc`"") { continue }
        
        # Insert after #pragma once
        if ($content -match "(?m)^#pragma once") {
            $content = $content -replace "(?m)^#pragma once", "`$0`r`n#include `"$inc`""
            Write-Host "Added $inc to $path"
        }
    }
    [System.IO.File]::WriteAllText($path, $content)
}

Patch-Header "c:\Fragua\eMule-Aishor\srchybrid\MediaInfo.h" @("resource.h", "OtherFunctions.h")
Patch-Header "c:\Fragua\eMule-Aishor\srchybrid\StatisticsDlg.h" @("resource.h")
