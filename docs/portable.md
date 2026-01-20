# Modo Portable (Stealth) en eMule-Aishor

## Introducción
El **Modo Portable** permite ejecutar eMule-Aishor sin dejar rastros en el registro de Windows ni utilizar las carpetas de usuario estándar (`AppData`). Esto es ideal para:
- Ejecutar eMule desde una memoria USB.
- Mantener múltiples instalaciones aisladas con configuraciones independientes.
- Entornos de prueba o desarrollo.

## ¿Cómo funciona?
eMule-Aishor incorpora un "interruptor físico" basado en la presencia de un archivo específico. Al iniciar, el programa busca un archivo llamado `portable.dat` en el mismo directorio donde se encuentra el ejecutable (`emule.exe`).

- **Si `portable.dat` EXISTE:**
  - Se activa el **Modo Portable**.
  - Se ignora cualquier configuración existente en el Registro de Windows.
  - Todas las carpetas de datos (`config`, `Incoming`, `Temp`, `logs`, etc.) se crean y buscan dentro del directorio del ejecutable.

- **Si `portable.dat` NO EXISTE:**
  - Se comporta como una aplicación estándar de Windows.
  - Utiliza las rutas de usuario (`C:\Users\[TuUsuario]\AppData\Local\eMule`...).
  - Lee y escribe en el Registro de Windows.

## Instrucciones de Activación

1.  **Localiza tu ejecutable**: Ve a la carpeta donde tienes `emule.exe` (por ejemplo, en tu USB o carpeta de Release).
2.  **Crea el archivo "llave"**:
    *   Haz clic derecho en un espacio vacío -> Nuevo -> Documento de texto.
    *   Renómbralo a `portable.dat`.
    *   **Importante**: Asegúrate de borrar la extensión `.txt`. El archivo debe llamarse exactamente `portable.dat`. (Puede ser un archivo vacío, su contenido es irrelevante).
3.  **Ejecuta eMule**: Inicia `emule.exe`. El programa detectará el archivo y creará automáticamente las carpetas `config`, `Incoming` y `Temp` a su lado.

## Migración a Portable
Si quieres convertir tu instalación actual a portable y conservar tus datos:
1.  Cierra eMule.
2.  Crea el archivo `portable.dat` junto a `emule.exe`.
3.  Copia el contenido de tu carpeta `config` original (normalmente en `%LocalAppData%\eMule\config`) a la carpeta `config` que está junto a `emule.exe` (créala si no existe).
4.  Mueve tus archivos `Temp` e `Incoming` si lo deseas, o reconfigura las rutas en *Preferencias -> Directorios* una vez iniciado.
