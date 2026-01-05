# Documentación del Wheel: llama-cpp-python (Blackwell Edition)

Este archivo `.whl` contiene una compilación personalizada de `llama-cpp-python` optimizada para GPUs de arquitectura Blackwell.

## Detalles del Archivo
- **Nombre**: `llama_cpp_python-0.3.16-cp311-cp311-win_amd64.whl`
- **Versión**: 0.3.16
- **Python**: 3.11 (cp311)
- **Plataforma**: Windows x64

## Especificaciones de Compilación
Este wheel fue compilado bajo las siguientes condiciones técnicas:
- **Arquitectura GPU (CUDA_ARCH)**: `100` (NVIDIA Blackwell / RTX 50 Series)
- **Versión de CUDA**: 13.1
- **Compilador C++**: Microsoft Visual Studio 2026 (v18)
- **Generador de Build**: Ninja
- **Flags Especiales**: `-allow-unsupported-compiler` (necesario para la compatibilidad entre CUDA 13.1 y VS 2026)

## Eficiencia y Tamaño
- **Tamaño del Archivo**: ~75 MB
- **Tipo de Build**: **Targeted (Native)**. A diferencia de los wheels genéricos ("Fat Binaries") de ~450MB que incluyen soporte para todas las GPUs antiguas (Pascal, Turing, Ampere), este archivo contiene **exclusivamente** el código compilado para la arquitectura **Blackwell (sm_100)**. Esto resulta en una carga más rápida y menor uso de disco sin sacrificar funcionalidad.

## Requisitos de Instalación
Para que este wheel funcione correctamente, el sistema de destino debe tener:
1. **Hardware**: GPU NVIDIA RTX serie 50 (Blackwell).
2. **Drivers**: NVIDIA Drivers compatibles con CUDA 13.1 o superior.
3. **Python**: Versión 3.11.x de 64 bits.

## Instrucciones de Uso
Para instalar este wheel en un nuevo entorno:
```bash
pip install wheels\llama_cpp_python-0.3.16-cp311-cp311-win_amd64.whl
```

## Notas Técnicas
Este binario incluye soporte completo para:
- **CUDA Offloading**: Todas las capas del modelo pueden cargarse en VRAM.
- **Optimización Blackwell**: Instrucciones específicas para el nuevo hardware.
- **GGUF**: Compatible con los últimos formatos de cuantización de llama.cpp.

---

## Procedimiento de Compilación (Reproducción)
Para volver a generar este wheel en otra máquina, sigue estos pasos exactos:

### 1. Preparación del Entorno
Limpia cualquier build previo para asegurar una compilación limpia.
```batch
rd /s /q build
rd /s /q dist
rd /s /q _skbuild
```

### 2. Configuración de Variables
Es crítico configurar las variables de entorno para CMake (`scikit-build-core`) antes de iniciar el build.

```batch
:: 1. Cargar entorno de Visual Studio (vcvars64)
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

:: 2. Definir ruta de compilador CUDA (NVCC)
set CUDA_PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.1
set CUDACXX=%CUDA_PATH%\bin\nvcc.exe

:: 3. Flags de Compilación (CRÍTICO para Blackwell/VS2026)
:: -G Ninja: Generador más rápido
:: -DGGML_CUDA=ON: Activa soporte GPU
:: -DCMAKE_CUDA_ARCHITECTURES=100: Target específico para Blackwell (sm_100)
:: -allow-unsupported-compiler: Bypass para error de compatibilidad VS2026/CUDA13.1
set SKBUILD_CMAKE_ARGS=-G Ninja;-DGGML_CUDA=ON;-DCMAKE_CUDA_ARCHITECTURES=100;-DCMAKE_CUDA_FLAGS="-allow-unsupported-compiler"
```

### 3. Ejecución del Build
Una vez configuradas las variables, ejecuta el build de Python dentro del directorio fuente `llama-cpp-python-src`.

```batch
cd llama-cpp-python-src
python -m pip install build scikit-build-core cmake ninja
python -m build --wheel --verbose
```

El archivo resultante (`.whl`) se generará en la carpeta `dist/`.

---
*Generado por Antigravity para el sistema CHAMAN - 2026*
