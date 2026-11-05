# world:lanelets

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/lanelets.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 14 linhas
- Alcance total: 529 unidades; usa 464 compartilhadas de outras; é dona principal de 63 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (3), `world:network` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `NodeCrossing.kind`, `NodeCrossing.segment`, `RoadDoc.nodes`, `RoadDoc.segments`, `RoadNode.control`, `RoadNode.crossing`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/roundabout.spec.ts`, `tests/editor/streetObjects.spec.ts`, `tests/editor/uturn.spec.ts`, `tests/render/vehicleFrame.spec.ts`, `tests/scale/edit.spec.ts`, `tests/scale/render/cityMemory.spec.ts`, `tests/sim/cycleUpdate.spec.ts`, `tests/sim/drive/convoy.spec.ts`, `tests/sim/drive/shortTail.spec.ts`, `tests/sim/drive/tactical.spec.ts`, `tests/sim/ghostLane.spec.ts`, `tests/sim/indexes/cyclesHeap.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/junctionRules.spec.ts`, `tests/sim/laneChangeIntent.spec.ts`, `tests/sim/laneChangeRouteFiling.spec.ts`, `tests/sim/laneReachUpdate.spec.ts`, `tests/sim/restingRears.spec.ts`, `tests/sim/routing/denseSearch.spec.ts`, `tests/sim/sidewalkUpdate.spec.ts`, `tests/sim/simResync.spec.ts`, `tests/sim/topologyOffPage.spec.ts`, `tests/sim/trapUpdate.spec.ts`, `tests/world/carriedRoad.spec.ts`, `tests/world/conflictCache.spec.ts`, `tests/world/conflictWindow.spec.ts`, `tests/world/conflictZones.spec.ts`, `tests/world/connectorFootprints.spec.ts`, `tests/world/junctionConnectors.spec.ts`, `tests/world/laneTurnRules.spec.ts`, `tests/world/laneletUpdate.spec.ts`, `tests/world/linksOfSegment.spec.ts`, `tests/world/roadSection.spec.ts`, `tests/world/transition.spec.ts`, `tests/world/turnOffSurface.spec.ts`, `tests/world/turnPathsEquivalence.spec.ts`, `tests/world/turnPlan.spec.ts`, `tests/world/turns.spec.ts`, `tests/world/verticalCurves.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `roads`: não portado 140
- porte `street-furniture`: não se aplica 4
- porte `traffic`: portado 2

## Código próprio (linhas — declaração — comentário)

### src/world/lanelets.ts
- 159-164 `LaneletGraph.build` (method)
- 716-723 `LaneletGraph.linksOnly` (method): @rebuild The link lanelets of every segment ALONE (no junction), made as the whole graph's build makes them: what a tile's bays bake reads (`sim/tileBays.ts`).
