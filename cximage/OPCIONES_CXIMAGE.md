# CxImage - Análisis de Opciones de Actualización

## Estado Actual
- **Versión en Forge**: CxImage 6.0.0 (2008)
- **Problema**: Código incompatible con libpng 1.6+ (estructuras opacas)
- **Requisito Original**: eMule/CxImage fue diseñado para **libpng 1.2 o 1.4**

## Investigación de Forks
❌ Ningún fork actualizado encontrado.
❌ CxImage 7.0.2 tampoco soporta libpng 1.6.

## Opciones Disponibles

### Opción 1: Downgrade a libpng 1.4 (Legacy)
**Descripción**: Reemplazar la versión 1.6 de libpng por la 1.4 (año 2012).
**Pros:**
- ✅ **Es lo que "pide" eMule/CxImage originalmente.**
- Restaura la compatibilidad nativa del código.
- No requiere modificar ni excluir CxImage.
- Funcionalidad PNG completa.

**Contras:**
- Usar una librería antigua (posibles vulns de seguridad, aunque en entorno local/controlado es menor riesgo).
- Requiere configurar proyecto para la nueva (vieja) librería.

**Esfuerzo**: Medio (Configurar libpng 1.4).

### Opción 2: Excluir CxImage Temporalmente
**Descripción**: Desactivar soporte de imágenes en la compilación.
**Pros:**
- Desbloqueo inmediato.
- Permite verificar el resto del código ya.

**Contras:**
- Sin soporte PNG en la aplicación.

### Opción 3: Parchear Manualmente
**Descripción**: Reescribir CxImage para libpng 1.6.
**Pros:** Usa librería moderna.
**Contras**: Esfuerzo titánico y propenso a errores.

## Recomendación Actualizada

**Opción 1: Downgrade a libpng 1.4**

Dado que tu prioridad es compilar "lo que eMule pide", esta es la solución más fiel.
1. Descargamos `libpng 1.4.12`.
2. Compilamos estáticamente.
3. CxImage funcionará tal cual.

¿Procedemos a descargar libpng 1.4?
