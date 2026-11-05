# editor:commit

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/commit.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 131 linhas
- Alcance total: 1216 unidades; usa 1152 compartilhadas de outras; é dona principal de 60 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#showInspector@3573` (1), `src/main.ts#duplicateSelectedSegment@3821` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (120), `world:road` (10), `world:network` (8), `editor:snap` (8), `editor:barriers` (4), `world:economy` (3), `world:ids` (3), `world:parking` (3), `editor:roads-economy` (2), `world:structures` (1), `editor:editRules` (1), `world:ribbons` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`, `RoadDoc.terrainRelief`, `RoadDoc.terrainStamps`, `RoadNode.blockedMovements`, `RoadNode.crossing`, `RoadNode.heightOffset`, `RoadNode.id`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.curve`, `RoadSegment.cutWalls`, `RoadSegment.dashOrigin`, `RoadSegment.direction`, `RoadSegment.id`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/buildPreview.spec.ts`, `tests/editor/buildings.spec.ts`, `tests/editor/crossStraight.spec.ts`, `tests/editor/crossingPairs.spec.ts`, `tests/editor/dryRunReuse.spec.ts`, `tests/editor/economy.spec.ts`, `tests/editor/editRules.spec.ts`, `tests/editor/editing.spec.ts`, `tests/editor/freeRoad.spec.ts`, `tests/editor/lotsCutByRoad.spec.ts`, `tests/editor/moveNode.spec.ts`, `tests/editor/snap.spec.ts`, `tests/editor/streetObjects.spec.ts`, `tests/editor/turnBans.spec.ts`, `tests/sim/laneletGraphCache.spec.ts`, `tests/sim/roundabout.spec.ts`, `tests/world/docFork.spec.ts`, `tests/world/incrementalRebuild.spec.ts`, `tests/world/lotCorners.spec.ts`, `tests/world/roadSection.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `economy`: portado 2
- porte `roads`: em parte 90
- porte `traffic`: de outra funcionalidade 2

## Código próprio (linhas — declaração — comentário)

### src/editor/commit.ts
- 32-32 `MIN_DRAFT_LENGTH` (variable): Shortest road the editor will create.
- 322-367 `commitDraft` (function): Commits a drafted road atomically, splitting anything it crosses.
- 768-814 `joinSegments` (function): Joins two compatible straight segments meeting at an otherwise unused node.
- 817-853 `duplicateSegment` (function): Creates a disconnected parallel copy of a road for rapid layout iteration.
