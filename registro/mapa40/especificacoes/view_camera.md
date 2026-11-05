# view:camera

Grupo: view. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/view/camera.ts`, `src/view/cameraGestures.ts`, `src/view/cameraMotion.ts`, `src/view/cameraProfile.ts`
- Unidades próprias (só esta funcionalidade alcança): 81 em 3 arquivo(s), 712 linhas
- Alcance total: 119 unidades; usa 9 compartilhadas de outras; é dona principal de 29 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#pointerDown@1217` (6), `src/main.ts#frame@2644` (5), `src/main.ts#pointerMove@1404` (4), `src/main.ts#gameKeys@2479` (2), `src/main.ts#IfStatement@320` (1), `src/main.ts#fitView@823` (1), `src/main.ts#fitOpeningView@2230` (1), `src/main.ts#roadTool@458` (1), `src/main.ts#coalescedPath@1034` (1), `src/main.ts#CanvasInput@1166` (1), `src/main.ts#cameraHand@553` (1), `src/main.ts#captureLost@1399` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (86), `view:viewport` (36), `editor:buildings` (12), `editor:buildingPlans` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/render/cameraPanBuildings.spec.ts`, `tests/render/cameraStreet.spec.ts`, `tests/ui/roadKeysCamera.spec.ts`, `tests/ui/visibleText.spec.ts`, `tests/view/cameraCoast.spec.ts`, `tests/view/cameraMotion.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `interface`: portado 198, não se aplica 6
- porte `roads`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/view/camera.ts
- 19-29 `Camera.matrix` (method): Transform mapping world coordinates to device pixels.
- 57-66 `Camera.viewBounds` (method): World-space rectangle currently visible, expanded by `margin`.
- 69-75 `Camera.fit` (method): Frames a world bounds rectangle with padding.
- 78-80 `Camera.px` (method): `n` CSS pixels expressed in world units, for hairlines and hit radii.

