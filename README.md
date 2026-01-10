# eMule-Aishor (Titanium Fiber R1.3)

**Versión:** `0.70.3-Build26-R1.3-X64`
**Estado:** En Desarrollo / Experimental

## Descripción
Repositorio consolidado de eMule-Aishor optimizado para arquitectura **x64**. 
Este proyecto integra todas las dependencias necesarias en forma de librerías estáticas (`.lib`) pre-compiladas, manteniendo el código fuente (`srchybrid`) limpio y enfocado.

## Características
*   **Arquitectura:** x64 Nativo (AVX2 Enabled).
*   **Toolset:** Visual Studio 2022 (v145).
*   **Seguridad:** SSL/TLS habilitado (mbedTLS 3.6.2).
*   **Dependencias:** Pre-integradas (zlib, libpng, cryptopp, etc.).
*   **Red**: Ajustes de ventana TCP (`TcpWindowSize`) configurables.
*   **🤖 LLM Integration:** API REST/JSON para control por IA (Claude, GPT-4, etc.).
*   **📊 Quality Detection:** Detector inteligente de calidad de video.

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
*   [**RELEASE R1.3 (LLM Integration)**](docs/LLM_API.md) - 10/01/2026 🆕
*   [RELEASE R1.2 (Titanium Fiber)](docs/RELEASE_v0.70b-Build26-R1.2-X64.md) - 07/01/2026
*   [RELEASE R1.1 (Consolidación)](docs/RELEASE_v0.70b-Build26-R1.1-X64.md) - 06/01/2026
*   [RELEASE R1.0 (Legacy)](docs/RELEASE_v0.70b-Build26-R1.0-X64.md) - 05/01/2026

*   **10/01/2026**: Versión `R1.3`. Integración con LLM mediante API REST/JSON. Detector inteligente de calidad de video.
*   **07/01/2026**: Versión `R1.2`. Restauración completa de build system (v145), fix de Splash Screen y configuración de puertos Firewall.

## 📜 Licencia y Atribución

Este proyecto es un **fork derivado** de [eMule v0.70b](https://github.com/irwir/eMule), 
mantenido por la comunidad en el repositorio de [irwir](https://github.com/irwir).

**Licencia**: GNU General Public License v2 (GPL-2.0)  
**Copyright**: © 2026 Aishor Contributors | Basado en eMule Project

Ver [LICENSE.txt](LICENSE.txt) para más detalles.

---
**Nota**: Esta es una modernización no oficial de eMule. No está afiliada con el proyecto oficial eMule.

