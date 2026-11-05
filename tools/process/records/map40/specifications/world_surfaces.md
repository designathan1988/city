# world:surfaces

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/surfaces.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 32 linhas
- Alcance total: 101 unidades; usa 89 compartilhadas de outras; é dona principal de 8 compartilhadas

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
- Usa: `render:mesh` (8), `world:road` (8), `render:network` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/render/tessellation.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/world/bulb.spec.ts`, `tests/world/connectorFootprints.spec.ts`, `tests/world/surfaces.spec.ts`, `tests/world/transition.spec.ts`, `tests/world/union.spec.ts`, `tests/world/walkways.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não portado 24
- porte `street-furniture`: não se aplica 4

## Código próprio (linhas — declaração — comentário)

### src/world/surfaces.ts
- 35-40 `Surfaces` (interface): Every surface of the network, one entry per level, outermost first.
- 43-52 `Bands` (interface): Bands, each the part of one level not covered by the level inside it.
- 133-140 `surfaces` (function)
- 142-149 `bands` (function)
