$pathZip = "c:\Fragua\eMule-Aishor\srchybrid\ZIPFile.cpp"
$linesZip = @(Get-Content $pathZip)
$linesZip[21] = '#include "stdafx.h"'
$linesZip[22] = '#include "ZIPFile.h"'
Set-Content -Path $pathZip -Value $linesZip

$pathPref = "c:\Fragua\eMule-Aishor\srchybrid\Preferences.cpp"
$linesPref = @(Get-Content $pathPref)
$listPref = [System.Collections.ArrayList]::new($linesPref)

$idxStdafx = -1
for($i=0; $i -lt $listPref.Count; $i++) {
    if ($listPref[$i].Trim() -eq '#include "stdafx.h"') {
        $idxStdafx = $i
        break
    }
}
if ($idxStdafx -ge 0) {
    $listPref.RemoveAt($idxStdafx)
}

$idxPref = -1
for($i=0; $i -lt $listPref.Count; $i++) {
    if ($listPref[$i].Trim() -eq '#include "Preferences.h"') {
        $idxPref = $i
        break
    }
}
if ($idxPref -ge 0) {
    $listPref.Insert($idxPref, '#include "stdafx.h"')
}

$dupIdx = -1
for($i=0; $i -lt $listPref.Count-1; $i++) {
   if ($listPref[$i].Trim() -eq 'UINT CPreferences::maxhalfconnections;' -and $listPref[$i+1].Trim() -eq 'UINT CPreferences::maxhalfconnections;') {
       $dupIdx = $i+1
       break
   }
}
if ($dupIdx -ge 0) { $listPref.RemoveAt($dupIdx) }

Set-Content -Path $pathPref -Value $listPref
