# FiberSight R1.3 - Instrucciones para Release Pack

## 📦 Copiar Archivos Python al Release Pack

Para preparar el Release Pack con los componentes Python, copia los siguientes archivos desde `tools\` a `Release_Pack\tools\`:

### Scripts de Instalación
```
tools\install_python_deps.bat  → Release_Pack\tools\
tools\activate_env.bat         → Release_Pack\tools\
tools\start_vision_auditor.bat → Release_Pack\tools\
tools\start_orchestrator.bat   → Release_Pack\tools\
```

### Componentes Python
```
tools\vision_auditor_v2.py      → Release_Pack\tools\
tools\emule_mcp_server.py       → Release_Pack\tools\
tools\orchestrator_agent.py     → Release_Pack\tools\
tools\orchestrator_handlers.py  → Release_Pack\tools\
```

### Requirements
```
tools\requirements.txt          → Release_Pack\tools\
tools\requirements_mcp.txt      → Release_Pack\tools\
tools\requirements_all.txt      → Release_Pack\tools\
```

### Documentación
```
tools\README.md                 → Release_Pack\tools\
tools\INSTALACION_PYTHON.md     → Release_Pack\tools\
tools\CLAUDE_DESKTOP_CONFIG.md  → Release_Pack\tools\
```

### Scripts Auxiliares
```
tools\run_auditor.bat           → Release_Pack\tools\
tools\run_auditor.sh            → Release_Pack\tools\
```

---

## 🚀 Instrucciones para el Usuario Final

### 1. Instalación Inicial

```
1. Extraer Release Pack
2. Abrir terminal en Release_Pack\tools\
3. Ejecutar: install_python_deps.bat
4. Esperar a que termine (crea _env\ e instala dependencias)
```

### 2. Configurar API Keys

```
Opción A: Variables de entorno
  setx EMULE_API_KEY "tu-key-de-emule"
  setx ANTHROPIC_API_KEY "tu-key-de-anthropic"

Opción B: Los scripts las pedirán interactivamente
```

### 3. Uso

```
# Iniciar eMule-Aishor primero
emule.exe

# Luego iniciar Vision Auditor
cd Release_Pack\tools
start_vision_auditor.bat

# O iniciar Orchestrator
start_orchestrator.bat
```

---

## 📁 Estructura Final del Release Pack

```
Release_Pack/
├── emule.exe
├── lang/
├── webinterface/
├── Configurar_Firewall.ps1
├── LEEME.txt
├── NOTAS_RELEASE.md
└── tools/                              # NUEVO
    ├── _env/                           # Creado por install_python_deps.bat
    ├── install_python_deps.bat         # Instalador
    ├── activate_env.bat                # Activar entorno
    ├── start_vision_auditor.bat        # Launcher Vision Auditor
    ├── start_orchestrator.bat          # Launcher Orchestrator
    ├── vision_auditor_v2.py            # Vision Auditor
    ├── emule_mcp_server.py             # MCP Server
    ├── orchestrator_agent.py           # Orchestrator
    ├── orchestrator_handlers.py        # Handlers
    ├── requirements.txt                # Deps Vision Auditor
    ├── requirements_mcp.txt            # Deps MCP
    ├── requirements_all.txt            # Todas las deps
    ├── README.md                       # Guía Vision Auditor
    ├── INSTALACION_PYTHON.md           # Guía instalación
    ├── CLAUDE_DESKTOP_CONFIG.md        # Guía MCP
    ├── run_auditor.bat                 # Script auxiliar
    └── run_auditor.sh                  # Script auxiliar
```

---

## ✅ Verificación

Después de copiar los archivos, el usuario debe poder:

1. Ejecutar `install_python_deps.bat` sin errores
2. Ver el directorio `_env\` creado
3. Ejecutar `start_vision_auditor.bat` y que funcione
4. Ejecutar `start_orchestrator.bat` y que funcione

---

**Nota:** El directorio `_env\` NO debe incluirse en el Release Pack, se crea automáticamente en la máquina del usuario.
