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

## Auto-Migración (Legacy a Portable)
Para los usuarios que vienen de una instalación antigua y quieren usar la versión portable sin perder su identidad (créditos, amigos, ID):

**¡Es automático!**
Al arrancar eMule en modo portable por primera vez:
1.  eMule detecta que estás activo (`portable.dat` existe).
2.  Comprueba si tu carpeta `config` portable está vacía.
3.  Si está vacía, busca tu instalación antigua en `%AppData%`.
4.  Si la encuentra, **copia automáticamente** tus archivos vitales:
    -   `cryptkey.dat` (Tu identidad/créditos)
    -   `preferences.ini` (Tu configuración)
    -   `clients.met` (Lo que otros te deben)
    -   `server.met` (Tus servidores)

**Nota**: Esto **SOLO** ocurre si la carpeta `config` junto al ejecutable no existe o está vacía (no contiene `preferences.ini`). Si ya has configurado el eMule portable, no se sobrescribirá nada.

