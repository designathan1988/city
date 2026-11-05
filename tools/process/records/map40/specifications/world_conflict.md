# world:conflict

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/conflictPoints.ts`, `src/world/conflictTable.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 4 linhas
- Alcance total: 225 unidades; usa 94 compartilhadas de outras; é dona principal de 130 compartilhadas

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
- Usa: `world:lanelets` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/conflictPool.spec.ts`, `tests/sim/bulb.spec.ts`, `tests/sim/collisions.spec.ts`, `tests/sim/crossingSpansSame.spec.ts`, `tests/sim/routing/denseSearch.spec.ts`, `tests/sim/topologyDeferred.spec.ts`, `tests/sim/topologyOffPage.spec.ts`, `tests/sim/trapLanes.spec.ts`, `tests/world/conflictCache.spec.ts`, `tests/world/conflictWindow.spec.ts`, `tests/world/conflictZones.spec.ts`, `tests/world/laneletUpdate.spec.ts`, `tests/world/pairTableGrid.spec.ts`, `tests/world/turnOffSurface.spec.ts`, `tests/world/turnPathsEquivalence.spec.ts`, `tests/world/turns.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `traffic`: já existia 3
- porte `people`: já existia 1

## Código próprio (linhas — declaração — comentário)

### src/world/conflictPoints.ts
- 484-487 `ConflictIndex.conflict` (method): True when the two connectors have at least one conflict point.
