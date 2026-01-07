# Changelog

Todos los cambios notables en este proyecto serán documentados en este archivo.

## [v0.70b-Build26-R1.2-X64] - 2026-01-06

### Añadido
- **TcpWindowSize**: Nueva opción configurable en `Preferences.ini` (`TcpWindowSize`) para establecer manualmente el tamaño de ventana TCP (SO_RCVBUF, SO_SNDBUF).
  - Implementado en `src/Preferences.cpp` y `src/AsyncSocketEx.cpp`.
  - Configurable vía edición directa de `preferences.ini` (sección `[eMule]`, clave `TcpWindowSize`).
- **Build System**: Soporte para Toolset `v145` (Visual Studio 2022 Preview/Next) en scripts de compilación automatizada.
- **Detección Automática**: `compila_x64.bat` ahora utiliza `vswhere.exe` para localizar la instalación de MSBuild, eliminando la dependencia de rutas hardcodeadas.

### Cambiado
- **Versión**: Actualizada de R1.1 a R1.2 Titanium Fiber.
- **Scripts**: `build_x64.ps1` fuerza el uso del PlatformToolset `v145` para `mbedTLS` y `zlib` para resolver conflictos de compilación (`MSB8020`).
- **Limpieza**: Eliminación de código obsoleto relacionado con `MaxHalfOpen` en `ListenSocket.cpp`.

### Corregido
- **Compilación Forzada**: Solucionados errores de linkeo en `zlib` y `mbedTLS` causados por incompatibilidad de versiones de build tools.

## [Fix-Build-Restoration] - 2026-01-07
### Mantenimiento
- **Limpieza de Higiene**: Scripts de mantenimiento `.ps1` movidos a `srchybrid/scripts/`.
- **Dependencias**:
  - Recompiladas en `Forge` y enlazadas: `id3lib` (x64), `libpng` (x64), `miniupnpc`, `ResizableLib`.
  - Corrección de rutas de librerías en `emule.vcxproj` y `copy_libs.bat` implícita mediante copia manual.
- **Correcciones**:
  - Eliminado BOM/Espacios en blanco corruptos al inicio de `emule.vcxproj`.
  - Solucionado error de enlazado `png_longjmp` recompilando `CxImage` contra `libpng` actualizado.
  - **Splash Screen**: Actualizado formato de versión para mostrar Build y Revision (R1.2) en pantalla de carga y About.
