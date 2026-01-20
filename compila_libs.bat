@echo off
set MSBUILD="C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"

echo ========================================
echo Compilando zlib (Release x64)
echo ========================================
%MSBUILD% c:\Fragua\eMule-Aishor\zlib\contrib\vstudio\vc17\zlibvc.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
if errorlevel 1 echo Error building zlib & exit /b 1

echo ========================================
echo Compilando cryptopp (Release x64)
echo ========================================
%MSBUILD% c:\Fragua\eMule-Aishor\cryptopp\cryptlib.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
if errorlevel 1 echo Error building cryptopp & exit /b 1

echo ========================================
echo Compilando id3lib (Release x64)
echo ========================================
%MSBUILD% c:\Fragua\eMule-Aishor\id3lib\libprj\id3lib.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
if errorlevel 1 echo Error building id3lib & exit /b 1

echo ========================================
echo Dependencies built successfully.
echo ========================================
