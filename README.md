# eMule-Aishor (Titanium Fiber R1.1)

**Versión:** `0.70b-build26-R1.1-X64`
**Estado:** Estable / Consolidado

## Descripción
Repositorio consolidado de eMule-Aishor optimizado para arquitectura **x64**. 
Este proyecto integra todas las dependencias necesarias en forma de librerías estáticas (`.lib`) pre-compiladas, manteniendo el código fuente (`srchybrid`) limpio y enfocado.

## Características
*   **Arquitectura:** x64 Nativo (AVX2 Enabled)
*   **Toolset:** Visual Studio 2022 (v143) / 2017 (v141) compatible.
*   **Seguridad:** SSL/TLS habilitado (mbedTLS 3.6.2).
*   **Dependencias:** Pre-integradas (zlib, libpng, cryptopp, etc.).

## Estructura del Repositorio
*   `srchybrid/`: Código fuente principal de eMule.
*   `[libs]/`: Carpetas de dependencias (headers + .lib).
*   `build_x64.ps1`: Script automatizado de compilación.
*   `GUIA_COMPILACION.md`: Instrucciones detalladas de build.

## Compilación Rápida
Ejecutar en PowerShell:
```powershell
.\build_x64.ps1
```
El ejecutable se generará en: `srchybrid\x64\Release\emule.exe`

## Changelog Reciente
*   **06/01/2026**: Versión `0.70b-build26-R1.1-X64`. Consolidación final de librerías y limpieza de dependencias.
