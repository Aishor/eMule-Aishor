"""
FiberSight Orchestrator Agent - Handlers de Tareas
Implementación de handlers específicos para cada tipo de tarea

Handlers implementados:
- SEARCH: Búsqueda inteligente con LLM
- VERIFY: Verificación visual de archivos
- OPTIMIZE: Optimización de descargas
- ANALYZE: Análisis de biblioteca
"""

import os
import time
import json
import logging
from typing import Dict, List, Optional
from datetime import datetime

import httpx

logger = logging.getLogger("orchestrator.handlers")

# ============================================================================
# LLM CLIENT
# ============================================================================

class LLMClient:
    """Cliente para interactuar con LLMs (Claude/GPT-4)"""
    
    def __init__(self, api_key: str, model: str = "claude-3-5-sonnet-20241022"):
        self.api_key = api_key
        self.model = model
        self.client = httpx.AsyncClient(timeout=60.0)
    
    async def complete(self, prompt: str, max_tokens: int = 2048) -> str:
        """Completar prompt con Claude"""
        try:
            headers = {
                "x-api-key": self.api_key,
                "anthropic-version": "2023-06-01",
                "content-type": "application/json"
            }
            
            payload = {
                "model": self.model,
                "max_tokens": max_tokens,
                "messages": [
                    {"role": "user", "content": prompt}
                ]
            }
            
            response = await self.client.post(
                "https://api.anthropic.com/v1/messages",
                headers=headers,
                json=payload
            )
            response.raise_for_status()
            
            data = response.json()
            return data["content"][0]["text"]
        
        except Exception as e:
            logger.error(f"Error en LLM: {e}")
            raise
    
    async def close(self):
        """Cerrar cliente HTTP"""
        await self.client.aclose()


# ============================================================================
# EMULE API CLIENT
# ============================================================================

class EMuleAPIClient:
    """Cliente para eMule API"""
    
    def __init__(self, base_url: str, api_key: str):
        self.base_url = base_url
        self.headers = {"Authorization": f"Bearer {api_key}"} if api_key else {}
        self.client = httpx.AsyncClient(timeout=30.0)
    
    async def get(self, endpoint: str) -> dict:
        """GET request"""
        url = f"{self.base_url}{endpoint}"
        response = await self.client.get(url, headers=self.headers)
        response.raise_for_status()
        return response.json()
    
    async def post(self, endpoint: str, json_data: dict = None) -> dict:
        """POST request"""
        url = f"{self.base_url}{endpoint}"
        response = await self.client.post(url, headers=self.headers, json=json_data or {})
        response.raise_for_status()
        return response.json()
    
    async def put(self, endpoint: str, json_data: dict = None) -> dict:
        """PUT request"""
        url = f"{self.base_url}{endpoint}"
        response = await self.client.put(url, headers=self.headers, json=json_data or {})
        response.raise_for_status()
        return response.json()
    
    async def close(self):
        """Cerrar cliente"""
        await self.client.aclose()


# ============================================================================
# TASK HANDLERS
# ============================================================================

