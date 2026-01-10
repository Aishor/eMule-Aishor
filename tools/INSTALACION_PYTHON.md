# FiberSight R1.3 - Guía de Instalación Python

## 🎯 Instalación Rápida

### Opción 1: Instalación Automática (Recomendada)

```bash
# 1. Ejecutar instalador
cd c:\Fragua\eMule-Aishor\tools
install_python_deps.bat

# Esto creará:
# - Entorno virtual en _env\
# - Instalará todas las dependencias
# - Configurará el entorno
```

### Opción 2: Instalación Manual

```bash
# 1. Crear entorno virtual
python -m venv _env

# 2. Activar entorno
_env\Scripts\activate.bat

# 3. Actualizar pip
python -m pip install --upgrade pip

# 4. Instalar dependencias
pip install -r requirements_all.txt
```

---

## 🚀 Uso del Entorno Virtual

### Activar Entorno

```bash
# Opción A: Script automático
activate_env.bat

# Opción B: Manual
_env\Scripts\activate.bat
```

### Desactivar Entorno

```bash
deactivate
```

---

## 📦 Scripts de Inicio

### Vision Auditor V2

```bash
# Inicia Vision Auditor en modo Observer
start_vision_auditor.bat
```

**Configuración:**
- Modo: Observer (solo logging)
- Intervalo: 5 minutos
- Solicita API Keys si no están configuradas

### Orchestrator Agent

```bash
# Inicia el agente orquestador
start_orchestrator.bat
```

**Configuración:**
- Base de datos: orchestrator.db
- Solicita API Keys si no están configuradas

---

## 🔑 Configuración de API Keys

### Método 1: Variables de Entorno (Recomendado)

```bash
# Crear archivo .env en tools\
echo EMULE_API_KEY=tu-key-aqui > .env
echo ANTHROPIC_API_KEY=tu-key-aqui >> .env
```

### Método 2: Variables del Sistema

```bash
# Windows
setx EMULE_API_KEY "tu-key-aqui"
setx ANTHROPIC_API_KEY "tu-key-aqui"
```

### Método 3: Interactivo

Los scripts de inicio solicitarán las keys si no están configuradas.

---

## 📋 Dependencias Instaladas

### Core
- `requests` - HTTP client
- `httpx` - HTTP client async
- `anthropic` - Claude API

### MCP
- `mcp` - MCP SDK
- `mcp-cli` - CLI tools

### Opcionales
- `colorlog` - Logging con colores
- `tqdm` - Progress bars

---

## 🛠️ Troubleshooting

### Error: "Python no encontrado"

**Solución:**
```bash
# Instalar Python 3.8+
# https://www.python.org/downloads/

# Verificar instalación
python --version
```

### Error: "No se pudo crear entorno virtual"

**Solución:**
```bash
# Instalar venv
python -m pip install --upgrade pip

# Verificar venv
python -m venv --help
```

### Error: "pip install falla"

**Solución:**
```bash
# Actualizar pip
python -m pip install --upgrade pip

# Instalar con verbose
pip install -r requirements_all.txt -v
```

### Error: "ModuleNotFoundError"

**Solución:**
```bash
# Verificar que el entorno está activado
# Debe aparecer (_env) en el prompt

# Reinstalar dependencias
pip install -r requirements_all.txt --force-reinstall
```

---

## 📁 Estructura de Archivos

```
tools/
├── _env/                          # Entorno virtual (creado)
├── install_python_deps.bat        # Instalador
├── activate_env.bat               # Activar entorno
├── start_vision_auditor.bat       # Launcher Vision Auditor
├── start_orchestrator.bat         # Launcher Orchestrator
├── requirements.txt               # Deps Vision Auditor
├── requirements_mcp.txt           # Deps MCP Server
├── requirements_all.txt           # Todas las deps
├── vision_auditor_v2.py           # Vision Auditor
├── emule_mcp_server.py            # MCP Server
├── orchestrator_agent.py          # Orchestrator
└── orchestrator_handlers.py       # Handlers
```

---

## ✅ Verificación de Instalación

```bash
# 1. Activar entorno
activate_env.bat

# 2. Verificar Python
python --version

# 3. Verificar dependencias
pip list

# 4. Verificar imports
python -c "import requests, anthropic, httpx, mcp; print('OK')"
```

Si todo funciona, verás: `OK`

---

## 🎓 Próximos Pasos

1. **Configurar API Keys** (ver sección anterior)
2. **Iniciar eMule-Aishor**
3. **Ejecutar Vision Auditor:**
   ```bash
   start_vision_auditor.bat
   ```
4. **O ejecutar Orchestrator:**
   ```bash
   start_orchestrator.bat
   ```

---

**Versión:** R1.3 "FiberSight"  
**Fecha:** Enero 2026
