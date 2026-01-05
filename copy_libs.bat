@echo off
echo Copiando librerías a ubicaciones esperadas...

REM Crear directorios necesarios
mkdir "libpng\projects\visualc\x64\Release" 2>nul
mkdir "cryptopp\x64\Release" 2>nul

REM Copiar librerías
copy /Y "libpng\projects\vstudio\x64\Release\libpng.lib" "libpng\projects\visualc\x64\Release\"
copy /Y "cryptopp\x64\Output\Release\cryptlib.lib" "cryptopp\x64\Release\"

echo Listo.
