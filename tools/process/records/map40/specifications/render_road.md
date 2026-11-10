# render:road

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/roadProgramStandIns.ts`, `src/render/roadStandIn.ts`, `src/render/roadStations.ts`, `src/render/roadSurfaceEntries.ts`, `src/render/roadSurfaceInputs.ts`, `src/render/roadSurfacePass.ts`, `src/render/roadSurfaceReads.ts`, `src/render/roadSurfaceRings.ts`, `src/render/roadSurfaces.ts`, `src/render/roadTileMesh.ts`, `src/render/roadTilePool.ts`, `src/render/roadTileShape.ts`, `src/render/roadTones.ts`
- Unidades próprias (só esta funcionalidade alcança): 17 em 3 arquivo(s), 132 linhas
- Alcance total: 1209 unidades; usa 942 compartilhadas de outras; é dona principal de 250 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:mesh` (6), `worker:job` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`, `RoadDoc.markingStyle`, `RoadDoc.nodes`, `RoadDoc.segments`, `RoadDoc.terrainRelief`, `RoadDoc.terrainStamps`, `RoadNode.control`, `RoadNode.heightOffset`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `earcut`, `meshoptimizer`, `three`

## Testes do 4.0 que a exercitam
- `tests/perf/budgets.spec.ts`, `tests/perf/fillEdit.spec.ts`, `tests/perf/fillNearView.spec.ts`, `tests/render/paintCatalogue.spec.ts`, `tests/render/roadPasses.spec.ts`, `tests/render/roadStandIn.spec.ts`, `tests/render/roadTileWorkers.spec.ts`, `tests/render/roadTiles.spec.ts`, `tests/render/surfaceInputs.spec.ts`, `tests/render/surfaceInputsResidency.spec.ts`, `tests/render/surfaceReads.spec.ts`, `tests/render/warmLoose.spec.ts`, `tests/scale/census/derivedBound.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 405, não portado 3
- porte `people`: já existia 5
- porte `traffic`: portado 2

## Código próprio (linhas — declaração — comentário)

### src/render/roadStandIn.ts
- 71-73 `RoadStandIn.constructor` (method)
- 111-113 `RoadStandIn.revisions` (method): The commits whose stand-ins wait for the world's build (`show`).
- 125-127 `RoadStandIn.clear` (method): Every stand-in let go (another map, the diary read from nothing).
- 129-133 `RoadStandIn.dispose` (method)

### src/render/roadSurfaceInputs.ts
- 670-672 `SurfaceInputs.paintedColours` (method): The colours the roads and junctions have been painted in.

### src/render/roadTilePool.ts
- 23-23 `SILENCE_MS` (variable): Silence past which a worker with tiles waiting is reported (it beats every 500 ms while it works, when it runs).
- 25-25 `MOST_LANES` (variable): Most workers the pool opens: on the desktop, workers of the job processes (`jobPool.ts` PROCESS_LANES).
- 68-70 `RoadTilePool.constructor` (method)
- 71-71 `RoadTilePool.nextId` (field)
- 72-72 `RoadTilePool.nextLane` (field)
- 74-85 `RoadTilePool.coarsen` (method)
- 87-109 `RoadTilePool.tiles` (method)
- 111-118 `RoadTilePool.check` (method)
- 120-124 `RoadTilePool.start` (method)
- 126-130 `RoadTilePool.startCoarse` (method)
- 132-180 `RoadTilePool.open` (method)
- 183-188 `RoadTilePool.fail` (method): The pool fails: every job waiting on any worker rejected (built on the page), every worker ended.
