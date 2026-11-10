# core:clipper

Grupo: core. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/clipper.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 15 linhas
- Alcance total: 23 unidades; usa 2 compartilhadas de outras; é dona principal de 20 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `clipper2-ts`, `polygon-clipping`

## Testes do 4.0 que a exercitam
- `tests/core/axisCut.spec.ts`, `tests/core/clipperDegenerate.spec.ts`, `tests/core/geometry.spec.ts`, `tests/core/polygonBoolean.spec.ts`, `tests/render/surfaceMesh.spec.ts`, `tests/render/tessellation.spec.ts`, `tests/render/tileShapeFlip.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/world/bulb.spec.ts`, `tests/world/connectorFootprints.spec.ts`, `tests/world/surfaces.spec.ts`, `tests/world/union.spec.ts`, `tests/world/walkways.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 2

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/clipper.ts
- 174-188 `area` (function): Signed area of the set, with explicit holes subtracted.
