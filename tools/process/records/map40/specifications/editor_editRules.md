# editor:editRules

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/editRules.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 15 linhas
- Alcance total: 1048 unidades; usa 1016 compartilhadas de outras; é dona principal de 31 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#mutateRoads@842` (3)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:network` (8), `world:doc` (6), `editor:roads-economy` (2), `world:economy` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.heightOffset`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.curve`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/economy.spec.ts`, `tests/editor/editRules.spec.ts`, `tests/scale/roadJudge.spec.ts`, `tests/world/incrementalRebuild.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `economy`: portado 2
- porte `roads`: em parte 62

## Código próprio (linhas — declaração — comentário)

### src/editor/editRules.ts
- 81-95 `guardRoadEdit` (function): DO NOT copy the map to judge an edit against (`doc.clone()` and the whole network, every in-place edit)
