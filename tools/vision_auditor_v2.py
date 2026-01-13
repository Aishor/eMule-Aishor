#!/usr/bin/env python3
"""
Vision Auditor V2 - Con Máquina de Estados y Mejoras Críticas
eMule-Aishor R1.3 - Vision Verification

Mejoras implementadas:
- Máquina de estados con 13 estados
- TTL de 6h para preview timeout
- Modo Observador/Cautious/Terminator
- Filtro pre-LLM heurístico
- Multi-frame analysis
- Gestión de reintentos
- Persistencia de estados
"""

import requests
import subprocess
import json
import time
import base64
import logging
import sqlite3
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass, asdict
from datetime import datetime, timedelta
from enum import Enum

# Configuración de logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('vision_auditor_v2.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)


# ============================================================================
# ENUMS Y DATACLASSES
# ============================================================================

class DownloadVerificationState(Enum):
    """Estados del ciclo de vida de verificación"""
    NEW = "new"
    WAITING_PREVIEW = "waiting_preview"
    READY_TO_SCAN = "ready_to_scan"
    ANALYZING = "analyzing"
    VERIFIED = "verified"
    FLAGGED = "flagged"
    PAUSED = "paused"
    DELETED = "deleted"
    TIMEOUT = "timeout"
    STALLED = "stalled"
    ERROR = "error"
    RETRY = "retry"
    UNVERIFIABLE = "unverifiable"


class OperationMode(Enum):
    """Modos de operación del auditor"""
    OBSERVER = "observer"      # Solo logging, no acciones
    CAUTIOUS = "cautious"      # Pausa, no borra
    TERMINATOR = "terminator"  # Borra automáticamente


@dataclass
class AuditResult:
    """Resultado del análisis de un archivo"""
    hash: str
    name: str
    verdict: str  # "REAL" o "FAKE"
    fake_type: Optional[str] = None  # "PASSWORD", "CODEC", "AD", "LOOP", etc.
    confidence: float = 0.0
    reason: str = ""
    timestamp: datetime = None
    frames_analyzed: int = 1
    
    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = datetime.now()


@dataclass
class DownloadTracking:
    """Estado de verificación de una descarga"""
    hash: str
    name: str
    state: DownloadVerificationState
    
    # Timestamps
    first_seen: datetime
    last_updated: datetime
    preview_requested: Optional[datetime] = None
    analysis_started: Optional[datetime] = None
    
    # Contadores
    preview_retries: int = 0
    analysis_retries: int = 0
    
    # Datos de progreso
    last_completed_size: int = 0
    stall_check_count: int = 0
    
    # Resultado
    audit_result: Optional[AuditResult] = None
    
    # Configuración
    ttl_hours: int = 6
    max_preview_retries: int = 3
    max_analysis_retries: int = 2
    
    def is_preview_timeout(self) -> bool:
        """Verificar si el preview ha expirado"""
        if not self.preview_requested:
            return False
        elapsed = datetime.now() - self.preview_requested
        return elapsed.total_seconds() > (self.ttl_hours * 3600)
    
    def can_retry_preview(self) -> bool:
        """Verificar si puede reintentar preview"""
        return self.preview_retries < self.max_preview_retries
    
    def can_retry_analysis(self) -> bool:
        """Verificar si puede reintentar análisis"""
        return self.analysis_retries < self.max_analysis_retries


# ============================================================================
# GESTOR DE ESTADOS
# ============================================================================

