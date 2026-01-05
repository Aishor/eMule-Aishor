# Release Notes: eMule Aishor v0.70b (Forge Edition)

**Versión**: 0.70b-Forge
**Fecha**: 2026-01-05
**Tipo**: Technical Release (Compilación)

## Resumen
Esta release marca el hito de la **migración completa a Visual Studio 2022 (Modern C++)**. No incluye nuevas funcionalidades de usuario final, pero establece una base técnica sólida para el desarrollo futuro, eliminando dependencias obsoletas y sistemas de construcción legados.

## Cambios Principales

### Infraestructura
- **Toolset Moderno**: Compilado con VS2022 (v145) y Windows SDK 10.0.26100.
- **Runtime Unificado**: Migración a `/MD` (Multi-threaded DLL) para reducir el tamaño del binario y mejorar la compatibilidad con el sistema operativo moderno.
- **Linkado Estático**: Librerías críticas (`id3lib`, `mbedtls`) compiladas estáticamente para portabilidad.

### Correcciones Técnicas
- **Unicode**: Soporte completo de Unicode en todas las capas de la aplicación y sus dependencias.
- **Seguridad**: Desactivación de interfaces criptográficas obsoletas o no utilizadas (`PSA_CRYPTO` en mbedTLS).
- **Estabilidad**: Solución a conflictos de Precompiled Headers (PCH) que impedían la compilación determinista.

## Instrucciones de Instalación
Esta es una build de desarrollo. Para ejecutar:
1. Copiar `srchybrid\Win32\Release\emule.exe` a una carpeta limpia.
2. Asegurar que el entorno tenga las DLLs redistribuibles de VC++ 2022 (aunque `/MD` las requiere del sistema).
3. Iniciar la aplicación.

## Problemas Conocidos
- Las funcionalidades avanzadas de seguridad de Kademlia podrían estar limitadas debido a los stubs en mbedTLS.
- La interfaz web (WebServer) no ha sido verificada exhaustivamente.
