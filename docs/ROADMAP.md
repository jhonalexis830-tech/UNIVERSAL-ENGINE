# Roadmap — UNIVERSAL ENGINE

## Fase 1 — Núcleo compilable (estado actual)

**Objetivo:** motor de base funcional + editor para trabajar en 3D de verdad.

- [x] Núcleo SOLID (core, math, platform, render, scene, anim, physics, media)
- [x] Renderizador OpenGL 4.x con FBO de viewport
- [x] Cámara orbital estilo Blender + vistas rápidas + modo vuelo
- [x] Picking de objetos y herramientas de selección
- [x] Gizmos de mover / rotar / escalar y edición de vértices
- [x] Primitivas: cubo, plano, esfera, cilindro, cono, cápsula, toro, rejilla
- [x] Inspector (transform, material, luces, física básica)
- [x] Animación por keyframes (auto-key) + reproducción
- [x] Captura BMP y grabación de video (secuencia BMP)
- [x] Guardar / abrir / nueva escena (`*.ueproj`)
- [x] Interfaz completa en español (tema Negro profundo + Azul metálico)

## Fase 2 — Construcción (siguiente)

- [ ] Malla como recurso con importador OBJ/FBX
- [ ] Instancia de objetos (duplicar, clonar con Ctrl+D)
- [ ] Sistema de ejes/ocultar y bloqueo por objeto; renombrar en Jerarquía
- [ ] Luces puntuales y focos renderizados (Fase 1: solo luz solar global)
- [ ] Sombras dinámicas (shadow mapping)
- [ ] Snap a rejilla, medición de distancias y ángulos
- [ ] Deshacer/rehacer (pila de comandos)
- [ ] Grabación de vista de cámara y exportación de video MP4 (vía FFmpeg)

## Fase 3 — Diseño y modelado

- [ ] Extrusión de caras / aristas, Subdivision Surface
- [ ] Herramientas booleanas, biselado, array (patrones)
- [ ] Pintura de vértices y UV mapping simple
- [ ] Importación de imágenes de referencia (fondos de encuadre)

## Fase 4 — Animación y simulación

- [ ] Editor de curvas (dope sheet / f-curves)
- [ ] Motor físico completo (colisiones, juntas, raycast)
- [ ] Simulación de partículas y cloth (básico)

## Fase 5 — Videojuegos e integración

- [ ] Sistema de scripts (C++ / Lua) con registro de componentes
- [ ] Optimización de renderizado por culling y batching
- [ ] Soporte multiplataforma (Linux)

© UNIVERSAL ENGINE — TEAM S.H.O.T. WARE