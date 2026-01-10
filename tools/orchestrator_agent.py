"""
FiberSight Orchestrator Agent - Agente Orquestador
Gestión de tareas asíncronas con sistema de prompts persistente

Este agente coordina entre:
- Usuario (vía MCP)
- eMule API
- LLM (Claude/GPT-4)

Características:
- Cola de tareas con prioridades
- Prompts versionados en SQLite
- Refinamiento automático de prompts
- Aprendizaje continuo

Versión: R1.3 "FiberSight"
"""

import asyncio
import sqlite3
import json
import logging
from datetime import datetime
from typing import Optional, Dict, List, Callable, Any
from dataclasses import dataclass, asdict
from enum import Enum
from pathlib import Path

import httpx

# Configuración de logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger("orchestrator")

# ============================================================================
# ENUMS
# ============================================================================

class TaskStatus(Enum):
    """Estados de una tarea"""
    PENDING = "pending"
    RUNNING = "running"
    WAITING = "waiting"      # Esperando recurso externo
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"


class TaskType(Enum):
    """Tipos de tareas"""
    SEARCH = "search"           # Búsqueda inteligente
    VERIFY = "verify"           # Verificación visual
    OPTIMIZE = "optimize"       # Optimización de descargas
    ANALYZE = "analyze"         # Análisis y recomendaciones
    CUSTOM = "custom"           # Tarea personalizada


# ============================================================================
# DATACLASSES
# ============================================================================

@dataclass
class Task:
    """Representa una tarea en el sistema"""
    id: Optional[int] = None
    task_type: str = ""
    status: str = TaskStatus.PENDING.value
    priority: int = 5
    
    # Timestamps
    created_at: Optional[datetime] = None
    started_at: Optional[datetime] = None
    completed_at: Optional[datetime] = None
    
    # Input
    user_request: str = ""
    parameters: Dict = None
    
    # Output
    result: Optional[Dict] = None
    error: Optional[str] = None
    
    # Metadata
    llm_calls: int = 0
    emule_calls: int = 0
    duration_seconds: float = 0.0
    
    # Estado de ejecución
    current_step: str = ""
    progress: float = 0.0
    
    def __post_init__(self):
        if self.parameters is None:
            self.parameters = {}
        if self.created_at is None:
            self.created_at = datetime.now()


@dataclass
class Prompt:
    """Representa un prompt template"""
    id: Optional[int] = None
    task_type: str = ""
    prompt_version: int = 1
    prompt_template: str = ""
    
    # Métricas
    success_rate: float = 0.0
    avg_confidence: float = 0.0
    usage_count: int = 0
    
    # Timestamps
    created_at: Optional[datetime] = None
    last_used: Optional[datetime] = None
    is_active: bool = True


@dataclass
class PromptExecution:
    """Registro de ejecución de un prompt"""
    id: Optional[int] = None
    task_id: int = 0
    prompt_id: int = 0
    
    # Input/Output
    prompt_filled: str = ""
    llm_response: str = ""
    
    # Evaluación
    success: bool = False
    confidence: float = 0.0
    execution_time: float = 0.0
    
    # Feedback
    user_feedback: Optional[str] = None
    auto_feedback: Optional[str] = None
    
    created_at: Optional[datetime] = None
    
    def __post_init__(self):
        if self.created_at is None:
            self.created_at = datetime.now()


# ============================================================================
# DATABASE MANAGER
# ============================================================================

