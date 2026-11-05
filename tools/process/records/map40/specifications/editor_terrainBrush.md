# editor:terrainBrush

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/terrainBrush.ts`
- Unidades próprias (só esta funcionalidade alcança): 23 em 1 arquivo(s), 311 linhas
- Alcance total: 209 unidades; usa 186 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#pointerMove@1404` (3), `src/main.ts#terrainBrush@579` (2), `src/main.ts#frame@2644` (2), `src/main.ts#drawOverlayScreen@2942` (2), `src/main.ts#dabSettings@591` (1), `src/main.ts#endPointer@1484` (1), `src/main.ts#currentGesture@1056` (1), `src/main.ts#gestureInProgress@1117` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#endTerrainStroke@1152` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (74), `world:doc` (16), `world:trees` (10), `editor:cloudTool` (8), `world:elements` (7), `world:terrain` (4), `editor:editStream` (3), `editor:barriers` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/terrainBrush.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: portado 48

## Código próprio (linhas — declaração — comentário)

### src/editor/terrainBrush.ts
- 13-13 `TERRAIN_SPACING` (variable): Spacing between dabs along a stroke, a fraction of the radius.
- 15-15 `TERRAIN_MIN_MS` (variable): The shortest time between dabs, ms.
- 17-17 `TERRAIN_STAMP_MARK` (variable): Where the terrain stamps' allocator stands in `RoadDoc.allocatorMarks`.
- 21-25 `Landform` (interface): A landform: a shape and its rock in one tool (`main.ts` LANDFORMS).
- 28-43 `DabSettings` (interface): Everything a dab reads of the brush in hand, read once per dab.
- 46-46 `NOISE_ORIGIN_REACH` (variable): How far a dab may be from its stroke's noise origin (`TerrainStamp.ox`) before the stroke takes a new one
- 49-55 `noiseOrigin` (function): A stroke's noise origin for a dab at `at`, on `at`'s map (moved on to `at` when it has gone too far).
- 59-138 `stampTerrain` (function): False: the dab's tile holds as many of its kind as it may (decision 5.1): nothing laid, the player told by the caller.
- 141-154 `TerrainBrushHost` (interface): What the stroke reads of the game and does to it.
- 157-168 `Stroke` (interface): A stroke being painted.
- 170-265 `TerrainBrush` (class)
- 171-171 `TerrainBrush.stroke` (field)
- 173-173 `TerrainBrush.repeat` (field): A flatten held: it levels on at every frame its interval allows (`tick`).
- 175-175 `TerrainBrush.constructor` (method)
- 178-180 `TerrainBrush.pointer` (method): The pointer of the stroke being painted, or null.
- 182-184 `TerrainBrush.stroking` (method)
- 187-189 `TerrainBrush.at` (method): Where the stroke's pointer is, and the level a flatten keeps (the brush ring is drawn there).
- 191-193 `TerrainBrush.level` (method)
- 196-206 `TerrainBrush.begin` (method): Starts a stroke, capturing the level target and arming the held repeat.
- 209-219 `TerrainBrush.tick` (method): Run from the frame (`main.ts` frame), as the game's other timed work
- 222-252 `TerrainBrush.paint` (method): @perInput Paints from the last dab to `at`, laying dabs along the way
- 255-260 `TerrainBrush.end` (method): Ends the stroke; true when one was being painted.
- 262-264 `TerrainBrush.disarm` (method)
