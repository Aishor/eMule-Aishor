# Modificaciones para Entorno Forge - miniupnpc

## Versión Original
miniupnpc 2.2.8

## Cambios Aplicados para Compilación en VS2022

### 1. Creación de Proyecto Moderno (.vcxproj)
**Archivo:** `msvc/miniupnpc.vcxproj` (NUEVO)

**Razón:** El proyecto original usaba formato `.vcproj` de Visual Studio 2005/2008, incompatible con VS2022. Se creó un archivo `.vcxproj` moderno compatible.

**Configuración:**
```xml
<PlatformToolset>v145</PlatformToolset>
<ConfigurationType>StaticLibrary</ConfigurationType>
<CharacterSet>MultiByte</CharacterSet>
```

**Archivos fuente incluidos:**
- `miniupnpc.c`
- `miniwget.c`
- `minisoap.c`
- `minixml.c`
- `igd_desc_parse.c`
- `upnpcommands.c`
- `upnpreplyparse.c`
- `upnperrors.c`
- `connecthostport.c`
- `portlistingparse.c`
- `receivedata.c`

### 2. Generación Manual de miniupnpcstrings.h
**Archivo:** `miniupnpcstrings.h` (NUEVO)

**Contenido:**
```c
#ifndef MINIUPNPCSTRINGS_H_INCLUDED
#define MINIUPNPCSTRINGS_H_INCLUDED

#define OS_STRING "Windows/22000"
#define MINIUPNPC_VERSION_STRING "2.2.8"

#endif
```

**Razón:** Este archivo normalmente se genera automáticamente por scripts de configuración (CMake/autoconf), pero no existe en compilaciones manuales con MSVC. Contiene información de versión y plataforma.

## Resultado
Compilación exitosa como librería estática (`miniupnpc.lib`) sin errores.

## Configuración del Proyecto
- **PlatformToolset:** v145 (Visual Studio 2022)
- **WindowsSDK:** 10.0.26100.0
- **ConfigurationType:** StaticLibrary
- **Ubicación de salida:** `msvc/Release/miniupnpc.lib`

## Notas
- El proyecto original no tenía soporte nativo para VS2022.
- Se mantuvo la compatibilidad total con la API de miniupnpc.
- La librería resultante es compatible con eMule para funcionalidad UPnP.

---
*Documentado: 2026-01-05*
*Entorno: eMule-Aishor-Forge*
