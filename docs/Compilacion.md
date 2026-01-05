# Guía de Compilación eMule-Aishor (VS2022 / v145)

## 1. Entorno de Desarrollo
*   **IDE**: Visual Studio 2022 Community
*   **Toolset**: v145
*   **SDK**: Windows 10.0.26100.0 (o compatible)
*   **Arquitectura**: x86 (Win32)

## 2. Dependencias Externas (Libs)
Todas las dependencias han sido migradas a **Visual Studio 2022** y configuradas para enlace dinámico de runtime (`/MD`) para compatibilidad con MFC.

### zlib (Estática)
*   **Ubicación**: `zlib\contrib\vstudio\vc17\zlibstat.vcxproj`
*   **Configuración**:
    *   Runtime: `/MD` (Release), `/MDd` (Debug)
    *   Sin `ZLIB_WINAPI` (convención de llamada cdecl estándar).

### id3lib (Estática)
*   **Ubicación**: `id3lib\libprj\id3lib.vcxproj`
*   **Importante**: Definir `ID3LIB_LINKOPTION=1` en los preprocesadores tanto de la lib como del proyecto principal para forzar enlace estático real y evitar símbolos `__imp__`.

### mbedTLS (Estática)
*   **Ubicación**: `mbedtls\visualc\VS2017\mbedTLS.vcxproj`
*   **Configuración**:
    *   Excluido `psa_crypto` (feature compleja no usada críticamente, stubs implementados).
    *   Activado `MBEDTLS_MD4_C` y `MBEDTLS_RIPEMD160_C`.

### ResizableLib (Estática)
*   **Ubicación**: `resizablelib\ResizableLib\ResizableLib.vcxproj`
*   **Cambio Clave**: Migrado a `Unicode` (antes NotSet/MBCS) para coincidir con eMule.

### Cryptopp (Estática)
*   **Ubicación**: `cryptopp\cryptlib.vcxproj`
*   **Cambio Clave**: Migrado a v145 y runtime `/MD`.

## 3. Proyecto Principal (emule.vcxproj)
*   **Runtime Library**: Multi-threaded DLL (`/MD`).
*   **Precompiled Headers (PCH)**:
    *   Habilitado (`Use`) globalmente (`stdafx.h`).
    *   **EXCEPCIÓN CRÍTICA**: Desactivado (`NotUsing`) para `OtherFunctions.cpp` debido a conflictos de inclusión circular con `Opcodes.h` y `Resource.h`.
*   **Definiciones Añadidas**: `ID3LIB_LINKOPTION=1`.

## 4. Proceso de Compilación
1. Abrir `emule.sln` (o cargar los proyectos individuales).
2. Seleccionar configuración **Release** | **Win32**.
3. Compilar ordenadamente:
   1. `zlib`
   2. `libpng`
   3. `mbedTLS`
   4. `id3lib`
   5. `ResizableLib`
   6. `cryptlib`
   7. `emule` (Proyecto principal)
4. El ejecutable se generará en `srchybrid\Win32\Release\emule.exe`.

## 5. Notas de Mantenimiento
*   **OtherFunctions.cpp**: Este archivo es delicado. Requiere un orden de inclusión específico y PCH desactivado en el archivo de proyecto para compilar correctamente las macros de recursos (`IDS_*`).
*   **Stubs**: Se han añadido implementaciones vacías para funciones de `mbedtls` (PSA interfaces) y `__ascii_stricmp` (compatibilidad legacy) en `OtherFunctions.cpp`. Documentar si se requiere implementación real a futuro.
