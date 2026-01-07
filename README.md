# eMule-Aishor (Titanium Fiber R1.2)

**Versión:** `0.70b-Build26-R1.2-X64`
**Estado:** Estable / Producción

## Descripción
Repositorio consolidado de eMule-Aishor optimizado para arquitectura **x64**. 
Este proyecto integra todas las dependencias necesarias en forma de librerías estáticas (`.lib`) pre-compiladas, manteniendo el código fuente (`srchybrid`) limpio y enfocado.

## Características
*   **Arquitectura:** x64 Nativo (AVX2 Enabled).
*   **Toolset:** Visual Studio 2022 (v145).
*   **Seguridad:** SSL/TLS habilitado (mbedTLS 3.6.2).
*   **Dependencias:** Pre-integradas (zlib, libpng, cryptopp, etc.).
*   **Red**: Ajustes de ventana TCP (`TcpWindowSize`) configurables.

## Estructura del Repositorio
*   `srchybrid/`: Código fuente principal de eMule.
*   `[libs]/`: Carpetas de dependencias (headers + .lib).
*   `build_x64.ps1`: Script automatizado de compilación.
*   `GUIA_COMPILACION.md`: Instrucciones detalladas de build.

## Compilación Rápida
Ejecutar en PowerShell (Admin recomendado para dependencias):
```powershell
.\build_x64.ps1
```
El ejecutable se generará en: `srchybrid\x64\Release\emule.exe`

## 📄 Releases
*   [**RELEASE R1.2 (Titanium Fiber)**](docs/RELEASE_v0.70b-Build26-R1.2-X64.md) - 07/01/2026
*   [RELEASE R1.0 (Legacy)](docs/RELEASE_v0.70b-Build26-R1.0.md)

## Changelog Reciente
*   **07/01/2026**: Versión `R1.2`. Restauración completa de build system (v145), fix de Splash Screen y configuración de puertos Firewall.
