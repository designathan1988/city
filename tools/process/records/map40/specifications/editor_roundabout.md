# editor:roundabout

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/roundabout.ts`
- Unidades próprias (só esta funcionalidade alcança): 6 em 1 arquivo(s), 71 linhas
- Alcance total: 901 unidades; usa 894 compartilhadas de outras; é dona principal de 1 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#pointerDown@1217` (6), `src/main.ts#drawOverlayScreen@2942` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (25), `worker:conflictTable` (13), `world:network` (8), `world:ids` (5), `world:road` (5), `editor:barriers` (4), `world:cityGen` (2), `world:ribbons` (1), `world:geometry` (1), `editor:commit` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.id`, `RoadSegment.direction`, `RoadSegment.id`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/editRules.spec.ts`, `tests/editor/roundabout.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não portado 14

## Código próprio (linhas — declaração — comentário)

### src/editor/roundabout.ts
- 10-10 `ROUNDABOUT_MIN_RADIUS` (variable)
- 11-11 `ROUNDABOUT_MAX_RADIUS` (variable)
- 12-12 `ROUNDABOUT_APPROACH_LENGTH` (variable)
- 14-16 `RoundaboutResult` (type)
- 19-36 `roundaboutRefusal` (function): Why a roundabout of `radius` round `centre` cannot be placed, or null when it can: what the click would answer, asked before it (the preview).
- 39-85 `commitRoundabout` (function): Places a single-lane roundabout on vacant ground.
