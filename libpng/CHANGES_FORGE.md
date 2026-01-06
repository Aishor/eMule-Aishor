# Modificaciones para Entorno Forge - libpng

## Versión Original
libpng 1.6.44

## Cambios Aplicados para Compilación Estática en VS2022

### 1. Conversión a Librería Estática
**Archivo:** `projects/vstudio/libpng/libpng.vcxproj`

**Cambio:** Líneas 67-76
```xml
<!-- Antes: DynamicLibrary -->
<ConfigurationType>StaticLibrary</ConfigurationType>
```

**Razón:** Maximizar portabilidad y evitar conflictos de DLLs. Todas las dependencias en Forge se compilan estáticamente.

### 2. Cambio de Dependencia de zlib
**Archivo:** `projects/vstudio/libpng/libpng.vcxproj`

**Cambio:** Líneas 183, 213, 297, 327
```xml
<!-- Antes: zlib.lib -->
<AdditionalDependencies>zlibstat.lib</AdditionalDependencies>
```

**Razón:** En Forge, zlib se compila como `zlibstat.lib` (librería estática). Usar `zlib.lib` causaba error de enlace LNK1181.

### 3. Adición de Ruta de Búsqueda para zlibstat.lib
**Archivo:** `projects/vstudio/libpng/libpng.vcxproj`

**Cambio:** Líneas 297-300
```xml
<AdditionalLibraryDirectories>
  ..\..\..\..\zlib\contrib\vstudio\vc17\x86\ZlibStatRelease
</AdditionalLibraryDirectories>
```

**Razón:** Asegurar que el linker localice `zlibstat.lib` en su ubicación real dentro de la estructura de Forge.

### 4. Definición de Macros de Compilación Estática
**Archivo:** `projects/vstudio/libpng/libpng.vcxproj`

**Cambio:** Líneas 279, 309
```xml
<PreprocessorDefinitions>
  PNG_STATIC;ZLIB_STATIC;%(PreprocessorDefinitions)
</PreprocessorDefinitions>
```

**Razón:** Forzar la compilación estática y evitar que libpng espere símbolos de importación de DLL (`__declspec(dllimport)`). Sin estas macros, se producían errores LNK2001 y LNK1120.

### 5. Actualización de PlatformToolset
**Archivo:** `projects/vstudio/libpng/libpng.vcxproj`

**Cambio:** Líneas 48-87
```xml
<PlatformToolset>v145</PlatformToolset>
```

**Razón:** Compatibilidad con Visual Studio 2022 instalado en el sistema.

### 6. Corrección de Ruta de zlib.props
**Archivo:** `projects/vstudio/libpng/libpng.vcxproj`

**Cambio:** Línea 42
```xml
<Import Project="..\zlib.props" />
```

**Razón:** Corregir ruta relativa para permitir compilación aislada del proyecto fuera de la solución principal.

### 7. Creación de zlib.props
**Archivo:** `projects/vstudio/zlib.props` (NUEVO)

**Contenido:**
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Label="UserMacros">
    <ZLibSrcDir>..\..\..\..\zlib</ZLibSrcDir>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>$(ZLibSrcDir);%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
  </ItemDefinitionGroup>
</Project>
```

**Razón:** Proveer la ubicación de las cabeceras de zlib a libpng.

## Resultado
Compilación exitosa como librería estática (`libpng16.lib`) sin errores.

## Configuración del Proyecto
- **PlatformToolset:** v145 (Visual Studio 2022)
- **WindowsSDK:** 10.0.26100.0
- **ConfigurationType:** StaticLibrary
- **Macros del Preprocesador:** `PNG_STATIC;ZLIB_STATIC`

---
*Documentado: 2026-01-05*
*Entorno: eMule-Aishor-Forge*
