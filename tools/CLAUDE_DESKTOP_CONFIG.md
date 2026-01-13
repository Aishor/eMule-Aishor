# eMule MCP Server - Configuración para Claude Desktop

Este archivo debe copiarse a la configuración de Claude Desktop.

## Ubicación del archivo de configuración:

**Windows:**
```
%APPDATA%\Claude\claude_desktop_config.json
```

**macOS:**
```
~/Library/Application Support/Claude/claude_desktop_config.json
```

**Linux:**
```
~/.config/Claude/claude_desktop_config.json
```

## Contenido del archivo:

```json
{
  "mcpServers": {
    "emule-fibersight": {
      "command": "python",
      "args": [
        "C:\\Fragua\\eMule-Aishor\\tools\\emule_mcp_server.py"
      ],
      "env": {
        "EMULE_API_URL": "http://localhost:4711/api/v1",
        "EMULE_API_KEY": "your-api-key-here"
      }
    }
  }
}
```

## Configuración:

1. **Ajustar ruta:** Cambiar `C:\\Fragua\\eMule-Aishor\\tools\\emule_mcp_server.py` a la ruta real en tu sistema

2. **API Key:** Reemplazar `your-api-key-here` con tu API key de eMule

3. **Reiniciar Claude Desktop** para que cargue la configuración

## Verificación:

Una vez configurado, Claude Desktop debería mostrar "eMule FiberSight" en la lista de herramientas disponibles.

Puedes probar con:
```
Claude, ¿qué descargas tengo activas en eMule?
```

## Troubleshooting:

**Error: "MCP server failed to start"**
- Verificar que Python está en PATH
- Verificar que las dependencias están instaladas: `pip install -r requirements_mcp.txt`
- Verificar que eMule está corriendo y la API en puerto 4711

**Error: "Connection refused"**
- Verificar que eMule-Aishor está ejecutándose
- Verificar que el puerto 4711 está abierto
- Verificar la URL en EMULE_API_URL

**Error: "Unauthorized"**
- Verificar que EMULE_API_KEY es correcta
- Verificar que la autenticación está habilitada en eMule
