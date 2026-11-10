# render:bake

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/bake/catalogue.ts`, `src/render/bake/detailRecipes.ts`, `src/render/bake/finishRecipes.ts`, `src/render/bake/roadRecipes.ts`, `src/render/bake/terrainRecipes.ts`, `src/render/bakePool.ts`
- Unidades próprias (só esta funcionalidade alcança): 34 em 4 arquivo(s), 814 linhas
- Alcance total: 136 unidades; usa 62 compartilhadas de outras; é dona principal de 40 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:mesh` (156), `editor:editStream` (28), `editor:barriers` (2), `world:buildings` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/finishFar.spec.ts`, `tests/render/shellFinishes.spec.ts`, `tests/render/surfaceBake.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: portado 58
- porte `roads`: não se aplica 40
- porte `people`: não se aplica 26
- porte `traffic`: não se aplica 1

## Código próprio (linhas — declaração — comentário)

### src/render/bake/catalogue.ts
- 11-11 `TextureUse` (type): How the texels are read (`scripts/vite/ktx2.ts` `TextureUse`).
- 13-22 `BakedTexture` (interface)
- 25-31 `surface` (function): A surface's three maps, or its colour alone.
- 34-43 `tiledTexels` (function): A square of texels repeated `tiles` times each way: its mip chain is its own chain repeated, level by level (a 2 x 2 box never straddles two copies).
- 46-59 `finishArrays` (function): The finishes as one texture array per map, a layer per finish in `FINISHES` order (`buildings/finishes.ts`): a cell's shells in one draw.
- 62-92 `bakeCatalogue` (function): One texture at a time, so the build holds one recipe's texels at once (the finishes' arrays: their layers).
- 95-102 `srgbEncoded` (function): Linear bytes (data) as sRGB-encoded ones: kept in a colour array, they read back as themselves (IEC 61966-2-1).
- 105-124 `resized` (function): Texels of a `from`-sided square at `to` texels a side, bilinear between texel centres, wrapping as the texture repeats (unchanged when equal).

### src/render/bake/detailRecipes.ts
- 10-10 `SIZE` (variable): Texels across a detail texture.
- 21-51 `stamp` (function): Adds a soft round mark into a wrapped float field.
- 54-88 `blade` (function): Draws a thin tapered stroke - one grass blade seen from above.
- 91-238 `bake` (function): Bakes one detail family.
- 241-261 `detailTexels` (function): One detail family's texels, rows top first as a canvas holds them: its albedo (linear data around 0.5, a multiplier of 2) and its normal map.

### src/render/bake/roadRecipes.ts
- 7-7 `ASPHALT_SIZE` (variable)
- 8-8 `ASPHALT_BASE` (variable)
- 9-9 `RAISED_ASPHALT_BASE` (variable)
- 16-21 `cellHash` (function): A small integer hash, for per-slab variation that is stable and tiles.
- 24-73 `asphaltRecipe` (function): The asphalt; `raised` its paler tone for raised crossings, whose colour alone is baked (it shares the road's normal and roughness).
- 76-123 `footwayRecipe` (function): The footway: interlocking concrete pavers in basketweave (it was large precast slabs; the player found them ugly).
- 126-156 `kerbRecipe` (function): The kerb: precast concrete, smooth, a shade lighter and cooler than the footway so the kerb line reads as its own edge
- 158-180 `vergeRecipe` (function)
- 182-205 `deckRecipe` (function)
- 208-217 `roadRecipes` (function): Every street surface by its key; `asphalt-raised` bakes its colour only (`mapOnly`): it shares the road's normal and roughness.

### src/render/bake/terrainRecipes.ts
- 7-11 `latticeHash` (function): 0..1 hash of an integer lattice point.
- 14-25 `makeNoiseXY` (function): Value noise tiling with its own period on each axis: a streak long down a face and narrow across it, still seamless
- 28-28 `cell` (variable): The nearest and second-nearest jittered points of an n x n tiling grid.
- 29-45 `cellular` (function)
- 47-50 `smoothstep` (function)
- 53-57 `tintTowards` (function): Mixes `out`'s colour towards an sRGB colour by `w`.
- 60-60 `ROCK_SIZE` (variable): Texels across a rock layer; every layer of the array is this size.
- 65-217 `ROCK_RECIPES` (variable): The rocks, two layers each - its FACE, read by the side projections of the triplanar mapping
- 220-225 `streakNoise` (function): Tiling noise stretched along v: the mean of samples stacked up the column
- 229-234 `GRASS_TONES` (variable): The dark tone close to the lit one
- 237-308 `terrainRecipes` (function): The land's surfaces: the grass, the rock layers (in `ROCK_RECIPES` order: one texture array) and the dirt.
