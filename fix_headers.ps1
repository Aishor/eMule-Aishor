$edits = @(
    @("c:\Fragua\eMule-Aishor\srchybrid\PPgDisplay.h", '#include "Resource.h"', '#pragma once'),
    @("c:\Fragua\eMule-Aishor\srchybrid\SearchParamsWnd.h", '#include "Resource.h"', '#pragma once'),
    @("c:\Fragua\eMule-Aishor\srchybrid\StatisticsDlg.h", '#include "Resource.h"', '#pragma once'),
    @("c:\Fragua\eMule-Aishor\srchybrid\DownloadQueue.h", '#include "OtherFunctions.h"', '#pragma once')
)

foreach ($item in $edits) {
    $path = $item[0]
    $insert = $item[1]
    $marker = $item[2]
    
    if (Test-Path $path) {
        $content = Get-Content $path -Encoding Utf8
        # Check if already included
        $alreadyThere = $false
        foreach ($line in $content) { if ($line -eq $insert) { $alreadyThere = $true; break } }
        
        if ($alreadyThere) {
            Write-Host "Already included in $path"
            continue
        }
        
        $newContent = @()
        foreach ($line in $content) {
            $newContent += $line
            if ($line -eq $marker) {
                $newContent += $insert
            }
        }
        $temp = $path + ".tmp"
        $newContent | Set-Content $temp -Encoding Utf8
        Move-Item $temp $path -Force
        Write-Host "Updated $path"
    }
    else {
        Write-Host "Missing $path"
    }
}
