$path = "c:\Fragua\eMule-Aishor\srchybrid\Preferences.cpp"
$temp = $path + ".tmp"
$content = Get-Content $path -Encoding Utf8
$newContent = @("#include ""stdafx.h""") + $content
$newContent | Set-Content $temp -Encoding Utf8
Move-Item $temp $path -Force
Write-Host "Fixed Preferences.cpp"
