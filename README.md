# eMule-Aishor (Titanium Fiber R0.1.1 "Broadband")

**Versión:** `0.72b-Build26-R0.1.1-x64`
**Estado:** Estable / Producción x64

## Descripción
Repositorio consolidado de eMule-Aishor optimizado para arquitectura **x64**. 
Esta versión (R0.1.1) marca un hito en la estabilidad y capacidad de manejo de archivos grandes, con una auditoría completa de 64-bit y nuevo motor de compresión.

## Características Nuevas (v0.72b)
*   **🚀 ZIP64 Nativo:** Implementación completa de `minizip` reemplazando el código legacy. Soporte real para comprimir/descomprimir archivos >4GB.
*   **🏗️ Full 64-bit I/O:** Auditoría y corrección total de punteros de archivo. Repara lectura de metadatos en archivos multimedia gigantes (>2GB) en `MediaInfo.cpp`.
*   **🛡️ Auditoría de Tipos:** Verificación exhaustiva de compatibilidad x64 en todos los módulos críticos.

## Características Base
*   **Arquitectura:** x64 Nativo (AVX2 Enabled).
*   **Toolset:** Visual Studio 2022 (v145).
*   **Seguridad:** SSL/TLS habilitado (mbedTLS 3.6.2).
*   **Dependencias:** Pre-integradas (zlib, libpng, cryptopp, etc.).
*   **Red:** Ajustes de ventana TCP (`TcpWindowSize`) configurables.
*   **🤖 LLM Integration (FiberSight):** API REST/JSON para control por IA.

## Estructura del Repositorio
*   `srchybrid/`: Código fuente principal de eMule.
*   `[libs]/`: Carpetas de dependencias (headers + .lib).
*   `build_x64.ps1`: Script automatizado de compilación.

## Compilación Rápida
Ejecutar en PowerShell:
```powershell
.\build_x64.ps1
```
El ejecutable se generará en: `srchybrid\x64\Release\emule.exe`

## 📄 Documentación
*   **[Manual de Búsqueda Kad](docs/KadSearch.md)**: Guía para búsquedas personalizadas (Tiempo/Límite).
*   **[Integración MCP (Claude)](docs/mcp.md)**: Guía de instalación y uso de Model Context Protocol.
*   **[Referencia API MCP](docs/api.mcp.md)**: Lista completa de herramientas disponibles para LLMs.
*   **[Historial de Cambios](docs/CHANGELOG.md)**: Registro completo de actualizaciones.

## 📄 Releases
*   **RELEASE R0.1.1 (Broadband)** - 13/01/2026
    - **ZIP64**: Soporte archivos >4GB.
    - **MediaInfo**: Fix I/O 64-bit.
*   [RELEASE R1.3 (FiberSight)](docs/LLM_API.md) - 10/01/2026
*   [RELEASE R1.2 (Titanium Fiber)](docs/RELEASE_v0.70b-Build26-R1.2-X64.md) - 07/01/2026

## 📜 Licencia y Atribución
Fork derivado de [eMule v0.70b](https://github.com/irwir/eMule).
**Licencia**: GPL-2.0
**Copyright**: © 2026 Aishor Contributors | Version R0.1.1
