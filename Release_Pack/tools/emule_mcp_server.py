"""
eMule-Aishor MCP Server
Model Context Protocol server para control conversacional de eMule mediante Claude

Expone la API REST de eMule como tools y resources MCP para que Claude
pueda controlar eMule mediante lenguaje natural.

Versi├│n: R1.3 "FiberSight"
"""

import asyncio
import os
import logging
from typing import Any, Optional
from urllib.parse import urljoin

import httpx
from mcp.server import Server
from mcp.types import Tool, TextContent, Resource

# Configuraci├│n de logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger("emule-mcp")

# ============================================================================
# CONFIGURACI├ôN
# ============================================================================

EMULE_API_URL = os.getenv("EMULE_API_URL", "http://localhost:4711/api/v1")
EMULE_API_KEY = os.getenv("EMULE_API_KEY", "")

# ============================================================================
# CLIENTE API REST
# ============================================================================

class EMuleAPIClient:
    """Cliente para interactuar con la API REST de eMule"""
    
    def __init__(self, base_url: str, api_key: str):
        self.base_url = base_url
        self.headers = {"Authorization": f"Bearer {api_key}"} if api_key else {}
        self.client = httpx.AsyncClient(timeout=30.0)
    
    async def get(self, endpoint: str) -> dict:
        """GET request a la API"""
        url = urljoin(self.base_url, endpoint)
        logger.debug(f"GET {url}")
        
        try:
            response = await self.client.get(url, headers=self.headers)
            response.raise_for_status()
            return response.json()
        except httpx.HTTPError as e:
            logger.error(f"API Error: {e}")
            raise
    
    async def post(self, endpoint: str, json_data: dict = None) -> dict:
        """POST request a la API"""
        url = urljoin(self.base_url, endpoint)
        logger.debug(f"POST {url}")
        
        try:
            response = await self.client.post(url, headers=self.headers, json=json_data or {})
            response.raise_for_status()
            return response.json()
        except httpx.HTTPError as e:
            logger.error(f"API Error: {e}")
            raise
    
    async def put(self, endpoint: str, json_data: dict = None) -> dict:
        """PUT request a la API"""
        url = urljoin(self.base_url, endpoint)
        logger.debug(f"PUT {url}")
        
        try:
            response = await self.client.put(url, headers=self.headers, json=json_data or {})
            response.raise_for_status()
            return response.json()
        except httpx.HTTPError as e:
            logger.error(f"API Error: {e}")
            raise
    
    async def delete(self, endpoint: str) -> dict:
        """DELETE request a la API"""
        url = urljoin(self.base_url, endpoint)
        logger.debug(f"DELETE {url}")
        
        try:
            response = await self.client.delete(url, headers=self.headers)
            response.raise_for_status()
            return response.json()
        except httpx.HTTPError as e:
            logger.error(f"API Error: {e}")
            raise
    
    async def close(self):
        """Cerrar cliente HTTP"""
        await self.client.aclose()


# ============================================================================
# HELPERS
# ============================================================================

async def resolve_download(api: EMuleAPIClient, hash_or_name: str) -> tuple[str, str]:
    """
    Resolver hash_or_name a hash completo y nombre
    
    Args:
        api: Cliente API
        hash_or_name: Hash MD4 completo, parcial o nombre de archivo
    
    Returns:
        Tupla (hash, nombre)
    
    Raises:
        ValueError: Si no se encuentra la descarga
    """
    # Si parece un hash MD4 completo (32 chars hex)
    if len(hash_or_name) == 32:
        try:
            int(hash_or_name, 16)  # Verificar que es hex v├ílido
            # Buscar para obtener el nombre
            downloads = await api.get("/downloads")
            for dl in downloads.get("downloads", []):
                if dl["hash"].upper() == hash_or_name.upper():
                    return dl["hash"], dl["name"]
            # Si no se encuentra, asumir que el hash es v├ílido
            return hash_or_name.upper(), hash_or_name
        except ValueError:
            pass  # No es hex v├ílido, buscar por nombre
    
    # Buscar por nombre o hash parcial
    downloads = await api.get("/downloads")
    
    # Buscar coincidencia exacta de nombre
    for dl in downloads.get("downloads", []):
        if hash_or_name.lower() == dl["name"].lower():
            return dl["hash"], dl["name"]
    
    # Buscar coincidencia parcial de nombre
    for dl in downloads.get("downloads", []):
        if hash_or_name.lower() in dl["name"].lower():
            return dl["hash"], dl["name"]
    
    # Buscar por hash parcial
    for dl in downloads.get("downloads", []):
        if dl["hash"].upper().startswith(hash_or_name.upper()):
            return dl["hash"], dl["name"]
    
    raise ValueError(f"No se encontr├│ descarga: {hash_or_name}")


