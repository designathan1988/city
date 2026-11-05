# core:region

Grupo: core. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/regionIndex.ts`
- Unidades próprias (só esta funcionalidade alcança): 25 em 1 arquivo(s), 322 linhas
- Alcance total: 63 unidades; usa 9 compartilhadas de outras; é dona principal de 29 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: `core:digest` (6), `core:vec` (1)

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/regionIndex.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: não se aplica 72
- porte `roads`: não se aplica 66

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/regionIndex.ts
- 15-15 `RegionIndex.hits` (field): Scratch for `run`: crossing parameters and winding steps.
- 17-34 `RegionIndex.constructor` (method)
- 65-72 `RegionIndex.fromMultiPoly` (method): From a clipper multipolygon (`[x, y]` pairs, holes after the outline).
- 97-108 `RegionIndex.digest` (method): An exact digest of every segment whose cell the box touches (a collision aside, at about one in 2^53).
- 111-118 `RegionIndex.boundaryDistance` (method): Distance from a point to the nearest boundary segment within `reach`, or Infinity.
- 121-160 `RegionIndex.run` (method): Along the line through `p` in direction `d`
- 163-166 `RegionIndex.span` (method): `run`, for a point that must be inside: false, leaving `out` alone, when it is not.
- 169-171 `RegionIndex.clusterCount` (method): How many dense grids the rings were filed in (one for one town).
- 184-184 `COARSE` (variable): The point lookup's coarse cell, in grid cells a side.
- 253-253 `Grid.count` (field)
- 262-262 `Grid.stamps` (field)
- 263-263 `Grid.stamp` (field)
- 266-295 `Grid.constructor` (method)
- 302-304 `Grid.bounds` (method): The area its cells cover.
- 314-316 `Grid.overlaps` (method): Whether a box reaches its cells.
- 319-325 `Grid.rasterise` (method): Every (cell, segment) pair a segment's supercover touches.
- 328-345 `Grid.cellsOfSegment` (method): Cells the segment (a, b) passes through, row by row.
- 348-396 `Grid.scanReference` (method): The winding number at every cell centre, one scanline per row.
- 431-453 `Grid.digestInto` (method): The segments whose cell the box touches, added to the digest's sums (`RegionIndex.digest`).
- 456-481 `Grid.nearest` (method): Distance from a point to its nearest segment in the cells within `reach`, or Infinity.
- 484-530 `Grid.crossings` (method): The crossings of the line through `p` along `d`, within `reach`, with its segments: appended to `hits` from `n`
- 534-534 `hashBits` (variable): One 32-bit half of a double's bits, for the segment hash below.
- 535-535 `hashWords` (variable)
- 536-539 `floatWord` (function)
- 542-553 `segmentHash` (function): A segment's own hash, for `digest`: the four numbers of its ends.
