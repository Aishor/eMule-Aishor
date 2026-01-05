# Mejoras Técnicas Aplicadas - eMule Aishor (Forge Edition)

Este documento detalla las mejoras arquitectónicas, de rendimiento y mantenibilidad implementadas en la versión **0.70b-Forge** respecto a la base de código original.

## 1. Modernización del Entorno (Toolchain)
*   **Visual Studio 2022 (v145)**: Migración completa desde entornos obsoletos (VS2002/2003/2010). Esto permite usar las últimas optimizaciones del compilador MSVC, mejorando la generación de código y la detección de errores en tiempo de compilación.
*   **Windows SDK 10.0.26100**: Actualización al SDK moderno de Windows, garantizando compatibilidad total con Windows 10 y Windows 11, y acceso a APIs modernas si fuera necesario.

## 2. Arquitectura y Rendimiento
*   **Runtime Dinámico (/MD)**: 
    *   **Mejora**: Se estandarizó todo el proyecto y sus dependencias (zlib, png, cripto) para usar la librería de tiempo de ejecución C dinámica Multi-hilo.
    *   **Beneficio**: Reducción del tamaño del ejecutable final, mejor gestión de memoria compartida con el sistema operativo y eliminación de conflictos de enlace (LNK2038).
*   **Enlace Estático de Dependencias**:
    *   **Mejora**: Librerías como `id3lib`, `mbedTLS`, `zlib` y `libpng` se integran directamente en el ejecutable `emule.exe`.
    *   **Beneficio**: "DLL Hell" eliminado. El ejecutable es portable y no requiere instalar DLLs de terceros en el sistema del usuario (salvo las redistribuibles de VC++).
*   **Unicode Nativo**:
    *   **Mejora**: Unificación de la gestión de caracteres (especialmente en `ResizableLib` y `emule`).
    *   **Beneficio**: Soporte correcto para nombres de archivo con caracteres internacionales (CJK, Árabe, etc.) y emojis, eliminando conversiones ANSI defectuosas.

## 3. Seguridad y Criptografía
*   **mbedTLS Moderno**:
    *   **Mejora**: Actualización a la rama 3.x de mbedTLS (frente a versiones legacy).
    *   **Beneficio**: Corrección de vulnerabilidades históricas en protocolos SSL/TLS.
*   **Higiene de Interfaces**: 
    *   **Mejora**: Implementación de interfaces criptográficas limpias (stubs controlados para `PSA_CRYPTO`) que permiten una integración más granular y auditable.

## 4. Calidad de Código (Clean Code)
*   **Sanitización de Cabeceras (Headers)**:
    *   **Mejora**: Reorganización masiva en `OtherFunctions.cpp` y otros archivos críticos.
    *   **Beneficio**: Se eliminaron dependencias circulares y se establecieron prioridades de inclusión claras (`stdafx.h` > `Resource.h`), haciendo el código más robusto y rápido de compilar.
*   **Eliminación de Macros Conflictivas**:
    *   **Mejora**: Limpieza de definiciones preprocesador (ej. `ZLIB_WINAPI`, `ID3LIB_LINKOPTION`).
    *   **Beneficio**: Menor polución del espacio de nombres global y comportamiento más predecible de las librerías.
*   **PCH Selectivo**:
    *   **Mejora**: Estrategia inteligente de Precompiled Headers, desactivándolos quirúrgicamente donde causan conflictos irresolubles en lugar de parchear el código indiscriminadamente.

## 5. Mantenibilidad
*   **Estandarización de Proyectos (.vcxproj)**:
    *   Todos los proyectos de la solución ahora comparten configuraciones base comunes, facilitando futuras actualizaciones del compilador o cambios de flags globales.
