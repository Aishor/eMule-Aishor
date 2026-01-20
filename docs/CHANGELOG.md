# Changelog

## [v0.72b-Build26-R0.1.3-x64] - 2026-01-20 - FiberSight Pro

### ⭐ New Features
- **Kad Search Customization:**
  - Nueva UI en pestaña Búsqueda: Panel "Kad Search".
  - Parámetros personalizables: **Tiempo** (hasta 600s) y **Máx Resultados** (hasta 3000).
  - Lógica de servidor (`CSearchManager`) adaptada para priorizar límites de usuario.
- **Model Context Protocol (MCP) Final:**
  - Integración estable con Claude Desktop.
  - Documentación completa en `docs/mcp.md` y `docs/api.mcp.md`.
  - Herramientas: Control total de descargas, búsqueda, estadísticas y librería.

### 🛠 Improvements
- **UI Refinement:**
  - Restaurado layout vertical clásico para botones Start/More/Cancel.
  - Diseño compacto y alineado para nuevos controles Kad.
- **PDB Locking Fix:**
  - Script de compilación `build_x64.ps1` mejorado para limpieza agresiva de bloqueos de debug.
- **Documentation:**
  - Reestructuración de carpeta `docs/`.
  - Nuevo manual de usuario `docs/KadSearch.md`.

---

## [v0.72b-Build26-R0.1.1-x64] - 2026-01-14 - Sistema MCP/API

### feat
- **Sistema Dual MCP/API:** Verificado e integrado sistema de control conversacional completo
  - API REST (LLMApiServer): 15+ endpoints funcionales en puerto 4711 (C++)
  - MCP Server: 10 herramientas + 3 recursos para CHAMAN (Python)
  - Entorno virtual aislado: `tools/.venv/` para dependencias MCP
  - Scripts de instalación: `tools/install_mcp.bat` (crea .venv automáticamente)
  - Tests de verificación: `tools/test_mcp_dependencies.py`
  - Documentación: `tools/CHAMAN_MCP_CONFIG.md`
  - Script helper: `tools/activate_mcp_env.bat`
- **Búsqueda MCP Avanzada (OT_007):**
  - Soporte para métodos de búsqueda: `Global`, `Kad`, `Server`.
- **OT_007:** (2026-01-14) Implementada búsqueda avanzada en API/MCP (filtros size, type, availability) y paginación.
- **OT_008:** (2026-01-14) Habilitado acceso externo al servidor MCP mediante SSE (Server-Sent Events) en puerto 4712. de resultados.
- **Búsqueda MCP Básica (OT_006):**
  - Backend C++: Nuevos endpoints `/api/v1/search` y `/api/v1/search/results`.
  - Frontend MCP: Herramientas para buscar y descargar por hash.

### Archivos
- `tools/install_mcp.bat` (actualizado con entorno virtual)
- `tools/activate_mcp_env.bat` (nuevo)
- `tools/test_mcp_dependencies.py` (nuevo)
- `tools/CHAMAN_MCP_CONFIG.md` (nuevo, reemplaza CLAUDE_DESKTOP_CONFIG.md)
- `tools/.venv/` (entorno virtual, creado automáticamente)
- `srchybrid/LLMApiServer.cpp` (verificado existente, 969 líneas)
- `tools/emule_mcp_server.py` (verificado existente, 687 líneas)

---

## [v0.72b-Build26-R0.1.1-x64] - 2026-01-13 [v0.72b-Build26-R0.1.1-x64] - 2026-01-13

### 🔥 Major Changes
- **Refactor ZIP Engine:** Reemplazo completo de `CZIPFile` legacy por librería `minizip`.
    - Soporte nativo ZIP64 (archivos > 4GB).
    - Eliminación de estructuras manuales de cabecera ZIP propensas a errores.
    - Integración de fuentes `minizip` (zlib contrib) en el proyecto.

### 🐛 Bug Fixes
- **MediaInfo 64-bit I/O:** Corregido truncamiento de punteros de archivo en `MediaInfo.cpp`.
    - Reemplazo de `_lseek` (32-bit) por `_lseeki64` (64-bit).
    - Permite análisis correcto de metadatos (códecs, duración) en archivos de video > 2GB.

### 🛡️ Security & Stability
- **Auditoría x64:** Finalizada revisión de tipos base (`types.h`, `EMFileSize`).
- **IPFilter:** Validación de descompresión de reglas IPFilter mediante nuevo motor ZIP.
