# editor:buildings

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/buildings.ts`
- Unidades próprias (só esta funcionalidade alcança): 7 em 1 arquivo(s), 79 linhas
- Alcance total: 435 unidades; usa 373 compartilhadas de outras; é dona principal de 55 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/buildingsWiring.ts#createBuildingWiring@81` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:buildings` (105), `world:doc` (8), `world:changes` (2), `editor:barriers` (2), `world:network` (2), `worker:conflictTable` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `polygon-clipping`

## Testes do 4.0 que a exercitam
- `tests/editor/builderMove.spec.ts`, `tests/editor/buildingCreator.spec.ts`, `tests/editor/buildingFloors.spec.ts`, `tests/editor/buildingProfile.spec.ts`, `tests/editor/buildings.spec.ts`, `tests/editor/elementHover.spec.ts`, `tests/editor/facadeZone.spec.ts`, `tests/editor/pickAlongRay.spec.ts`, `tests/editor/snapNear.spec.ts`, `tests/editor/stairRun.spec.ts`, `tests/editor/weld.spec.ts`, `tests/world/buildingPlans.spec.ts`, `tests/world/buildingStoreOwnership.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interior`: outro porte 90, já existia 24

## Código próprio (linhas — declaração — comentário)

### src/editor/buildings.ts
- 485-497 `placeBuilding` (function): Places a new building. The record is validated before it is stored.
- 576-594 `clearBuildingsOnRoads` (function): DO NOT walk `buildings.all()` for an edit
- 597-601 `clearBuildingsAfterEdit` (function): DO NOT look since the last edit: the roads a tile brought in between two edits would demolish or rewrite the buildings they meet.
- 604-610 `everyBuilding` (function): @rebuild What changed is not known (a map loaded): every building, once, one record read at a time: DO NOT copy every record into an array (the whole map held decoded while it is walked, `tableRows.ts`). Its ids taken first: a record writte
- 613-620 `buildingsIn` (function): The buildings whose box meets one of `rects`, each once, in the store's order per rectangle.
- 773-777 `lookOf` (function): What a mass looks like, resolved: its walls, each side's, and its roof.
- 779-800 `opUnionVolumes` (function)
