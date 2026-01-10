$path = "srchybrid\ServerWnd.cpp"
$lines = Get-Content $path

# Filter out existing includes of stdafx.h and Resource.h to avoid duplicates/wrong position
$cleanLines = $lines | Where-Object { 
    $_ -notmatch '^\s*#include\s+"stdafx.h"' -and 
    $_ -notmatch '^\s*#include\s+"Resource.h"' 
}

# Prepend them in correct order
$newContent = @('#include "stdafx.h"', '#include "Resource.h"') + $cleanLines

$newContent | Set-Content $path
Write-Host "Cleaned and prepended headers."
Get-Content $path -TotalCount 5
