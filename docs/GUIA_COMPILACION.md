# 🛠️ Guía de Compilación: eMule-Aishor (R1.2 Titanium Fiber)

Esta guía detalla el proceso completo de compilación de **eMule-Aishor** en arquitectura **x64 nativa** con Visual Studio 2022.

---

## 📋 Requisitos del Sistema

### Hardware Mínimo
- **CPU**: x64 compatible (Intel/AMD)
- **RAM**: 8 GB (recomendado 16 GB para compilación paralela)
- **Disco**: 15 GB libres (fuentes + librerías + build artifacts)

### Software Requerido

#### Visual Studio 2022
- **Edición**: Community (gratuita) o superior
- **Versión mínima**: 17.0
- **Toolset**: **v145** (MSVC 14.45.x o superior)

**Componentes obligatorios** (Instalador de VS):
- ✅ "Desarrollo de escritorio con C++"
- ✅ **MSVC v145 - VS 2022 C++ x64/x86 build tools (Latest)**
- ✅ **Windows 10/11 SDK** (10.0.26100.0 o superior)
- ✅ **ATL/MFC for C++ (x86 & x64)**
- ✅ **C++ CMake tools** (opcional, para mbedTLS)

#### Herramientas Adicionales
- **Git**: Para clonar el repositorio
- **PowerShell**: v5.0+ (incluido en Windows 10/11)

---

## 📂 Preparación del Entorno

### 1. Clonar el Repositorio

```powershell
git clone https://github.com/Aishor/eMule-Aishor.git
cd eMule-Aishor
git checkout v0.70b-Build26-R1.2-X64
```

### 2. Verificar Estructura de Carpetas

```
eMule-Aishor/
├── srchybrid/          # Código principal de eMule
├── cryptopp/           # Headers + cryptlib.lib
├── mbedtls/            # Headers + mbedTLS.lib
├── zlib/               # Headers + zlib.lib
├── libpng/             # Headers + libpng16.lib
├── id3lib/             # Headers + id3lib.lib
├── cximage/            # Headers + cximage.lib
├── miniupnpc/          # Headers + miniupnpc.lib
├── resizablelib/       # Headers + resizablelib.lib
├── build_x64.ps1       # Script de compilación automatizada
└── docs/               # Documentación
```

> **⚠️ Nota Importante**: Este repositorio ya incluye las **librerías (.lib) precompiladas**. No necesitas compilar las dependencias desde cero a menos que desees modificarlas.

---

## 🚀 Compilación Rápida (Recomendado)

### Método Automatizado

El proyecto incluye un script PowerShell que compila automáticamente todas las dependencias y el ejecutable principal.

```powershell
# Desde la raíz del proyecto
.\build_x64.ps1
```

**¿Qué hace este script?**
1. ✅ Verifica la instalación de Visual Studio
2. ✅ Compila **zlib** (x64 Release)
3. ✅ Compila **mbedTLS** (3.x con PSA deshabilitado)
4. ✅ Compila **CryptoPP, libpng, CxImage, id3lib, ResizableLib, miniupnpc**
5. ✅ Ejecuta `copy_libs.bat` para organizar binarios
6. ✅ Compila **emule.vcxproj** con optimizaciones (LTCG, AVX2)

**Resultado esperado**:
```
========================================
  BUILD EXITOSO
========================================
  Ejecutable: srchybrid\x64\Release\emule.exe
========================================
```

---

## 🔧 Compilación Manual (Paso a Paso)

Si prefieres controlar el proceso o el script falla, sigue estos pasos:

### Fase 1: Compilar Dependencias

Todas las dependencias deben compilarse en configuración **Release | x64** con las siguientes características comunes:
- **Runtime Library**: `/MD` (Multi-threaded DLL)
- **Platform Toolset**: `v145`
- **Windows SDK**: `10.0` (última disponible)

#### 1.1. zlib (Compresión)

```powershell
cd zlib\contrib\vstudio\vc17
msbuild zlibvc.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**Salida**: `zlib\contrib\vstudio\vc\x64\Release\zlib.lib`

#### 1.2. mbedTLS (Criptografía TLS/SSL)

```powershell
cd mbedtls\visualc\VS2017
msbuild mbedTLS.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**Configuraciones críticas**:
- ✅ `MBEDTLS_MD4_C` habilitado (requerido por eMule)
- ✅ `MBEDTLS_RIPEMD160_C` habilitado
- ⛔ PSA Crypto deshabilitado (stubs implementados)