def format_size(bytes_size: int) -> str:
    """Formatear tama├▒o en bytes a formato legible"""
    for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
        if bytes_size < 1024.0:
            return f"{bytes_size:.1f} {unit}"
        bytes_size /= 1024.0
    return f"{bytes_size:.1f} PB"


def format_speed(bytes_per_sec: int) -> str:
    """Formatear velocidad en bytes/s a formato legible"""
    return f"{format_size(bytes_per_sec)}/s"


# ============================================================================
# MCP SERVER
# ============================================================================

# Crear servidor MCP
server = Server("emule-fibersight")

# Cliente API global
api_client: Optional[EMuleAPIClient] = None


@server.list_tools()
async def list_tools() -> list[Tool]:
    """Listar herramientas disponibles"""
    return [
        Tool(
            name="get_downloads",
            description="Obtener lista de descargas de eMule. Puede filtrar por estado (active, paused, all).",
            inputSchema={
                "type": "object",
                "properties": {
                    "filter": {
                        "type": "string",
                        "enum": ["active", "paused", "all"],
                        "description": "Filtro de estado de descargas",
                        "default": "all"
                    }
                }
            }
        ),
        Tool(
            name="get_download_info",
            description="Obtener informaci├│n detallada de una descarga espec├¡fica. Acepta hash MD4, hash parcial o nombre del archivo.",
            inputSchema={
                "type": "object",
                "properties": {
                    "hash_or_name": {
                        "type": "string",
                        "description": "Hash MD4 completo, hash parcial o nombre del archivo"
                    }
                },
                "required": ["hash_or_name"]
            }
        ),
        Tool(
            name="pause_download",
            description="Pausar una descarga de eMule. Acepta hash MD4, hash parcial o nombre del archivo.",
            inputSchema={
                "type": "object",
                "properties": {
                    "hash_or_name": {
                        "type": "string",
                        "description": "Hash MD4 completo, hash parcial o nombre del archivo"
                    }
                },
                "required": ["hash_or_name"]
            }
        ),
        Tool(
            name="resume_download",
            description="Reanudar una descarga pausada de eMule. Acepta hash MD4, hash parcial o nombre del archivo.",
            inputSchema={
                "type": "object",
                "properties": {
                    "hash_or_name": {
                        "type": "string",
                        "description": "Hash MD4 completo, hash parcial o nombre del archivo"
                    }
                },
                "required": ["hash_or_name"]
            }
        ),
        Tool(
            name="delete_download",
            description="Eliminar una descarga de eMule (DESTRUCTIVO). Requiere confirmaci├│n expl├¡cita.",
            inputSchema={
                "type": "object",
                "properties": {
                    "hash_or_name": {
                        "type": "string",
                        "description": "Hash MD4 completo, hash parcial o nombre del archivo"
                    },
                    "confirm": {
                        "type": "boolean",
                        "description": "Debe ser true para confirmar la eliminaci├│n",
                        "default": False
                    }
                },
                "required": ["hash_or_name", "confirm"]
            }
        ),
        Tool(
            name="get_status",
            description="Obtener estado general de eMule (conexi├│n, velocidades, estad├¡sticas).",
            inputSchema={
                "type": "object",
                "properties": {}
            }
        ),
        Tool(
            name="add_download",
            description="A├▒adir una nueva descarga a eMule mediante enlace ed2k.",
            inputSchema={
                "type": "object",
                "properties": {
                    "ed2k_link": {
                        "type": "string",
                        "description": "Enlace ed2k del archivo a descargar"
                    }
                },
                "required": ["ed2k_link"]
            }
        ),
        Tool(
            name="get_library",
            description="Obtener lista de archivos compartidos (biblioteca de eMule).",
            inputSchema={
                "type": "object",
                "properties": {
                    "category": {
                        "type": "string",
                        "description": "Filtrar por categor├¡a (opcional)"
                    },
                    "min_quality": {
                        "type": "string",
                        "description": "Calidad m├¡nima de video (720p, 1080p, 2160p)"
                    }
                }
            }
        ),
        Tool(
            name="get_stats",
            description="Obtener estad├¡sticas detalladas de eMule (descargado, subido, sesi├│n, totales).",
            inputSchema={
                "type": "object",
                "properties": {}
            }
        ),
        Tool(
            name="enable_preview",
            description="Activar modo preview para una descarga (prioriza chunks iniciales para an├ílisis visual).",
            inputSchema={
                "type": "object",
                "properties": {
                    "hash_or_name": {
                        "type": "string",
                        "description": "Hash MD4 completo, hash parcial o nombre del archivo"
                    }
                },
                "required": ["hash_or_name"]
            }
        )
    ]


