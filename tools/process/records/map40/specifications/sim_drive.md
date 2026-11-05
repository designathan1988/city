# sim:drive

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/drive/operational.ts`, `src/sim/drive/physicalMotion.ts`, `src/sim/drive/tactical.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 149 unidades; usa 132 compartilhadas de outras; é dona principal de 17 compartilhadas

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
- Lê: `LaneletRuntime.ghost`, `LaneletRuntime.order`, `SimWorld.graph`, `SimWorld.laneReaches`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/sim/drive/convoy.spec.ts`, `tests/sim/drive/tactical.spec.ts`, `tests/sim/routing/denseSearch.spec.ts`, `tests/sim/routing/searchBudget.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `traffic`: já existia 3

## Código próprio (linhas — declaração — comentário)
