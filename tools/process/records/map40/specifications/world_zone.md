# world:zone

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/zoneGrid.ts`
- Unidades próprias (só esta funcionalidade alcança): 12 em 1 arquivo(s), 178 linhas
- Alcance total: 204 unidades; usa 186 compartilhadas de outras; é dona principal de 6 compartilhadas

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
- Usa: `worker:conflictTable` (76), `editor:barriers` (5), `world:doc` (5), `world:ids` (4), `world:network` (3), `world:ribbons` (3), `world:road` (3), `world:grid` (1), `world:pedestrian` (1), `editor:buildingSnap` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.segments`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/zoning.spec.ts`, `tests/world/lotCorners.spec.ts`, `tests/world/lotRaster.spec.ts`, `tests/world/lots.spec.ts`, `tests/world/zoneGridOverlap.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `street-furniture`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/world/zoneGrid.ts
- 15-15 `ZONE_CELL` (variable): The zoning grid: the land along every street, cut into cells the player zones and buildings grow on.
- 16-16 `ZONE_DEPTH` (variable)
- 18-18 `SUB_CELL` (variable): The subgrid a cell is trimmed on where a whole cell does not fit (1 m).
- 20-37 `ZoneCell` (interface)
- 39-48 `ZoneGrid` (interface)
- 50-51 `key` (function)
- 54-54 `BUCKET` (variable): Spatial hash bucket edge: one cell.
- 55-55 `bucketKey` (function)
- 57-64 `insideQuad` (function)
- 67-67 `OVERLAP_SLACK` (variable): How far two cells may run into each other and still count as apart: rounding, and cells that only share an edge.
- 178-183 `buildZoneGrid` (function): Builds the grid of every ground street that carries pedestrians.
- 186-313 `zoneGridSteps` (function): `buildZoneGrid` a street at a time: it yields after each street, so the grid can be laid over several frames
