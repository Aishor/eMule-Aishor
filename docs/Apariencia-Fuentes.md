# Modernización de Interfaz: Apariencia y Fuentes

Este documento detalla las nuevas características de personalización de interfaz implementadas en **eMule Titanium** (Enero 2026), diseñadas para mejorar la experiencia en monitores de alta resolución (High DPI/4K).

## 1. Nueva Pestaña "Apariencia" 🎨
Se ha añadido una pestaña dedicada en **Preferencias -> Apariencia** (`PPgAppearance`), centralizando todas las opciones visuales y eliminando controles antiguos dispersos.

### Características Principales
- **Gestión de Fuentes**: Control total sobre la tipografía de la aplicación.
- **Escalado de Iconos**: Ajuste manual del tamaño de iconos para mejor visibilidad.
- **Persistencia**: Configuración guardada automáticamente en `preferences.ini`.

---

## 2. Sistema de Fuentes Personalizadas 🔤
El sistema permite configurar **3 tipos de fuentes independientes**, aplicándose los cambios en tiempo real sin reiniciar la aplicación.

| Tipo de Fuente | Descripción | Uso | Variable Interna |
| :--- | :--- | :--- | :--- |
| **Fuente Interfaz** | Tipografía global de la aplicación. | Diálogos, Botones, Menús, Pestañas. | `m_fontApp` |
| **Fuente Listas** | Optimizada para tablas de datos. | Lista de Descargas, Servidores, Subidas, Archivos. | `m_fontList` |
| **Fuente Logs** | Tipografía monoespaciada (idealmente). | Ventana de Registro, Chat IRC, Mensajes. | `m_fontLog` |

> **Nota:** Al cambiar el tamaño de la "Fuente Interfaz", muchos diálogos ajustarán su tamaño automáticamente gracias al escalado nativo de Windows.

---

## 3. Escalado de Iconos (Icon Scaling) 🔍
Para resolver el problema de iconos "diminutos" en pantallas 4K, se ha implementado un sistema de escalado manual.

### Opciones Disponibles
- **Automático (16px)**: Comportamiento clásico.
- **Escala 1 (20px)**: Ligero aumento (+25%).
- **Escala 2 (24px)**: Tamaño medio (+50%).
- **Escala 3 (32px)**: Tamaño doble (HiDPI), ideal para 4K.

### Cobertura de la Implementación
El escalado se aplica actualmente a las vistas más críticas:
- ✅ **Lista de Transferencias** (Iconos de estado, barras de progreso, iconos de clientes).
- ✅ **Lista de Servidores** (Iconos de servidores).
- ✅ **Lista de Subidas** (Iconos de clientes/archivos).
- ✅ **Lista de Archivos Compartidos** (Iconos de archivos).

*(Nota: Los iconos de la barra de herramientas y pestañas mantienen su tamaño estándar por el momento)*.

---

## 4. Detalles Técnicos para Desarrolladores 🛠️

### Infraestructura
- **Clase**: `CPPgAppearance` (en `PPgAppearance.cpp/h`).
- **Recursos**: `IDD_PPG_APPEARANCE` (Diálogo).
- **Persistencia**: Se guardan en `preferences.ini` bajo la sección `[eMule]`:
  - `AppFontName`, `AppFontSize`
  - `ListFontName`, `ListFontSize`
  - `LogFontName`, `LogFontSize`
  - `IconScale` (0-3)

### Cómo extender el escalado de iconos
Para añadir soporte a nuevas listas (ej. Búsqueda), usar la función helper en `DrawItem` o inicialización:
```cpp
// Obtener tamaño actual
int iIconSize = theApp.GetScaledIconSize(); 

// Crear ImageList escalada
m_ImageList.Create(iIconSize, iIconSize, ...);

// Ajustar dibujo (si es Owner Draw)
rcItem.left += iScaledIconSize + padding;
```

---

## 5. Solución de Problemas (Troubleshooting)

- **La ventana se ve borrosa**: Esto es normal si tienes el escalado de Windows >100%. eMule delega el escalado a Windows para mantener el tamaño correcto. Usa las opciones de "Fuente Interfaz" para mejorar la legibilidad.
- **Iconos pixelados**: Al usar escalas >16px, los iconos originales se estiran. Esto es intencional para priorizar la visibilidad sobre la perfección pixel-art en monitores donde 16px es invisible.