@server.call_tool()
async def call_tool(name: str, arguments: Any) -> list[TextContent]:
    """Ejecutar una herramienta"""
    
    global api_client
    if not api_client:
        api_client = EMuleAPIClient(EMULE_API_URL, EMULE_API_KEY)
    
    try:
        if name == "get_downloads":
            return await tool_get_downloads(arguments)
        
        elif name == "get_download_info":
            return await tool_get_download_info(arguments)
        
        elif name == "pause_download":
            return await tool_pause_download(arguments)
        
        elif name == "resume_download":
            return await tool_resume_download(arguments)
        
        elif name == "delete_download":
            return await tool_delete_download(arguments)
        
        elif name == "get_status":
            return await tool_get_status(arguments)
        
        elif name == "add_download":
            return await tool_add_download(arguments)
        
        elif name == "get_library":
            return await tool_get_library(arguments)
        
        elif name == "get_stats":
            return await tool_get_stats(arguments)
        
        elif name == "enable_preview":
            return await tool_enable_preview(arguments)
        
        else:
            return [TextContent(type="text", text=f"Herramienta desconocida: {name}")]
    
    except Exception as e:
        logger.error(f"Error en tool {name}: {e}")
        return [TextContent(type="text", text=f"Error: {str(e)}")]


# ============================================================================
# IMPLEMENTACI├ôN DE TOOLS
# ============================================================================

async def tool_get_downloads(args: dict) -> list[TextContent]:
    """Obtener lista de descargas"""
    filter_type = args.get("filter", "all")
    
    if filter_type == "active":
        data = await api_client.get("/downloads/active")
    else:
        data = await api_client.get("/downloads")
    
    downloads = data.get("downloads", [])
    
    if not downloads:
        return [TextContent(type="text", text="No hay descargas")]
    
    # Formatear respuesta
    lines = [f"# Descargas de eMule ({len(downloads)} archivos)\n"]
    
    for dl in downloads:
        lines.append(f"## {dl['name']}")
        lines.append(f"- **Hash:** `{dl['hash'][:8]}...`")
        lines.append(f"- **Progreso:** {dl.get('progress', 0):.1f}%")
        lines.append(f"- **Tama├▒o:** {format_size(dl.get('size', 0))}")
        
        if 'speed' in dl:
            lines.append(f"- **Velocidad:** {format_speed(dl['speed'])}")
        
        if 'sources' in dl:
            lines.append(f"- **Fuentes:** {dl['sources']}")
        
        if 'state' in dl:
            lines.append(f"- **Estado:** {dl['state']}")
        
        lines.append("")
    
    return [TextContent(type="text", text="\n".join(lines))]


async def tool_get_download_info(args: dict) -> list[TextContent]:
    """Obtener info detallada de una descarga"""
    hash_or_name = args["hash_or_name"]
    
    try:
        file_hash, file_name = await resolve_download(api_client, hash_or_name)
        
        # Obtener info detallada
        data = await api_client.get(f"/downloads/{file_hash}/file_info")
        
        lines = [f"# Informaci├│n de: {file_name}\n"]
        lines.append(f"**Hash:** `{file_hash}`")
        lines.append(f"**Tama├▒o:** {format_size(data.get('file_size', 0))}")
        lines.append(f"**Descargado:** {format_size(data.get('completed_size', 0))}")
        lines.append(f"**Progreso:** {data.get('progress', 0):.1f}%")
        
        if 'chunks' in data:
            chunks = data['chunks']
            lines.append(f"\n**Chunks:**")
            lines.append(f"- Total: {chunks.get('total', 0)}")
            lines.append(f"- Completados: {chunks.get('completed', 0)}")
            lines.append(f"- Primer chunk: {'Ô£à' if chunks.get('first_chunk_complete') else 'ÔØî'}")
            lines.append(f"- ├Ültimo chunk: {'Ô£à' if chunks.get('last_chunk_complete') else 'ÔØî'}")
        
        if data.get('is_video'):
            lines.append(f"\n**Video:**")
            lines.append(f"- Preview ready: {'Ô£à' if data.get('preview_ready') else 'ÔØî'}")
            lines.append(f"- Preview mode: {'Ô£à' if data.get('preview_mode') else 'ÔØî'}")
        
        return [TextContent(type="text", text="\n".join(lines))]
    
    except ValueError as e:
        return [TextContent(type="text", text=f"Error: {str(e)}")]


