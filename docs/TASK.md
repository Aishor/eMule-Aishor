# Task List eMule-Aishor (Forge)

## Fase 1: Restauración de Compilación (✅ COMPLETADO)
- [x] **Diagnóstico Inicial**: Identificar toolset obsoleto y errores de linker.
- [x] **Actualización de Toolchain**: Migrar a VS2022 (v145).
- [x] **Dependencias**:
    - [x] zlib (Fix /MD)
    - [x] libpng (Fix /MD)
    - [x] mbedTLS (Fix stubs y config)
    - [x] id3lib (Fix static link)
    - [x] ResizableLib (Fix Unicode)
- [x] **Proyecto Core (emule)**:
    - [x] Fix PCH conflicts (OtherFunctions.cpp)
    - [x] Resolve Linker errors (LNK2001, LNK2038)
    - [x] Generar `.exe`

## Fase 2: Validación y Estabilización (🚧 EN PROGRESO)
- [ ] **Smoke Test UI**: Verificar que la GUI carga correctamente.
- [ ] **Smoke Test Red**: Verificar conexión básica TCP/UDP.
- [ ] **Auditoría de Stubs**: Revisar impacto de funciones crypto "stubeadas".

## Fase 3: Modernización (📅 PLANIFICADO)
- [ ] Limpieza de warnings de compilación.
- [ ] Refactorización de `OtherFunctions.cpp`.
- [ ] Actualización de librerías externas a versiones upstream `stable`.
