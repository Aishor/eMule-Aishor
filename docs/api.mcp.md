# Referencia de Herramientas MCP (eMule)

Estas son las herramientas (Tools) expuestas por el servidor MCP de eMule para ser utilizadas por el LLM.

## Gestión de Descargas

### `get_downloads`
Obtiene la lista actual de descargas.
*   **Argumentos**:
    *   `filter` (opcional): Filtra por estado: `"active"`, `"paused"`, o `"all"` (defecto).

### `get_download_info`
Obtiene información detallada técnica de una descarga específica.
*   **Argumentos**:
    *   `hash_or_name` (requerido): Puede ser el Hash MD4 completo, un hash parcial, o parte del nombre del archivo.

### `add_download`
Añade una nueva descarga mediante un enlace ed2k.
*   **Argumentos**:
    *   `ed2k_link` (requerido): El enlace ed2k completo (`ed2k://|file|...`).

### `pause_download` / `resume_download`
Pausa o reanuda una descarga.
*   **Argumentos**:
    *   `hash_or_name` (requerido): Identificador del archivo.

### `delete_download`
Elimina un archivo de la cola de descargas. **Acción destructiva**.
*   **Argumentos**:
    *   `hash_or_name` (requerido): Identificador del archivo.
    *   `confirm` (requerido): Debe ser `true` para confirmar la eliminación.

### `enable_preview`
Activa el modo de vista previa (descarga prioritaria del primer/último chunk).
*   **Argumentos**:
    *   `hash_or_name` (requerido): Identificador del archivo.

## Búsqueda

### `search_files`
Realiza una búsqueda en la red eD2k o Kademlia.
*   **Argumentos**:
    *   `query` (requerido): Términos de búsqueda.
    *   `type` (opcional): Tipo de archivo (`"video"`, `"audio"`, `"archive"`, `"program"`, `"any"`).
    *   `method` (opcional): `"global"` (Servidores), `"kad"` (Kademlia), `"server"` (Local).
    *   `min_size` / `max_size` (opcional): Filtros de tamaño (ej: "100MB").
    *   `availability` (opcional): Mínimo de fuentes.

### `download_search_result`
Inicia la descarga de un archivo encontrado en una búsqueda previa.
*   **Argumentos**:
    *   `hash` (requerido): El Hash MD4 del resultado de búsqueda.

## Información del Sistema

### `get_status`
Obtiene el estado general de la conexión y velocidades actuales de subida/bajada.

### `get_stats`
Obtiene estadísticas de sesión y acumuladas (total transferido, tiempo de sesión, ratio).

### `get_library`
Lista los archivos compartidos (completados).
*   **Argumentos**:
    *   `category` (opcional): Filtrar por categoría.
    *   `min_quality` (opcional): Filtrar por resolución de video (ej: "1080p").
