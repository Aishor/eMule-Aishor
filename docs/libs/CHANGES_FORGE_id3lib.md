# Modificaciones para Entorno Forge - id3lib

## Versión Original
id3lib 3.8.3

## Cambios Aplicados para Compilación Estática en VS2022

### 1. Eliminación de Macros de Exportación DLL
**Archivos afectados:**
- `include/id3/utils.h`
- `include/id3/reader.h`
- `include/id3/writer.h`
- `include/id3/tag.h`
- `include/id3/field.h`
- `include/id3/io_strings.h`
- `include/id3/writers.h`
- `include/id3/readers.h`
- `include/id3/io_decorators.h`
- `include/id3/io_helpers.h`
- `src/utils.cpp`
- `src/c_wrapper.cpp`
- `src/tag.cpp`
- `src/field.cpp`
- `src/io.cpp`

**Cambio:** Se eliminaron todas las ocurrencias de `ID3_CPP_EXPORT` e `ID3_C_EXPORT` para evitar errores de vinculación incoherente (C2491, C4273) al compilar como librería estática.

**Razón:** Estas macros están diseñadas para exportación de DLL. En compilación estática con `ID3LIB_LINKOPTION=1`, deben estar vacías, pero su presencia causaba conflictos.

### 2. Anulación de Macros de Depuración
**Archivo:** `config.h`

**Cambio:**
```cpp
// Líneas 177-179
#undef ID3D_NOTICE
#define ID3D_NOTICE(x)
#undef ID3D_WARNING
#define ID3D_WARNING(x)
```

**Razón:** Las macros originales usaban sintaxis obsoleta de `iostream.h` incompatible con `<iostream>` moderno de C++.

### 3. Definición Manual de Macros de Versión
**Archivo:** `src/globals.cpp`

**Cambio:**
```cpp
// Líneas 34-40
#ifndef _ID3LIB_MAJOR_VERSION
#define _ID3LIB_MAJOR_VERSION 3
#define _ID3LIB_MINOR_VERSION 8
#define _ID3LIB_PATCH_VERSION 3
#define _ID3LIB_INTERFACE_AGE 0
#define _ID3LIB_BINARY_AGE 0
#endif
```

**Razón:** Resolver errores C2065 (identificador no declarado) en `globals.cpp`.

### 4. Corrección de Inclusión Circular
**Archivo:** `src/flags.h`

**Cambio:** Línea 31
```cpp
// Antes:
#include "flags.h"
// Después:
#include "id3/globals.h"
```

**Razón:** El archivo se incluía a sí mismo en lugar de incluir `globals.h` donde está definido `flags_t`.

### 5. Actualización de Cabecera Obsoleta
**Archivo:** `include/id3/id3lib_bitset`

**Cambio:**
```cpp
// Líneas 40-43
#include <string>
#include <stdexcept>
#include <iostream>
using namespace std;
```

**Razón:** Actualización de `iostream.h` obsoleto a `<iostream>` moderno.

### 6. Definición de Macros SGI STL
**Archivo:** `include/id3/id3lib_bitset`

**Cambio:** Líneas 44-54
```cpp
#ifndef __STL_BEGIN_NAMESPACE
#define __STL_BEGIN_NAMESPACE namespace std {
#endif
#ifndef __STL_END_NAMESPACE
#define __STL_END_NAMESPACE }
#endif
#ifndef __STL_THROW
#define __STL_THROW(x) throw x
#endif
```

**Razón:** id3lib_bitset utiliza código legacy de SGI STL que espera estas macros. Sin ellas, se producen errores de sintaxis (C2143, C4430).

### 7. Actualización de Tipo en writer.h
**Archivo:** `include/id3/writer.h`

**Cambio:** Línea 31
```cpp
#include <fstream>
```

**Razón:** Proveer la definición de `std::streamoff` necesaria para compatibilidad con iostream moderno.

## Resultado
Compilación exitosa como librería estática (`id3lib.lib`) con advertencias menores (C4244, C4275) que no afectan la funcionalidad.

## Configuración del Proyecto
- **PlatformToolset:** v145 (Visual Studio 2022)
- **WindowsSDK:** 10.0.26100.0
- **ConfigurationType:** StaticLibrary
- **Macros del Preprocesador:** `ID3LIB_LINKOPTION=1;ID3_WANT_NOTHROW`

---
*Documentado: 2026-01-05*
*Entorno: eMule-Aishor-Forge*
