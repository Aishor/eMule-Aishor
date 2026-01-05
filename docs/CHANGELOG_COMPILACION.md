# Changelog Técnico - eMule Aishor Forge

## [Unreleased] - 2026-01-05

### Sistema de Construcción (Build System)
*   **General**: Migración completa de todo el proyecto y sus dependencias a Visual Studio 2022 (v145).
*   **Runtime**: Estandarización de todas las librerías estáticas y el proyecto principal a **Multi-threaded DLL (/MD)** en Release, eliminando conflictos `LNK2038`.
*   **Unicode**: Unificación de `CharacterSet` a Unicode en dependencias clave (`ResizableLib`).

### Dependencias
*   **id3lib**:
    *   Corregido `ID3LIB_LINKOPTION` a `1` para forzar enlace estático verdadero.
    *   Eliminada dependencia de símbolos DLL import (`__imp__`).
*   **mbedTLS**:
    *   Actualizado a v145.
    *   Deshabilitado subsistema `PSA_CRYPTO` para simplificar enlazado.
    *   Implementados stubs para `psa_crypto_init`, `mbedtls_psa_crypto_free` y generación de claves RSA en `OtherFunctions.cpp`.
*   **zlib/libpng**:
    *   Reconfigurados para `/MD` y eliminación de macros conflictivas `ZLIB_WINAPI`.

### Código Fuente (Fixes)
*   **OtherFunctions.cpp**:
    *   Limpieza masiva de directivas `#include`.
    *   Reordenamiento de cabeceras para priorizar `stdafx.h`, `Resource.h`, `OtherFunctions.h` y `MD4.h`.
    *   Implementación de función legacy perdida `__ascii_stricmp`.
    *   Inyección de `MDX_DIGEST_SIZE` vía inclusión de `MD4.h`.
*   **emule.vcxproj**:
    *   Desactivación de Precompiled Headers (PCH) específicamente para `OtherFunctions.cpp` para resolver conflictos de resolución de macros de recursos.
    *   Corrección de rutas de librerías (`cryptlib.lib`, `htmlhelp.lib`).

### Artefactos
*   Generación exitosa de `emule.exe` (Win32/Release).
