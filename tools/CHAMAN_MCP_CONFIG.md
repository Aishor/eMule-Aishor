# Configuración de CHAMAN para eMule-Aishor MCP Server

Este documento explica cómo configurar **CHAMAN** para controlar eMule conversacionalmente mediante el protocolo MCP (Model Context Protocol).

## Requisitos Previos

- **Python 3.11+** instalado y en el PATH
- **CHAMAN** instalado y configurado
- **eMule-Aishor** ejecutándose con LLMApiServer activo en puerto 4711
- Dependencias MCP instaladas en entorno virtual (ejecutar `install_mcp.bat`)

---

## Paso 1: Instalar Dependencias MCP

Desde la raíz del proyecto eMule-Aishor:

```batch
tools\install_mcp.bat
```

Esto creará un entorno virtual en `_env\` (en la raíz) e instalará:
- `mcp` (>=0.9.0) - SDK de Model Context Protocol
- `httpx` (>=0.27.0) - Cliente HTTP asíncrono

---

## Paso 2: Configurar Variables de Entorno (Opcional)

### API URL
Por defecto, el MCP server se conecta a `http://localhost:4711/api/v1`

Para usar una URL diferente:
```batch
set EMULE_API_URL=http://192.168.1.100:4711/api/v1
```

### API Key
Si has configurado autenticación en LLMApiServer:
```batch
set EMULE_API_KEY=tu_api_key_aqui
```

> **Nota:** Si eMule corre localmente sin autenticación, no necesitas configurar la API key.

---

## Paso 3: Configurar CHAMAN

### Configuración del MCP Server en CHAMAN

El MCP server de eMule utiliza **transport stdio** (stdin/stdout).

**Ruta del ejecutable Python (entorno virtual):**
```
C:\Fragua\eMule-Aishor\_env\Scripts\python.exe
```

**Ruta del script MCP:**
```
C:\Fragua\eMule-Aishor\tools\emule_mcp_server.py
```

**Configuración típica para CHAMAN** (ajustar según la estructura de config de CHAMAN):

```json
{
  "mcpServers": {
    "emule": {
      "command": "C:\\Fragua\\eMule-Aishor\\_env\\Scripts\\python.exe",
      "args": [
        "C:\\Fragua\\eMule-Aishor\\tools\\emule_mcp_server.py"
      ],
      "env": {
        "EMULE_API_URL": "http://localhost:4711/api/v1",
        "EMULE_API_KEY": ""
      }
    }
  }
}
```

> **IMPORTANTE:** 
> - Usa **rutas absolutas** para evitar problemas
> - Usa **dobles barras invertidas** (`\\`) en rutas de Windows
> - Ajusta las rutas según tu instalación
> - La ruta al Python debe apuntar al **entorno virtual** `_env\Scripts\python.exe`

---

## Paso 4: Verificar la Conexión

1. **Iniciar eMule-Aishor:**
   - Ejecuta `emule.exe`
   - Ve a **Preferencias → WebServer**
   - Asegúrate de que **API LLM** está habilitada en puerto **4711**

2. **Reiniciar CHAMAN:**
   - Reinicia CHAMAN después de modificar la configuración
   - El MCP server de eMule debería aparecer en los servidores disponibles

3. **Probar el MCP:**
   En CHAMAN, prueba comandos como:
   ```
   ¿Cuál es el estado de eMule?
   ```
   
   Si todo está configurado correctamente, CHAMAN responderá con información en tiempo real de tu cliente eMule.

---

## Herramientas MCP Disponibles

El MCP server de eMule expone 10 herramientas que CHAMAN puede usar:

### Gestión de Descargas
- **get_downloads** - Lista todas las descargas (filtros: active, paused, all)
- **get_download_info** - Información detallada de una descarga
- **add_download** - Añadir nueva descarga desde ed2k link
- **pause_download** - Pausar una descarga
- **resume_download** - Reanudar una descarga
- **delete_download** - Eliminar una descarga (solicita confirmación)

