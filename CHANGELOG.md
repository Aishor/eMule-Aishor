# Changelog

## [v0.72b-Build26-R0.1.1-x64] - 2026-01-13

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
