# sim:invariants

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/invariants.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 386 unidades; usa 368 compartilhadas de outras; é dona principal de 18 compartilhadas

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
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.nodes`, `?.serial`, `ConnectorClearance.connector`, `CrossingOccupant.id`, `CrossingState.occupants`, `Kinematics.lanelet`, `Kinematics.s`, `LaneletRuntime.order`, `SimWorld.claims`, `SimWorld.clock`, `SimWorld.controllers`, `SimWorld.crossingSpans`, `SimWorld.crossingStates`, `SimWorld.graph`, `SimWorld.lastAdmission`, `SimWorld.pedEngine`, `SimWorld.runtime`, `SimWorld.vehicles`, `Vehicle.admittedConnector`, `Vehicle.archetype`, `Vehicle.claims`, `Vehicle.clearingConnectors`, `Vehicle.constraints`, `Vehicle.firstRequestTick`, `Vehicle.greenDenied`, `Vehicle.greenStall`, `Vehicle.id`, `Vehicle.lanelet`, `Vehicle.lastMovedTick`, `Vehicle.prev`, `Vehicle.reservedConnectors`, `Vehicle.route`, `Vehicle.s`, `Vehicle.v`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/sim/indexes/audit.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `people`: já existia 1
- porte `traffic`: já existia 1

## Código próprio (linhas — declaração — comentário)