### src/view/cameraGestures.ts
- 6-16 `CameraGesturesHost` (interface): What the camera's gestures read of the game and do to it.
- 19-19 `ORBIT_PER_PX` (variable): Camera turn and tilt per CSS pixel of an orbit drag, rad: a full turn in ~1000 px.
- 21-21 `TWIST_SIGN` (variable): Which way a twist of two fingers turns the camera, so the map turns with them
- 23-23 `CLICK_SLOP` (variable): How far a press may travel, CSS px, and still be a click rather than a drag.
- 25-25 `TWIST_MIN_SPREAD` (variable): Below this many CSS px between the fingers a twist is noise, not a turn.
- 27-27 `COAST_DECAY_S` (variable): The camera's coast after a drag is let go (three's OrbitControls `enableDamping`, Cesium's `inertiaSpin`)
- 29-29 `COAST_MIN_ORBIT` (variable): Below this the coast is over: orbit rad/s, pan CSS px/s of the hand.
- 30-30 `COAST_MIN_PAN` (variable)
- 32-32 `SPEED_HORIZON_MS` (variable): DO NOT read `performance.now()` in the handler instead: Chrome holds `pointermove` until the next animation frame
- 33-33 `SPEED_STOPPED_MS` (variable)
- 34-34 `MAX_FLING_PX_S` (variable)
- 36-36 `COAST_MAX_STEP_S` (variable): The longest step a coast takes at once, s: the closed form holds for any step; this only bounds a resume after the tab slept.
- 39-43 `PointerSample` (interface): A pointer position at a time: CSS px of the canvas, and the event's time stamp (ms, `performance.now()`'s clock).
- 46-49 `Coast` (type): DO NOT coast an orbit about the view's centre or a pan by the scale at the centre
- 52-333 `CameraGestures` (class): THE CAMERA'S GESTURES: the pointers on the canvas, a pan, an orbit, a two-finger pinch
- 53-53 `CameraGestures.pointers` (field)
- 55-55 `CameraGestures.pan` (field): A pan: the ground point held, or none (`grabbed` null) when it slides by the drag; `last` the pointer, CSS px.
- 57-57 `CameraGestures.orbit` (field): An orbit: where it was pressed and last was, CSS px; a right click that stays a click cancels the gesture in progress.
- 58-58 `CameraGestures.pinch` (field)
- 60-60 `CameraGestures.pressedAt` (field): When the last pointer went down, ms (the event's time).
- 62-62 `CameraGestures.samples` (field): The drag's recent course (`SPEED_HORIZON_MS`): the pointer's positions, CSS px, at the events' times.
- 64-64 `CameraGestures.coast` (field): The camera coasting after a drag let go (`advance`).
- 66-66 `CameraGestures.constructor` (method)
- 69-71 `CameraGestures.active` (method): Something of the camera is being moved by hand.
- 73-75 `CameraGestures.pinching` (method)
- 78-80 `CameraGestures.touches` (method): Pointers on the canvas now.
- 82-84 `CameraGestures.has` (method)
- 87-92 `CameraGestures.gesture` (method): DO NOT write the player's text here.
- 95-99 `CameraGestures.press` (method): A pointer pressed at `screen`, CSS px, at `time` (the event's, ms): a coast in progress is caught.
- 102-104 `CameraGestures.coasting` (method): The camera still coasting after a drag: the frame loop keeps going.
- 107-110 `CameraGestures.startTrack` (method): The drag's course starts at `at`, where it was pressed.
- 113-120 `CameraGestures.track` (method): The pointer was at (x, y) at `t`, ms: kept while within `SPEED_HORIZON_MS` of the newest.
- 123-136 `CameraGestures.handSpeed` (method): The hand's speed as it let go, CSS px/s: the slope of a line fitted by least squares to the last samples
- 139-173 `CameraGestures.throwCoast` (method): A drag let go at `time` (the release's own time stamp, ms)
- 176-206 `CameraGestures.advance` (method): Takes the coast to `now` (ms, the frame's time on the events' clock): true when the camera moved.
- 209-213 `CameraGestures.anchor` (method): The point a pan or pinch holds under the pointer
- 216-227 `CameraGestures.startPinch` (method): Two fingers (or more) down: a pinch from where they are.
- 230-233 `CameraGestures.startOrbit` (method): The camera swung round the ground under `at` (at `height`); `cancelOnClick`: a click cancels what is in progress.
- 236-243 `CameraGestures.startPan` (method): The ground under `at` grabbed and dragged.
- 246-300 `CameraGestures.move` (method): Pointer `id` moved to `screen` at `time` (the event's time stamp, ms): true when the camera took the move.
- 303-318 `CameraGestures.release` (method): Pointer `id` let go at `time` (the release's time stamp, ms).
- 321-325 `CameraGestures.cancel` (method): The gesture in progress cut short (a tool switch, undo, Escape): the pan and the orbit end.
- 328-332 `CameraGestures.releaseAll` (method): Every press forgotten (the window lost focus, the capture was taken): its release will never arrive.

### src/view/cameraMotion.ts
- 19-19 `AXES` (variable)
- 22-22 `PAN_PX_PER_S` (variable): The pan's speed in screen pixels a second: at the closest zoom about a walk (1.5 m/s), far away a map's width in a few seconds.
- 24-24 `TURN_PER_S` (variable): Turn, rad/s (a quarter turn in a second), tilt, rad/s, and zoom, log factor a second.
- 25-25 `TILT_PER_S` (variable)
- 26-26 `ZOOM_PER_S` (variable)
- 28-28 `SHIFT` (variable): Shift held: this much faster.
- 30-30 `EASE_S` (variable): How long a held key takes to reach its speed and to stop, s (time constant).
- 32-32 `ZOOM_EASE_S` (variable): How long a notch of the wheel takes to be spent, s (time constant).
- 34-34 `TURN_EASE_S` (variable): How long a quarter turn by Shift+Q/E takes to be spent, s.
- 36-41 `MotionHost` (interface)
- 44-44 `AIM_CLOSE` (variable): An aimed turn (`aim`) is over when less than this is left, rad; under `AIM_SNAP` the rest is taken at once.
- 45-45 `AIM_SNAP` (variable)
- 46-46 `AIM_MAX_S` (variable)
- 48-48 `MAX_STEP_S` (variable): The longest step the glide takes at once, s: the exact steps hold for any length; this only bounds a resume after the tab slept.
- 51-51 `CameraMotion.held` (field)
- 52-52 `CameraMotion.velocity` (field)
- 53-53 `CameraMotion.shift` (field)
- 55-55 `CameraMotion.pendingZoom` (field): Zoom still to spend (log factor), the screen point it is about, and the ground point it holds there
- 56-56 `CameraMotion.zoomAt` (field)
- 58-58 `CameraMotion.pendingTurn` (field): Turn still to spend, rad.
- 60-60 `CameraMotion.aiming` (field): A heading being turned to (`aim`): what is still left of the turn, asked again each frame, as the view now stands.
- 62-62 `CameraMotion.aimedFor` (field): Seconds the aim has been turning: given up after `AIM_MAX_S`, so a heading never reached cannot hold the frames going.
- 64-64 `CameraMotion.at` (field): A glide starts at the key, notch or turn that began it - DO NOT measure its first step from the last frame drawn
- 66-66 `CameraMotion.constructor` (method)
- 69-71 `CameraMotion.wake` (method): Something began moving at `time`, ms: the glide counts from it if it was still.
- 74-81 `CameraMotion.press` (method): A key went down (at `time`, ms): true when it is one of the camera's.
- 84-87 `CameraMotion.release` (method): A key came up.
- 90-93 `CameraMotion.releaseAll` (method): Every key let go at once (the window lost focus).
- 101-110 `CameraMotion.wheel` (method): A notch of the wheel: `logFactor` of zoom (positive: closer) about the screen point `at`.
- 113-116 `CameraMotion.turn` (method): A turn to spend smoothly (Shift+Q/E's quarter turn), rad.
- 119-123 `CameraMotion.aim` (method): Turns smoothly to a heading (the compass's north): `left` gives the turn still wanted
- 126-133 `CameraMotion.stop` (method): Stops everything at once.
- 136-139 `CameraMotion.moving` (method): Something is still moving (or held): the frame loop keeps going.
- 142-194 `CameraMotion.step` (method): DO NOT step a position by the speed at the end of the step times the step, nor cut the step to a tenth of a second
