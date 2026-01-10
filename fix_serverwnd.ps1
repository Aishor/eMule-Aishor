$path = "srchybrid\ServerWnd.cpp"
$content = Get-Content $path
$headers = @('#include "stdafx.h"', '#include "Resource.h"')

# Check if already present to avoid duplication
if ($content[0] -ne '#include "stdafx.h"') {
    $newContent = $headers + $content
    $newContent | Set-Content $path
    Write-Host "Headers prepended."
}
else {
    Write-Host "Headers already present."
}

# Verify
Get-Content $path -TotalCount 5
