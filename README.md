# eMule-Aishor-Forge

## Descripción
Carpeta de trabajo limpia con todas las dependencias de eMule-Aishor, separada del proyecto principal para compilación y modificaciones experimentales.

## Dependencias Incluidas

### 1. **zlib** - Compresión (v1.3.1.2)
- **Ubicación**: `zlib/`
- **Propósito**: Compresión/descompresión de datos
- **Estado**: ✅ Copiado desde proyecto original
- **Proyecto**: [zlib](https://github.com/madler/zlib)

### 2. **mbedtls** - Criptografía TLS (v3.6.2)
- **Ubicación**: `mbedtls/`
- **Propósito**: Librería de criptografía y TLS/SSL
- **Estado**: ⚠️ Requiere fix (archivos de test con errores de threading)
- **Proyecto**: [Mbed-TLS](https://github.com/Mbed-TLS/mbedtls)

### 3. **libpng** - Imágenes PNG (v1.8.0.git)
- **Ubicación**: `libpng/`
- **Propósito**: Lectura/escritura de archivos PNG
- **Estado**: ✅ Copiado desde proyecto original
- **Proyecto**: [libpng](https://github.com/pnggroup/libpng)

### 4. **miniupnpc** - UPnP Client (v1.9)
- **Ubicación**: `miniupnpc/`
- **Propósito**: Universal Plug and Play para NAT traversal
- **Estado**: ✅ Copiado desde proyecto original
- **Proyecto**: [miniupnp](https://github.com/miniupnp/miniupnp)

### 5. **cryptopp** - Crypto++ (v8.9.0)
- **Ubicación**: `cryptopp/`
- **Propósito**: Librería de criptografía C++
- **Estado**: ✅ Copiado desde proyecto original
- **Proyecto**: [cryptopp](https://github.com/weidai11/cryptopp)

### 6. **id3lib** - Tags ID3 (v3.8.3)
- **Ubicación**: `id3lib/`
- **Propósito**: Manejo de etiquetas ID3 en archivos MP3
- **Estado**: ⚠️ Requiere fix (código C++ obsoleto, iostream.h)
- **Proyecto**: id3lib (SourceForge)

### 7. **CxImage** - Procesamiento de Imágenes (v6.0.0)
- **Ubicación**: `cximage/`
- **Propósito**: Librería de procesamiento de imágenes multi-formato
- **Estado**: ⚠️ Requiere fix (incompatible con libpng 1.6+)
- **Proyecto**: CxImage

### 8. **ResizableLib** - Controles MFC Redimensionables
- **Ubicación**: `resizablelib/`
- **Propósito**: Librería para diálogos y controles MFC redimensionables
- **Estado**: ✅ Copiado desde proyecto original
- **Proyecto**: [ResizableLib](https://github.com/ppescher/resizablelib)

## Próximos Pasos

### 1. Fix mbedtls
- Excluir archivos de test del proyecto vcxproj
- O configurar threading apropiado de Windows

### 2. Fix id3lib
- Actualizar headers: `iostream.h` → `<iostream>`
- Agregar namespace `std`
- Definir macros faltantes (`ID3D_NOTICE`)

### 3. Fix CxImage
- Aplicar parches para libpng 1.6+
- O usar versión compatible de libpng (1.5.x)

## Estructura de Carpetas
```
c:\Fragua\eMule-Aishor-Forge\
├── zlib\           # Compresión
├── mbedtls\        # Criptografía TLS
├── libpng\         # Imágenes PNG
├── miniupnpc\      # UPnP Client
├── cryptopp\       # Crypto++
├── id3lib\         # Tags ID3
├── cximage\        # Procesamiento de imágenes
└── resizablelib\   # Controles MFC redimensionables
```

## Notas
- Todas las dependencias fueron copiadas desde el proyecto original en `c:\Fragua\eMule-Aishor`
- Esta carpeta Forge es para trabajo experimental y compilaciones limpias
- Las modificaciones deben probarse aquí antes de aplicarlas al proyecto principal
