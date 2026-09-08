# Arquitectura — UNIVERSAL ENGINE

Diseño orientado a principios **SOLID**, módulos desacoplados por interfaces y
nombres de tipos en español dentro del namespace `ue`.

## Capas

```
  Aplicacións  (apps/editor)  ->  EditorApp + UiSystem + paneles
  Editor UI    (editor)       ->  herram., gizmos, cámara, escenas, acciones
  Motor        (core, math, render, scene, anim, physics, media, platform)
  Dependencias externas       ->  GLFW, OpenGL 4.x, Dear ImGui, nlohmann::json
```

## Módulos

| Módulo | Responsabilidad | SOLID |
| --- | --- | --- |
| `ue::core` | Tipos, log, tiempo, servicios (IService/ServiceContainer), eventos (EventBus), UUID, rutas de usuario | S: una responsabilidad por clase; **D**: servicios por interfaz |
| `ue::math` | Vectores, matrices, cuaterniones, color, transformaciones | — |
| `ue::platform` | `IWindow`, `IInput` (abstracción de GLFW); el resto del motor solo trabaja con interfaces | **D**: el motor no conoce GLFW |
| `ue::render` | `IRenderer`, `GpuMesh`, materiales, texturas, cámara y luces | **O**: la fuente `OpenGLRenderer` implementa la interfaz sin tocar el resto de módulos |
| `ue::scene` | `Scene`, `GameObject`, `Component`, `Transform`, `MeshComponent`, primitivas y serializador JSON | **L**: `Component` base intercambiable por cualquiera derivada |
| `ue::editor` | Cámara de autor (orbital/vuelo), picking, gizmos, `EditorState` central, paneles, acciones | **I**: cada panel implementa `IPanel`; **S** en acciones aisladas |
| `ue::anim` | Clips de animación y `Animator` (evaluador por tiempo) | — |
| `ue::physics` | `PhysicsWorld` (gravedad + rebote) | — |
| `ue::media` | BMP (capturas) y `FrameRecorder` (secuencia de imágenes) | — |

## Decisiones clave

- **Interfaces mínimas y únicas**: `IWindow`, `IInput`, `IRenderer`,
  `IService`, `IEditorUserInterface`, `IPanel`. El editor nunca incluye GLFW
  o ImGui directamente salvo a través de `ui::`.
- **El viewport es off-screen**: la escena se dibuja en un FBO RGBA8 cuyo
  resultado se muestra como textura de ImGui (fila 0 abajo → UVs invertidas) y
  se puede leer como píxeles para capturas (con volteo vertical antes de BMP).
- La rejilla de trabajo y el marcador de ejes XYZ los dibuja el propio
  renderizador (`drawGrid`/`drawAxes`), no la escena, para no ensuciar la
  jerarquía; se activan con Ctrl+G y Ctrl+A.
- **Cámara de autor** (`editor::CameraController`): esfera orbital (MMB orbitar,
  Shift+MMB desplazar, rueda zoom, 1/3/7 vistas rápidas) y modo vuelo
  (Shift+F): orientación libre + WASD/E/Q.
- **Transform es la verdad central**: `GameObject.transform`; `MeshComponent`
  expone `revision()` y `markEdited()` para re-subida eficiente de la malla
  editada a la GPU.
- **Los objetos del editor pertenecen al Editor**, no al motor: resetear el
  escenario (nueva escena) descarta objetos `EditorState` sin fugas
  (criptas por `shared_ptr`/`unique_ptr`).
- **Serialización** JSON (`nlohmann`): cabecera `"UNIVERSAL ENGINE"` v1,
  extensión `*.ueproj`. Plantillas en `assets/templates`.

## Flujo del editor (bucle principal)

```
poll  ->  input.beginFrame  ->  resize FBO  ->  physics/animator
      ->  render FBO (rejilla + ejes + objetos)  ->  captura si grabando
      ->  presentToScreen  ->  ImGui (menu, herramientas, jerarquia,
          inspector, viewport)  ->  swapBuffers
```

## Formato de escena (`*.ueproj`)

```json
{
  "formato": "UNIVERSAL ENGINE",
  "version": 1,
  "escena": "Nombre",
  "luz_sol": { "direccion": [...], "color": [...], "ambiente": [...] },
  "camera": { "posicion": [...], "objetivo": [...], "fov": 55 },
  "objetos": [ { "nombre": "...", "posicion": [...], "rotacion": [...],
                 "escala": [...], "primitiva": "cubo", "material": {...},
                 "malla": { "vertices": [[pos, normal, uv], ...],
                            "indices": [...] },
                 "luz": { "tipo": 0, "direccion": [...], "intensidad": 1 } } ]
}
```

© UNIVERSAL ENGINE — TEAM S.H.O.T. WARE