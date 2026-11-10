# editor:cityGenerator

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/cityGenerator.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 1175 unidades; usa 1168 compartilhadas de outras; é dona principal de 7 compartilhadas

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
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.nodes`, `RoadDoc.segments`, `RoadNode.id`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.id`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/growthWorker.spec.ts`, `tests/editor/powerLinePlanner.spec.ts`, `tests/render/paintCatalogue.spec.ts`, `tests/render/poleLayer.spec.ts`, `tests/render/roadTileWorkers.spec.ts`, `tests/render/surfaceInputs.spec.ts`, `tests/render/surfaceInputsResidency.spec.ts`, `tests/render/surfaceReads.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/render/warmLoose.spec.ts`, `tests/sim/agents/baysWorkers.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/crossingSpansSame.spec.ts`, `tests/sim/indexes/lotGridSame.spec.ts`, `tests/sim/topologyOffPage.spec.ts`, `tests/world/buildingFloors.spec.ts`, `tests/world/cityGreen.spec.ts`, `tests/world/docFork.spec.ts`, `tests/world/elevationScope.spec.ts`, `tests/world/junctionConnectors.spec.ts`, `tests/world/landscapeBatch.spec.ts`, `tests/world/lotRaster.spec.ts`, `tests/world/poleLinesCells.spec.ts`, `tests/world/roadFurniture.spec.ts`, `tests/world/transitNear.spec.ts`, `tests/world/turnPathsEquivalence.spec.ts`, `tests/world/turnPlan.spec.ts`, `tests/world/walkwayCorners.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: já existia 2
- porte `street-furniture`: já existia 1

## Código próprio (linhas — declaração — comentário)
