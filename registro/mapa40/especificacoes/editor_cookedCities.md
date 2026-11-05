# editor:cookedCities

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/cookedCities.ts`
- Unidades próprias (só esta funcionalidade alcança): 8 em 1 arquivo(s), 36 linhas
- Alcance total: 91 unidades; usa 83 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#cityData@2056` (5), `src/main.ts#cityAhead@2146` (3), `src/main.ts#nextCitySeed@2136` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): `…`
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/cookedCities.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `interface`: não se aplica 16

## Código próprio (linhas — declaração — comentário)

### src/editor/cookedCities.ts
- 6-6 `COOKED_CITY_SEEDS` (variable): The seed each size's shipped city is grown from: the player's first new city of that size.
- 7-7 `CookedKind` (type)
- 10-10 `COOKED_CITIES_DIR` (variable): Where they are kept beside the game (`cook-plugin.ts` copies the folder into the build).
- 13-16 `CookedCitiesManifest` (interface): What `cook-cities.mjs` writes: the generator's fingerprint it grew them with (`__CITY_GEN_HASH__`), each city's two files.
- 19-22 `CookedCity` (interface): A shipped city as the page opens it: its document without buildings, its buildings' table (`prepareCity`).
- 25-28 `gunzipped` (function): A gzip file's bytes undone (DecompressionStream, every browser the game runs in).
- 30-30 `manifest` (variable)
- 33-52 `readCookedCity` (function): The shipped city of `kind` grown from `seed` by this build's generator (`hash`), or null: another seed, another generator (the city grown here then), or none shipped.
