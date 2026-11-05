# sim:crossings

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/crossings/permission.ts`, `src/sim/crossings/state.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 363 unidades; usa 346 compartilhadas de outras; é dona principal de 17 compartilhadas

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
- Lê: `?.vehicle`, `ConnectorClearance.connector`, `CrossingOccupant.forward`, `CrossingOccupant.held`, `CrossingOccupant.id`, `CrossingOccupant.s`, `CrossingOccupant.v`, `CrossingState.demand`, `CrossingState.length`, `CrossingState.longestWait`, `CrossingState.occupants`, `CrossingState.waitingFrom`, `CrossingState.waitingTo`, `RoadNode.incident`, `SimWorld.clock`, `SimWorld.crossingSpans`, `SimWorld.graph`, `Vehicle.admittedConnector`, `Vehicle.archetype`, `Vehicle.clearingConnectors`, `Vehicle.driver`, `Vehicle.heldByPedestriansTick`, `Vehicle.lanelet`, `Vehicle.route`, `Vehicle.s`, `Vehicle.v`, `Vehicle.waited`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/sim/collisions.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: não se aplica 2
- porte `people`: já existia 2
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)
