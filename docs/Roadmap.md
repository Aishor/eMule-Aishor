# Roadmap de Desarrollo: eMule-Aishor

Este documento traza la ruta técnica para las futuras versiones del mod Aishor.

## Fase 2: Estabilización y Documentación (Actual)
- [x] Mapeo de módulos y dependencias.
- [/] Formalización del entorno de compilación.
- [ ] Resolución de errores de compilación en `CxImage` e `id3lib`.

## Fase 3: Optimizaciones de Rendimiento y Hashing
### Hashing MD4
- **Objetivo**: Implementar aceleración por hardware (SSE/AVX) para el cálculo del hash del archivo.
- **Impacto**: Reducción significativa del tiempo de subida/descarga de archivos grandes y menor uso de CPU durante el hashing inicial.

### Hashing SHA (AICH)
- **Objetivo**: Integrar implementaciones más modernas de SHA (SHA-256/3) para el sistema de árboles de hash (AICH).

## Fase 4: Refactorización y Sistema de Logs
- **Logging**: Migrar el sistema de logs actual a una arquitectura más flexible que permita niveles de log (Debug, Info, Warn, Error) y rotación de archivos.
- **Limpieza de Código**: Eliminar macros obsoletas de compatibilidad con Windows 9x/Me.

## Fase 5: Seguridad y Red
- **MbedTLS**: Mantener actualizada la librería mbedTLS para asegurar el protocolo de obfuscación y conexiones seguras.
- **UPnP**: Mejorar la detección y apertura de puertos mediante la actualización de `miniupnpc`.

---
*Este Roadmap es dinámico y se ajustará según las prioridades del proyecto.*
