$files = @(
    "c:\Fragua\eMule-Aishor\srchybrid\MuleListCtrl.cpp",
    "c:\Fragua\eMule-Aishor\srchybrid\PPgDisplay.cpp"
)

foreach ($path in $files) {
    if (Test-Path $path) {
        $temp = $path + ".tmp"
        $content = Get-Content $path -Encoding Utf8
        # Check if already has stdafx.h at top
        if ($content[0] -ne '#include "stdafx.h"') {
            $newContent = @("#include ""stdafx.h""") + $content
            $newContent | Set-Content $temp -Encoding Utf8
            Move-Item $temp $path -Force
            Write-Host "Fixed $path"
        }
        else {
            Write-Host "Already has stdafx.h: $path"
        }
    }
    else {
        Write-Host "File not found: $path"
    }
}
