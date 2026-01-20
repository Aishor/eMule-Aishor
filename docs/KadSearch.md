# Documentación de Búsqueda Kademlia Personalizada (Kad Search)

Este documento describe la funcionalidad de búsqueda personalizada en la red Kademlia implementada en eMule. Esta característica permite a los usuarios avanzados especificar la duración y la profundidad de las búsquedas, superando los límites predeterminados estándar.

## Interfaz de Usuario (UI)

La pestaña de **Búsqueda** incluye un nuevo panel de control específico para la red Kad.

### Panel "Kad Search"

Ubicado a la derecha de los botones de control estándar, este panel se habilita automáticamente cuando se selecciona "Kad Network" como método de búsqueda.

#### Campos de Configuración

1.  **Botón "Start Kad"**
    *   **Función**: Inicia una búsqueda explícita en la red Kademlia utilizando los parámetros definidos en los campos de texto adjuntos.
    *   **Comportamiento**: A diferencia del botón "Start" estándar (que usa valores por defecto), este botón lee los valores de "Time" y "Max" para configurar la búsqueda.

2.  **Campo "Time" (Tiempo)**
    *   **Descripción**: Define la duración total de vida de la búsqueda en la red.
    *   **Unidad**: Segundos.
    *   **Límite Máximo**: `600` segundos (10 minutos).
    *   **Valor por Defecto**: Si se deja vacío o en 0, se utiliza el valor estándar de eMule (normalmente 45 segundos).
    *   **Uso**: Aumentar este valor permite que la búsqueda permanezca activa más tiempo, propagándose más lejos en la red y encontrando fuentes más raras.

3.  **Campo "Max" (Límite de Resultados)**
    *   **Descripción**: Define el número máximo de resultados que se aceptarán y procesarán para esta búsqueda.
    *   **Límite Máximo**: `3000` resultados.
    *   **Valor por Defecto**: Si se deja vacío o en 0, se utiliza el valor estándar de eMule (normalmente 300 resultados).
    *   **Uso**: Aumentar este valor es útil para términos de búsqueda muy populares donde se desea obtener una lista masiva de fuentes.

## Límites Técnicos y Validación

El sistema implementa validaciones tanto en la interfaz como en el motor de búsqueda para garantizar la estabilidad:

*   **Validación de Entrada**:
    *   Si se introduce un valor de Tiempo > 600, se mostrará una advertencia y la búsqueda no se iniciará.
    *   Si se introduce un valor Max > 3000, se mostrará una advertencia.
*   **Prioridad**: Los valores personalizados tienen prioridad sobre las constantes `SEARCHKEYWORD_LIFETIME` y `SEARCHKEYWORD_TOTAL` definidas internamente, siempre que sean válidos (`> 0`).

## Ejemplo de Uso

Para buscar un archivo raro que tiene pocas fuentes:
1.  Seleccione **Método**: `Kad Network`.
2.  Escriba el nombre del archivo.
3.  En el panel Kad Search:
    *   **Time**: `300` (5 minutos para dar tiempo a la propagación).
    *   **Max**: `0` (o dejar vacío para usar el estándar, ya que no se esperan muchos resultados).
4.  Pulse **Start Kad**.

Para buscar un archivo muy popular y obtener el máximo de fuentes posibles:
1.  Seleccione **Método**: `Kad Network`.
2.  Escriba el nombre.
3.  En el panel Kad Search:
    *   **Time**: `60` (1 minuto es suficiente para algo popular).
    *   **Max**: `1000` (para capturar hasta 1000 resultados).
4.  Pulse **Start Kad**.