**Salida**: `mbedtls\visualc\VS2017\x64\Release\mbedTLS.lib`

#### 1.3. CryptoPP (MD4/SHA)

```powershell
cd cryptopp
msbuild cryptlib.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**Salida**: `cryptopp\x64\Release\cryptlib.lib`

#### 1.4. libpng (Gráficos)

```powershell
cd libpng\projects\vstudio
msbuild libpng.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**Salida**: `libpng\projects\vstudio\x64\Release\libpng16.lib`

#### 1.5. CxImage (Procesamiento de Imágenes)

```powershell
cd cximage
msbuild cximage.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**Configuración crítica** en `ximacfg.h`:
```cpp
#define CXIMAGE_SUPPORT_PNG 1
```

**Salida**: `cximage\x64\Release\cximage.lib`

#### 1.6. id3lib (Metadatos MP3)

```powershell
cd id3lib\libprj
msbuild id3lib.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**⚠️ Definición CRÍTICA**: `ID3LIB_LINKOPTION=1` (fuerza enlace estático, evita símbolos `__imp__`)

**Salida**: `id3lib\libprj\x64\Release\id3lib.lib`

#### 1.7. ResizableLib (Controles UI)

```powershell
cd resizablelib\ResizableLib
msbuild ResizableLib.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**Cambio clave**: Migrado a **Unicode** (antes MBCS) para coincidir con eMule.

**Salida**: `resizablelib\x64\Release\resizablelib.lib`

#### 1.8. miniupnpc (UPnP)

```powershell
cd miniupnpc\msvc
msbuild miniupnpc.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**Salida**: `miniupnpc\msvc\x64\Release\miniupnpc.lib`

### Fase 2: Organizar Librerías (opcional si usas script)

Si compilaste manualmente, ejecuta el script de utilidad:

```cmd
copy_libs.bat
```

Esto mueve `cryptlib.lib` y otros binarios a las ubicaciones esperadas por `emule.vcxproj`.

### Fase 3: Compilar eMule Principal

#### Opción A: Desde Visual Studio (GUI)

1. Abrir `srchybrid\emule.sln` en Visual Studio 2022
2. Seleccionar **Release** | **x64** en la barra superior
3. Menú **Compilar** → **Recompilar solución** (Ctrl+Alt+F7)

#### Opción B: Desde línea de comandos

```powershell
cd srchybrid
msbuild emule.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```

**Optimizaciones activas**:
- ✅ **LTCG** (Link-Time Code Generation)
- ✅ **AVX2** (si tu CPU lo soporta)
- ✅ **WholeProgramOptimization**

**Resultado esperado**:
```
emule.vcxproj -> C:\...\srchybrid\x64\Release\emule.exe
```

---

## ✅ Verificación del Ejecutable

### 1. Comprobar Arquitectura

```cmd
dumpbin /headers srchybrid\x64\Release\emule.exe | findstr "machine"
```

**Salida esperada**: `8664 machine (x64)`

### 2. Comprobar Dependencias DLL

```cmd
dumpbin /dependents srchybrid\x64\Release\emule.exe
```

**DLLs esperadas** (sistema):
- `KERNEL32.dll`, `USER32.dll`, `GDI32.dll`
- **MFC140U.dll** (MFC Unicode)
- `WS2_32.dll` (Winsock 2)
- `CRYPT32.dll`, `BCRYPT.dll` (Crypto API)

> No debe listar librerías personalizadas (zlib, cryptopp, etc.) porque están enlazadas estáticamente.

### 3. Verificar Versión

Ejecuta `emule.exe` y verifica la pantalla de inicio (Splash Screen):
- **Versión esperada**: `eMule 0.70b-Build26-R1.2-x64`

---

## 📐 Configuraciones Técnicas Avanzadas

### Proyecto Principal: emule.vcxproj

#### Runtime Library
```xml
<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary> <!-- /MD -->
```

**Razón**: MFC requiere enlace dinámico. Todas las dependencias deben usar `/MD` para evitar conflictos.

#### Precompiled Headers (PCH)

**Habilitado** globalmente (`stdafx.h`) excepto para:

- ❌ **`OtherFunctions.cpp`**: Desactivado (`NotUsing`)
  - **Razón**: Orden de inclusión crítico (`Resource.h` antes de `stdafx.h`) para acceso a macros `IDS_*`