### Información y Control
- **get_status** - Estado general de eMule (conexiones, velocidades, etc.)
- **get_stats** - Estadísticas detalladas (total descargado, ratio, etc.)
- **get_library** - Lista de archivos compartidos en biblioteca
- **enable_preview** - Activar/desactivar modo preview para Vision Verification

### Recursos en Tiempo Real
- `emule://status` - Estado actual del cliente
- `emule://downloads/active` - Descargas activas
- `emule://stats` - Estadísticas en tiempo real

---

## Ejemplos de Uso Conversacional con CHAMAN

### Consultas de Estado
```
Usuario: ¿Está eMule conectado?
CHAMAN: [usa get_status] Sí, eMule está conectado al servidor X con 5 descargas activas...

Usuario: Muéstrame estadísticas generales
CHAMAN: [usa get_stats] Has descargado 125.3 GB en total, ratio de 1.24...
```

### Gestión de Descargas
```
Usuario: ¿Qué estoy descargando ahora?
CHAMAN: [usa get_downloads con filtro active] Tienes 3 descargas activas: ...

Usuario: Pausa la descarga de "Ubuntu ISO"
CHAMAN: [usa pause_download] He pausado la descarga de ubuntu-22.04.iso

Usuario: Añade este link: ed2k://|file|archivo.zip|1234567|...
CHAMAN: [usa add_download] He añadido la descarga de archivo.zip
```

### Búsqueda en Biblioteca
```
Usuario: ¿Qué películas tengo en mi biblioteca?
CHAMAN: [usa get_library con filtro] Tienes 15 películas en tu biblioteca: ...
```

---

## Troubleshooting

### Error: "MCP server 'emule' not found"

**Causa:** CHAMAN no encuentra el servidor MCP

**Solución:**
1. Verifica que la ruta en la configuración de CHAMAN es correcta
2. Asegúrate de usar rutas absolutas
3. Verifica que el entorno virtual existe: `_env\Scripts\python.exe`
4. Reinicia CHAMAN completamente

---

### Error: "Connection refused" o "API Error"

**Causa:** El MCP server no puede conectarse a la API de eMule

**Solución:**
1. Verifica que eMule está ejecutándose
2. Comprueba que LLMApiServer está habilitado en Preferencias → WebServer
3. Confirma el puerto (debe ser 4711 por defecto)
4. Prueba manualmente: `http://localhost:4711/api/v1/status` en el navegador

---

### Error: "Python not found" o ruta incorrecta

**Causa:** Ruta al Python del entorno virtual incorrecta

**Solución:**
1. Verifica que `_env\` existe
2. Ejecuta `tools\install_mcp.bat` si no existe
3. Usa ruta absoluta completa: `C:\Fragua\eMule-Aishor\_env\Scripts\python.exe`

---

### Error: "No module named 'mcp'" o "No module named 'httpx'"

**Causa:** Dependencias MCP no instaladas en el entorno virtual

**Solución:**
```batch
tools\install_mcp.bat
```

Luego reinicia CHAMAN.

---

## Test Manual del MCP Server

Para probar el MCP server de forma independiente (sin CHAMAN):

```batch
cd c:\Fragua\eMule-Aishor
tools\.venv\Scripts\python.exe tools\emule_mcp_server.py
```

El servidor debe iniciar sin errores. Para probarlo de forma interactiva, necesitarías un cliente MCP compatible.

> **Nota:** El MCP server usa stdin/stdout para comunicación, por lo que no verás mucha salida al ejecutarlo directamente.

---

## Seguridad y Privacidad

- El MCP server **solo** se conecta a tu eMule local
- **No envía datos** a servicios externos
- CHAMAN ejecuta el MCP server **localmente** en tu máquina
- Si usas API Key, asegúrate de no compartir tu configuración

---

## Soporte

Para problemas o preguntas:
1. Revisa la sección de Troubleshooting
2. Verifica los logs de eMule en `logs/`
3. Consulta la documentación del proyecto en `Documentacion/`
4. Verifica que el entorno virtual está correctamente creado

---

**Versión:** eMule-Aishor R0.1.1  
**Última actualización:** 2026-01-14  
**App Cliente:** CHAMAN (MCP compatible)