class DatabaseManager:
    """Gestor de base de datos SQLite"""
    
    def __init__(self, db_path: str = "orchestrator.db"):
        self.db_path = Path(db_path)
        self.init_database()
    
    def get_connection(self) -> sqlite3.Connection:
        """Obtener conexión a la base de datos"""
        conn = sqlite3.connect(self.db_path)
        conn.row_factory = sqlite3.Row
        return conn
    
    def init_database(self):
        """Inicializar esquema de base de datos"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        # Tabla: tasks
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS tasks (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                task_type TEXT NOT NULL,
                status TEXT NOT NULL,
                priority INTEGER DEFAULT 5,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                started_at TIMESTAMP,
                completed_at TIMESTAMP,
                
                user_request TEXT NOT NULL,
                parameters TEXT,
                
                result TEXT,
                error TEXT,
                
                llm_calls INTEGER DEFAULT 0,
                emule_calls INTEGER DEFAULT 0,
                duration_seconds REAL DEFAULT 0.0,
                
                current_step TEXT DEFAULT '',
                progress REAL DEFAULT 0.0
            )
        ''')
        
        # Tabla: prompts
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS prompts (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                task_type TEXT NOT NULL,
                prompt_version INTEGER NOT NULL,
                prompt_template TEXT NOT NULL,
                
                success_rate REAL DEFAULT 0.0,
                avg_confidence REAL DEFAULT 0.0,
                usage_count INTEGER DEFAULT 0,
                
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                last_used TIMESTAMP,
                is_active BOOLEAN DEFAULT 1,
                
                UNIQUE(task_type, prompt_version)
            )
        ''')
        
        # Tabla: prompt_executions
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS prompt_executions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                task_id INTEGER NOT NULL,
                prompt_id INTEGER NOT NULL,
                
                prompt_filled TEXT NOT NULL,
                llm_response TEXT NOT NULL,
                
                success BOOLEAN DEFAULT 0,
                confidence REAL DEFAULT 0.0,
                execution_time REAL DEFAULT 0.0,
                
                user_feedback TEXT,
                auto_feedback TEXT,
                
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                
                FOREIGN KEY (task_id) REFERENCES tasks(id),
                FOREIGN KEY (prompt_id) REFERENCES prompts(id)
            )
        ''')
        
        # Tabla: prompt_refinements
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS prompt_refinements (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                original_prompt_id INTEGER NOT NULL,
                refined_prompt_id INTEGER NOT NULL,
                
                reason TEXT NOT NULL,
                improvement_metric TEXT,
                improvement_value REAL,
                
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                
                FOREIGN KEY (original_prompt_id) REFERENCES prompts(id),
                FOREIGN KEY (refined_prompt_id) REFERENCES prompts(id)
            )
        ''')
        
        conn.commit()
        conn.close()
        
        logger.info("Base de datos inicializada")
    
    # ========================================================================
    # TASKS
    # ========================================================================
    
    def create_task(self, task: Task) -> int:
        """Crear nueva tarea"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            INSERT INTO tasks (
                task_type, status, priority, user_request, parameters,
                current_step, progress
            ) VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (
            task.task_type,
            task.status,
            task.priority,
            task.user_request,
            json.dumps(task.parameters),
            task.current_step,
            task.progress
        ))
        
        task_id = cursor.lastrowid
        conn.commit()
        conn.close()
        
        return task_id
    
    def get_task(self, task_id: int) -> Optional[Task]:
        """Obtener tarea por ID"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        cursor.execute('SELECT * FROM tasks WHERE id = ?', (task_id,))
        row = cursor.fetchone()
        conn.close()
        
        if not row:
            return None
        
        return Task(
            id=row['id'],
            task_type=row['task_type'],
            status=row['status'],
            priority=row['priority'],
            created_at=datetime.fromisoformat(row['created_at']) if row['created_at'] else None,
            started_at=datetime.fromisoformat(row['started_at']) if row['started_at'] else None,
            completed_at=datetime.fromisoformat(row['completed_at']) if row['completed_at'] else None,
            user_request=row['user_request'],
            parameters=json.loads(row['parameters']) if row['parameters'] else {},
            result=json.loads(row['result']) if row['result'] else None,
            error=row['error'],
            llm_calls=row['llm_calls'],
            emule_calls=row['emule_calls'],
            duration_seconds=row['duration_seconds'],
            current_step=row['current_step'],
            progress=row['progress']
        )
    
    def update_task_status(self, task_id: int, status: str, current_step: str = "", progress: float = 0.0):
        """Actualizar estado de tarea"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        updates = ['status = ?', 'current_step = ?', 'progress = ?']
        values = [status, current_step, progress]
        
        if status == TaskStatus.RUNNING.value:
            updates.append('started_at = CURRENT_TIMESTAMP')
        elif status in [TaskStatus.COMPLETED.value, TaskStatus.FAILED.value]:
            updates.append('completed_at = CURRENT_TIMESTAMP')
        
        cursor.execute(f'''
            UPDATE tasks 
            SET {', '.join(updates)}
            WHERE id = ?
        ''', values + [task_id])
        
        conn.commit()
        conn.close()
    
    def update_task_result(self, task_id: int, result: Dict):
        """Actualizar resultado de tarea"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            UPDATE tasks 
            SET result = ?, status = ?, completed_at = CURRENT_TIMESTAMP
            WHERE id = ?
        ''', (json.dumps(result), TaskStatus.COMPLETED.value, task_id))
        
        conn.commit()
        conn.close()
    
    def update_task_error(self, task_id: int, error: str):
        """Actualizar error de tarea"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            UPDATE tasks 
            SET error = ?, status = ?, completed_at = CURRENT_TIMESTAMP
            WHERE id = ?
        ''', (error, TaskStatus.FAILED.value, task_id))
        
        conn.commit()
        conn.close()
    
    def get_pending_tasks(self, limit: int = 10) -> List[Task]:
        """Obtener tareas pendientes ordenadas por prioridad"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT * FROM tasks 
            WHERE status = ?
            ORDER BY priority DESC, created_at ASC
            LIMIT ?
        ''', (TaskStatus.PENDING.value, limit))
        
        rows = cursor.fetchall()
        conn.close()
        
        tasks = []
        for row in rows:
            tasks.append(Task(
                id=row['id'],
                task_type=row['task_type'],
                status=row['status'],
                priority=row['priority'],
                user_request=row['user_request'],
                parameters=json.loads(row['parameters']) if row['parameters'] else {},
                current_step=row['current_step'],
                progress=row['progress']
            ))
        
        return tasks
    
    # ========================================================================
    # PROMPTS
    # ========================================================================
    
    def create_prompt(self, prompt: Prompt) -> int:
        """Crear nuevo prompt"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            INSERT INTO prompts (
                task_type, prompt_version, prompt_template,
                success_rate, avg_confidence, usage_count, is_active
            ) VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (
            prompt.task_type,
            prompt.prompt_version,
            prompt.prompt_template,
            prompt.success_rate,
            prompt.avg_confidence,
            prompt.usage_count,
            prompt.is_active
        ))
        
        prompt_id = cursor.lastrowid
        conn.commit()
        conn.close()
        
        return prompt_id
    
    def get_active_prompt(self, task_type: str) -> Optional[Prompt]:
        """Obtener prompt activo para un tipo de tarea"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT * FROM prompts 
            WHERE task_type = ? AND is_active = 1
            ORDER BY prompt_version DESC
            LIMIT 1
        ''', (task_type,))
        
        row = cursor.fetchone()
        conn.close()
        
        if not row:
            return None
        
        return Prompt(
            id=row['id'],
            task_type=row['task_type'],
            prompt_version=row['prompt_version'],
            prompt_template=row['prompt_template'],
            success_rate=row['success_rate'],
            avg_confidence=row['avg_confidence'],
            usage_count=row['usage_count'],
            is_active=bool(row['is_active'])
        )
    
    def update_prompt_metrics(self, prompt_id: int, success: bool, confidence: float):
        """Actualizar métricas de un prompt"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        # Obtener métricas actuales
        cursor.execute('SELECT success_rate, avg_confidence, usage_count FROM prompts WHERE id = ?', (prompt_id,))
        row = cursor.fetchone()
        
        if row:
            current_success_rate = row['success_rate']
            current_avg_confidence = row['avg_confidence']
            current_usage_count = row['usage_count']
            
            # Calcular nuevas métricas (media móvil)
            new_usage_count = current_usage_count + 1
            new_success_rate = (current_success_rate * current_usage_count + (1 if success else 0)) / new_usage_count
            new_avg_confidence = (current_avg_confidence * current_usage_count + confidence) / new_usage_count
            
            cursor.execute('''
                UPDATE prompts 
                SET success_rate = ?, avg_confidence = ?, usage_count = ?, last_used = CURRENT_TIMESTAMP
                WHERE id = ?
            ''', (new_success_rate, new_avg_confidence, new_usage_count, prompt_id))
            
            conn.commit()
        
        conn.close()
    
    # ========================================================================
    # PROMPT EXECUTIONS
    # ========================================================================
    
    def create_prompt_execution(self, execution: PromptExecution) -> int:
        """Registrar ejecución de prompt"""
        conn = self.get_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            INSERT INTO prompt_executions (
                task_id, prompt_id, prompt_filled, llm_response,
                success, confidence, execution_time
            ) VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (
            execution.task_id,
            execution.prompt_id,
            execution.prompt_filled,
            execution.llm_response,
            execution.success,
            execution.confidence,
            execution.execution_time
        ))
        
        execution_id = cursor.lastrowid
        conn.commit()
        conn.close()
        
        return execution_id
    
    def seed_initial_prompts(self):
        """Poblar base de datos con prompts iniciales"""
        prompts = [
            Prompt(
                task_type="search_parse",
                prompt_version=1,
                prompt_template="""Extrae criterios de búsqueda estructurados de esta petición del usuario:

"{user_request}"

Responde SOLO con JSON válido:
{{
  "query": "términos de búsqueda",
  "type": "movie|series|music|software|all",
  "min_quality": "720p|1080p|2160p|null",
  "genre": "género si se especifica|null",
  "year_min": año mínimo|null,
  "year_max": año máximo|null,
  "max_results": número máximo de resultados|20
}}""",
                is_active=True
            ),
            Prompt(
                task_type="search_filter",
                prompt_version=1,
                prompt_template="""Filtra estos resultados de búsqueda según los criterios del usuario.

Criterios: {criteria}

Resultados:
{results}

Para cada resultado, determina si cumple los criterios.
Responde con JSON:
{{
  "matches": [
    {{"name": "...", "reason": "por qué cumple", "confidence": 0.0-1.0}}
  ],
  "rejected": [
    {{"name": "...", "reason": "por qué no cumple"}}
  ]
}}""",
                is_active=True
            )
        ]
        
        for prompt in prompts:
            try:
                self.create_prompt(prompt)
                logger.info(f"Prompt seeded: {prompt.task_type} v{prompt.prompt_version}")
            except sqlite3.IntegrityError:
                logger.debug(f"Prompt ya existe: {prompt.task_type} v{prompt.prompt_version}")


# ============================================================================
# TASK QUEUE MANAGER
# ============================================================================

class TaskQueueManager:
    """Gestor de cola de tareas"""
    
    def __init__(self, db: DatabaseManager, max_concurrent: int = 3):
        self.db = db
        self.max_concurrent = max_concurrent
        self.running_tasks: Dict[int, asyncio.Task] = {}
        self.is_running = False
    
    async def add_task(self, task: Task) -> int:
        """Añadir tarea a la cola"""
        task_id = self.db.create_task(task)
        logger.info(f"Tarea añadida: ID={task_id}, tipo={task.task_type}, prioridad={task.priority}")
        return task_id
    
    async def start(self):
        """Iniciar procesamiento de cola"""
        self.is_running = True
        logger.info("Task Queue Manager iniciado")
        
        while self.is_running:
            try:
                # Limpiar tareas completadas
                completed = [tid for tid, t in self.running_tasks.items() if t.done()]
                for tid in completed:
                    del self.running_tasks[tid]
                
                # Procesar nuevas tareas si hay slots disponibles
                if len(self.running_tasks) < self.max_concurrent:
                    pending = self.db.get_pending_tasks(limit=self.max_concurrent - len(self.running_tasks))
                    
                    for task in pending:
                        if task.id not in self.running_tasks:
                            # Crear tarea asíncrona
                            async_task = asyncio.create_task(self.execute_task(task))
                            self.running_tasks[task.id] = async_task
                            logger.info(f"Ejecutando tarea {task.id}")
                
                await asyncio.sleep(1)
            
            except Exception as e:
                logger.error(f"Error en Task Queue Manager: {e}")
                await asyncio.sleep(5)
    
    async def execute_task(self, task: Task):
        """Ejecutar una tarea (placeholder)"""
        self.db.update_task_status(task.id, TaskStatus.RUNNING.value, "Iniciando...", 0.0)
        
        try:
            # TODO: Implementar handlers específicos por tipo
            await asyncio.sleep(5)  # Simular trabajo
            
            result = {"status": "completed", "message": "Task executed successfully"}
            self.db.update_task_result(task.id, result)
            
        except Exception as e:
            logger.error(f"Error ejecutando tarea {task.id}: {e}")
            self.db.update_task_error(task.id, str(e))
    
    async def stop(self):
        """Detener procesamiento de cola"""
        self.is_running = False
        logger.info("Task Queue Manager detenido")


# ============================================================================
# MAIN
# ============================================================================

async def main():
    """Punto de entrada principal"""
    # Inicializar base de datos
    db = DatabaseManager("orchestrator.db")
    db.seed_initial_prompts()
    
    # Inicializar queue manager
    queue = TaskQueueManager(db, max_concurrent=3)
    
    # Iniciar procesamiento
    await queue.start()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Orquestador detenido por el usuario")
