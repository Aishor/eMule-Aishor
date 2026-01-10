$path = "srchybrid\SplashScreen.cpp"
$lines = Get-Content $path

# Filter out existing includes of stdafx.h to avoid duplicates
$cleanLines = $lines | Where-Object { 
    $_ -notmatch '^\s*#include\s+"stdafx.h"'
}

# Prepend it
$newContent = @('#include "stdafx.h"') + $cleanLines

$newContent | Set-Content $path
Write-Host "Cleaned and prepended stdafx.h to SplashScreen.cpp"
Get-Content $path -TotalCount 5
