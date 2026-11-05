# world:cityGen

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/cityGen/field.ts`, `src/world/cityGen/largeCity.ts`, `src/world/cityGen/mapSize.ts`, `src/world/cityGen/plan.ts`, `src/world/cityGen/smallCity.ts`, `src/world/cityGen/squares.ts`, `src/world/cityGen/streets.ts`, `src/world/cityGen/wholeMap.ts`
- Unidades próprias (só esta funcionalidade alcança): 38 em 2 arquivo(s), 186 linhas
- Alcance total: 199 unidades; usa 92 compartilhadas de outras; é dona principal de 69 compartilhadas

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
- Usa: `worker:conflictTable` (38), `editor:barriers` (24), `sim:agents` (17), `editor:editStream` (7), `world:road` (6), `world:zones` (5), `world:tiles` (4), `world:terrain` (3), `world:growth` (1), `sim:meso` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.segments`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/growthWorker.spec.ts`, `tests/editor/powerLinePlanner.spec.ts`, `tests/editor/roundabout.spec.ts`, `tests/render/farSlab.spec.ts`, `tests/render/isoViewport.spec.ts`, `tests/render/paintCatalogue.spec.ts`, `tests/render/poleLayer.spec.ts`, `tests/render/roadTileWorkers.spec.ts`, `tests/render/surfaceInputs.spec.ts`, `tests/render/surfaceInputsResidency.spec.ts`, `tests/render/surfaceReads.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/render/warmLoose.spec.ts`, `tests/sim/agents/baysWorkers.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/crossingSpansSame.spec.ts`, `tests/sim/indexes/lotGridSame.spec.ts`, `tests/sim/topologyOffPage.spec.ts`, `tests/world/buildingFloors.spec.ts`, `tests/world/cityBlockLevels.spec.ts`, `tests/world/cityBlocks.spec.ts`, `tests/world/cityGreen.spec.ts`, `tests/world/docFork.spec.ts`, `tests/world/elevationScope.spec.ts`, `tests/world/junctionConnectors.spec.ts`, `tests/world/landscapeBatch.spec.ts`, `tests/world/lotRaster.spec.ts`, `tests/world/poleLinesCells.spec.ts`, `tests/world/roadFurniture.spec.ts`, `tests/world/transitNear.spec.ts`, `tests/world/turnPathsEquivalence.spec.ts`, `tests/world/turnPlan.spec.ts`, `tests/world/unboundedPlates.spec.ts`, `tests/world/walkwayCorners.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: já existia 10
- porte `roads`: não portado 165

## Código próprio (linhas — declaração — comentário)

### src/world/cityGen/field.ts
- 57-60 `TensorField.strength` (method): How much the field holds here, 0 where no basis field reaches.

### src/world/cityGen/wholeMap.ts
- 16-16 `PLAN_VERSION` (variable): Bumped when what the plan puts where changes (the map's edge, the city's spread): a world's stored tiles and far bakes are kept under it, so a city of an older plan is never shown for a new one.
- 18-18 `worldKey` (function): The key a world is stored and baked under: the seed, the map's size and the plan's version.
- 20-20 `tileOnMap` (function): Whether tile (i, j) is on the map (-5..4 each way at 10).
- 26-26 `CORNER` (variable): No gate this near a tile's corner: the corner is the two border avenues' junction.
- 28-28 `GATE_MIN` (variable): Distance between two gates of a border, at least and at most.
- 29-29 `GATE_MAX` (variable)
- 31-31 `AVENUE_GATES` (variable): Share of the gates that are avenues: a straight avenue across the world every few kilometres.
- 34-34 `WholeMapOptions` (type): The whole map's options: a generated city's (`CityOptions`) without a size; style 'mixed' picks a style per tile.
- 37-37 `WHOLE_MAP_DEFAULTS` (variable): What the whole map asks when the player asks only for it: a style per tile, three centres, some parks.
- 40-40 `borderLine` (function): The line of border `k` along either axis, for any integer `k`: the tiles' edges (`tiles.ts`), exact multiples of `TILE`.
- 43-43 `EDGE_INSET` (variable): How far in from the map's edge the avenues closing it run: half an avenue with its footways, so nothing of them stands past the ground, which ends at the edge (`flatAtlas.ts` slab).
- 46-46 `mapLine` (function): Where border `k`'s avenue runs: on the border, or `EDGE_INSET` inside the map on its edges (west and south at -MAP_TILES / 2, east and north at MAP_TILES / 2).
- 49-52 `Gate` (interface): A street's crossing of a border avenue; `level` 0 an avenue, 1 a collector (`LEVEL_ROAD`).
- 55-63 `mix` (function): A hash of a few integers: the seed of a rule's own stream.
- 66-76 `gatesOf` (function): The gates of row or column `k`, any integer (`axis` 0: across the borders x = const of row k, at these y; 1: across y = const of column k, at these x).
- 79-79 `Hub` (interface): A centre: how strongly it draws and how far; a downtown or a neighbourhood's.
- 82-82 `TileKind` (type): What a tile is: how built up, and how its streets are laid.
- 84-84 `TileStyle` (type): A tile's street style: a generated city's (`CityStyle`), or the countryside's (its border avenues and avenue gates only).
- 86-93 `TileCharacter` (interface)
- 96-96 `DOWNTOWN` (variable): Heat at or over which a place is downtown, urban, a suburb; under the last, countryside.
- 97-97 `URBAN` (variable)
- 98-98 `SUBURB` (variable)
- 100-100 `MAP_FLOOR` (variable): The least heat on the map: built up everywhere (suburbs at the least), the downtowns and the centres over it (the city over the whole map).
- 102-102 `SUB_CENTRES` (variable): Centres of neighbourhoods besides a city's downtowns.
- 104-104 `WORKS` (variable): Industrial districts of a city, and their reach.
- 105-105 `WORKS_REACH` (variable)
- 108-108 `DISTRICT` (variable): A district cell's side: 8 tiles, so its edges are tiles' edges; cell (0, 0) is centred on the origin.
- 110-110 `NEAR` (variable): Cells each way whose centres can reach a place: a city's centres stand within ~7.3 km of its cell's middle and pull a radius of 3 km at most, so a cell 3 away adds under 1e-7 - left out.
- 112-112 `CITY_JITTER` (variable): A city's first downtown, this far from its cell's middle at most (the cell at the origin: the player's start, near the middle).
- 113-113 `ORIGIN_JITTER` (variable)
- 115-115 `CITY_SHARE` (variable): Share of district cells holding a city at density 0 and 1 (the cell at the origin always does).
- 117-117 `VILLAGE_SHARE` (variable): Share of the other cells holding a village: one neighbourhood centre in the countryside.
- 119-128 `WholeMapPlan` (interface)
- 131-131 `tileMiddle` (function): The tile's middle.
- 134-134 `District` (interface): What one district cell holds: its centres and its industry.
- 137-174 `districtOf` (function): District cell (`cx`, `cy`): a city (downtowns, neighbourhood centres round them, industry away from them), a village, or open country - a pure function of the seed and the cell.
- 176-247 `planWholeMap` (function)
