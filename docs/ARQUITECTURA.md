# FiberSight R1.3 - Guía de Arquitectura del Sistema

## 🏗️ Visión General

FiberSight R1.3 es un sistema **dual** que permite tanto control conversacional (vía Claude Desktop) como automatización completa (scripts Python). Todos los componentes son independientes y pueden usarse según las necesidades del usuario.

---

## 📊 Arquitectura Completa

```
┌─────────────────────────────────────────────────────────────────┐
│                    eMule-Aishor R1.3 Core                       │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  LLMApiServer (C++) - Puerto 4711                        │  │
│  │  - 15 Endpoints REST/JSON                                │  │
│  │  - Autenticación API Key                                 │  │
│  │  - CRUD de descargas                                     │  │
│  │  - Vision Ready endpoints                                │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         │ HTTP REST API (localhost:4711)
                         │
        ┌────────────────┴────────────────┐
        │                                 │
        ▼                                 ▼
┌───────────────────────────┐   ┌─────────────────────────┐
│   MODO A: CONVERSACIONAL  │   │   MODO B: AUTOMÁTICO    │
│                           │   │                         │
│  ┌─────────────────────┐ │   │  ┌───────────────────┐  │
│  │ MCP Server          │ │   │  │ Vision Auditor V2 │  │
│  │ (stdio transport)   │ │   │  │ (daemon polling)  │  │
│  └─────────────────────┘ │   │  └───────────────────┘  │
│           │               │   │                         │
│           ▼               │   │  - Polling cada 5 min  │
│  ┌─────────────────────┐ │   │  - FFmpeg + Claude     │
│  │ Orchestrator Agent  │ │   │  - Detección fakes     │
│  │ (background queue)  │ │   │  - Acciones auto       │
│  └─────────────────────┘ │   └─────────────────────────┘
│           │               │
│           ▼               │
│  ┌─────────────────────┐ │
│  │ Claude Desktop      │ │
│  │ (UI conversacional) │ │
│  └─────────────────────┘ │
└───────────────────────────┘
```

---

## 🎯 Modos de Operación

### Modo A: Conversacional (Interactivo)

**Componentes necesarios:**
1. eMule-Aishor (siempre)
2. MCP Server
3. Orchestrator Agent (opcional pero recomendado)
4. Claude Desktop

**Flujo de trabajo:**
```
Usuario: "Claude, busca películas de ciencia ficción en 1080p"
    ↓
Claude Desktop envía comando via MCP
    ↓
MCP Server crea tarea en Orchestrator
    ↓
Orchestrator:
  1. Parsea request con LLM
  2. Ejecuta búsqueda en eMule API
  3. Filtra resultados con LLM
  4. Retorna matches al usuario
    ↓
Claude Desktop muestra resultados
```

**Casos de uso:**
- Búsquedas complejas con lenguaje natural
- Análisis de biblioteca
- Optimización manual de descargas
- Verificación selectiva de archivos
- Recomendaciones personalizadas

---

### Modo B: Automático (Desatendido)

**Componentes necesarios:**
1. eMule-Aishor (siempre)
2. Vision Auditor V2

**Flujo de trabajo:**
```
Vision Auditor V2 (cada 5 minutos):
    ↓
GET /api/v1/downloads/active
    ↓
Para cada descarga de video:
  1. Verificar progreso >5MB
  2. Activar preview mode
  3. Esperar chunks críticos
  4. Extraer 3 frames con FFmpeg
  5. Analizar con Claude Vision
  6. Si FAKE → pausar/eliminar/banear
    ↓
Guardar estado en SQLite
```

**Casos de uso:**
- Verificación automática 24/7
- Detección de fakes sin intervención
- Protección continua
- Ahorro de ancho de banda

---

### Modo C: Híbrido (Recomendado)

**Componentes necesarios:**
1. eMule-Aishor (siempre)
2. Vision Auditor V2 (automático)
3. MCP Server + Orchestrator (manual)
4. Claude Desktop (UI)

**Ventajas:**
- Verificación automática en background
- Control manual cuando se necesita
- Mejor de ambos mundos

---

## 📦 Componentes del Sistema

### 1. eMule-Aishor Core (C++)

**Archivos:**
- `srchybrid/LLMApiServer.cpp` (~700 líneas)
- `srchybrid/LLMApiServer.h`
- `srchybrid/JsonResponse.cpp`
- `srchybrid/QualityDetector.cpp`

**Funcionalidad:**
- Servidor HTTP en puerto 4711
- 15 endpoints REST/JSON
- Autenticación API Key
- Integración con core de eMule

**Inicio automático:** Sí (con eMule)

---

### 2. Vision Auditor V2 (Python)

**Archivo:** `tools/vision_auditor_v2.py` (~900 líneas)

