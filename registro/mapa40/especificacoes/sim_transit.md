# sim:transit

Grupo: sim. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/transit/transit.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 4 linhas
- Alcance total: 693 unidades; usa 636 compartilhadas de outras; é dona principal de 56 compartilhadas

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
- Lê: `?.at`, `?.lanelet`, `?.s`, `?.vehicle`, `RoadDoc.transit`, `SimStreams.driver`, `SimWorld.clock`, `SimWorld.nextVehicleId`, `SimWorld.vehicles`, `Vehicle.archetype`, `Vehicle.commute`, `Vehicle.destination`, `Vehicle.doors`, `Vehicle.id`, `Vehicle.kerbStop`, `Vehicle.lanelet`, `Vehicle.s`, `Vehicle.v`
- Escreve: `Vehicle.commute`, `Vehicle.destination`, `Vehicle.errand`, `Vehicle.prev`, `Vehicle.s`, `Vehicle.seats`, `Vehicle.v`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/sim/indexes/levelCrossings.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `demolition`: não se aplica 2
- porte `people`: já existia 8
- porte `traffic`: de outra funcionalidade 2
- porte `transit`: portado 106, não se aplica 14

## Código próprio (linhas — declaração — comentário)

### src/sim/transit/transit.ts
- 473-476 `TransitSim.crossingNear` (method): True when a level crossing lies within `reach` of `at` along a lane (open or closed).
