# world:roads

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/roads/buildMode.ts`, `src/world/roads/catalog.ts`, `src/world/roads/connectors.ts`, `src/world/roads/derivedSigns.ts`, `src/world/roads/footwayRise.ts`, `src/world/roads/furnitureCatalog.ts`, `src/world/roads/furnitureSets.ts`, `src/world/roads/laneEnds.ts`, `src/world/roads/markingStyle.ts`, `src/world/roads/profile.ts`, `src/world/roads/rules.ts`, `src/world/roads/streetChain.ts`, `src/world/roads/streetNames.ts`, `src/world/roads/templates.ts`, `src/world/roads/tuning.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 2 arquivo(s), 26 linhas
- Alcance total: 533 unidades; usa 409 compartilhadas de outras; é dona principal de 121 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#buildSummary@3478` (6), `src/main.ts#showInspector@3573` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.markingStyle`, `RoadNode.approachRules`, `RoadNode.control`, `RoadNode.id`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/buildPreview.spec.ts`, `tests/editor/economy.spec.ts`, `tests/editor/growthWorker.spec.ts`, `tests/editor/signRule.spec.ts`, `tests/editor/streetKit.spec.ts`, `tests/render/cellPaving.spec.ts`, `tests/render/paintCatalogue.spec.ts`, `tests/render/pierOnRoad.spec.ts`, `tests/sim/advisorReach.spec.ts`, `tests/sim/junctionRules.spec.ts`, `tests/sim/laneConnectors.spec.ts`, `tests/ui/connectorPanel.spec.ts`, `tests/ui/roadCatalogPanel.spec.ts`, `tests/ui/roadProfileEditor.spec.ts`, `tests/ui/roadTemplates.spec.ts`, `tests/world/cutWalls.spec.ts`, `tests/world/economy.spec.ts`, `tests/world/roadCatalog.spec.ts`, `tests/world/roadFurniture.spec.ts`, `tests/world/roadPaint.spec.ts`, `tests/world/roadProfile.spec.ts`, `tests/world/roadTuning.spec.ts`, `tests/world/streetChain.spec.ts`, `tests/world/streetNames.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `economy`: não se aplica 2
- porte `roads`: não se aplica 33, portado 12, não portado 86, em parte 34
- porte `interface`: outra sessão 25
- porte `interior`: outro porte 6
- porte `traffic`: já existia 3, portado 1
- porte `street-furniture`: parcial 27, portado 1
- porte `people`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/world/roads/buildMode.ts
- 37-41 `BuildRun` (interface): A stretch built one way.
- 110-124 `buildRuns` (function): The stations as stretches of one way of building, in order.

### src/world/roads/streetChain.ts
- 63-68 `flipProfile` (function): A profile seen from the other end: its elements in the other order, each lane's direction swapped.
