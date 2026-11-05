# render:surface

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/surfaceBakeClient.ts`, `src/render/surfaceTextures.ts`
- Unidades próprias (só esta funcionalidade alcança): 16 em 2 arquivo(s), 111 linhas
- Alcance total: 38 unidades; usa 8 compartilhadas de outras; é dona principal de 14 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#surfaceBake@223` (1), `src/main.ts#ExpressionStatement@657` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:buildings` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): `…`
- Bibliotecas externas: `three`, `three/addons/loaders/KTX2Loader.js`, `three/examples/jsm/libs/ktx-parse.module.js`, `three/examples/jsm/libs/zstddec.module.js`

## Testes do 4.0 que a exercitam
- `tests/render/plateNoWater.spec.ts`, `tests/render/plateWoods.spec.ts`, `tests/render/shellFinishes.spec.ts`, `tests/render/shoreTextures.spec.ts`, `tests/render/terrainUnits.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 62
- porte `build`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/render/surfaceBakeClient.ts
- 13-17 `ManifestEntry` (interface): The procedural surfaces, baked at build time into KTX2 files (`scripts/vite/surface-bake-plugin.ts`) and read here once, while the town is built: nothing is baked during play
- 19-23 `Manifest` (interface)
- 25-28 `SurfaceBakeResult` (interface)
- 30-30 `__SURFACE_BAKE_HASH__` (variable)
- 32-32 `BAKE_HASH` (variable): The fingerprint of the recipes' code (`surface-bake-plugin.ts`): a new bake is a new URL, never an old file from the cache
- 33-33 `FOLDER` (variable)
- 35-35 `TRANSCODER` (variable): Basis Universal's transcoder from three's addons, served by the plugin at a fixed path (Vite's prebundled three cannot find it beside the loader).
- 38-42 `rawLevels` (function): Each level's texels of an uncompressed file (`rgba8`), Zstandard undone.
- 45-54 `rgbaTexture` (function): An RGBA8 file as a texture of its own mip levels: three uploads an RGBA "compressed" texture level by level (WebGLTextures, `texSubImage2D`/`3D`), which a DataArrayTexture cannot.
- 57-65 `transcodeTargets` (function): What the GPU can take a UASTC file as (BC7, ASTC, ETC2...), asked of a throwaway WebGL2 context: the game's renderer is made after the textures are read.
- 68-100 `readFiles` (function): Reads the named files; UASTC through three's KTX2Loader, whose transcoding workers are let go once they are done.
- 103-111 `startSurfaceBake` (function): Reads every baked surface while the main thread builds the town.
- 114-121 `primeSurfaceBake` (function): Install the textures before the first material is made; a texture whose texels were let go reads its file again when it must be uploaded anew.

### src/render/surfaceTextures.ts
- 25-28 `SurfaceTextureSet.letTexelsGo` (field): DO NOT keep a texture's texels once uploaded: some 250 MB of the page's heap were copies of what the GPU holds.
- 42-48 `primeSurfaceTextures` (function): The textures read from the build's files (`surfaceBakeClient.ts`), and where to read them again.
- 51-58 `refillSurfaceTexels` (function): Texels read again: each let-go texture given its own and uploaded anew (it lets them go again after).
