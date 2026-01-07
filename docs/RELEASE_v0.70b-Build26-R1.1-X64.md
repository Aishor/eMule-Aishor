# Release v0.70b-Build26-R1.1-X64

**Fecha de Release:** 06/01/2026
**Versión:** v0.70b-Build26-R1.1-X64
**Codename:** Titanium Fiber (Beta)

## 🎯 Resumen
Esta versión intermedia se centró en la **consolidación de dependencias** y la preparación final para la compilación **x64 nativa y estable**. Se solucionaron los conflictos de enlazado y se limpió el árbol de dependencias.

## 🆕 Cambios Principales

### 🛠️ Build System
- **Consolidación de Librerías**: Todas las dependencias (`zlib`, `libpng`, `mbedTLS`, etc.) fueron recompiladas estáticamente para x64 con el Toolset v145 y Runtime `/MD`.
- **Limpieza de Proyecto**: Eliminación de referencias a rutas hardcodeadas y archivos obsoletos.
- **Preparación x64**: Corrección de advertencias y errores específicos de arquitectura de 64 bits (punteros, tamaños de tipos).

### 🔧 Correcciones
- Solución al error de redefinición de símbolos en `mbedTLS`.
- Ajustes en `id3lib` para enlazado correcto en Release.
- Corrección de rutas de salida para binarios y archivos intermedios.

## 📦 Componentes
Esta release sentó las bases para la R1.2 final, asegurando que:
- El ejecutable `emule.exe` pudiera compilarse sin errores de enlazado.
- Las librerías `.lib` estuvieran sincronizadas con la configuración de compilación del proyecto principal.

## 🛠️ Compilación

Esta versión intermedia ya está integrada en R1.2. Para compilar desde fuentes, consulta la [**Guía de Compilación**](GUIA_COMPILACION.md) (basada en R1.2, compatible con R1.1).

---
*Release previa a la versión de producción R1.2.*
