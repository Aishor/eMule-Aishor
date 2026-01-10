# eMule-Aishor R1.3 - API LLM

## 🚀 Integración con LLM (Large Language Models)

eMule-Aishor R1.3 introduce una **API REST/JSON** completa que permite el control total de eMule mediante lenguaje natural a través de un LLM externo (Claude, GPT-4, Llama, etc.).

---

## 📋 Características

### Control por Lenguaje Natural
- **"Busca y descarga Inception en 1080p"** → El LLM busca, analiza y descarga automáticamente
- **"Actualiza mi biblioteca a mejor calidad"** → Escanea archivos y busca versiones superiores
- **"¿Cómo van mis descargas?"** → Reporta estado en lenguaje natural

### Detección Inteligente de Calidad
- Análisis automático de resolución (480p, 720p, 1080p, 4K, 8K)
- Detección de fuente (BluRay, WEB-DL, HDTV, etc.)
- Identificación de codec (H.264, H.265, AV1, etc.)
- Sistema de puntuación (0-100) para comparar versiones

### API REST Completa
- **Puerto:** 4711 (configurable)
- **Formato:** JSON
- **Autenticación:** API Key
- **Endpoints:** 15+ endpoints para control total

---

## 🔌 Endpoints Principales

### Estado y Monitoreo
```http
GET /api/v1/status
```
Retorna estado general de eMule (conectado, velocidades, descargas activas)

### Descargas
```http
GET  /api/v1/downloads              # Listar todas
GET  /api/v1/downloads/active       # Solo activas
POST /api/v1/downloads              # Añadir nueva
PUT  /api/v1/downloads/{hash}/pause # Pausar
DELETE /api/v1/downloads/{hash}     # Eliminar
```

### Búsqueda
```http
GET /api/v1/search?q=inception&type=video&min_sources=5
```

### Biblioteca
```http
GET /api/v1/library?category=Movies&min_quality=720p
```

### Servidores
```http
GET  /api/v1/servers           # Listar servidores
POST /api/v1/servers/connect   # Conectar
POST /api/v1/servers/disconnect # Desconectar
```

---

## 💡 Ejemplos de Uso

### Ejemplo 1: Buscar y Descargar

**Usuario:** *"Quiero descargar Matrix en 1080p"*

**LLM hace:**
```javascript
// 1. Buscar
GET /api/v1/search?q=matrix+1080p&type=video

// 2. Analizar resultados y elegir el mejor
POST /api/v1/downloads
{
  "hash": "31D6CFE0D16AE931B73C59D7E0C089C0",
  "category": "Movies"
}
```

**LLM responde:** *"He añadido Matrix (1999) 1080p BluRay x264 (2.1GB) a tus descargas"*

### Ejemplo 2: Actualizar Biblioteca

**Usuario:** *"Revisa mi biblioteca y actualiza películas a mejor calidad"*

**LLM hace:**
```javascript
// 1. Obtener biblioteca
GET /api/v1/library

// Respuesta:
{
  "files": [
    {
      "name": "Inception.2010.720p.mkv",
      "quality": "720p BluRay",
      "score": 43
    }
  ]
}

// 2. Buscar versión mejor
GET /api/v1/search?q=inception+2010+1080p

// 3. Si encuentra mejor calidad, descargar
POST /api/v1/downloads
{
  "hash": "...",
  "category": "Movies"
}
```

**LLM responde:** *"Encontré Inception en 1080p BluRay (mejor que tu versión 720p). He iniciado la descarga."*

---

## 🛠️ Configuración

### Habilitar API LLM

1. Abrir **Preferencias → Web Server**
2. Activar **"Enable LLM API Server"**
3. Configurar puerto (por defecto: 4711)
4. Generar API Key
5. Guardar y reiniciar

### Conectar con Claude Desktop (MCP)

Crear archivo `claude_desktop_config.json`:
```json
{
  "mcpServers": {
    "emule": {
      "command": "node",
      "args": ["emule-mcp-server.js"],
      "env": {
        "EMULE_API_URL": "http://localhost:4711",
        "EMULE_API_KEY": "tu-api-key-aqui"
      }
    }
  }
}
```

### Usar con API REST Directa

```python
import requests

API_URL = "http://localhost:4711/api/v1"
API_KEY = "tu-api-key"

headers = {"Authorization": f"Bearer {API_KEY}"}

# Buscar
response = requests.get(
    f"{API_URL}/search",
    params={"q": "inception", "type": "video"},
    headers=headers
)

results = response.json()

# Descargar el mejor resultado
best = max(results["results"], key=lambda x: x["quality_score"])
requests.post(
    f"{API_URL}/downloads",
    json={"hash": best["hash"]},
    headers=headers
)
```

---

## 📊 Formato de Respuestas

### Éxito
```json
{
  "status": "success",
  "data": { ... }
}
```

### Error
```json
{
  "status": "error",
  "code": 404,
  "message": "Resource not found"
}
```

### Información de Descarga
```json
{
  "hash": "31D6CFE0D16AE931B73C59D7E0C089C0",
  "name": "Inception.2010.1080p.BluRay.x264.mkv",
  "size": 2147483648,
  "progress": 67.5,
  "speed": 1048576,
  "sources": 12,
  "eta_seconds": 1800,
  "quality": {
    "resolution": "1080p",
    "source": "BluRay",
    "codec": "H.264",
    "score": 68
  }
}
```

---

## 🔒 Seguridad

> [!WARNING]
> **La API permite control total de eMule**

- Solo accesible desde `localhost` por defecto
- Requiere API Key válida
- Límite de rate: 100 requests/minuto
- Logs de todas las operaciones

---

## 🧪 Testing

### Test Manual
```bash
# Estado
curl http://localhost:4711/api/v1/status \
  -H "Authorization: Bearer tu-api-key"

# Búsqueda
curl "http://localhost:4711/api/v1/search?q=matrix" \
  -H "Authorization: Bearer tu-api-key"
```

### Test con LLM
Usa Claude, GPT-4 o cualquier LLM con capacidad de function calling y dale acceso a la API.

---

## 📚 Documentación Completa

Ver [API_REFERENCE.md](docs/API_REFERENCE.md) para documentación detallada de todos los endpoints.

---

## 🎯 Roadmap

- [x] API REST base
- [x] Detector de calidad
- [x] Endpoints de descargas
- [ ] Servidor MCP para Claude Desktop
- [ ] WebSocket para eventos en tiempo real
- [ ] Integración con TMDb/IMDb
- [ ] Dashboard web para monitoreo

---

## 📝 Changelog

### R1.3 (10/01/2026)
- ✨ Nueva API REST/JSON para control por LLM
- ✨ Detector inteligente de calidad de video
- ✨ Sistema de puntuación de calidad (0-100)
- ✨ Endpoints completos para descargas, búsqueda y biblioteca
- 🔒 Autenticación por API Key

---

**Versión:** 0.70.3-Build26-R1.3-X64  
**Licencia:** GPL-2.0  
**Proyecto:** eMule-Aishor Titanium Fiber
