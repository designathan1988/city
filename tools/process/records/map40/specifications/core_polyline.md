# core:polyline

Grupo: core. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/polyline.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 10 linhas
- Alcance total: 46 unidades; usa 7 compartilhadas de outras; é dona principal de 36 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: `core:scalar` (1), `core:vec` (1)

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/geometry.spec.ts`, `tests/sim/crossingSpansSame.spec.ts`, `tests/sim/topologyOffPage.spec.ts`, `tests/ui/congestion.spec.ts`, `tests/world/roadFieldPrune.spec.ts`, `tests/world/turnOffSurface.spec.ts`, `tests/world/turnPathsEquivalence.spec.ts`, `tests/world/walkLogBound.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: não se aplica 2

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/polyline.ts
- 50-53 `Polyline.constructor` (method)
- 189-191 `Polyline.sampleNorm` (method): Frame at normalized position `f` in [0,1].
- 193-195 `Polyline.tangentAt` (method)
