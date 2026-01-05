# Modificaciones para Entorno Forge - mbedTLS

## Versión Original
mbedTLS 3.6.2

## Cambios Aplicados para Compilación en VS2022

### 1. Exclusión de Archivos de Test
**Archivo:** `visualc/VS2017/mbedTLS.vcxproj`

**Cambio:** Líneas 248-276, 451-473
```xml
<!-- Archivos excluidos de compilación Release|Win32 -->
<ClCompile Include="..\..\tests\src\*.c">
  <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">true</ExcludedFromBuild>
</ClCompile>
```

**Archivos afectados:**
- `tests/src/asn1_write.c`
- `tests/src/certs.c`
- `tests/src/drivers/*.c`
- `tests/src/helpers.c`
- `tests/src/random.c`
- `tests/src/threading_helpers.c`

**Razón:** Los archivos de tests usan tipos de threading (`mbedtls_threading_mutex_t`) definidos en `threading_alt.h`, que requiere configuración específica de plataforma y no es necesaria para la librería de producción. Intentar compilarlos causaba errores C2079, C2065 (identificadores no declarados).

## Resultado
Compilación exitosa como librería estática (`mbedTLS.lib`) sin errores.

## Configuración del Proyecto
- **PlatformToolset:** v145 (Visual Studio 2022)
- **WindowsSDK:** 10.0.26100.0
- **ConfigurationType:** StaticLibrary

## Notas
- No se requirieron cambios en el código fuente de la librería principal.
- Los tests no son necesarios para el uso de mbedTLS en eMule.
- La librería resultante es completamente funcional para las necesidades de eMule (criptografía, TLS).

---
*Documentado: 2026-01-05*
*Entorno: eMule-Aishor-Forge*
