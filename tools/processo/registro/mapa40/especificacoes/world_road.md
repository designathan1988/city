# world:road

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/roadField.ts`, `src/world/roadSection.ts`, `src/world/roadTypes.ts`
- Unidades próprias (só esta funcionalidade alcança): 7 em 1 arquivo(s), 61 linhas
- Alcance total: 191 unidades; usa 54 compartilhadas de outras; é dona principal de 130 compartilhadas

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
- Usa: `render:buildings` (6)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/buildingCorner.spec.ts`, `tests/editor/freeRoad.spec.ts`, `tests/editor/poles.spec.ts`, `tests/editor/roadPriority.spec.ts`, `tests/editor/roundabout.spec.ts`, `tests/editor/snapNear.spec.ts`, `tests/editor/uturn.spec.ts`, `tests/editor/zoning.spec.ts`, `tests/perf/fillEdit.spec.ts`, `tests/render/cellPaving.spec.ts`, `tests/render/pedSlope.spec.ts`, `tests/render/pierOnRoad.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/sim/laneConnectors.spec.ts`, `tests/sim/uturn.spec.ts`, `tests/sim/walkHeight.spec.ts`, `tests/ui/roadProfileEditor.spec.ts`, `tests/ui/roadSectionEditor.spec.ts`, `tests/world/bendSeam.spec.ts`, `tests/world/bulb.spec.ts`, `tests/world/connectorFootprints.spec.ts`, `tests/world/curveFit.spec.ts`, `tests/world/cutWalls.spec.ts`, `tests/world/economy.spec.ts`, `tests/world/elevation.spec.ts`, `tests/world/facadeOverlap.spec.ts`, `tests/world/forkApproach.spec.ts`, `tests/world/garageUnder.spec.ts`, `tests/world/junction.spec.ts`, `tests/world/laneLines.spec.ts`, `tests/world/laneTurnRules.spec.ts`, `tests/world/levelFlights.spec.ts`, `tests/world/lorryDocks.spec.ts`, `tests/world/lotCorners.spec.ts`, `tests/world/lotPhysics.spec.ts`, `tests/world/roadFieldPrune.spec.ts`, `tests/world/roadPaint.spec.ts`, `tests/world/roadProfile.spec.ts`, `tests/world/roadSection.spec.ts`, `tests/world/roadSectionFlipSame.spec.ts`, `tests/world/section.spec.ts`, `tests/world/shopFloor.spec.ts`, `tests/world/splitLevel.spec.ts`, `tests/world/steppedYards.spec.ts`, `tests/world/streetDoors.spec.ts`, `tests/world/streetFurniture.spec.ts`, `tests/world/surfaces.spec.ts`, `tests/world/terrace.spec.ts`, `tests/world/transition.spec.ts`, `tests/world/transitionLevels.spec.ts`, `tests/world/turnOffSurface.spec.ts`, `tests/world/union.spec.ts`, `tests/world/unzone.spec.ts`, `tests/world/walkways.spec.ts`, `tests/world/zoneGridOverlap.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `road-elevation`: a portar 20
- porte `roads`: não portado 168, em parte 70
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/world/roadField.ts
- 159-161 `SpatialIndex.isOversized` (method): By a set: asked for every profile of the build (`profileSummaries`).
- 190-209 `SpatialIndex.digest` (method): Digest of every profile `near` can return for a point of the rectangle, in its order; a profile `of` gives null for is left out.
- 212-212 `SpatialIndex.ranks` (field): Each profile's place in the list the index was built from, and a mark per place (`meeting`).
- 213-213 `SpatialIndex.marks` (field)
- 214-214 `SpatialIndex.mark` (field)
- 217-248 `SpatialIndex.meeting` (method): Every profile `near` can return for a point of the rectangles, and `also`, in the order the index was built from
- 379-381 `PointHeights.count` (method)
