# Documentación de Modificaciones - Librerías Forge

Esta carpeta contiene la documentación detallada de todas las modificaciones aplicadas a las dependencias de eMule-Aishor durante la migración al entorno **Forge** con Visual Studio 2022.

## Documentos Disponibles

### 📄 Resumen Consolidado
- **[CHANGES_FORGE_SUMMARY.md](CHANGES_FORGE_SUMMARY.md)** - Resumen ejecutivo de todas las modificaciones aplicadas a las 8 librerías.

### 📚 Documentación por Librería

| Librería | Documento | Estado Compilación |
|:---|:---|:---:|
| **id3lib 3.8.3** | [CHANGES_FORGE_id3lib.md](CHANGES_FORGE_id3lib.md) | ✓ |
| **libpng 1.6.44** | [CHANGES_FORGE_libpng.md](CHANGES_FORGE_libpng.md) | ✓ |
| **zlib 1.3.1.2** | [CHANGES_FORGE_zlib.md](CHANGES_FORGE_zlib.md) | ✓ |
| **mbedTLS 3.6.2** | [CHANGES_FORGE_mbedtls.md](CHANGES_FORGE_mbedtls.md) | ✓ |
| **miniupnpc 2.2.8** | [CHANGES_FORGE_miniupnpc.md](CHANGES_FORGE_miniupnpc.md) | ✓ |
| **cryptopp 8.9.0** | *Sin cambios requeridos* | ✓ |
| **ResizableLib 1.3** | *Sin cambios requeridos* | ✓ |
| **CxImage 6.0.0** | *Incompatible con libpng 1.6+* | ❌ |

## Contenido de Cada Documento

Cada archivo `CHANGES_FORGE_*.md` incluye:

1. **Versión Original** - Identificación de la versión de la librería
2. **Cambios Aplicados** - Lista detallada con:
   - Archivos afectados
   - Números de línea modificados
   - Código antes/después
   - Razón técnica del cambio
3. **Resultado** - Estado de compilación
4. **Configuración del Proyecto** - Parámetros de VS2022

## Contexto del Entorno Forge

**Forge** es el entorno de compilación modernizado de eMule-Aishor que utiliza:
- Visual Studio 2022 (v145)
- Windows SDK 10.0.26100.0
- Compilación estática de todas las dependencias

## Resumen de Modificaciones por Complejidad

| Nivel | Librería | Archivos Modificados | Tipo de Cambios |
|:---:|:---|:---:|:---|
| 🔴 Alta | id3lib | 20+ | Eliminación de macros DLL, fixes de namespace |
| 🟡 Media | libpng | 2 | Conversión estática, cambio de zlib.lib |
| 🟡 Media | miniupnpc | 2 | Creación de proyecto moderno |
| 🟢 Baja | mbedTLS | 1 | Exclusión de tests |
| 🟢 Baja | zlib | 1 | Actualización de toolset |

## Problemas Conocidos

### CxImage - Incompatibilidad con libpng 1.6+

CxImage 6.0.0 no es compatible con libpng 1.6+ debido al cambio a estructuras opacas (`png_info_def`). Se requiere:
- Parche extenso de `ximapng.cpp` (100+ cambios), o
- Versión actualizada de CxImage, o
- Downgrade de libpng (no recomendado)

**Estado:** Bloqueando compilación del proyecto principal eMule.

## Enlaces Relacionados

- [Manual de Compilación](../Compilacion.md) - Guía general de compilación
- [Roadmap](../Roadmap.md) - Planificación del proyecto
- Ubicación original: `C:\Fragua\eMule-Aishor-Forge\CHANGES_FORGE_SUMMARY.md`

---
*Última actualización: 2026-01-05*  
*Entorno: eMule-Aishor-Forge*  
*Documentado por: Antigravity AI - Tech Lead*
