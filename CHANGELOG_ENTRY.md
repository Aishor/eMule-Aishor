---
**Fecha:** 2025-01-09  
**Tipo:** Feature  
**Descripción:** Fuentes configurables para controles de lista

**Archivos Modificados:**
- `srchybrid/Preferences.h` - Añadido `m_lfListText`, `GetListLogFont()`, `SetListFont()`
- `srchybrid/Preferences.cpp` - Carga/guardado de configuración de fuente de listas
- `srchybrid/Emule.h` - Añadido `CFont m_fontList`
- `srchybrid/Emule.cpp` - Inicialización de `m_fontList` en `CreateAllFonts()`
- `srchybrid/EmuleDlg.h` - Declaración de `ApplyListFont()`
- `srchybrid/EmuleDlg.cpp` - Implementación de aplicación dinámica de fuente a todas las listas
- `srchybrid/PPgDisplay.h` - Añadido enum `sfList`
- `srchybrid/PPgDisplay.cpp` - Selector de fuente con Shift+Click
- `srchybrid/MuleListCtrl.cpp` - Aplicación de fuente en `PreSubclassWindow()`

**Correcciones de Compilación:**
- Corregido orden de includes (`stdafx.h` primero) en: `Preferences.cpp`, `MuleListCtrl.cpp`, `PPgDisplay.cpp`
- Añadido `#include "Resource.h"` en: `PPgDisplay.h`, `SearchParamsWnd.h`, `StatisticsDlg.h`
- Añadido `#include "OtherFunctions.h"` en: `DownloadQueue.h`, `Friend.h`
- Corregido acceso a `searchlistctrl` vía `m_pwndResults` en `EmuleDlg.cpp`

**Scripts Añadidos:**
- `build_optimized.ps1` - Compilación optimizada con 4 hilos (evita congelamiento)
- `fix_headers.ps1` - Script para corregir includes faltantes
- `fix_includes.ps1` / `fix_pref.ps1` - Scripts para corregir orden de includes

**Documentación:**
- `docs/FUENTES_CONFIGURABLES.md` - Guía de usuario

**Funcionalidad:**
- Los usuarios pueden configurar una fuente personalizada para todas las listas (servidores, descargas, uploads, búsquedas, archivos compartidos)
- Acceso: Preferencias → Display → Shift+Click en "Select font..."
- La configuración persiste en `preferences.ini`
- Mejora significativa en legibilidad para usuarios

---
