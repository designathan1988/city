# render:mesh

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/mesh/cylinder.ts`, `src/render/mesh/detailLayer.ts`, `src/render/mesh/farBlockBatches.ts`, `src/render/mesh/gpuOnly.ts`, `src/render/mesh/hiddenFaces.ts`, `src/render/mesh/roadEdges.ts`, `src/render/mesh/roadLod.ts`, `src/render/mesh/roadLodPure.ts`, `src/render/mesh/roadResidency.ts`, `src/render/mesh/simplify.ts`, `src/render/mesh/stream.ts`, `src/render/mesh/surfaceMesh.ts`, `src/render/mesh/surfaceTiles.ts`, `src/render/mesh/textureBaker.ts`, `src/render/mesh/tileMesh.ts`, `src/render/mesh/tileShape.ts`, `src/render/mesh/updateRanges.ts`, `src/render/mesh/weld.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 2 arquivo(s), 24 linhas
- Alcance total: 263 unidades; usa 52 compartilhadas de outras; é dona principal de 206 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:dispose`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `onBeforeCompile`
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `earcut`, `meshoptimizer`, `three`, `three/addons/utils/BufferGeometryUtils.js`

## Testes do 4.0 que a exercitam
- `tests/perf/noHoles.spec.ts`, `tests/render/buildingShellMemory.spec.ts`, `tests/render/cylinderWith.spec.ts`, `tests/render/gpuOnly.spec.ts`, `tests/render/normalTexels.spec.ts`, `tests/render/roadLod.spec.ts`, `tests/render/roadPasses.spec.ts`, `tests/render/roadTileWorkers.spec.ts`, `tests/render/roadTiles.spec.ts`, `tests/render/stream.spec.ts`, `tests/render/surfaceBake.spec.ts`, `tests/render/surfaceInputsResidency.spec.ts`, `tests/render/surfaceReads.spec.ts`, `tests/render/tileAttributes.spec.ts`, `tests/render/tileShapeFlip.spec.ts`, `tests/render/updateRanges.spec.ts`, `tests/render/weld.spec.ts`, `tests/render/weldVertices.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: portado 7, não se aplica 2
- porte `roads`: não se aplica 150
- porte `build`: a portar 1
- porte `traffic`: não se aplica 1

## Código próprio (linhas — declaração — comentário)

### src/render/mesh/farBlockBatches.ts
- 56-59 `FarBlockBatches.constructor` (method)
- 101-104 `FarBlockBatches.dispose` (method)

### src/render/mesh/updateRanges.ts
- 4-4 `MAX_RANGES` (variable): Most update ranges one attribute keeps waiting for a draw; past it the whole array goes (a plate nobody draws is edited again and again).
- 7-10 `markAll` (function): The whole attribute marked changed, any ranges waiting dropped (three.js sends ranges only, when there are any).
- 13-23 `markBox` (function): DO NOT set `needsUpdate` alone for a corner edit: a 301 x 301 plate is resent whole.