async def tool_pause_download(args: dict) -> list[TextContent]:
    """Pausar descarga"""
    hash_or_name = args["hash_or_name"]
    
    try:
        file_hash, file_name = await resolve_download(api_client, hash_or_name)
        
        await api_client.put(f"/downloads/{file_hash}/pause")
        
        return [TextContent(type="text", text=f"Ô£à Descarga pausada: **{file_name}**")]
    
    except ValueError as e:
        return [TextContent(type="text", text=f"Error: {str(e)}")]


async def tool_resume_download(args: dict) -> list[TextContent]:
    """Reanudar descarga"""
    hash_or_name = args["hash_or_name"]
    
    try:
        file_hash, file_name = await resolve_download(api_client, hash_or_name)
        
        await api_client.put(f"/downloads/{file_hash}/resume")
        
        return [TextContent(type="text", text=f"Ô£à Descarga reanudada: **{file_name}**")]
    
    except ValueError as e:
        return [TextContent(type="text", text=f"Error: {str(e)}")]


async def tool_delete_download(args: dict) -> list[TextContent]:
    """Eliminar descarga"""
    hash_or_name = args["hash_or_name"]
    confirm = args.get("confirm", False)
    
    if not confirm:
        return [TextContent(type="text", text="ÔÜá´©Å **Operaci├│n destructiva:** Debes confirmar con `confirm=true`")]
    
    try:
        file_hash, file_name = await resolve_download(api_client, hash_or_name)
        
        await api_client.delete(f"/downloads/{file_hash}")
        
        return [TextContent(type="text", text=f"­ƒùæ´©Å Descarga eliminada: **{file_name}**")]
    
    except ValueError as e:
        return [TextContent(type="text", text=f"Error: {str(e)}")]


async def tool_get_status(args: dict) -> list[TextContent]:
    """Obtener estado de eMule"""
    data = await api_client.get("/status")
    
    lines = ["# Estado de eMule\n"]
    lines.append(f"**Versi├│n:** {data.get('version', 'N/A')}")
    lines.append(f"**Conectado:** {'Ô£à' if data.get('connected') else 'ÔØî'}")
    
    if 'server' in data:
        lines.append(f"**Servidor:** {data['server'].get('name', 'N/A')}")
    
    lines.append(f"\n**Velocidades:**")
    lines.append(f"- Descarga: {format_speed(data.get('download_speed', 0))}")
    lines.append(f"- Subida: {format_speed(data.get('upload_speed', 0))}")
    
    lines.append(f"\n**Descargas:**")
    lines.append(f"- Activas: {data.get('active_downloads', 0)}")
    lines.append(f"- En cola: {data.get('queued_downloads', 0)}")
    
    return [TextContent(type="text", text="\n".join(lines))]


async def tool_add_download(args: dict) -> list[TextContent]:
    """A├▒adir descarga"""
    ed2k_link = args["ed2k_link"]
    
    if not ed2k_link.startswith("ed2k://"):
        return [TextContent(type="text", text="ÔØî Error: El enlace debe empezar con 'ed2k://'")]
    
    try:
        data = await api_client.post("/downloads", {"ed2k": ed2k_link})
        
        return [TextContent(type="text", text=f"Ô£à Descarga a├▒adida correctamente")]
    
    except Exception as e:
        return [TextContent(type="text", text=f"ÔØî Error a├▒adiendo descarga: {str(e)}")]


