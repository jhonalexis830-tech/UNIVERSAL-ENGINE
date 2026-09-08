# UNIVERSAL ENGINE

Motor gráfico 3D multi-herramienta desarrollado por **TEAM S.H.O.T. WARE**.
Una sola base tecnológica para arquitectura, diseño industrial, animación,
videojuegos y simulación. Interfaz en español, tema *Negro profundo + Azul metálico*.

## Estado

- **Fase 1 — Núcleo compilable**: motor SOLID + visor 3D + cámara de estilo
  Blender + edición básica de mallas con gizmos + grabación de animación y
  capturas + interfaz completa en español.
- Editor ejecutable: `UE_Editor`.

## Características (Fase 1)

- Viewport 3D con órbita/pan/zoom (ratón), vistas rápidas (1 / 3 / 7) y modo vuelo (Shift+F, WASD + E/Q).
- Panel Inspector: posición, rotación y escala; color y brillo de materiales; propiedades de luces; botón de física («Cae con gravedad»).
- Gizmos de mover / rotar / escalar y edición de vértices (herramienta «Vértice»).
- Creación de primitivas: cubo, plano, esfera, cilindro, cono, cápsula, toro y rejilla.
- Grabación de animación (auto-key por clave de transformación) y reproducción.
- Captura del viewport en BMP y grabación de video como secuencia BMP.
- Guardar / abrir / nueva escena en formato JSON propio (`*.ueproj`).
- Estado del motor: gravedad y rebote básicos para simulación.

## Descargar el editor (usuarios finales)

Descargue el **ZIP portable** desde *Releases* de GitHub:

> GitHub → `Releases` → `UniversalEngine-Editor-vX.Y...-win-x64.zip`

Es un ejecutable único que **no necesita instalar nada**.

### Requisitos

- Windows 10/11 de 64 bits.
- Tarjeta gráfica compatible con **OpenGL 4.3 o superior**.
- No requiere internet en el equipo del usuario (ni backend, ni registro).

### Instalación en 3 pasos

1. **Descomprima** el ZIP en una carpeta, p. ej. `C:\Pepe\UniversalEngine`.
2. **Ejecute** `UE_Editor.exe` (doble clic).
3. Cree o abra un proyecto desde el menú **Archivo** (o use **Archivo → Plantillas → Escena demo**).

> No borre la carpeta `assets\templates` que va junto al ejecutable: las
> plantillas de escena se leen desde ahí. Todo (capturas, videos y proyectos)
> se guarda solo en `Mis Documentos\UNIVERSAL ENGINE`.
>
> Si aparece un letrero y la ventana no abre, actualice los controladores de la
> tarjeta gráfica.

### Controles rápidos

| Tecla | Acción |
| --- | --- |
| `Q` `W` `E` `R` `T` | Seleccionar / Mover / Rotar / Escalar / Editar vértices |
| `F` / `Shift+F` | Enfocar cámara / Modo vuelo (WASD + E/Q + ratón) |
| `Ctrl+R` | Grabar animación (BMP en Mis Documentos) |
| `1` `3` `7` | Vistas frontal / lateral / superior |

Ver `INSTRUCCIONES.txt` dentro del ZIP para la lista completa.

---

## Compilación (desarrolladores)

### Requisitos

- Windows 10/11 con **Visual Studio 2022** (compilador MSVC C++17).
  - Instale solo **Herramientas de compilación de C++ para VS** desde el
    instalador de VS si no quiere instalar el IDE completo.
- CMake 3.20+ (se descargan GLFW, Dear ImGui y nlohmann::json automáticamente).

### Pasos (línea de comandos)

```bat
cd "C:\Users\Mariel\Documents\Default Project"
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Ejecutar:

```bat
build\apps\editor\Release\UE_Editor.exe
```

> También puede abrir `build\UNIVERSAL_ENGINE.sln` con Visual Studio y compilar
> el proyecto `UE_Editor`.

### Compilación automática y publicación (GitHub Actions)

Al **empujar** cambios a la rama principal, el workflow `.github/workflows/build-release.yml`
compila automáticamente `UE_Editor.exe` en Windows x64 (Release) y sube el ZIP portable
como artefacto. Cuando se publica una **etiqueta `v*`** (p. ej. `v0.1.0`), además crea una
*Release* de GitHub con el ZIP descargable:

```bat
git tag v0.1.0
git push origin v0.1.0
```

Para empaquetar un ZIP manualmente desde un build local:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\make_dist.ps1 -BuildDir build -Config Release
```

### Atajos de teclado

| Tecla | Acción |
| --- | --- |
| `Q` `W` `E` `R` `T` | Seleccionar / Mover / Rotar / Escalar / Editar vértices |
| `Supr` | Eliminar objeto seleccionado |
| `F` | Enfocar cámara en la selección |
| `Shift+F` | Alternar modo vuelo (WASD + E/Q + ratón) |
| `Ctrl+G` `Ctrl+A` `Ctrl+S` | Mostrar rejilla / ejes / sombreado |
| `Ctrl+R` | Iniciar / detener grabación de video |
| `1` `3` `7` | Vista frontal / lateral / superior |

### Rutas de usuario

- Capturas: `%USERPROFILE%\Documents\UNIVERSAL ENGINE\Capturas`
- Videoclips: `%USERPROFILE%\Documents\UNIVERSAL ENGINE\Recordings`
- Proyectos: `%USERPROFILE%\Documents\UNIVERSAL ENGINE\Proyectos`

## Estructura

```
engine/    Núcleo del motor (core, math, platform, render, scene, editor, anim, physics, media)
apps/      Programas (editor)
assets/    Recursos (branding, plantillas de escena *.ueproj)
/.github/  Workflow de compilación y publicación (CI)
scripts/   Herramientas (empaquetado del ZIP portable)
docs/      Arquitectura y roadmap
```

Consulte `docs/ARQUITECTURA.md` para los detalles de diseño (SOLID) y
`docs/ROADMAP.md` para el plan de fases.

© UNIVERSAL ENGINE — TEAM S.H.O.T. WARE