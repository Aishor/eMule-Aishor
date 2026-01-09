# Fuentes Configurables en Listas

## ¿Qué es?

eMule-Aishor ahora permite personalizar la fuente utilizada en **todas las listas** de la aplicación para mejorar la legibilidad.

## Listas Afectadas

La fuente personalizada se aplica a:
- 📋 Lista de Servidores
- ⬇️ Lista de Descargas
- ⬆️ Lista de Uploads
- 🔍 Resultados de Búsqueda
- 📁 Archivos Compartidos

## Cómo Configurar la Fuente

### Paso 1: Abrir Preferencias
1. Click en menú **Herramientas** → **Preferencias**
2. O presiona `Ctrl+P`

### Paso 2: Ir a Sección Display
1. En el panel izquierdo, selecciona **Display**

### Paso 3: Seleccionar Fuente de Lista
**Método Recomendado (Nuevo):**
1. Haz click en el botón **"Select List Font..."** (Seleccionar Fuente de Lista)

**Método Alternativo:**
1. Mantén presionada la tecla `Shift`
2. Haz click en el botón "Select font..."

### Paso 4: Elegir tu Fuente Preferida
Recomendaciones:
- **Fuente:** Segoe UI, Tahoma, Verdana, Arial
- **Tamaño:** 10-12pt (según tu pantalla y preferencias)
- **Estilo:** Regular o Bold para mayor contraste

**Ejemplo popular:**
- Fuente: `Segoe UI`
- Tamaño: `11`
- Estilo: `Regular`

### Paso 5: Aplicar y Verificar
1. Click **OK** en el selector de fuentes
2. Click **Aplicar** en las Preferencias
3. Las listas cambiarán inmediatamente

## Persistencia

✅ La configuración se guarda automáticamente en `preferences.ini`  
✅ Se mantiene tras reiniciar eMule

## Restaurar Fuente Predeterminada

Para volver a la fuente original del sistema:

1. Ve a **Preferencias** → **Display**
2. Mantén `Shift` y click en **"Select font..."**
3. Selecciona: `MS Shell Dlg`, tamaño `8`
4. Click **OK** → **Aplicar**

## Notas Técnicas

- La configuración se almacena independientemente de otras fuentes (hypertext, log)
- Compatible con la opción "Use system font for main controls"
- No afecta a otros elementos de la interfaz (menús, diálogos, etc.)

## Solución de Problemas

### "No veo cambios tras seleccionar la fuente"
- Verifica que mantuviste presionada la tecla `Shift` al hacer click
- Asegúrate de hacer click en **"Aplicar"** en las Preferencias

### "La fuente no se mantiene tras reiniciar"
- Comprueba que tienes permisos de escritura en la carpeta de eMule
- Verifica que `preferences.ini` no esté en modo solo lectura

### "Algunas listas no cambian"
- Reinicia completamente eMule
- Si persiste, reporta el problema en GitHub

## Capturas de Pantalla Sugeridas

Para crear capturas:
1. **Antes:** Lista con fuente pequeña predeterminada
2. **Proceso:** Selector de fuentes con `Shift` presionado
3. **Después:** Lista con fuente más grande y legible

---

**Versión:** eMule-Aishor v0.70b-Build26-R1.2  
**Fecha:** Enero 2026
