# Estado del Proyecto eMule-Aishor Forge

**Fecha**: 05-Ene-2026
**Estado Global**: 🟢 **Compilable** (Ejecutable generado)
**Plataforma**: Windows (VS2022 / x86)

## 1. Módulos Principales

| Módulo | Estado | Notas |
| :--- | :---: | :--- |
| **emule.exe** | ✅ Compila | Generado correctamente. Requiere smoke test funcional. |
| **emuleDlg** | ❓ Pendiente | Interfaz principal. Compilación pasó, falta verificar runtime UX. |
| **Kademlia** | ⚠️ Revisar | Compilado, pero depende de `mbedtls` que tiene stubs en crypto. Verificar funcionalidad nodo. |
| **WebServer** | ❓ Pendiente | Depende de `WebSocket.cpp`, PCH desactivado en algunos archivos. |

## 2. Dependencias

| Librería | Versión | Estado | Acción Requerida |
| :--- | :---: | :---: | :--- |
| **zlib** | 1.3.1 (aprox) | ✅ OK | Configurada /MD. |
| **libpng** | 1.6.x | ✅ OK | Configurada /MD. |
| **mbedTLS** | 3.x | ⚠️ Parcial | PSA Crypto deshabilitado/stubbed. SSL/TLS básico debería funcionar. |
| **id3lib** | Legacy | ✅ OK | Link estático forzado. |
| **ResizableLib** | Legacy | ✅ OK | Adaptado a Unicode/VS2022. |
| **Cryptopp** | Legacy | ✅ OK | Actualizado a v145. |
| **MiniUPnPc** | - | - | Integrado, estado a verificar. |

## 3. Deuda Técnica Conocida
1.  **Stubs en Criptografía**: Se han "puenteado" funciones de `psa_crypto` de mbedTLS. Esto podría afectar funcionalidades avanzadas de seguridad o Kademlia si usan esas rutas específicas.
    *   *Riesgo*: Medio/Alto para Kademlia.
2.  **OtherFunctions.cpp Hack**: La compilación de este archivo depende de una configuración PCH específica (`NotUsing`) y un orden de includes manual. Es frágil ante cambios futuros de refactorización.
3.  **Warnings**: El log de compilación muestra warnings, especialmente `C5204` (clases virtuales) y advertencias de conversión.
4.  **Macros HARDCODED**: `ID3LIB_LINKOPTION=1` está definido en proyecto. Debería ser estándar.

## 4. Próximos Pasos (Roadmap Técnico)
1.  **Smoke Test**: Ejecutar `emule.exe` y verificar:
    *   Arranque sin crash.
    *   Carga de lista de servidores.
    *   Conexión a un servidor eD2k.
    *   Bootstrap de Kademlia.
    *   Hashing de archivos compartidos.
2.  **Limpieza de Código**:
    *   Investigar si se puede reactivar PCH en `OtherFunctions.cpp` arreglando `Opcodes.h`/`Resource.h` (circular dependency).
    *   Refactorizar `OtherFunctions.cpp` para separar utilidades y reducir su tamaño/complejidad.
