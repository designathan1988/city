# sim:vehicle

Grupo: sim. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/vehicleSignals.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 86 unidades; usa 64 compartilhadas de outras; é dona principal de 21 compartilhadas

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
- Usa: `sim:world` (1), `sim:vehicles` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `Kinematics.lanelet`, `Kinematics.lateralSlope`, `Kinematics.s`, `Vehicle.desiredLane`, `Vehicle.laneIntent`, `Vehicle.lanelet`, `Vehicle.lateral`, `Vehicle.lateralSlope`, `Vehicle.prev`, `Vehicle.route`, `Vehicle.s`, `Vehicle.v`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/render/vehicleSignals.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/sim/vehicleSignals.ts
- 80-82 `steerAngle` (function): Front-wheel steering angle, radians, positive to the left.
