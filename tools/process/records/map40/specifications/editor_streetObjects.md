# editor:streetObjects

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/streetObjects.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 1 arquivo(s), 90 linhas
- Alcance total: 1183 unidades; usa 1178 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#showInspector@3573` (10)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (48), `world:network` (18), `world:ids` (5), `world:approach` (5), `world:lanelets` (5), `world:road` (4), `editor:buildingSnap` (4), `editor:barriers` (4), `world:section` (2), `world:geometry` (2), `editor:commit` (2), `world:landscape` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.incident`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/streetObjects.spec.ts`, `tests/editor/uturn.spec.ts`, `tests/render/signalBatches.spec.ts`, `tests/sim/uturn.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 10
- porte `street-furniture`: não se aplica 12
- porte `traffic`: de outra funcionalidade 2

## Código próprio (linhas — declaração — comentário)

### src/editor/streetObjects.ts
- 11-11 `PedestrianCrossingKind` (type)
- 12-14 `PedestrianCrossingResult` (type)
- 17-65 `commitPedestrianCrossing` (function): Inserts one crossing whose centre is at the selected arc distance (midpoint by default).
- 67-69 `UturnResult` (type)
- 72-105 `commitUturn` (function): Puts a U-turn through the median at the selected arc distance (the middle by default, retorno)
