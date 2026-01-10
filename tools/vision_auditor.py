#!/usr/bin/env python3
"""
Vision Auditor - Análisis visual automático de descargas de eMule
Parte de eMule-Aishor R1.3 - Vision Verification

Este script:
1. Consulta la API de eMule para obtener descargas activas
2. Filtra videos con progreso suficiente (>5MB)
3. Activa preview mode para priorizar chunks críticos
4. Extrae fotogramas con FFmpeg
5. Analiza con Claude Vision para detectar fakes
6. Toma acciones automáticas (cancelar, banear)
"""

import requests
import subprocess
import json
import time
import base64
import logging
from pathlib import Path
from typing import Dict, List, Optional
from dataclasses import dataclass
from datetime import datetime

# Configuración de logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('vision_auditor.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)


@dataclass
class AuditResult:
    """Resultado del análisis de un archivo"""
    hash: str
    name: str
    verdict: str  # "REAL" o "FAKE"
    confidence: float  # 0.0 - 1.0
    reason: str
    timestamp: datetime


class VisionAuditor:
    """Auditor visual de descargas de eMule"""
    
    def __init__(self, emule_api_url: str, api_key: str, anthropic_api_key: str):
        """
        Inicializar el auditor
        
        Args:
            emule_api_url: URL base de la API de eMule (ej: http://localhost:4711/api/v1)
            api_key: API Key para autenticación con eMule
            anthropic_api_key: API Key de Anthropic para Claude
        """
        self.api_url = emule_api_url
        self.headers = {"Authorization": f"Bearer {api_key}"}
        self.anthropic_api_key = anthropic_api_key
        self.temp_dir = Path("./temp_frames")
        self.temp_dir.mkdir(exist_ok=True)
        
        # Estadísticas
        self.stats = {
            "total_analyzed": 0,
            "fakes_detected": 0,
            "real_files": 0,
            "errors": 0
        }
        
        logger.info("Vision Auditor iniciado")
        logger.info(f"API URL: {self.api_url}")
        logger.info(f"Temp dir: {self.temp_dir}")
    
    def get_downloads(self) -> List[Dict]:
        """Obtener lista de descargas activas"""
        try:
            resp = requests.get(
                f"{self.api_url}/downloads/active",
                headers=self.headers,
                timeout=10
            )
            resp.raise_for_status()
            data = resp.json()
            return data.get("downloads", [])
        except Exception as e:
            logger.error(f"Error obteniendo descargas: {e}")
            return []
    
    def get_file_info(self, file_hash: str) -> Optional[Dict]:
        """Obtener información detallada del archivo"""
        try:
            resp = requests.get(
                f"{self.api_url}/downloads/{file_hash}/file_info",
                headers=self.headers,
                timeout=10
            )
            resp.raise_for_status()
            return resp.json()
        except Exception as e:
            logger.error(f"Error obteniendo info de {file_hash}: {e}")
            return None
    
    def enable_preview(self, file_hash: str) -> bool:
        """Activar modo preview para un archivo"""
        try:
            resp = requests.post(
                f"{self.api_url}/downloads/{file_hash}/preview",
                json={"enable": True},
                headers=self.headers,
                timeout=10
            )
            resp.raise_for_status()
            logger.info(f"Preview mode activado para {file_hash}")
            return True
        except Exception as e:
            logger.error(f"Error activando preview para {file_hash}: {e}")
            return False
    
    def extract_frame(self, file_path: str, output_path: str, timestamp: str = "00:05:00") -> bool:
        """
        Extraer fotograma con FFmpeg
        
        Args:
            file_path: Ruta al archivo .part
            output_path: Ruta donde guardar el fotograma
            timestamp: Timestamp del fotograma (formato HH:MM:SS)
        
        Returns:
            True si la extracción fue exitosa
        """
        try:
            cmd = [
                "ffmpeg",
                "-ss", timestamp,  # Seek a timestamp
                "-i", file_path,   # Input file
                "-vframes", "1",   # Extraer 1 frame
                "-q:v", "2",       # Calidad alta
                "-y",              # Sobrescribir
                output_path
            ]
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode == 0 and Path(output_path).exists():
                logger.info(f"Fotograma extraído: {output_path}")
                return True
            else:
                logger.error(f"FFmpeg falló: {result.stderr}")
                return False
                
        except subprocess.TimeoutExpired:
            logger.error(f"FFmpeg timeout para {file_path}")
            return False
        except Exception as e:
            logger.error(f"Error extrayendo fotograma: {e}")
            return False
    
    def analyze_frame(self, image_path: str) -> Dict:
        """
        Analizar fotograma con Claude Vision
        
        Args:
            image_path: Ruta al fotograma
        
        Returns:
            Dict con verdict, confidence, reason
        """
        try:
            # Leer imagen y convertir a base64
            with open(image_path, "rb") as f:
                image_data = base64.standard_b64encode(f.read()).decode("utf-8")
            
            # Llamar a Claude API
            headers = {
                "x-api-key": self.anthropic_api_key,
                "anthropic-version": "2023-06-01",
                "content-type": "application/json"
            }
            
            payload = {
                "model": "claude-3-5-sonnet-20241022",
                "max_tokens": 1024,
                "messages": [{
                    "role": "user",
                    "content": [
                        {
                            "type": "image",
                            "source": {
                                "type": "base64",
                                "media_type": "image/jpeg",
                                "data": image_data
                            }
                        },
                        {
                            "type": "text",
                            "text": """Analiza esta imagen extraída de un archivo de video descargado.

Determina si es:
1. REAL: Contenido de video legítimo (película, serie, documental, etc.)
2. FAKE: Publicidad, spam, contenido no relacionado, pantalla negra, error, etc.

Responde SOLO con un JSON válido:
{
  "verdict": "REAL" o "FAKE",
  "confidence": 0.0-1.0,
  "reason": "breve explicación en español"
}

No incluyas ningún texto adicional fuera del JSON."""
                        }
                    ]
                }]
            }
            
            resp = requests.post(
                "https://api.anthropic.com/v1/messages",
                headers=headers,
                json=payload,
                timeout=30
            )
            resp.raise_for_status()
            
            # Parsear respuesta
            response_data = resp.json()
            content = response_data["content"][0]["text"]
            
            # Extraer JSON de la respuesta
            # Claude a veces añade texto antes/después del JSON
            start = content.find("{")
            end = content.rfind("}") + 1
            if start != -1 and end > start:
                json_str = content[start:end]
                result = json.loads(json_str)
                
                logger.info(f"Análisis: {result['verdict']} (confianza: {result['confidence']:.2f})")
                return result
            else:
                logger.error(f"No se pudo parsear JSON de respuesta: {content}")
                return {
                    "verdict": "UNKNOWN",
                    "confidence": 0.0,
                    "reason": "Error parseando respuesta de Claude"
                }
                
        except Exception as e:
            logger.error(f"Error analizando con Claude: {e}")
            return {
                "verdict": "ERROR",
                "confidence": 0.0,
                "reason": str(e)
            }
    
    def ban_and_delete(self, file_hash: str) -> bool:
        """Banear fuentes y eliminar descarga"""
        try:
            # Ban all sources
            resp1 = requests.post(
                f"{self.api_url}/downloads/{file_hash}/action",
                json={"action": "ban_all_sources"},
                headers=self.headers,
                timeout=10
            )
            
            # Delete file
            resp2 = requests.post(
                f"{self.api_url}/downloads/{file_hash}/action",
                json={"action": "delete"},
                headers=self.headers,
                timeout=10
            )
            
            logger.info(f"Archivo {file_hash} eliminado y fuentes baneadas")
            return True
            
        except Exception as e:
            logger.error(f"Error eliminando {file_hash}: {e}")
            return False
    
    def audit_single_file(self, download: Dict) -> Optional[AuditResult]:
        """Auditar un solo archivo"""
        file_hash = download.get("hash")
        file_name = download.get("name")
        
        logger.info(f"Auditando: {file_name}")
        
        # Verificar que sea video
        if not download.get("is_video"):
            logger.debug(f"Saltando {file_name}: no es video")
            return None
        
        # Verificar progreso mínimo
        completed_size = download.get("completed_size", 0)
        if completed_size < 5 * 1024 * 1024:  # 5MB
            logger.debug(f"Saltando {file_name}: progreso insuficiente ({completed_size / 1024 / 1024:.1f}MB)")
            return None
        
        # Obtener info detallada
        info = self.get_file_info(file_hash)
        if not info:
            logger.error(f"No se pudo obtener info de {file_name}")
            self.stats["errors"] += 1
            return None
        
        # Activar preview si no está listo
        if not info.get("preview_ready"):
            logger.info(f"Activando preview mode para {file_name}")
            self.enable_preview(file_hash)
            return None  # Esperar al siguiente ciclo
        
        # Verificar que preview mode esté activo
        if not info.get("preview_mode"):
            logger.info(f"Activando preview mode para {file_name}")
            self.enable_preview(file_hash)
        
        # Extraer fotograma
        file_path = info.get("file_path")
        if not file_path:
            logger.error(f"No se pudo obtener ruta de {file_name}")
            self.stats["errors"] += 1
            return None
        
        frame_path = self.temp_dir / f"frame_{file_hash}.jpg"
        
        if not self.extract_frame(file_path, str(frame_path)):
            logger.error(f"Error extrayendo fotograma de {file_name}")
            self.stats["errors"] += 1
            return None
        
        # Analizar con Claude
        analysis = self.analyze_frame(str(frame_path))
        
        # Crear resultado
        result = AuditResult(
            hash=file_hash,
            name=file_name,
            verdict=analysis.get("verdict", "UNKNOWN"),
            confidence=analysis.get("confidence", 0.0),
            reason=analysis.get("reason", ""),
            timestamp=datetime.now()
        )
        
        # Limpiar fotograma temporal
        try:
            frame_path.unlink()
        except:
            pass
        
        # Actualizar estadísticas
        self.stats["total_analyzed"] += 1
        
        # Tomar acción si es fake
        if result.verdict == "FAKE" and result.confidence > 0.7:
            logger.warning(f"⚠️ FAKE DETECTADO: {file_name}")
            logger.warning(f"   Confianza: {result.confidence:.2%}")
            logger.warning(f"   Razón: {result.reason}")
            
            self.ban_and_delete(file_hash)
            self.stats["fakes_detected"] += 1
        elif result.verdict == "REAL":
            logger.info(f"✅ REAL: {file_name} (confianza: {result.confidence:.2%})")
            self.stats["real_files"] += 1
        
        return result
    
    def audit_downloads(self) -> List[AuditResult]:
        """Proceso principal de auditoría"""
        logger.info("Iniciando ciclo de auditoría...")
        
        downloads = self.get_downloads()
        logger.info(f"Encontradas {len(downloads)} descargas activas")
        
        results = []
        
        for dl in downloads:
            try:
                result = self.audit_single_file(dl)
                if result:
                    results.append(result)
            except Exception as e:
                logger.error(f"Error auditando {dl.get('name', 'unknown')}: {e}")
                self.stats["errors"] += 1
        
        logger.info(f"Ciclo completado. Analizados: {len(results)}")
        return results
    
    def run_continuous(self, interval: int = 300):
        """
        Ejecutar auditoría continua
        
        Args:
            interval: Intervalo entre ciclos en segundos (default: 5 minutos)
        """
        logger.info(f"Iniciando auditoría continua (intervalo: {interval}s)")
        
        try:
            while True:
                self.audit_downloads()
                
                # Mostrar estadísticas
                logger.info("=" * 50)
                logger.info("ESTADÍSTICAS:")
                logger.info(f"  Total analizados: {self.stats['total_analyzed']}")
                logger.info(f"  Fakes detectados: {self.stats['fakes_detected']}")
                logger.info(f"  Archivos reales: {self.stats['real_files']}")
                logger.info(f"  Errores: {self.stats['errors']}")
                logger.info("=" * 50)
                
                logger.info(f"Esperando {interval} segundos...")
                time.sleep(interval)
                
        except KeyboardInterrupt:
            logger.info("Auditoría detenida por el usuario")
        except Exception as e:
            logger.error(f"Error fatal: {e}")
            raise


def main():
    """Punto de entrada principal"""
    import argparse
    
    parser = argparse.ArgumentParser(description="Vision Auditor para eMule-Aishor")
    parser.add_argument(
        "--api-url",
        default="http://localhost:4711/api/v1",
        help="URL de la API de eMule"
    )
    parser.add_argument(
        "--api-key",
        required=True,
        help="API Key de eMule"
    )
    parser.add_argument(
        "--anthropic-key",
        required=True,
        help="API Key de Anthropic"
    )
    parser.add_argument(
        "--interval",
        type=int,
        default=300,
        help="Intervalo entre ciclos en segundos (default: 300)"
    )
    parser.add_argument(
        "--once",
        action="store_true",
        help="Ejecutar solo un ciclo y salir"
    )
    
    args = parser.parse_args()
    
    # Crear auditor
    auditor = VisionAuditor(
        emule_api_url=args.api_url,
        api_key=args.api_key,
        anthropic_api_key=args.anthropic_key
    )
    
    # Ejecutar
    if args.once:
        auditor.audit_downloads()
    else:
        auditor.run_continuous(interval=args.interval)


if __name__ == "__main__":
    main()
