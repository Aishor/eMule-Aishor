# eMule-Aishor

[![Version](https://img.shields.io/badge/version-0.70b--Build26--R1.0-blue.svg)](https://github.com/yourusername/eMule-Aishor/releases)
[![License](https://img.shields.io/badge/license-GPL%20v2-green.svg)](LICENSE)
[![Build](https://img.shields.io/badge/build-Visual%20Studio%202022-purple.svg)](https://visualstudio.microsoft.com/)

**eMule-Aishor** es un fork modernizado del cliente P2P eMule, actualizado para compilar en **Visual Studio 2022** con soporte completo para **Windows 10/11** y arquitectura **Win32/x64**.

---

## 🚀 Características

- ✅ **Compilación moderna**: Visual Studio 2022 (PlatformToolset v145)
- ✅ **Runtime unificado**: `/MD` (Multi-threaded DLL) en todas las dependencias
- ✅ **Unicode nativo**: Soporte completo para caracteres internacionales
- ✅ **Dependencias actualizadas**: zlib, mbedTLS, CxImage, CryptoPP, id3lib
- ✅ **Windows SDK 10.0**: Compatible con Windows 10/11

---

## 📦 Compilación

### Requisitos Previos

- **Visual Studio 2022 Community** (o superior)
  - Componente: "Desarrollo para el escritorio con C++"
  - Windows 10 SDK (10.0.26100.0 o superior)
- **Git** (para clonar el repositorio)

### Pasos de Compilación

1. **Clonar el repositorio**
   ```bash
   git clone https://github.com/yourusername/eMule-Aishor.git
   cd eMule-Aishor
   ```

2. **Abrir la solución**
   ```bash
   # Desde Visual Studio:
   # Archivo > Abrir > Proyecto/Solución > srchybrid/emule.sln
   ```

3. **Configurar y Compilar**
   - Seleccionar configuración: **Release** | **Win32**
   - Build > Compilar solución (Ctrl+Shift+B)

4. **Ejecutable generado**
   ```
   srchybrid/Win32/Release/emule.exe
   ```

### Compilación desde Línea de Comandos

```powershell
# Usando MSBuild
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
  srchybrid\emule.vcxproj `
  /p:Configuration=Release `
  /p:Platform=Win32 `
  /p:WindowsTargetPlatformVersion=10.0.26100.0
```

---

## 📚 Documentación

- **[Compilacion.md](docs/Compilacion.md)** - Guía detallada de compilación
- **[CHANGELOG_COMPILACION.md](docs/CHANGELOG_COMPILACION.md)** - Historial de cambios de compilación
- **[ESTADO_PROYECTO.md](docs/ESTADO_PROYECTO.md)** - Estado actual del proyecto
- **[emule-mejoras.md](docs/emule-mejoras.md)** - Mejoras técnicas implementadas

---

## 🔧 Dependencias

| Librería | Versión | Configuración |
|----------|---------|---------------|
| **zlib** | 1.2.x | `/MD`, v145 |
| **libpng** | 1.6.x | `/MD`, v145 |
| **mbedTLS** | 3.x | `/MD`, v145, PSA_CRYPTO=OFF |
| **id3lib** | 3.8.3 | Enlace estático, v145 |
| **CryptoPP** | 8.9.0 | `/MD`, v145 |
| **CxImage** | 7.0.2 | PNG habilitado |
| **ResizableLib** | - | Unicode, `/MD`, v145 |
| **MiniUPnPc** | 2.x | Estático, v145 |

---

## 📝 Notas de la Versión

### v0.70b-Build26-R1.0 (2026-01-05)

**Cambios principales:**
- Migración completa a Visual Studio 2022 (PlatformToolset v145)
- Corrección de orden de includes en `OtherFunctions.cpp` (PCH)
- Implementación de stubs para mbedTLS (PSA_CRYPTO deshabilitado)
- Soporte completo para CxImage PNG
- Unificación de Runtime Library a `/MD`

**⚠️ Nota importante:** Esta versión requiere recompilación de todas las dependencias con Visual Studio 2022.

Ver [RELEASE_v0.70b-Build26-R1.0.md](docs/RELEASE_v0.70b-Build26-R1.0.md) para detalles completos y [CHANGELOG_COMPILACION.md](docs/CHANGELOG_COMPILACION.md) para historial técnico.

---

## 🐛 Problemas Conocidos

- **mbedTLS**: PSA_CRYPTO está deshabilitado mediante stubs. Las funciones TLS 1.3 avanzadas están limitadas.
- **CxImage**: Solo formato PNG habilitado. Otros formatos requieren configuración adicional en `ximacfg.h`.

---

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Por favor:

1. Fork el proyecto
2. Crea una rama para tu feature (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

---

## 📄 Licencia

Este proyecto está bajo la licencia **GPL v2**. Ver [LICENSE](LICENSE) para más información.

---

## 🔗 Enlaces

- **Proyecto Original eMule**: [emule-project.net](https://www.emule-project.net/)
- **Documentación**: [docs/](docs/)
- **Releases**: [GitHub Releases](https://github.com/yourusername/eMule-Aishor/releases)

---

## 👨‍💻 Autor

**Aishor Team** - Modernización y mantenimiento

---

## 🙏 Agradecimientos

- Equipo original de eMule por el proyecto base
- Comunidad de desarrolladores de las librerías open-source utilizadas
- Todos los contribuidores del proyecto

---

**Nota**: Este es un proyecto de modernización no oficial de eMule. No está afiliado con el proyecto oficial eMule.
