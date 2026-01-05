@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
msbuild emule.sln /t:Clean /p:Configuration=Debug /p:Platform=Win32
msbuild emule.sln /p:Configuration=Debug /p:Platform=Win32 /v:minimal /maxcpucount:1 /p:CL_MPCount=1
