# editor:snap

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/snap.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 21 linhas
- Alcance total: 180 unidades; usa 145 compartilhadas de outras; é dona principal de 31 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#mountInterface@1859` (2)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.heightOffset`, `RoadNode.id`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.id`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/dryRunReuse.spec.ts`, `tests/editor/editRules.spec.ts`, `tests/editor/snap.spec.ts`, `tests/editor/snapNear.spec.ts`, `tests/sim/roundabout.spec.ts`, `tests/world/incrementalRebuild.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: em parte 2, portado 36, não se aplica 4, não portado 10, já existia 2

## Código próprio (linhas — declaração — comentário)

### src/editor/snap.ts
- 45-45 `ROAD_SNAP_KEY` (variable)
- 47-55 `TryStatement` (statement)
- 61-63 `roadSnap` (function)
- 64-71 `setRoadSnap` (function)
