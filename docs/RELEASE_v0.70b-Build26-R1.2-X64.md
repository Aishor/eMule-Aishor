# eMule-Aishor Release v0.70b-Build26-R1.2-X64

**Nombre en Clave**: Titanium Fiber
**Fecha**: 07/01/2026
**Plataforma**: Windows x64
**Compilador**: Visual Studio 2022 (v145)

## 📝 Resumen
Esta versión **R1.2** marca la restauración completa de la capacidad de compilación en arquitecturas **x64** modernas, solucionando años de deuda técnica en de dependencias y configuraciones de proyecto. Se ha estabilizado el núcleo, actualizado la cadena de herramientas y preparado el terreno para futuras modernizaciones.

## 🚀 Novedades y Cambios

### ✨ Nuevas Características
*   **TcpWindowSize**: Opción configurable en `preferences.ini` para optimizar el tamaño de ventana TCP.
*   **Splash Screen Extendido**: La pantalla de carga y la ventana "Acerca de" ahora muestran la versión completa del build y la revisión (R1.2).
*   **Soporte x64 Nativo**: Binario compilado puramente para 64 bits para mejor gestión de memoria.

### 🛠️ Correcciones y Mantenimiento
*   **Compilación Restaurada**:
    *   Migración a Toolset **v145** (VS2022).
    *   Resolución de conflictos de librerías estáticas (`zlib`, `libpng`, `id3lib`, `mbedTLS`, `ResizableLib`).
    *   Corrección de errores de enlazado y definiciones XML corruptas en `emule.vcxproj`.
*   **Higiene de Código**:
    *   Scripts de mantenimiento movidos a carpeta dedicada.
    *   Limpieza de lógica obsoleta (`MaxHalfOpen`).
    *   Auditoría de seguridad en stubs criptográficos (`mbedTLS`, `WebSocket`).

## 📦 Instalación
Este release se distribuye como un binario portable.
1.  Reemplazar el archivo `emule.exe` existente con el suministrado en este paquete.
2.  (Opcional) Ejecutar el script `srchybrid/scripts/configure_firewall.ps1` como Administrador para configurar puertos automáticamente.

## 🔒 Verificación (Checksum)
**Archivo**: `emule.exe`
**Algoritmo**: SHA256
**Hash**: `7E9DE88772720D65FF47B06DC2A691C0557CD0FFC4C13B6885CB1EB8F402BA71`

---
*Hecho en La Fragua - 2026*
