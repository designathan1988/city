# world:junction

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/junction/build.ts`, `src/world/junction/bulb.ts`, `src/world/junction/corners.ts`, `src/world/junction/derive.ts`, `src/world/junction/legs.ts`, `src/world/junction/levels.ts`, `src/world/junction/mouthOverlap.ts`, `src/world/junction/polygon.ts`, `src/world/junction/transition.ts`, `src/world/junction/trim.ts`, `src/world/junction/trimSummary.ts`, `src/world/junction/types.ts`, `src/world/junctionBuild.ts`, `src/world/junctionTurns.ts`
- Unidades próprias (só esta funcionalidade alcança): 35 em 5 arquivo(s), 118 linhas
- Alcance total: 721 unidades; usa 472 compartilhadas de outras; é dona principal de 214 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#clearBulbs@154` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (17), `world:doc` (15), `world:road` (11), `world:carriageway` (10), `world:network` (9), `editor:snap` (4), `world:ids` (3), `world:elements` (3), `editor:growthFeed` (2), `editor:transitTools` (2), `world:utilities` (1), `world:landscape` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `NodeCrossing.kind`, `NodeCrossing.segment`, `RoadNode.blockedMovements`, `RoadNode.control`, `RoadNode.crossing`, `RoadNode.end`, `RoadNode.heightOffset`, `RoadNode.id`, `RoadNode.incident`, `RoadNode.laneLinks`, `RoadNode.uturn`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.curve`, `RoadSegment.direction`, `RoadSegment.id`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/freeRoad.spec.ts`, `tests/sim/topologyOffPage.spec.ts`, `tests/world/bulb.spec.ts`, `tests/world/junction.spec.ts`, `tests/world/transition.spec.ts`, `tests/world/turnOffSurface.spec.ts`, `tests/world/turnPlan.spec.ts`, `tests/world/turns.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `roads`: não portado 273, não se aplica 98
- porte `people`: já existia 2
- porte `street-furniture`: não se aplica 2
- porte `road-elevation`: a portar 98
- porte `traffic`: já existia 3

## Código próprio (linhas — declaração — comentário)

### src/world/junction/bulb.ts
- 94-101 `bulbClearing` (function): DO NOT walk every junction of the map, and every pole and element of the map for each, after every edit
- 104-135 `clearBulbsOf` (function): `bulbClearing`'s work: the circles `delta` names, or every junction when it is null.

### src/world/junction/corners.ts
- 12-12 `CornerData` (type): A corner as plain data (`cornerData`, `cornerOf`): what a message's clone keeps of it (`NetworkBake`).
- 19-19 `PackedCorner.constructor` (method)
- 21-21 `PackedCorner.psi` (method)
- 22-22 `PackedCorner.trimI` (method)
- 23-23 `PackedCorner.trimJ` (method)
- 24-24 `PackedCorner.x` (method)
- 25-28 `PackedCorner.fillet` (method)
- 67-69 `cornerData` (function): A corner's fields as plain data (`CornerData`).

### src/world/junction/legs.ts
- 13-13 `LegData` (type): A leg as plain data, every field its own (`legData`, `legOf`): what a message's clone keeps of it (`NetworkBake`).
- 24-33 `PackedLeg.constructor` (method)
- 35-35 `PackedLeg.origin` (method)
- 36-36 `PackedLeg.dir` (method)
- 37-37 `PackedLeg.nrm` (method)
- 38-38 `PackedLeg.ang` (method)
- 39-39 `PackedLeg.hw` (method)
- 40-40 `PackedLeg.hwLeft` (method)
- 41-41 `PackedLeg.hwRight` (method)
- 42-42 `PackedLeg.hwSidewalk` (method)
- 43-43 `PackedLeg.length` (method)
- 44-44 `PackedLeg.shift` (method)
- 87-93 `legData` (function): A leg's fields as plain data (`LegData`).

### src/world/junction/levels.ts
- 27-27 `HELD_RINGS` (variable): How many carriageways' tongues and rings made again are held.
- 36-36 `madeRings` (variable): A carriageway's tongues and rings made again from its legs, trims and outline (`LeanJunction`).
- 45-49 `fill` (function): The tongues and rings `buildJunctionRing` and `deriveJunctionLevel` lay round a junction's outline: one tongue a leg cut back, a cap where every leg leaves on one side.
- 68-70 `LeanJunction.tongues` (method)
- 72-74 `LeanJunction.rings` (method)
- 77-77 `made` (function)
- 302-304 `JunctionLevels.keys` (method)
- 306-308 `JunctionLevels.values` (method)
- 310-312 `JunctionLevels.entries` (method)
- 314-316 `JunctionLevels.MethodDeclaration` (method)
- 318-320 `JunctionLevels.forEach` (method)

### src/world/junction/transition.ts
- 67-74 `TransitionAxis.constructor` (method)