class DownloadStateManager:
    """Gestiona el ciclo de vida de verificación de descargas"""
    
    def __init__(self, db_path: str = "download_states.db"):
        self.tracked: Dict[str, DownloadTracking] = {}
        self.db_path = Path(db_path)
        self.init_database()
        self.load_state()
    
    def init_database(self):
        """Inicializar base de datos SQLite"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS download_tracking (
                hash TEXT PRIMARY KEY,
                name TEXT,
                state TEXT,
                first_seen TEXT,
                last_updated TEXT,
                preview_requested TEXT,
                preview_retries INTEGER,
                analysis_retries INTEGER,
                last_completed_size INTEGER,
                verdict TEXT,
                confidence REAL,
                reason TEXT
            )
        ''')
        
        conn.commit()
        conn.close()
    
    def save_state(self):
        """Guardar estado a base de datos"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        for tracking in self.tracked.values():
            cursor.execute('''
                INSERT OR REPLACE INTO download_tracking VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            ''', (
                tracking.hash,
                tracking.name,
                tracking.state.value,
                tracking.first_seen.isoformat(),
                tracking.last_updated.isoformat(),
                tracking.preview_requested.isoformat() if tracking.preview_requested else None,
                tracking.preview_retries,
                tracking.analysis_retries,
                tracking.last_completed_size,
                tracking.audit_result.verdict if tracking.audit_result else None,
                tracking.audit_result.confidence if tracking.audit_result else None,
                tracking.audit_result.reason if tracking.audit_result else None
            ))
        
        conn.commit()
        conn.close()
    
    def load_state(self):
        """Cargar estado desde base de datos"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('SELECT * FROM download_tracking')
        rows = cursor.fetchall()
        
        for row in rows:
            hash_val, name, state, first_seen, last_updated, preview_req, \
            preview_retries, analysis_retries, last_size, verdict, confidence, reason = row
            
            tracking = DownloadTracking(
                hash=hash_val,
                name=name,
                state=DownloadVerificationState(state),
                first_seen=datetime.fromisoformat(first_seen),
                last_updated=datetime.fromisoformat(last_updated),
                preview_requested=datetime.fromisoformat(preview_req) if preview_req else None,
                preview_retries=preview_retries,
                analysis_retries=analysis_retries,
                last_completed_size=last_size
            )
            
            if verdict:
                tracking.audit_result = AuditResult(
                    hash=hash_val,
                    name=name,
                    verdict=verdict,
                    confidence=confidence or 0.0,
                    reason=reason or ""
                )
            
            self.tracked[hash_val] = tracking
        
        conn.close()
        logger.info(f"Cargados {len(self.tracked)} estados desde base de datos")


# ============================================================================
# VISION AUDITOR V2
# ============================================================================

class VisionAuditorV2:
    """Auditor visual de descargas con máquina de estados"""
    
    def __init__(self, emule_api_url: str, api_key: str, anthropic_api_key: str,
                 mode: OperationMode = OperationMode.OBSERVER):
        """
        Inicializar el auditor
        
        Args:
            emule_api_url: URL base de la API de eMule
            api_key: API Key para autenticación con eMule
            anthropic_api_key: API Key de Anthropic para Claude
            mode: Modo de operación (OBSERVER, CAUTIOUS, TERMINATOR)
        """
        self.api_url = emule_api_url
        self.headers = {"Authorization": f"Bearer {api_key}"}
        self.anthropic_api_key = anthropic_api_key
        self.mode = mode
        self.temp_dir = Path("./temp_frames")
        self.temp_dir.mkdir(exist_ok=True)
        
        # Gestor de estados
        self.state_manager = DownloadStateManager()
        
        # Estadísticas
        self.stats = {
            "total_analyzed": 0,
            "fakes_detected": 0,
            "real_files": 0,
            "errors": 0,
            "pre_llm_filtered": 0,
            "cost_saved": 0.0
        }
        
        logger.info("=" * 60)
        logger.info("Vision Auditor V2 iniciado")
        logger.info(f"Modo de operación: {self.mode.value.upper()}")
        logger.info(f"API URL: {self.api_url}")
        logger.info(f"Temp dir: {self.temp_dir}")
        logger.info("=" * 60)
    
    # ========================================================================
    # API METHODS
    # ========================================================================
    
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
            return True
        except Exception as e:
            logger.error(f"Error activando preview para {file_hash}: {e}")
            return False
    
    def pause_download(self, file_hash: str) -> bool:
        """Pausar descarga"""
        try:
            resp = requests.put(
                f"{self.api_url}/downloads/{file_hash}/pause",
                headers=self.headers,
                timeout=10
            )
            resp.raise_for_status()
            return True
        except Exception as e:
            logger.error(f"Error pausando {file_hash}: {e}")
            return False
    
    def ban_and_delete(self, file_hash: str) -> bool:
        """Banear fuentes y eliminar descarga"""
        try:
            # Ban all sources
            requests.post(
                f"{self.api_url}/downloads/{file_hash}/action",
                json={"action": "ban_all_sources"},
                headers=self.headers,
                timeout=10
            )
            
            # Delete file
            requests.post(
                f"{self.api_url}/downloads/{file_hash}/action",
                json={"action": "delete"},
                headers=self.headers,
                timeout=10
            )
            
            return True
        except Exception as e:
            logger.error(f"Error eliminando {file_hash}: {e}")
            return False
    
    # ========================================================================
    # PRE-LLM FILTER
    # ========================================================================
    
    def pre_llm_filter(self, file_path: str, info: Dict) -> Optional[str]:
        """
        Filtro heurístico ANTES de llamar al LLM
        Retorna: None si pasa filtro, "REASON" si falla
        """
        try:
            # 1. Verificar que FFprobe puede leer el archivo
            probe_cmd = [
                "ffprobe",
                "-v", "error",
                "-show_format",
                "-show_streams",
                file_path
            ]
            
            result = subprocess.run(probe_cmd, capture_output=True, text=True, timeout=10)
            
            if result.returncode != 0:
                logger.warning(f"FFprobe falló: {result.stderr[:200]}")
                return "CODEC_ERROR"
            
            # 2. Parsear duración
            duration = self._extract_duration(result.stdout)
            if duration and duration < 300:  # Menos de 5 minutos
                logger.warning(f"Video muy corto: {duration}s")
                return "TOO_SHORT"
            
            # 3. Verificar codec
            codec = self._extract_codec(result.stdout)
            if codec and codec.lower() not in ["h264", "h265", "hevc", "vp9", "av1", "mpeg4"]:
                logger.warning(f"Codec desconocido: {codec}")
                return "UNKNOWN_CODEC"
            
            # 4. Verificar bitrate sospechoso
            if duration:
                bitrate = (info["file_size"] * 8) / duration
                if bitrate > 50_000_000:  # >50 Mbps
                    logger.warning(f"Bitrate sospechoso: {bitrate/1_000_000:.1f} Mbps")
                    return "SUSPICIOUS_BITRATE"
            
            return None  # Pasa todos los filtros
            
        except subprocess.TimeoutExpired:
            logger.error("FFprobe timeout")
            return "TIMEOUT"
        except Exception as e:
            logger.error(f"Error en pre-filter: {e}")
            return "ERROR"
    
    def _extract_duration(self, ffprobe_output: str) -> Optional[float]:
        """Extraer duración del output de ffprobe"""
        for line in ffprobe_output.split('\n'):
            if 'duration=' in line.lower():
                try:
                    duration_str = line.split('=')[1].strip()
                    return float(duration_str)
                except:
                    pass
        return None
    
    def _extract_codec(self, ffprobe_output: str) -> Optional[str]:
        """Extraer codec del output de ffprobe"""
        for line in ffprobe_output.split('\n'):
            if 'codec_name=' in line.lower():
                try:
                    codec = line.split('=')[1].strip()
                    return codec
                except:
                    pass
        return None
    
    # ========================================================================
    # FRAME EXTRACTION
    # ========================================================================
    
    def extract_frame(self, file_path: str, output_path: str, timestamp: str = "00:05:00") -> bool:
        """Extraer fotograma con FFmpeg"""
        try:
            cmd = [
                "ffmpeg",
                "-err_detect", "ignore_err",
                "-fflags", "+genpts+igndts",
                "-ss", timestamp,
                "-i", file_path,
                "-vframes", "1",
                "-f", "image2",
                "-q:v", "2",
                "-y",
                output_path
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0 and Path(output_path).exists():
                return True
            else:
                logger.error(f"FFmpeg falló: {result.stderr[:200]}")
                return False
                
        except subprocess.TimeoutExpired:
            logger.error(f"FFmpeg timeout para {file_path}")
            return False
        except Exception as e:
            logger.error(f"Error extrayendo fotograma: {e}")
            return False
    
    def extract_multiple_frames(self, file_path: str, file_hash: str) -> List[str]:
        """Extraer múltiples frames para análisis robusto"""
        timestamps = ["00:01:00", "00:05:00", "00:10:00"]
        frames = []
        
        for i, ts in enumerate(timestamps):
            frame_path = self.temp_dir / f"frame_{file_hash}_{i}.jpg"
            if self.extract_frame(file_path, str(frame_path), ts):
                frames.append(str(frame_path))
        
        return frames
    
    # ========================================================================
    # LLM ANALYSIS
    # ========================================================================
    
    def analyze_frames(self, frame_paths: List[str]) -> Dict:
        """Analizar fotogramas con Claude Vision"""
        try:
            # Preparar imágenes
            images_data = []
            for frame_path in frame_paths:
                with open(frame_path, "rb") as f:
                    image_data = base64.standard_b64encode(f.read()).decode("utf-8")
                    images_data.append(image_data)
            
            # Construir mensaje
            content = []
            for img_data in images_data:
                content.append({
                    "type": "image",
                    "source": {
                        "type": "base64",
                        "media_type": "image/jpeg",
                        "data": img_data
                    }
                })
            
            # Prompt mejorado
            prompt = f"""Analiza estas {len(frame_paths)} imágenes extraídas de un archivo de video en diferentes momentos.