class TaskHandlers:
    """Handlers para diferentes tipos de tareas"""
    
    def __init__(self, db, llm_client: LLMClient, emule_client: EMuleAPIClient):
        self.db = db
        self.llm = llm_client
        self.emule = emule_client
    
    # ========================================================================
    # SEARCH HANDLER
    # ========================================================================
    
    async def handle_search(self, task) -> Dict:
        """
        Handler para búsqueda inteligente
        
        Flujo:
        1. Parsear request del usuario con LLM
        2. Ejecutar búsqueda en eMule
        3. Filtrar resultados con LLM
        4. Retornar matches
        """
        logger.info(f"[SEARCH] Iniciando búsqueda: {task.user_request}")
        
        # Paso 1: Parsear request con LLM
        self.db.update_task_status(task.id, "running", "Parseando consulta con LLM", 0.1)
        
        parse_prompt = self.db.get_active_prompt("search_parse")
        if not parse_prompt:
            raise ValueError("Prompt 'search_parse' no encontrado")
        
        filled_prompt = parse_prompt.prompt_template.format(
            user_request=task.user_request
        )
        
        start_time = time.time()
        llm_response = await self.llm.complete(filled_prompt)
        execution_time = time.time() - start_time
        
        # Parsear JSON de respuesta
        try:
            criteria = json.loads(llm_response)
            success = True
            confidence = 0.9
        except json.JSONDecodeError:
            logger.error(f"Error parseando JSON del LLM: {llm_response}")
            success = False
            confidence = 0.0
            criteria = {"query": task.user_request, "type": "all"}
        
        # Guardar ejecución de prompt
        from orchestrator_agent import PromptExecution
        execution = PromptExecution(
            task_id=task.id,
            prompt_id=parse_prompt.id,
            prompt_filled=filled_prompt,
            llm_response=llm_response,
            success=success,
            confidence=confidence,
            execution_time=execution_time
        )
        self.db.create_prompt_execution(execution)
        self.db.update_prompt_metrics(parse_prompt.id, success, confidence)
        
        # Paso 2: Ejecutar búsqueda en eMule
        self.db.update_task_status(task.id, "running", "Buscando en eMule", 0.4)
        
        query = criteria.get("query", "")
        file_type = criteria.get("type", "all")
        
        # TODO: Implementar endpoint de búsqueda real en eMule API
        # Por ahora, simular con downloads existentes
        downloads_data = await self.emule.get("/downloads")
        all_results = downloads_data.get("downloads", [])
        
        # Filtrar por tipo si se especifica
        if file_type != "all":
            # Filtrado básico por extensión
            type_extensions = {
                "movie": [".avi", ".mkv", ".mp4", ".mov"],
                "series": [".avi", ".mkv", ".mp4"],
                "music": [".mp3", ".flac", ".wav"],
                "software": [".exe", ".zip", ".rar"]
            }
            extensions = type_extensions.get(file_type, [])
            all_results = [r for r in all_results if any(r["name"].lower().endswith(ext) for ext in extensions)]
        
        logger.info(f"[SEARCH] Encontrados {len(all_results)} resultados en eMule")
        
        # Paso 3: Filtrar con LLM
        if len(all_results) > 0:
            self.db.update_task_status(task.id, "running", "Filtrando resultados con LLM", 0.7)
            
            filter_prompt = self.db.get_active_prompt("search_filter")
            if filter_prompt:
                # Preparar resultados para el LLM (limitar a 50 para no saturar)
                results_sample = all_results[:50]
                results_text = "\n".join([
                    f"- {r['name']} ({r.get('size', 0) / 1024 / 1024:.1f} MB)"
                    for r in results_sample
                ])
                
                filled_filter_prompt = filter_prompt.prompt_template.format(
                    criteria=json.dumps(criteria, indent=2),
                    results=results_text
                )
                
                start_time = time.time()
                filter_response = await self.llm.complete(filled_filter_prompt)
                execution_time = time.time() - start_time
                
                try:
                    filter_result = json.loads(filter_response)
                    matches = filter_result.get("matches", [])
                    rejected = filter_result.get("rejected", [])
                    success = True
                    confidence = sum(m.get("confidence", 0.5) for m in matches) / max(len(matches), 1)
                except json.JSONDecodeError:
                    logger.error(f"Error parseando filtro JSON: {filter_response}")
                    matches = results_sample  # Fallback: retornar todos
                    rejected = []
                    success = False
                    confidence = 0.5
                
                # Guardar ejecución
                execution = PromptExecution(
                    task_id=task.id,
                    prompt_id=filter_prompt.id,
                    prompt_filled=filled_filter_prompt[:1000],  # Truncar
                    llm_response=filter_response,
                    success=success,
                    confidence=confidence,
                    execution_time=execution_time
                )
                self.db.create_prompt_execution(execution)
                self.db.update_prompt_metrics(filter_prompt.id, success, confidence)
            else:
                matches = all_results
                rejected = []
        else:
            matches = []
            rejected = []
        
        # Paso 4: Retornar resultado
        self.db.update_task_status(task.id, "running", "Finalizando", 1.0)
        
        result = {
            "criteria": criteria,
            "total_results": len(all_results),
            "matches": len(matches),
            "rejected": len(rejected),
            "files": matches[:20],  # Limitar a 20 para respuesta
            "llm_calls": 2 if filter_prompt else 1
        }
        
        logger.info(f"[SEARCH] Completado: {len(matches)} matches de {len(all_results)} resultados")
        
        return result
    
    # ========================================================================
    # VERIFY HANDLER
    # ========================================================================
    
    async def handle_verify(self, task) -> Dict:
        """
        Handler para verificación visual
        
        Flujo:
        1. Resolver hash del archivo
        2. Activar preview mode
        3. Esperar chunks (con timeout)
        4. Llamar a Vision Auditor (o hacer análisis aquí)
        5. Retornar veredicto
        """
        logger.info(f"[VERIFY] Iniciando verificación: {task.parameters.get('target')}")
        
        target = task.parameters.get("target", "")
        
        # Paso 1: Resolver hash
        self.db.update_task_status(task.id, "running", "Resolviendo archivo", 0.1)
        
        # Buscar en descargas
        downloads = await self.emule.get("/downloads")
        file_hash = None
        file_name = None
        
        for dl in downloads.get("downloads", []):
            if target.lower() in dl["name"].lower() or dl["hash"].startswith(target.upper()):
                file_hash = dl["hash"]
                file_name = dl["name"]
                break
        
        if not file_hash:
            raise ValueError(f"Archivo no encontrado: {target}")
        
        # Paso 2: Activar preview mode
        self.db.update_task_status(task.id, "running", "Activando preview mode", 0.2)
        
        await self.emule.post(f"/downloads/{file_hash}/preview", {"enable": True})
        
        # Paso 3: Esperar chunks (polling con timeout)
        self.db.update_task_status(task.id, "waiting", "Esperando chunks críticos", 0.3)
        
        max_wait = 300  # 5 minutos
        waited = 0
        preview_ready = False
        
        while waited < max_wait:
            info = await self.emule.get(f"/downloads/{file_hash}/file_info")
            if info.get("preview_ready"):
                preview_ready = True
                break
            
            await asyncio.sleep(10)
            waited += 10
        
        if not preview_ready:
            raise TimeoutError("Timeout esperando chunks críticos")
        
        # Paso 4: Análisis (simplificado - en producción llamar a Vision Auditor)
        self.db.update_task_status(task.id, "running", "Analizando con IA", 0.7)
        
        # Por ahora, retornar resultado simulado
        # En producción: extraer frames + llamar Claude Vision
        
        result = {
            "file_hash": file_hash,
            "file_name": file_name,
            "verdict": "REAL",  # Simulado
            "confidence": 0.85,
            "reason": "Análisis visual completado (simulado)",
            "preview_ready": True
        }
        
        logger.info(f"[VERIFY] Completado: {file_name} → {result['verdict']}")
        
        return result
    
    # ========================================================================
    # OPTIMIZE HANDLER
    # ========================================================================
    
    async def handle_optimize(self, task) -> Dict:
        """
        Handler para optimización de descargas
        
        Flujo:
        1. Obtener todas las descargas
        2. Clasificar con LLM (lentas, rápidas, estancadas)
        3. Ejecutar acciones (pausar lentas, priorizar rápidas)
        4. Retornar reporte
        """
        logger.info(f"[OPTIMIZE] Iniciando optimización")
        
        # Paso 1: Obtener descargas
        self.db.update_task_status(task.id, "running", "Obteniendo descargas", 0.2)
        
        downloads = await self.emule.get("/downloads/active")
        files = downloads.get("downloads", [])
        
        if not files:
            return {"message": "No hay descargas activas", "actions": []}
        
        # Paso 2: Clasificar con LLM
        self.db.update_task_status(task.id, "running", "Clasificando con IA", 0.5)
        
        # Preparar datos para LLM
        files_summary = "\n".join([
            f"- {f['name']}: {f.get('speed', 0) / 1024:.1f} KB/s, "
            f"{f.get('progress', 0):.1f}%, {f.get('sources', 0)} fuentes"
            for f in files
        ])
        
        optimize_prompt = f"""Analiza estas descargas de eMule y clasifícalas:

{files_summary}

Clasifica cada descarga como:
- SLOW: Velocidad <50 KB/s (pausar)
- FAST: Velocidad >200 KB/s (priorizar)
- STALLED: 0 KB/s (reintentar)
- NORMAL: Resto (mantener)

Responde JSON:
{{
  "slow": ["nombre1", "nombre2"],
  "fast": ["nombre3"],
  "stalled": ["nombre4"],
  "normal": ["nombre5"]
}}"""
        
        llm_response = await self.llm.complete(optimize_prompt)
        
        try:
            classification = json.loads(llm_response)
        except:
            classification = {"slow": [], "fast": [], "stalled": [], "normal": []}
        
        # Paso 3: Ejecutar acciones
        self.db.update_task_status(task.id, "running", "Ejecutando optimizaciones", 0.8)
        
        actions = []
        
        # Pausar lentas
        for name in classification.get("slow", []):
            for f in files:
                if name.lower() in f["name"].lower():
                    await self.emule.put(f"/downloads/{f['hash']}/pause")
                    actions.append(f"Pausada (lenta): {f['name']}")
                    break
        
        # Reanudar estancadas
        for name in classification.get("stalled", []):
            for f in files:
                if name.lower() in f["name"].lower():
                    await self.emule.put(f"/downloads/{f['hash']}/resume")
                    actions.append(f"Reanudada (estancada): {f['name']}")
                    break
        
        result = {
            "total_files": len(files),
            "classification": classification,
            "actions_taken": len(actions),
            "actions": actions
        }
        
        logger.info(f"[OPTIMIZE] Completado: {len(actions)} acciones ejecutadas")
        
        return result
    
    # ========================================================================
    # ANALYZE HANDLER
    # ========================================================================
    
    async def handle_analyze(self, task) -> Dict:
        """
        Handler para análisis de biblioteca
        
        Flujo:
        1. Obtener biblioteca completa
        2. Analizar con LLM (duplicados, baja calidad, etc)
        3. Generar recomendaciones
        4. Retornar reporte
        """
        logger.info(f"[ANALYZE] Iniciando análisis de biblioteca")
        
        # Paso 1: Obtener biblioteca
        self.db.update_task_status(task.id, "running", "Obteniendo biblioteca", 0.2)
        
        library = await self.emule.get("/library")
        files = library.get("files", [])
        
        if not files:
            return {"message": "Biblioteca vacía", "recommendations": []}
        
        # Paso 2: Analizar con LLM
        self.db.update_task_status(task.id, "running", "Analizando con IA", 0.6)
        
        # Preparar muestra (limitar a 100 archivos)
        files_sample = files[:100]
        files_summary = "\n".join([
            f"- {f['name']} ({f.get('size', 0) / 1024 / 1024:.1f} MB)"
            for f in files_sample
        ])
        
        analyze_prompt = f"""Analiza esta biblioteca de archivos compartidos:

{files_summary}

Identifica:
1. Duplicados (mismo nombre, diferentes versiones)
2. Archivos de baja calidad (CAM, TS, etc)
3. Archivos muy antiguos o irrelevantes

Responde JSON:
{{
  "duplicates": [["archivo1.mkv", "archivo1.avi"]],
  "low_quality": ["archivo_CAM.avi"],
  "recommendations": ["Eliminar archivo_CAM.avi por baja calidad"]
}}"""
        
        llm_response = await self.llm.complete(analyze_prompt)
        
        try:
            analysis = json.loads(llm_response)
        except:
            analysis = {"duplicates": [], "low_quality": [], "recommendations": []}
        
        result = {
            "total_files": len(files),
            "analyzed": len(files_sample),
            "duplicates_found": len(analysis.get("duplicates", [])),
            "low_quality_found": len(analysis.get("low_quality", [])),
            "recommendations": analysis.get("recommendations", [])
        }
        
        logger.info(f"[ANALYZE] Completado: {len(analysis.get('recommendations', []))} recomendaciones")
        
        return result


# Importar asyncio para usar en handlers
import asyncio
