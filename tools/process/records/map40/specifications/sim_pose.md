# sim:pose

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/pose.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 2 arquivo(s), 11 linhas
- Alcance total: 99 unidades; usa 74 compartilhadas de outras; é dona principal de 23 compartilhadas

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
- Usa: `sim:people` (11), `worker:conflictTable` (9), `editor:buildings` (1), `editor:commit` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `FreePose.angle`, `FreePose.pangle`, `FreePose.px`, `FreePose.py`, `FreePose.x`, `FreePose.y`, `Kinematics.lanelet`, `Kinematics.lateral`, `Kinematics.lateralSlope`, `Kinematics.rearPath`, `Kinematics.s`, `Vehicle.admittedConnector`, `Vehicle.archetype`, `Vehicle.free`, `Vehicle.lanelet`, `Vehicle.lateralSlope`, `Vehicle.prev`, `Vehicle.s`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/geometry.spec.ts`, `tests/core/isSimple.spec.ts`, `tests/core/regionIndex.spec.ts`, `tests/editor/poleCrossing.spec.ts`, `tests/editor/snapNear.spec.ts`, `tests/render/cheapAudit.spec.ts`, `tests/scale/lotEdit.spec.ts`, `tests/sim/collisions.spec.ts`, `tests/sim/fleetGrid.spec.ts`, `tests/sim/indexes/levelCrossings.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/sim/microWindowCells.spec.ts`, `tests/sim/pose.spec.ts`, `tests/sim/transitLoop.spec.ts`, `tests/sim/vehiclesNear.spec.ts`, `tests/ui/congestion.spec.ts`, `tests/world/connectorFootprints.spec.ts`, `tests/world/footprintsOverlap.spec.ts`, `tests/world/padRingDistance.spec.ts`, `tests/world/poleLinesCells.spec.ts`, `tests/world/transitNear.spec.ts`, `tests/world/turnOffSurface.spec.ts`, `tests/world/turns.spec.ts`, `tests/world/union.spec.ts`, `tests/world/zoneGridOverlap.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: já existia 3
- porte `people`: já existia 4
- porte `traffic`: portado 2
- porte `roads`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/core/vec2.ts
- 30-33 `lerpVec` (function)

### src/sim/pose.ts
- 208-214 `pedPose` (function)
