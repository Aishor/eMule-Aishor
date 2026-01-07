$hPath = "srchybrid\Preferences.h"
$cPath = "srchybrid\Preferences.cpp"

if (-not (Test-Path $hPath)) { Write-Error "File not found: $hPath"; exit 1 }
if (-not (Test-Path $cPath)) { Write-Error "File not found: $cPath"; exit 1 }

$h = Get-Content $hPath -Raw
$hReplacement = @"
	// Titanium: TCP Window Size
	static uint32	m_uTcpWindowSize;
	static uint32	GetTcpWindowSize()					{ return m_uTcpWindowSize; }
	static void		SetTcpWindowSize(uint32 nSize)		{ m_uTcpWindowSize = nSize; }

	static bool		IsNotifierSendMailEnabled()
"@
$h = $h -replace 'static bool\s+IsNotifierSendMailEnabled\(\)', $hReplacement
Set-Content $hPath -Value $h

$cpp = Get-Content $cPath -Raw
$cpp = $cpp -replace 'uint32\s+CPreferences::m_minupload;', "uint32	CPreferences::m_minupload;`r`nuint32	CPreferences::m_uTcpWindowSize;"

# Escape regex special chars for find string
$findSave = [regex]::Escape('ini.WriteInt(_T("MinUpload"), m_minupload);')
$replaceSave = 'ini.WriteInt(_T("MinUpload"), m_minupload);' + "`r`n`tini.WriteInt(_T(`"TcpWindowSize`"), m_uTcpWindowSize);"
$cpp = $cpp -replace $findSave, $replaceSave

$findLoad = [regex]::Escape('m_minupload = (uint32)ini.GetInt(_T("MinUpload"), 1);')
$replaceLoad = 'm_minupload = (uint32)ini.GetInt(_T("MinUpload"), 1);' + "`r`n`tm_uTcpWindowSize = (uint32)ini.GetInt(_T(`"TcpWindowSize`"), 0);"
$cpp = $cpp -replace $findLoad, $replaceLoad

Set-Content $cPath -Value $cpp

Write-Host "Preferences updated successfully."