**Funcionalidad:**
- Máquina de estados (13 estados)
- Filtro pre-LLM (FFprobe)
- Multi-frame analysis (3 frames)
- Modos: Observer/Cautious/Terminator
- Persistencia SQLite

**Inicio:** Manual
```bash
python vision_auditor_v2.py --mode observer
```

**Dependencias:**
- FFmpeg (extracción de frames)
- Claude API (análisis visual)
- httpx, anthropic

---

### 3. MCP Server (Python)

**Archivo:** `tools/emule_mcp_server.py` (~700 líneas)

**Funcionalidad:**
- 10 tools MCP para Claude
- 3 resources (status, downloads, stats)
- Búsqueda flexible (hash/nombre/parcial)
- Integración con Claude Desktop

**Inicio:** Automático (vía Claude Desktop)

**Configuración:**
```json
// claude_desktop_config.json
{
  "mcpServers": {
    "emule-fibersight": {
      "command": "python",
      "args": ["C:\\path\\to\\emule_mcp_server.py"],
      "env": {
        "EMULE_API_URL": "http://localhost:4711/api/v1",
        "EMULE_API_KEY": "your-key"
      }
    }
  }
}
```

---

### 4. Orchestrator Agent (Python)

**Archivo:** `tools/orchestrator_agent.py` (~800 líneas base)

**Funcionalidad:**
- Cola de tareas asíncrona
- Prompts versionados en SQLite
- Refinamiento automático
- Handlers: SEARCH, VERIFY, OPTIMIZE, ANALYZE
- Integración LLM (Claude/GPT-4)

**Inicio:** Manual (background)
```bash
python orchestrator_agent.py &
```

**Base de datos:** `orchestrator.db` (SQLite)

---

## 🔌 Endpoints API REST

### Core (Lectura)
- `GET /api/v1/status` - Estado de eMule
- `GET /api/v1/downloads` - Todas las descargas
- `GET /api/v1/downloads/active` - Solo activas
- `GET /api/v1/library` - Archivos compartidos
- `GET /api/v1/servers` - Lista de servidores
- `GET /api/v1/stats` - Estadísticas
- `GET /api/v1/preferences` - Preferencias

### Core (Escritura)
- `POST /api/v1/downloads` - Añadir descarga
- `PUT /api/v1/downloads/{hash}/pause` - Pausar
- `PUT /api/v1/downloads/{hash}/resume` - Reanudar
- `DELETE /api/v1/downloads/{hash}` - Eliminar
- `POST /api/v1/servers/disconnect` - Desconectar

### Vision Ready
- `GET /api/v1/downloads/{hash}/file_info` - Info + ruta física
- `POST /api/v1/downloads/{hash}/preview` - Preview mode
- `POST /api/v1/downloads/{hash}/action` - Acciones (delete/ban)

---

## 🛠️ Tools MCP

### Gestión de Descargas
- `get_downloads` - Listar descargas
- `get_download_info` - Info detallada
- `pause_download` - Pausar
- `resume_download` - Reanudar
- `delete_download` - Eliminar (con confirmación)
- `add_download` - Añadir ed2k link

### Información
- `get_status` - Estado de eMule
- `get_stats` - Estadísticas
- `get_library` - Biblioteca

### Vision Verification
- `enable_preview` - Activar preview mode

### Tareas Asíncronas (con Orchestrator)
- `smart_search` - Búsqueda inteligente
- `verify_download_smart` - Verificación visual
- `optimize_downloads` - Optimización automática
- `analyze_library` - Análisis de biblioteca
- `get_task_status` - Estado de tarea

---

## 📋 Escenarios de Uso

### Escenario 1: Usuario Casual

**Objetivo:** Control fácil de eMule con lenguaje natural

**Configuración:**
```bash
# 1. Iniciar eMule
emule.exe

# 2. Configurar Claude Desktop (una vez)
# Editar claude_desktop_config.json

# 3. Usar Claude Desktop
```

**Ejemplos:**
```
"Claude, ¿qué estoy descargando?"
"Claude, pausa las descargas lentas"
"Claude, busca la película Inception en 1080p"
```

---

### Escenario 2: Usuario Técnico

**Objetivo:** Verificación automática de fakes 24/7

**Configuración:**
```bash
# 1. Iniciar eMule
emule.exe

# 2. Iniciar Vision Auditor (background)
python vision_auditor_v2.py --mode cautious &

# 3. Olvidarse, funciona solo
```

**Resultado:**
- Verificación cada 5 minutos
- Pausar fakes automáticamente
- Logs en `vision_auditor_v2.log`

---

### Escenario 3: Power User

**Objetivo:** Control total + automatización

