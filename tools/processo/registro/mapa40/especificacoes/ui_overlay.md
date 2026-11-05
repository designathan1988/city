# ui:overlay

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/overlay/builderGrid.ts`, `src/ui/overlay/buildingOverlay.ts`, `src/ui/overlay/congestion.ts`, `src/ui/overlay/palette.ts`, `src/ui/overlay/surface.ts`
- Unidades próprias (só esta funcionalidade alcança): 33 em 5 arquivo(s), 516 linhas
- Alcance total: 201 unidades; usa 168 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:change`, `ouve:resize`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#buildings@760` (25), `src/main.ts#drawOverlayScreen@2942` (13), `src/main.ts#syncViewFromFlatCamera@812` (4), `src/main.ts#worldAtScreen@959` (4), `src/buildingsWiring.ts#createBuildingWiring@81` (3), `src/main.ts#drawPolePlan@2849` (3), `src/main.ts#drawBarrierPlan@2786` (2), `src/main.ts#drawStreetscapeHover@2808` (2), `src/main.ts#IfStatement@320` (2), `src/main.ts#cameraHand@553` (2), `src/main.ts#cameraMotion@563` (2), `src/main.ts#fitView@823` (2)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (149), `world:buildings` (102), `editor:barriers` (13), `world:ribbons` (8), `world:ids` (4), `world:network` (4), `editor:buildingSnap` (4), `world:road` (3), `ui:i18n` (1), `editor:blueprintLibrary` (1), `sim:agents` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/ui/builderGrid.spec.ts`, `tests/ui/congestion.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/ui/overlay/builderGrid.ts
- 6-31 `drawBuilderGrid` (function): DO NOT grow a second layer of gizmos here

### src/ui/overlay/buildingOverlay.ts
- 23-42 `BuildingOverlayInput` (interface): The building tool's hairlines on the overlay canvas
- 44-225 `drawBuildingOverlay` (function)
- 227-236 `GLYPH` (variable)
- 239-239 `SELECTION_FAINT` (variable): The selected volume's roof edge: present, but quieter than the base line.
- 242-242 `RELIEF` (variable): The push-pull handle of a face region, in the second accent.
- 244-316 `drawHandle` (function)

### src/ui/overlay/congestion.ts
- 10-10 `Moving` (interface): A vehicle as the map reads it: the road its lane is on (undefined on a junction's connector or off the road).
- 13-13 `CAR_SPACE` (variable): Road a queued car takes up, its length and the gap to the next, m (HCM jam density: some 130 cars a lane-km).
- 15-15 `STOPS` (variable): Free, busy, slowing, jammed: share of the lanes' length taken at each colour's start.
- 18-22 `congestionColour` (function): The colour of a road whose lanes are `occupancy` full.
- 25-36 `roadOccupancy` (function): Each road's occupancy: the length its vehicles take over the length of its travel lanes (a few cars waiting at a light leave a long street free).
- 39-39 `sampled` (variable): A road's line as the map draws it: the middle, the middle moved across by the road's width, then each point of the band (x, y).
- 42-58 `bandOf` (function): The points of a road's band, sampled once: a road changed is a new ribbon (`networkBuild.ts`), so DO NOT sample it again each frame.
- 61-84 `drawCongestion` (function): The congestion map drawn on the editor's overlay: the roads meeting `view` (a world box), as bands as wide as their carriageway.

### src/ui/overlay/palette.ts
- 4-4 `SELECTION` (variable): Editor feedback.
- 5-5 `HOVER` (variable)
- 6-6 `INVALID` (variable)

### src/ui/overlay/surface.ts
- 2-2 `MAX_DPR` (variable): Canvas sizing and device-pixel-ratio handling.
- 5-67 `CanvasSurface` (class): DO NOT ask the canvas for a context nor size its backing store to the screen (UI-11): nothing draws on it (the pointer's target and the screen's measure), and its opaque, desynchronized 2D store of the whole screen was memory and a composit
- 6-6 `CanvasSurface.dpr` (field)
- 7-7 `CanvasSurface.cssW` (field)
- 8-8 `CanvasSurface.cssH` (field)
- 11-11 `CanvasSurface.deviceW` (field): The screen in device pixels as last measured: what a change is told by.
- 12-12 `CanvasSurface.deviceH` (field)
- 13-13 `CanvasSurface.ro` (field)
- 14-14 `CanvasSurface.stopDprWatch` (field)
- 16-23 `CanvasSurface.constructor` (method)
- 26-34 `CanvasSurface.observe` (method): Starts watching for size and pixel-density changes.
- 36-43 `CanvasSurface.dispose` (method)
- 45-47 `CanvasSurface.handleChange` (field)
- 50-66 `CanvasSurface.resize` (method): Returns true when the screen's size in device pixels changed.
- 70-91 `watchDpr` (function): Calls `cb` whenever the device pixel ratio changes.