async def tool_get_library(args: dict) -> list[TextContent]:
    """Obtener biblioteca"""
    params = []
    
    if "category" in args:
        params.append(f"category={args['category']}")
    
    if "min_quality" in args:
        params.append(f"min_quality={args['min_quality']}")
    
    endpoint = "/library"
    if params:
        endpoint += "?" + "&".join(params)
    
    data = await api_client.get(endpoint)
    files = data.get("files", [])
    
    if not files:
        return [TextContent(type="text", text="La biblioteca est├í vac├¡a")]
    
    lines = [f"# Biblioteca de eMule ({len(files)} archivos)\n"]
    
    for file in files[:20]:  # Limitar a 20 archivos
        lines.append(f"## {file['name']}")
        lines.append(f"- **Tama├▒o:** {format_size(file.get('size', 0))}")
        
        if 'quality' in file:
            lines.append(f"- **Calidad:** {file['quality']}")
        
        lines.append("")
    
    if len(files) > 20:
        lines.append(f"\n_... y {len(files) - 20} archivos m├ís_")
    
    return [TextContent(type="text", text="\n".join(lines))]


async def tool_get_stats(args: dict) -> list[TextContent]:
    """Obtener estad├¡sticas"""
    data = await api_client.get("/stats")
    
    lines = ["# Estad├¡sticas de eMule\n"]
    
    lines.append("## Sesi├│n Actual")
    lines.append(f"- Descargado: {format_size(data.get('session_downloaded', 0))}")
    lines.append(f"- Subido: {format_size(data.get('session_uploaded', 0))}")
    lines.append(f"- Duraci├│n: {data.get('session_duration', 0) // 3600}h")
    
    lines.append("\n## Totales")
    lines.append(f"- Descargado: {format_size(data.get('total_downloaded', 0))}")
    lines.append(f"- Subido: {format_size(data.get('total_uploaded', 0))}")
    
    if 'ratio' in data:
        lines.append(f"- Ratio: {data['ratio']:.2f}")
    
    return [TextContent(type="text", text="\n".join(lines))]


async def tool_enable_preview(args: dict) -> list[TextContent]:
    """Activar preview mode"""
    hash_or_name = args["hash_or_name"]
    
    try:
        file_hash, file_name = await resolve_download(api_client, hash_or_name)
        
        await api_client.post(f"/downloads/{file_hash}/preview", {"enable": True})
        
        return [TextContent(
            type="text",
            text=f"Ô£à Preview mode activado para: **{file_name}**\n\n"
                 f"Los chunks iniciales y finales ser├ín priorizados para permitir "
                 f"an├ílisis visual del archivo."
        )]
    
    except ValueError as e:
        return [TextContent(type="text", text=f"Error: {str(e)}")]


# ============================================================================
# RESOURCES
# ============================================================================

@server.list_resources()
async def list_resources() -> list[Resource]:
    """Listar resources disponibles"""
    return [
        Resource(
            uri="emule://status",
            name="Estado de eMule",
            mimeType="text/plain",
            description="Estado actual de eMule (conexi├│n, velocidades, descargas)"
        ),
        Resource(
            uri="emule://downloads/active",
            name="Descargas Activas",
            mimeType="text/markdown",
            description="Lista de descargas activas en formato legible"
        ),
        Resource(
            uri="emule://stats",
            name="Estad├¡sticas",
            mimeType="text/markdown",
            description="Estad├¡sticas de sesi├│n y totales"
        )
    ]


@server.read_resource()
async def read_resource(uri: str) -> str:
    """Leer un resource"""
    
    global api_client
    if not api_client:
        api_client = EMuleAPIClient(EMULE_API_URL, EMULE_API_KEY)
    
    if uri == "emule://status":
        result = await tool_get_status({})
        return result[0].text
    
    elif uri == "emule://downloads/active":
        result = await tool_get_downloads({"filter": "active"})
        return result[0].text
    
    elif uri == "emule://stats":
        result = await tool_get_stats({})
        return result[0].text
    
    else:
        raise ValueError(f"Resource desconocido: {uri}")


# ============================================================================
# MAIN
# ============================================================================

async def main():
    """Punto de entrada principal"""
    from mcp.server.stdio import stdio_server
    
    logger.info("Iniciando eMule MCP Server...")
    logger.info(f"API URL: {EMULE_API_URL}")
    
    async with stdio_server() as (read_stream, write_stream):
        await server.run(
            read_stream,
            write_stream,
            server.create_initialization_options()
        )


if __name__ == "__main__":
    asyncio.run(main())
