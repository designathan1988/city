# world:terrain

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/terrain.ts`, `src/world/terrainPaint.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 31 linhas
- Alcance total: 64 unidades; usa 5 compartilhadas de outras; é dona principal de 57 compartilhadas

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
- `tests/editor/buildPreview.spec.ts`, `tests/editor/persistence.spec.ts`, `tests/render/ecologyKept.spec.ts`, `tests/render/farWoods.spec.ts`, `tests/render/flatAtlas.spec.ts`, `tests/render/landStroke.spec.ts`, `tests/render/pedSlope.spec.ts`, `tests/render/plateNoWater.spec.ts`, `tests/render/platePaint.spec.ts`, `tests/render/plateWoods.spec.ts`, `tests/render/sharedGround.spec.ts`, `tests/render/shoreTextures.spec.ts`, `tests/render/vehicleFrame.spec.ts`, `tests/ui/i18nKeys.spec.ts`, `tests/world/cutWalls.spec.ts`, `tests/world/defaultTown.spec.ts`, `tests/world/diaryPlaces.spec.ts`, `tests/world/ecologyRegion.spec.ts`, `tests/world/elevationLocal.spec.ts`, `tests/world/elevationScope.spec.ts`, `tests/world/incrementalRebuild.spec.ts`, `tests/world/terrain.spec.ts`, `tests/world/tileCaps.spec.ts`, `tests/world/verticalCurves.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `interface`: outra sessão 4
- porte `landscape`: portado 109

## Código próprio (linhas — declaração — comentário)

### src/world/terrain.ts
- 165-165 `TerrainIndex.revision` (field): The revision the caller built it at (`RoadDoc.terrainRevision`), kept for it; the index itself never reads it.
- 173-202 `TerrainIndex.constructor` (method): `before`: an index of the first of these stamps on the same land (a dab appended): its cells shared, only the ones the new stamps cover filed again - DO NOT file the map's every stamp again for a dab of the brush.
