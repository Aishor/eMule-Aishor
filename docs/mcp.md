# eMule Model Context Protocol (MCP) Integration

Esta integración permite conectar **Claude** (y otros clientes MCP) directamente con eMule-Aishor para controlar descargas, búsquedas y estado mediante lenguaje natural.

## Arquitectura

La solución se compone de dos partes:
1.  **MCP Server (`tools/emule_mcp_server.py`)**: Un servidor escrito en Python utilizando el SDK oficial de MCP. Actúa como puente.
2.  **eMule LLM API**: Una API REST interna implementada en C++ dentro de eMule (`srchybrid/LLMApiServer.cpp`) que expone las funcionalidades del núcleo.

```mermaid
graph LR
    Claude[Claude Desktop] -- stdio --> Python[MCP Server Python]
    Python -- HTTP JSON --> eMule[eMule API (C++)]
    eMule -- Actions --> Core[eMule Core]
```

## Instalación y Configuración

### Prerrequisitos
*   eMule-Aishor R1.3+ "FiberSight" (compilado con soporte API).
*   Python 3.10+ instalado en el sistema.
*   Librerías Python: `mcp`, `httpx`.

### 1. Preparar el Entorno Python
Se recomienda usar los scripts automáticos incluidos en `tools/`:

```cmd
cd tools
install_mcp.bat
```

Esto creará un entorno virtual (`.venv`) e instalará las dependencias necesarias.

### 2. Configurar Claude Desktop
Edita tu archivo de configuración de Claude (`%APPDATA%\Claude\claude_desktop_config.json`) y añade el servidor:

```json
{
  "mcpServers": {
    "eMule": {
      "command": "C:\\ruta\\a\\eMule-Aishor\\tools\\start.bat",
      "args": [],
      "env": {
        "EMULE_API_URL": "http://localhost:4711/api/v1",
        "EMULE_API_KEY": "" 
      }
    }
  }
}
```
*Nota: Asegúrate de que la ruta a `start.bat` sea absoluta y correcta.*

### 3. Configurar eMule
1.  Abre eMule -> **Preferencias**.
2.  Ve a **Servidor Web / API**.
3.  Activa "Habilitar API REST".
4.  Puerto por defecto: `4711`.

## Uso

Una vez conectado, podrás pedirle a Claude cosas como:
*   "Busca la ISO de Ubuntu 22.04 en la red Kad."
*   "¿Cómo va la descarga de Matrix?"
*   "Pausa todas las descargas activas."
*   "Muéstrame mis estadísticas de subida."
*   "Añade este enlace ed2k: ..."

## Solución de Problemas

*   **Error de conexión**: Asegúrate de que eMule se está ejecutando antes de abrir Claude.
*   **Logs**: El servidor MCP genera logs en `stderr` que Claude Desktop captura en sus logs de aplicación.
