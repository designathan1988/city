# world:bounds

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/bounds.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 10 linhas
- Alcance total: 27 unidades; usa 8 compartilhadas de outras; é dona principal de 16 compartilhadas

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
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/growthWorker.spec.ts`, `tests/editor/persistence.spec.ts`, `tests/render/coverSweep.spec.ts`, `tests/render/farGround.spec.ts`, `tests/render/farWoods.spec.ts`, `tests/render/flatAtlas.spec.ts`, `tests/render/groundShapeRegions.spec.ts`, `tests/render/lotOverlay.spec.ts`, `tests/render/platePrograms.spec.ts`, `tests/render/plateProgramsFail.spec.ts`, `tests/render/sharedGround.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/world/buildings.spec.ts`, `tests/world/unboundedPlates.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: a portar 2

## Código próprio (linhas — declaração — comentário)

### src/world/bounds.ts
- 34-34 `plateIJ` (function): The plate a point lies in.
- 60-60 `regionAt` (function): The plate a point lies in, as its square.
- 71-78 `platesAround` (function): The plates within `ring` plates of plate (pi, pj) each way, the nearest ring first: a square of (2 ring + 1)^2.
