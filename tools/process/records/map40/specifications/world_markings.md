# world:markings

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/markings.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 13 linhas
- Alcance total: 347 unidades; usa 323 compartilhadas de outras; é dona principal de 22 compartilhadas

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
- Usa: `world:network` (5), `world:doc` (2), `world:roads` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.nodes`, `RoadNode.control`, `RoadNode.crossing`, `RoadNode.id`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/roundabout.spec.ts`, `tests/editor/streetObjects.spec.ts`, `tests/world/bulb.spec.ts`, `tests/world/forkApproach.spec.ts`, `tests/world/laneLines.spec.ts`, `tests/world/laneTurnRules.spec.ts`, `tests/world/roadPaint.spec.ts`, `tests/world/transition.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: em parte 60

## Código próprio (linhas — declaração — comentário)

### src/world/markings.ts
- 169-173 `approachLegends` (function): What a junction's rule paints on each leg before its stop line (MBST vol. IV): "PARE"
- 336-343 `junctionDetail` (function): Collects stop bars and crosswalks for every junction approach
