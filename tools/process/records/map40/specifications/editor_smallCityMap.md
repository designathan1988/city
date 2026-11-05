# editor:smallCityMap

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/smallCityMap.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 9 linhas
- Alcance total: 2117 unidades; usa 2107 compartilhadas de outras; é dona principal de 7 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#cityData@2056` (2), `src/main.ts#openCity@2213` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (4), `world:cityGen` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/census/worldCensus.spec.ts`, `tests/perf/fillEdit.spec.ts`, `tests/perf/fillNearView.spec.ts`, `tests/perf/geometryBudget.spec.ts`, `tests/perf/largeOpening.spec.ts`, `tests/perf/noHoles.spec.ts`, `tests/scale/census/derivedBound.spec.ts`, `tests/scale/cityGeneration.spec.ts`, `tests/scale/world/cityGoldenLarge.spec.ts`, `tests/world/cityGolden.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: já existia 6
- porte `people`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/editor/smallCityMap.ts
- 21-23 `buildSmallCityMap` (function): THE SMALL CITY OF ROADCRAFT 2.0, WHOLE, AS ONE MAP (the player, 2026-10-18: no city by tiles). Planned as 2.0 planned it (`smallCity.ts`: straight blocks, medium size), then laid out on one document: its footways, lots, green and zones, and
- 26-28 `buildLargeCityMap` (function): THE LARGE CITY, WHOLE, AS ONE MAP: the small city's plan over a larger radius (`largeCity.ts`), laid out the same way.
- 31-33 `buildCityMap` (function): A planned city laid out as one map: its footways, lots, green and zones, and every lot grown, on the flat ground of a new map. The size is the plan's (`cityGen/plan.ts`): the probes' larger cities test the renderer's scale.