**Configuración:**
```bash
# 1. Iniciar eMule
emule.exe

# 2. Iniciar Vision Auditor (modo observer)
python vision_auditor_v2.py --mode observer &

# 3. Iniciar Orchestrator
python orchestrator_agent.py &

# 4. Usar Claude Desktop para control manual
```

**Resultado:**
- Verificación automática (solo logging)
- Control conversacional avanzado
- Búsquedas inteligentes con LLM
- Análisis y recomendaciones

---

## 🔐 Seguridad

### API Key

**Generar:**
```bash
# En eMule: Preferencias → LLM API → Generar API Key
```

**Usar:**
```bash
# Vision Auditor
export EMULE_API_KEY="your-key-here"

# MCP Server (claude_desktop_config.json)
"EMULE_API_KEY": "your-key-here"
```

### Firewall

**Puerto 4711:** Solo localhost por defecto

**Cambiar:**
```cpp
// LLMApiServer.cpp
// Cambiar bind address si necesitas acceso remoto
```

---

## 📊 Monitoreo

### Logs

**eMule API:**
```
# Ver en eMule: Ventana de log
```

**Vision Auditor:**
```bash
tail -f vision_auditor_v2.log
```

**Orchestrator:**
```bash
tail -f orchestrator.log
```

### Base de Datos

**Vision Auditor:**
```bash
sqlite3 download_states.db "SELECT * FROM download_tracking"
```

**Orchestrator:**
```bash
sqlite3 orchestrator.db "SELECT * FROM tasks WHERE status='running'"
```

---

## 🚀 Inicio Rápido

### Opción A: Solo Automático

```bash
# Terminal 1: eMule
emule.exe

# Terminal 2: Vision Auditor
cd tools
pip install -r requirements.txt
python vision_auditor_v2.py --api-key YOUR_KEY --anthropic-key YOUR_KEY --mode observer
```

### Opción B: Solo Conversacional

```bash
# 1. Iniciar eMule
emule.exe

# 2. Configurar Claude Desktop
# Editar: %APPDATA%\Claude\claude_desktop_config.json

# 3. Reiniciar Claude Desktop

# 4. Probar: "Claude, ¿qué descargas tengo?"
```

### Opción C: Completo

```bash
# Terminal 1: eMule
emule.exe

# Terminal 2: Vision Auditor
python vision_auditor_v2.py --mode cautious &

# Terminal 3: Orchestrator
python orchestrator_agent.py &

# Terminal 4: Usar Claude Desktop
```

---

## 🎓 Mejores Prácticas

### 1. Empezar en Modo Observer

```bash
# Primeras 1-2 semanas
python vision_auditor_v2.py --mode observer
```

**Razón:** Calibrar precisión antes de borrar automáticamente

### 2. Monitorear Logs

```bash
# Revisar diariamente
tail -100 vision_auditor_v2.log | grep "FAKE"
```

### 3. Ajustar Umbrales

```python
# En vision_auditor_v2.py
if result.confidence > 0.7:  # Ajustar según precisión
    take_action()
```

### 4. Backup de Base de Datos

```bash
# Semanal
cp orchestrator.db orchestrator.db.backup
cp download_states.db download_states.db.backup
```

---

## 🆘 Troubleshooting

### "Connection refused" en API

**Solución:**
```bash
# Verificar que eMule está corriendo
# Verificar puerto 4711 abierto
netstat -an | grep 4711
```

### "Unauthorized" en API

**Solución:**
```bash
# Verificar API Key correcta
# Regenerar si es necesario en eMule
```

### FFmpeg no encontrado

**Solución:**
```bash
# Windows
choco install ffmpeg

# Linux
sudo apt install ffmpeg

# Verificar
ffmpeg -version
```

### Claude Desktop no ve MCP Server

**Solución:**
```bash
# 1. Verificar ruta en claude_desktop_config.json
# 2. Verificar que Python está en PATH
# 3. Reiniciar Claude Desktop completamente
# 4. Ver logs: %APPDATA%\Claude\logs
```

---

## 📚 Documentación Adicional

- **API REST:** `docs/LLM_API.md`
- **Vision Auditor:** `tools/README.md`
- **MCP Server:** `tools/CLAUDE_DESKTOP_CONFIG.md`
- **Walkthrough:** `.gemini/antigravity/brain/.../walkthrough.md`

---

## 🎯 Roadmap Futuro

### v1.4 (Próxima)
- [ ] Dashboard web para monitoreo
- [ ] Notificaciones (email, Telegram)
- [ ] ML local (sin API externa)
- [ ] Cache de análisis

### v1.5
- [ ] Soporte multi-idioma
- [ ] Integración con TMDb
- [ ] Whitelist de fuentes confiables
- [ ] A/B testing automático de prompts

---

**Versión:** R1.3 "FiberSight"  
**Fecha:** Enero 2026  
**Estado:** Production Ready ✅
