# eMule-Aishor (Titanium Fiber R0.1.1 "Broadband")

**Versión:** `0.72b-Build26-R0.1.2-x64`
**Estado:** Estable / Producción x64

## Descripción
Repositorio consolidado de eMule-Aishor optimizado para arquitectura **x64**. 
Esta versión (R0.1.2) integra completamente el control por IA (MCP) y búsquedas Kad avanzadas.

## Características Nuevas
*   **Kad Search Custom**: Búsqueda Kademlia con parámetros de tiempo y resultados personalizables.
*   **Model Context Protocol (MCP)**: Servidor integrado para control total mediante IA (Claude).
*   **🚀 ZIP64 Nativo:** Soporte real para archivos >4GB.
*   **🏗️ Full 64-bit I/O:** Auditoría total de punteros de archivo y MediaInfo.

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
*   `tools/`: Scripts de soporte MCP/Python.
*   `docs/`: Documentación del proyecto.

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
*   **[RELEASE R0.1.2 (FiberSight Pro)](docs/CHANGELOG.md)** - 20/01/2026 🆕
    - **Kad Search**: Búsqueda personalizada (Tiempo/Límite).
    - **MCP**: Integración completa con Claude.
    - **Docs**: Nueva documentación API y MCP.
*   [RELEASE R0.1.1 (Broadband)](docs/CHANGELOG.md) - 13/01/2026
    - **ZIP64**: Soporte archivos >4GB.
    - **MediaInfo**: Fix I/O 64-bit.
*   [RELEASE R1.3 (FiberSight)](docs/LLM_API.md) - 10/01/2026
*   [RELEASE R1.2 (Titanium Fiber)](docs/RELEASE_v0.70b-Build26-R1.2-X64.md) - 07/01/2026

## 📜 Licencia y Atribución
Fork derivado de [eMule v0.70b](https://github.com/irwir/eMule).
**Licencia**: GPL-2.0
**Copyright**: © 2026 Aishor Contributors | Version R0.1.1
