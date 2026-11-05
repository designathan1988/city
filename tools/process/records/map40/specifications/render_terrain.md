# render:terrain

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/terrain.ts`, `src/render/terrainGrid.ts`, `src/render/terrainLevels.ts`, `src/render/terrainLightCompute.ts`, `src/render/terrainLightPool.ts`, `src/render/terrainLod.ts`, `src/render/terrainShaping.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 17 linhas
- Alcance total: 553 unidades; usa 389 compartilhadas de outras; é dona principal de 162 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
- Workers: -
- Ganchos do three.js: `onBeforeCompile`
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`, `RoadDoc.nature`, `RoadDoc.terrainPaint`, `RoadDoc.terrainRelief`, `RoadDoc.terrainStamps`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/coverSweep.spec.ts`, `tests/render/ecologyKept.spec.ts`, `tests/render/farWoods.spec.ts`, `tests/render/flatAtlas.spec.ts`, `tests/render/landStroke.spec.ts`, `tests/render/paintGrow.spec.ts`, `tests/render/plateNoWater.spec.ts`, `tests/render/plateWoods.spec.ts`, `tests/render/sharedGround.spec.ts`, `tests/render/shoreTextures.spec.ts`, `tests/render/terrainLod.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/render/terrainSlope.spec.ts`, `tests/render/terrainUnits.spec.ts`, `tests/render/water.spec.ts`, `tests/world/steppedYards.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: portado em parte 154, já existia 5, não se aplica 118

## Código próprio (linhas — declaração — comentário)

### src/render/terrainLevels.ts
- 28-40 `LandLevels.constructor` (method)
- 62-65 `LandLevels.markCorner` (method): The nodes holding corner `i` marked to be measured again.
