# world:tiles

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/tiles.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 22 linhas
- Alcance total: 20 unidades; usa 4 compartilhadas de outras; é dona principal de 13 compartilhadas

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
- Usa: `editor:barriers` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/buildings.spec.ts`, `tests/editor/freeRoad.spec.ts`, `tests/editor/poleTool.spec.ts`, `tests/editor/roadTop.spec.ts`, `tests/render/agentsResidency.spec.ts`, `tests/render/densityCars.spec.ts`, `tests/render/pageGarbage.spec.ts`, `tests/world/docTiles.spec.ts`, `tests/world/incrementalRebuild.spec.ts`, `tests/world/loadTileDirect.spec.ts`, `tests/world/migrateOnce.spec.ts`, `tests/world/roadEntries.spec.ts`, `tests/world/tileCaps.spec.ts`, `tests/world/tileGates.spec.ts`, `tests/world/tileIndex.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/world/tiles.ts
- 54-54 `EDGE_STEP` (variable): Steps an edge node's place is told in along its edge: a centimetre (an edge is 102,400 of them, under 2^17).
- 58-66 `edgeNodeId` (function): A node on a tile's edge: the same id from both tiles that lay it (each makes it alone, from the seed and the place, `cityGen/wholeMap.ts`) - a point on a tile edge belongs to the tile whose low edge it is (`tileIJ`); its id is told by whic
- 69-80 `tilesAround` (function): The tiles within `radius` tiles of the tile at (`i`, `j`), the nearest first (a square ring by ring).
