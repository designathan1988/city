# editor:roadTop

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/roadTop.ts`
- Unidades próprias (só esta funcionalidade alcança): 13 em 1 arquivo(s), 127 linhas
- Alcance total: 131 unidades; usa 118 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#roadTop@997` (1), `src/main.ts#surfaceTop@998` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (16), `world:cells` (11), `world:changes` (5), `world:derived` (5), `world:network` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`, `RoadDoc.nodes`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/roadTop.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 28

## Código próprio (linhas — declaração — comentário)

### src/editor/roadTop.ts
- 8-8 `KINDS` (variable): DO NOT walk `doc.nodes` on a road edit for the top: every street drawn read every node of the city again
- 10-14 `Source` (interface)
- 16-73 `RoadTop` (class)
- 17-17 `RoadTop.source` (field)
- 19-19 `RoadTop.byHeight` (field): How many cells have their top at each whole height (`Math.ceil`).
- 20-20 `RoadTop.top` (field)
- 21-21 `RoadTop.cells` (field)
- 24-43 `RoadTop.constructor` (method): `heightAt`: the solved road height at a point (`SceneHandle.elevationAt`).
- 46-53 `RoadTop.height` (method): The highest road node's height (rounded up to a whole unit); -Infinity with no roads.
- 56-58 `RoadTop.lastComputes` (method): What the last `height` computed: cells (for the tests).
- 61-72 `RoadTop.cellTop` (method): The highest node standing in cell `key`: the nodes of the roads that meet it, only those inside it.
- 76-80 `allCells` (function): @rebuild The diary cannot say what moved (first look, a map loaded, a whole solve): every node's cell.
- 83-93 `dirtyCells` (function): The cells a change's rectangles reach; a change of the whole map, every node's cell.
