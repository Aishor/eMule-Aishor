# Release v0.70b-Build26-R1.0

**Fecha de Release:** 2026-01-05  
**Versión:** 0.70b-Build26-R1.0

## 🎯 Resumen

Primera release oficial de **eMule-Aishor** con soporte completo para **Visual Studio 2022** y **Windows 10/11**. Esta versión moderniza completamente la infraestructura de compilación manteniendo la compatibilidad con la base de código eMule 0.70b.

---

## 🆕 Novedades Principales

### Modernización de Compilación
- ✅ **Visual Studio 2022** (PlatformToolset v145)
- ✅ **Windows SDK 10.0.26100.0**
- ✅ **Runtime Unificado**: `/MD` (Multi-threaded DLL) en todas las dependencias
- ✅ **Unicode nativo** en toda la aplicación

### Correcciones Técnicas
- ✅ Orden de includes corregido en `OtherFunctions.cpp` (soporte PCH restaurado)
- ✅ Implementación de stubs para mbedTLS (TLS 1.3 básico)
- ✅ Soporte completo para CxImage PNG
- ✅ Eliminación de conflictos de Runtime Library

---

## 📦 Dependencias Actualizadas

| Librería | Versión | Cambios |
|----------|---------|---------|
| **zlib** | 1.2.x | Migrado a /MD, v145 |
| **libpng** | 1.6.x | Migrado a /MD, v145 |
| **mbedTLS** | 3.x | PSA_CRYPTO deshabilitado, stubs TLS 1.3 |
| **id3lib** | 3.8.3 | Enlace estático, v145 |
| **CryptoPP** | 8.9.0 | Migrado a /MD, v145 |
| **CxImage** | 7.0.2 | PNG habilitado via `ximacfg.h` |
| **ResizableLib** | - | Unicode, /MD, v145 |
| **MiniUPnPc** | 2.x | Compilación estática, v145 |

---

## 🔧 Cambios Técnicos Detallados

### Proyecto Principal (`emule.vcxproj`)
- Actualizado PlatformToolset a `v145`
- RuntimeLibrary cambiado a `/MD`
- WindowsTargetPlatformVersion: `10.0.26100.0`
- CharacterSet: `Unicode`

### OtherFunctions.cpp
**Problema:** Errores de compilación por orden incorrecto de includes
**Solución:** 
```cpp
#include "stdafx.h"
#include "Resource.h"
#include "OtherFunctions.h"
#include "MD4.h"
#include "CxImage/xImage.h"
```

### mbedTLS (PSA_CRYPTO)
**Problema:** Funciones PSA Crypto no implementadas
**Solución:** Stubs para funciones TLS 1.3:
- `mbedtls_ssl_conf_session_tickets_cb`
- `mbedtls_ssl_conf_new_session_tickets`
- `mbedtls_ssl_conf_tls13_key_exchange_modes`

### CxImage
**Problema:** `CXIMAGE_FORMAT_PNG` no definido
**Solución:** Actualizado `ximacfg.h` con:
```cpp
#define CXIMAGE_SUPPORT_PNG 1
```

---

## ⚠️ Problemas Conocidos

### mbedTLS - Limitaciones PSA_CRYPTO
- **Estado:** PSA_CRYPTO está deshabilitado mediante stubs
- **Impacto:** Funcionalidades TLS 1.3 avanzadas limitadas
- **Workaround:** Las conexiones TLS 1.2 funcionan normalmente
- **Fix futuro:** Implementación completa de PSA_CRYPTO en siguiente release

### CxImage - Formatos Limitados
- **Estado:** Solo formato PNG habilitado
- **Impacto:** Otros formatos (GIF, JPEG, etc.) requieren configuración adicional
- **Workaround:** Modificar `ximacfg.h` manualmente si necesitas otros formatos

---

## 📝 Archivos Modificados

### Código Fuente Principal
- `srchybrid/Version.h` - Nueva cadena de versión
- `srchybrid/OtherFunctions.cpp` - Orden de includes corregido
- `srchybrid/SendMail.cpp` - Stubs mbedTLS
- `srchybrid/WebSocket.cpp` - Stubs mbedTLS
- `srchybrid/BaseClient.cpp` - Soporte CXIMAGE_FORMAT_PNG

### Configuraciones de Proyecto
- `srchybrid/emule.vcxproj`
- `zlib/contrib/vstudio/vc17/zlibstat.vcxproj`
- `id3lib/libprj/id3lib.vcxproj`
- `mbedtls/visualc/VS2017/mbedTLS.vcxproj`
- `resizablelib/ResizableLib/ResizableLib.vcxproj`
- `cryptopp/cryptlib.vcxproj`

### Headers de Configuración
- `cximage/ximacfg.h`
- `mbedtls/include/mbedtls/mbedtls_config.h`
- `id3lib/include/*.h`

---

## 📊 Estadísticas del Proyecto

```
Archivos modificados: 112
Líneas de código: ~500,000
Tamaño del ejecutable: 5.3 MB
Dependencias: 8 librerías externas
Tiempo de compilación: ~3-5 minutos (Release/Win32)
```

---

## 🚀 Instrucciones de Compilación

### Requisitos Previos
- Visual Studio 2022 Community o superior
- Windows SDK 10.0.26100.0
- Git (para clonar el repositorio)

### Compilar
```powershell
# Clonar repositorio
git clone https://github.com/[usuario]/eMule-Aishor.git
cd eMule-Aishor

# Abrir solución
start srchybrid/emule.sln

# O compilar desde línea de comandos
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
  srchybrid\emule.vcxproj `
  /p:Configuration=Release `
  /p:Platform=Win32 `
  /p:WindowsTargetPlatformVersion=10.0.26100.0
```

---

## 📌 Próximas Versiones

### v0.70b-Build26-R1.1 (Hotfix)
- Corrección de bugs menores reportados
- Optimizaciones de rendimiento

### v0.70b-Build26-R2.0 (Feature Release)
- Implementación completa de PSA_CRYPTO en mbedTLS
- Soporte para todos los formatos de CxImage
- Migración a arquitectura x64

---

## 🙏 Agradecimientos

- Equipo original de eMule por el proyecto base
- Comunidad de desarrolladores de librerías open-source
- Todos los que contribuyeron con reportes y pruebas

---

## 📄 Licencia

Este proyecto está bajo la licencia **GPL v2**.  
Ver [LICENSE](../LICENSE) para más información.

---

**Nota**: Esta es una modernización no oficial de eMule. No está afiliada con el proyecto oficial eMule.
