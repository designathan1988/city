# editor:preparedCity

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/preparedCity.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 1 arquivo(s), 22 linhas
- Alcance total: 258 unidades; usa 245 compartilhadas de outras; é dona principal de 8 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#openCityNow@2175` (6), `src/main.ts#restoreBootSave@248` (4), `src/main.ts#cityData@2056` (3), `src/main.ts#openedOf@2079` (2), `src/main.ts#KeptCity@2053` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (5), `world:buildings` (5)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `SerializedDoc.buildings`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/binarySave.spec.ts`, `tests/editor/preparedCity.spec.ts`, `tests/perf/fillEdit.spec.ts`, `tests/perf/fillNearView.spec.ts`, `tests/perf/geometryBudget.spec.ts`, `tests/perf/largeOpening.spec.ts`, `tests/perf/noHoles.spec.ts`, `tests/perf/trafficBuildingsBench.spec.ts`, `tests/scale/cityGeneration.spec.ts`, `tests/scale/render/cityMemory.spec.ts`, `tests/scale/render/layerMemory.spec.ts`, `tests/world/docBinary.spec.ts`, `tests/world/docMemory.spec.ts`, `tests/world/openingOffPage.spec.ts`, `tests/world/pageWorldMemory.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/editor/preparedCity.ts
- 19-19 `PREPARE_BATCH` (variable): 2026-10-22). Written as a generated city's are (`PreparedCityWriter`, the members shared a batch at a time): the same table.
- 21-31 `prepareCity` (function)
- 71-74 `OpenedCity` (interface): A city as the page opens it; `table`: its buildings' table (`prepareCity`), the document's source of truth as it is (`BuildingStore.loadSections`), null: its buildings are in `data`.
- 77-79 `preparedCity` (function): A prepared city as the page opens it: DO NOT decode its table here (every record an object, ~8 KB of heap a building): the document reads a row when asked.
- 82-84 `openedBuildings` (function): How many buildings an opened city has, in its data or its table.