TIPOS DE FAKE A DETECTAR:
1. PASSWORD PROTECTED: Muestra mensaje "Visit www... for password"
2. CODEC ERROR: Mensaje "Download codec to view"
3. PUBLICIDAD: Anuncios, banners, sitios web comerciales
4. LOOP: Las imágenes son idénticas o muy similares (video repetido)
5. CONTENIDO INCORRECTO: No coincide con el nombre del archivo
6. PANTALLA NEGRA/GRIS: Solo negro, gris o sin contenido

REAL: Contenido de película/serie legítimo con progresión de escena

Responde SOLO con un JSON válido:
{{
  "verdict": "REAL" o "FAKE",
  "fake_type": "PASSWORD|CODEC|AD|LOOP|MISMATCH|BLACK|null",
  "confidence": 0.0-1.0,
  "reason": "explicación breve en español"
}}

No incluyas ningún texto adicional fuera del JSON."""
            
            content.append({"type": "text", "text": prompt})
            
            # Llamar a Claude API
            headers = {
                "x-api-key": self.anthropic_api_key,
                "anthropic-version": "2023-06-01",
                "content-type": "application/json"
            }
            
            payload = {
                "model": "claude-3-5-sonnet-20241022",
                "max_tokens": 1024,
                "messages": [{"role": "user", "content": content}]
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
            content_text = response_data["content"][0]["text"]
            
            # Extraer JSON
            start = content_text.find("{")
            end = content_text.rfind("}") + 1
            if start != -1 and end > start:
                json_str = content_text[start:end]
                result = json.loads(json_str)
                return result
            else:
                logger.error(f"No se pudo parsear JSON: {content_text}")
                return {"verdict": "ERROR", "confidence": 0.0, "reason": "Parse error"}
                
        except Exception as e:
            logger.error(f"Error analizando con Claude: {e}")
            return {"verdict": "ERROR", "confidence": 0.0, "reason": str(e)}
    
    # ========================================================================
    # STATE MACHINE
    # ========================================================================
    
    def process_download(self, download: Dict) -> DownloadTracking:
        """Procesar una descarga según su estado actual"""
        
        file_hash = download["hash"]
        
        # Obtener o crear tracking
        if file_hash not in self.state_manager.tracked:
            self.state_manager.tracked[file_hash] = DownloadTracking(
                hash=file_hash,
                name=download["name"],
                state=DownloadVerificationState.NEW,
                first_seen=datetime.now(),
                last_updated=datetime.now()
            )
        
        tracking = self.state_manager.tracked[file_hash]
        tracking.last_updated = datetime.now()
        
        # Máquina de estados
        if tracking.state == DownloadVerificationState.NEW:
            self._handle_new(tracking, download)
        
        elif tracking.state == DownloadVerificationState.WAITING_PREVIEW:
            self._handle_waiting_preview(tracking, download)
        
        elif tracking.state == DownloadVerificationState.READY_TO_SCAN:
            self._handle_ready_to_scan(tracking, download)
        
        elif tracking.state == DownloadVerificationState.ANALYZING:
            self._handle_analyzing(tracking, download)
        
        elif tracking.state == DownloadVerificationState.ERROR:
            self._handle_error(tracking, download)
        
        elif tracking.state == DownloadVerificationState.FLAGGED:
            self._handle_flagged(tracking, download)
        
        return tracking
    
    def _handle_new(self, tracking: DownloadTracking, download: Dict):
        """Estado NEW: Verificar si cumple requisitos"""
        
        # Filtrar por tipo
        if not download.get("is_video"):
            tracking.state = DownloadVerificationState.UNVERIFIABLE
            logger.debug(f"[{tracking.hash[:8]}] NEW → UNVERIFIABLE (no es video)")
            return
        
        # Verificar progreso mínimo
        if download["completed_size"] < 5 * 1024 * 1024:
            logger.debug(f"[{tracking.hash[:8]}] NEW - Esperando progreso ({download['completed_size']/1024/1024:.1f}MB)")
            return
        
        # Activar preview mode
        if self.enable_preview(tracking.hash):
            tracking.state = DownloadVerificationState.WAITING_PREVIEW
            tracking.preview_requested = datetime.now()
            logger.info(f"[{tracking.hash[:8]}] NEW → WAITING_PREVIEW")
    
    def _handle_waiting_preview(self, tracking: DownloadTracking, download: Dict):
        """Estado WAITING_PREVIEW: Esperar chunks críticos"""
        
        # Verificar timeout
        if tracking.is_preview_timeout():
            if tracking.can_retry_preview():
                tracking.preview_retries += 1
                tracking.preview_requested = datetime.now()
                self.enable_preview(tracking.hash)
                logger.warning(f"[{tracking.hash[:8]}] Preview timeout, reintento {tracking.preview_retries}/{tracking.max_preview_retries}")
            else:
                tracking.state = DownloadVerificationState.TIMEOUT
                logger.error(f"[{tracking.hash[:8]}] WAITING_PREVIEW → TIMEOUT (max reintentos)")
            return
        
        # Verificar si está estancado
        if download["completed_size"] == tracking.last_completed_size:
            tracking.stall_check_count += 1
            if tracking.stall_check_count > 4:  # 4 ciclos sin progreso
                tracking.state = DownloadVerificationState.STALLED
                logger.error(f"[{tracking.hash[:8]}] WAITING_PREVIEW → STALLED (sin progreso)")
                return
        else:
            tracking.last_completed_size = download["completed_size"]
            tracking.stall_check_count = 0
        
        # Verificar si está listo
        info = self.get_file_info(tracking.hash)
        if info and info.get("preview_ready"):
            tracking.state = DownloadVerificationState.READY_TO_SCAN
            logger.info(f"[{tracking.hash[:8]}] WAITING_PREVIEW → READY_TO_SCAN")
    
    def _handle_ready_to_scan(self, tracking: DownloadTracking, download: Dict):
        """Estado READY_TO_SCAN: Iniciar análisis"""
        
        tracking.state = DownloadVerificationState.ANALYZING
        tracking.analysis_started = datetime.now()
        logger.info(f"[{tracking.hash[:8]}] READY_TO_SCAN → ANALYZING")
    
    def _handle_analyzing(self, tracking: DownloadTracking, download: Dict):
        """Estado ANALYZING: Ejecutar análisis completo"""
        
        try:
            info = self.get_file_info(tracking.hash)
            if not info:
                tracking.state = DownloadVerificationState.ERROR
                return
            
            file_path = info.get("file_path")
            if not file_path or not Path(file_path).exists():
                tracking.state = DownloadVerificationState.ERROR
                logger.error(f"[{tracking.hash[:8]}] Archivo no accesible: {file_path}")
                return
            
            # FILTRO PRE-LLM
            filter_result = self.pre_llm_filter(file_path, info)
            if filter_result:
                logger.warning(f"[{tracking.hash[:8]}] Filtrado pre-LLM: {filter_result}")
                self.stats["pre_llm_filtered"] += 1
                self.stats["cost_saved"] += 0.0045  # Ahorro estimado
                
                tracking.audit_result = AuditResult(
                    hash=tracking.hash,
                    name=tracking.name,
                    verdict="FAKE",
                    fake_type=filter_result,
                    confidence=0.9,
                    reason=f"Filtro pre-LLM: {filter_result}"
                )
                tracking.state = DownloadVerificationState.FLAGGED
                self.stats["fakes_detected"] += 1
                return
            
            # Extraer frames
            frames = self.extract_multiple_frames(file_path, tracking.hash)
            if not frames:
                tracking.state = DownloadVerificationState.ERROR
                logger.error(f"[{tracking.hash[:8]}] No se pudieron extraer frames")
                return
            
            # Analizar con LLM
            analysis = self.analyze_frames(frames)
            
            # Limpiar frames temporales
            for frame in frames:
                try:
                    Path(frame).unlink()
                except:
                    pass
            
            # Crear resultado
            tracking.audit_result = AuditResult(
                hash=tracking.hash,
                name=tracking.name,
                verdict=analysis.get("verdict", "ERROR"),
                fake_type=analysis.get("fake_type"),
                confidence=analysis.get("confidence", 0.0),
                reason=analysis.get("reason", ""),
                frames_analyzed=len(frames)
            )
            
            self.stats["total_analyzed"] += 1
            
            if tracking.audit_result.verdict == "REAL":
                tracking.state = DownloadVerificationState.VERIFIED
                self.stats["real_files"] += 1
                logger.info(f"[{tracking.hash[:8]}] ANALYZING → VERIFIED (confianza: {tracking.audit_result.confidence:.2%})")
            
            elif tracking.audit_result.verdict == "FAKE":
                tracking.state = DownloadVerificationState.FLAGGED
                self.stats["fakes_detected"] += 1
                logger.warning(f"[{tracking.hash[:8]}] ANALYZING → FLAGGED (tipo: {tracking.audit_result.fake_type}, confianza: {tracking.audit_result.confidence:.2%})")
            
            else:
                tracking.state = DownloadVerificationState.ERROR
                self.stats["errors"] += 1
        
        except Exception as e:
            logger.error(f"Error analizando {tracking.hash[:8]}: {e}")
            tracking.state = DownloadVerificationState.ERROR
            self.stats["errors"] += 1
    
    def _handle_error(self, tracking: DownloadTracking, download: Dict):
        """Estado ERROR: Reintentar o marcar como no verificable"""
        
        if tracking.can_retry_analysis():
            tracking.analysis_retries += 1
            tracking.state = DownloadVerificationState.READY_TO_SCAN
            logger.warning(f"[{tracking.hash[:8]}] ERROR → RETRY (intento {tracking.analysis_retries}/{tracking.max_analysis_retries})")
        else:
            tracking.state = DownloadVerificationState.UNVERIFIABLE
            logger.error(f"[{tracking.hash[:8]}] ERROR → UNVERIFIABLE (max reintentos)")
    
    def _handle_flagged(self, tracking: DownloadTracking, download: Dict):
        """Estado FLAGGED: Tomar acción según modo"""
        
        if not tracking.audit_result or tracking.audit_result.confidence < 0.7:
            return  # No tomar acción si confianza baja
        
        if self.mode == OperationMode.OBSERVER:
            logger.warning(f"[OBSERVER] Fake detectado: {tracking.name}")
            logger.warning(f"[OBSERVER] Tipo: {tracking.audit_result.fake_type}, Confianza: {tracking.audit_result.confidence:.2%}")
            logger.warning(f"[OBSERVER] Razón: {tracking.audit_result.reason}")
            logger.warning(f"[OBSERVER] NO SE TOMÓ ACCIÓN (modo observador)")
        
        elif self.mode == OperationMode.CAUTIOUS:
            logger.warning(f"[CAUTIOUS] Pausando: {tracking.name}")
            if self.pause_download(tracking.hash):
                tracking.state = DownloadVerificationState.PAUSED
                logger.info(f"[{tracking.hash[:8]}] FLAGGED → PAUSED")
        
        elif self.mode == OperationMode.TERMINATOR:
            logger.error(f"[TERMINATOR] Eliminando: {tracking.name}")
            logger.error(f"[TERMINATOR] Tipo: {tracking.audit_result.fake_type}, Confianza: {tracking.audit_result.confidence:.2%}")
            if self.ban_and_delete(tracking.hash):
                tracking.state = DownloadVerificationState.DELETED
                logger.info(f"[{tracking.hash[:8]}] FLAGGED → DELETED")
    
    # ========================================================================
    # MAIN LOOP
    # ========================================================================
    
    def audit_downloads(self):
        """Proceso principal de auditoría"""
        logger.info("Iniciando ciclo de auditoría...")
        
        downloads = self.get_downloads()
        logger.info(f"Encontradas {len(downloads)} descargas activas")
        
        for dl in downloads:
            try:
                self.process_download(dl)
            except Exception as e:
                logger.error(f"Error procesando {dl.get('name', 'unknown')}: {e}")
        
        # Guardar estado
        self.state_manager.save_state()
        
        # Mostrar estadísticas
        self.show_stats()
    
    def show_stats(self):
        """Mostrar estadísticas"""
        logger.info("=" * 60)
        logger.info("ESTADÍSTICAS:")
        logger.info(f"  Total analizados: {self.stats['total_analyzed']}")
        logger.info(f"  Fakes detectados: {self.stats['fakes_detected']}")
        logger.info(f"  Archivos reales: {self.stats['real_files']}")
        logger.info(f"  Filtrados pre-LLM: {self.stats['pre_llm_filtered']}")
        logger.info(f"  Ahorro estimado: ${self.stats['cost_saved']:.2f}")
        logger.info(f"  Errores: {self.stats['errors']}")
        logger.info("=" * 60)
    
    def run_continuous(self, interval: int = 300):
        """Ejecutar auditoría continua"""
        logger.info(f"Iniciando auditoría continua (intervalo: {interval}s)")
        logger.info(f"Modo: {self.mode.value.upper()}")
        
        try:
            while True:
                self.audit_downloads()
                logger.info(f"Esperando {interval} segundos...")
                time.sleep(interval)
        except KeyboardInterrupt:
            logger.info("Auditoría detenida por el usuario")
            self.state_manager.save_state()


def main():
    """Punto de entrada principal"""
    import argparse
    
    parser = argparse.ArgumentParser(description="Vision Auditor V2 para eMule-Aishor")
    parser.add_argument("--api-url", default="http://localhost:4711/api/v1")
    parser.add_argument("--api-key", required=True)
    parser.add_argument("--anthropic-key", required=True)
    parser.add_argument("--interval", type=int, default=300)
    parser.add_argument("--once", action="store_true")
    parser.add_argument(
        "--mode",
        choices=["observer", "cautious", "terminator"],
        default="observer",
        help="Modo de operación (observer=solo logging, cautious=pausa, terminator=elimina)"
    )
    
    args = parser.parse_args()
    
    # Crear auditor
    mode = OperationMode(args.mode)
    auditor = VisionAuditorV2(
        emule_api_url=args.api_url,
        api_key=args.api_key,
        anthropic_api_key=args.anthropic_key,
        mode=mode
    )
    
    # Ejecutar
    if args.once:
        auditor.audit_downloads()
    else:
        auditor.run_continuous(interval=args.interval)


if __name__ == "__main__":
    main()
