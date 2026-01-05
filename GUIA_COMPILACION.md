# 🛠️ Guía de Compilación: eMule x64 Release

Esta guía detalla los pasos necesarios para compilar **eMule-Aishor-Forge** en arquitectura **x64** (Nativa) con optimizaciones activadas.

## 📋 Requisitos del Sistema

*   **Sistema Operativo:** Windows 10 x64 o superior.
*   **IDE:** Visual Studio 2026 (v18) Community/Pro/Enterprise.
*   **Componentes de Visual Studio:**
    *   "Desarrollo de escritorio con C++"
    *   **MSVC v145** - VS 2026 C++ x64/x86 build tools (Latest)
    *   **Windows 10/11 SDK**
    *   **MFC for C++** (x86 & x64)
*   **PowerShell:** v5.0 o superior.

## 📂 Estructura de Carpetas

Asegúrate de que el repositorio esté en una ruta corta para evitar errores de `MAX_PATH`.
Ejemplo recomendado: `C:\Fragua\eMule-Aishor-Forge`

## 🚀 Instrucciones de Compilación (Automatizada)

Hemos creado un script maestro que compila todas las dependencias y el proyecto principal en el orden correcto.

1.  Abre una terminal **PowerShell**.
2.  Navega a la raíz del proyecto:
    ```powershell
    cd C:\Fragua\eMule-Aishor-Forge
    ```
3.  Ejecuta el script de construcción:
    ```powershell
    .\build_x64.ps1
    ```

**¿Qué hace este script?**
1.  Compila **zlib** (x64 Release)
2.  Compila **mbedTLS** (Crypto, x509, TLS)
3.  Compila **CryptoPP, libpng, CxImage, id3lib, ResizableLib, miniupnpc**
4.  Ejecuta `copy_libs.bat` para organizar las librerías estáticas (`.lib`) en las rutas esperadas.
5.  Compila **emule.sln** con todas las optimizaciones (LTCG, AVX2).

## ⚠️ Compilación Manual (Paso a Paso)

Si el script falla o prefieres hacerlo manualmente:

### 1. Compilar Dependencias
Debes compilar en configuración **Release | x64** las siguientes soluciones/proyectos:
*   `zlib\contrib\vstudio\vc17\zlibvc.sln`
*   `mbedtls\visualc\VS2017\mbedTLS.sln`
*   `cryptopp\cryptlib.vcxproj`
*   `libpng\projects\vstudio\libpng\libpng.vcxproj`
*   `cximage\cximage.vcxproj`
*   `id3lib\libprj\id3lib.vcxproj`
*   `resizablelib\ResizableLib\ResizableLib.vcxproj`
*   `miniupnpc\msvc\miniupnpc.vcxproj`

### 2. Preparar Librerías
Ejecuta el script de copiado:
```cmd
copy_libs.bat
```
Esto mueve `libpng.lib` y `cryptlib.lib` a las carpetas que `emule.vcxproj` espera encontrar.

### 3. Compilar eMule
*   Abre `srchybrid\emule.sln` en Visual Studio 2026.
*   Selecciona **Release** y **x64**.
*   Menú **Compilar** -> **Compilar solución**.

## ✅ Verificación

Al finalizar, el ejecutable se generará en:
`C:\Fragua\eMule-Aishor-Forge\srchybrid\x64\Release\emule.exe`

Para verificar que es un binario x64 válido:
```cmd
dumpbin /headers srchybrid\x64\Release\emule.exe | findstr "machine"
# Salida esperada: "8664 machine (x64)"
```

## 🐛 Solución de Problemas Comunes

**Error: `LNK1181: no se puede abrir el archivo de entrada '...lib'`**
*   Ejecuta `copy_libs.bat` manualmente.
*   Verifica que todas las dependencias compilaron sin errores.

**Error: `CVT1100: recurso duplicado (Manifest)`**
*   Asegúrate de que `GenerateManifest` está activado en las propiedades del Linker y que NO hay una inclusión manual en `emule.rc` (debería estar comentada).

**Visual: Columnas de lista muy estrechas**
*   Es un efecto secundario conocido del soporte High DPI en controles antiguos. Funcionalidad no afectada.
