#!/bin/bash
# Script de ejemplo para ejecutar Vision Auditor en Linux/Mac
# eMule-Aishor R1.3 - Vision Verification

echo "========================================"
echo "Vision Auditor - eMule-Aishor R1.3"
echo "========================================"
echo

# Configuración
EMULE_API_KEY="YOUR_EMULE_API_KEY_HERE"
ANTHROPIC_KEY="YOUR_ANTHROPIC_API_KEY_HERE"
INTERVAL=300

# Verificar que FFmpeg esté instalado
if ! command -v ffmpeg &> /dev/null; then
    echo "ERROR: FFmpeg no encontrado"
    echo
    echo "Instalar FFmpeg:"
    echo "  Ubuntu/Debian: sudo apt install ffmpeg"
    echo "  macOS: brew install ffmpeg"
    exit 1
fi

echo "FFmpeg encontrado: OK"
echo

# Verificar que Python esté instalado
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 no encontrado"
    exit 1
fi

echo "Python encontrado: OK"
echo

# Instalar dependencias si es necesario
echo "Verificando dependencias Python..."
python3 -c "import requests" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "Instalando dependencias..."
    pip3 install -r requirements.txt
fi

echo
echo "Iniciando Vision Auditor..."
echo "Intervalo: $INTERVAL segundos"
echo
echo "Presiona Ctrl+C para detener"
echo

# Ejecutar auditor
python3 vision_auditor.py \
  --api-key "$EMULE_API_KEY" \
  --anthropic-key "$ANTHROPIC_KEY" \
  --interval $INTERVAL
