# render:cell

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/cellInstances.ts`, `src/render/cellKeeps.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 2 arquivo(s), 15 linhas
- Alcance total: 111 unidades; usa 40 compartilhadas de outras; é dona principal de 68 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `customDepthMaterial`, `onBeforeCompile`, `onBeforeRender`
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/cellKeeps.spec.ts`, `tests/render/coverSweep.spec.ts`, `tests/render/depthNoColour.spec.ts`, `tests/render/elementCells.spec.ts`, `tests/render/gardenCull.spec.ts`, `tests/render/gardenFrame.spec.ts`, `tests/render/gardenPacks.spec.ts`, `tests/render/instanceRuns.spec.ts`, `tests/render/plantLevels.spec.ts`, `tests/render/shadowCasters.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/scale/census/derivedBound.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: já existia 6
- porte `street-furniture`: já existia 12

## Código próprio (linhas — declaração — comentário)

### src/render/cellInstances.ts
- 205-210 `posesOf` (function): A mesh the cells draw: its level's poses (one buffer for the level's meshes) and their format.
- 213-218 `drawnMatrices` (function): The matrices of the instances a mesh draws, as the GPU makes them from their poses (`matrixInto`, the same math): what a probe reads.

### src/render/cellKeeps.ts
- 39-41 `CellKeeps.size` (method): Cells holding places.