```cpp
// OtherFunctions.cpp - Orden correcto
#include "stdafx.h"
#include "Resource.h"
#include "OtherFunctions.h"
#include "MD4.h"
#include "CxImage/xImage.h"
```

#### Definiciones del Preprocesador

```
ID3LIB_LINKOPTION=1         // Fuerza enlace estático de id3lib
MINIUPNP_STATICLIB          // Enlace estático miniupnpc
SUPPORT_LARGE_FILES         // Archivos >2GB
XP_BUILD                    // (Legacy, planificado remover)
```

### Stubs Implementados

Debido a limitaciones en mbedTLS 3.x y compatibilidad legacy:

#### mbedTLS (PSA Crypto)

Las siguientes funciones TLS 1.3 avanzadas están "stubeadas" (sin funcionalidad real):

```cpp
// WebSocket.cpp - Líneas deshabilitadas
// mbedtls_ssl_conf_session_tickets_cb(...)
// mbedtls_ssl_conf_new_session_tickets(...)
// mbedtls_ssl_conf_tls13_key_exchange_modes(...)
```

**Impacto**: Conexiones TLS 1.2 funcionan normalmente. TLS 1.3 avanzado no soportado.

---

## 🐛 Solución de Problemas

### Error: LNK1181 (Archivo .lib no encontrado)

**Causa**: Las librerías no están en las rutas esperadas.

**Solución**:
1. Verificar que todas las dependencias compilaron exitosamente
2. Ejecutar `copy_libs.bat` manualmente
3. Revisar las rutas en `emule.vcxproj` (sección `<AdditionalDependencies>`)

### Error: C1083 (Header no encontrado - cryptopp/md4.h)

**Causa**: Headers de CryptoPP no presentes tras limpieza.

**Solución**:
```powershell
# Restaurar headers desde Forge (si disponible)
xcopy ..\eMule-Aishor-Forge\cryptopp\*.h cryptopp\ /Y
```

O recompilar desde fuente original de CryptoPP.

### Error: MSB8020 (Conflicto de Platform Toolset)

**Causa**: Una dependencia usa toolset diferente (v143, v141).

**Solución**:
```powershell
# Forzar v145 en todas las dependencias
msbuild proyecto.vcxproj /p:PlatformToolset=v145 /p:Configuration=Release /p:Platform=x64
```

### Error: CVT1100 (Manifest duplicado)

**Causa**: Inclusión manual del manifest en `emule.rc` + generación automática.

**Solución**:
1. Abrir `emule.vcxproj` en VS
2. Propiedades → Linker → Manifest File
3. Verificar `Generate Manifest = Yes`
4. Comentar líneas de manifest en `emule.rc` si existen

### Warning: LNK4099 (PDB no encontrado)

**Impacto**: Solo afecta depuración. El ejecutable Release funciona correctamente.

**Solución (opcional)**: Recompilar dependencias con `/Zi` (genera PDB).

### Visual: Columnas de lista muy estrechas (High DPI)

**Causa**: Bug conocido en controles legacy MFC con escalado High DPI.

**Impacto**: Solo visual. Funcionalidad no afectada.

---

## 📊 Tamaños de Referencia

| Componente | Tamaño (.lib) |
|------------|---------------|
| zlib.lib | ~570 KB |
| mbedTLS.lib | ~11.8 MB |
| cryptlib.lib | ~45 MB |
| libpng16.lib | ~320 KB |
| cximage.lib | ~1.2 MB |
| id3lib.lib | ~780 KB |
| resizablelib.lib | ~180 KB |
| miniupnpc.lib | ~95 KB |
| **emule.exe** | **~6.0 MB** |

---

## 📚 Referencias

- [Documentación oficial eMule](https://www.emule-project.net)
- [mbedTLS Documentation](https://mbed-tls.readthedocs.io/)
- [CryptoPP Wiki](https://www.cryptopp.com/wiki/)
- [Visual Studio 2022 Release Notes](https://docs.microsoft.com/en-us/visualstudio/releases/2022/)

---

## 🔄 Changelog de la Guía

- **07/01/2026**: Versión inicial basada en R1.2 Titanium Fiber (x64)
- Fusión de `Compilacion.md` y `GUIA_COMPILACION.md` legacy

---

**© 2026 eMule-Aishor Project**
