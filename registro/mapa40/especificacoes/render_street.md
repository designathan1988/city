# render:street

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/streetFurnitureLayer.ts`
- Unidades próprias (só esta funcionalidade alcança): 12 em 1 arquivo(s), 63 linhas
- Alcance total: 714 unidades; usa 672 compartilhadas de outras; é dona principal de 30 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `recebe:bench`, `recebe:bin`, `recebe:drain`, `recebe:hydrant`, `recebe:lamp`, `recebe:phone`, `recebe:postbox`, `recebe:shrub`, `recebe:streetTree`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:changes` (10), `world:landscape` (7), `world:cells` (6), `world:network` (4), `world:doc` (4), `render:scenery` (2), `world:derived` (2), `editor:barriers` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/streetFurnitureLayer.spec.ts`, `tests/scale/census/derivedBound.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: já existia 2
- porte `roads`: não se aplica 102
- porte `street-furniture`: portado 94, já existia 10

## Código próprio (linhas — declaração — comentário)

### src/render/streetFurnitureLayer.ts
- 31-31 `DECK_REACH` (variable): Its deck's height read at its point.
- 233-234 `sameColour` (function)
- 235-243 `samePlace` (function)
- 247-247 `PlacedProduct.byId` (field)
- 248-248 `PlacedProduct.kinds` (field)
- 250-250 `PlacedProduct.filing` (field): The items holding a value, by place: the ones a change reaches, gone from the document or not.
- 252-260 `PlacedProduct.constructor` (method)
- 263-268 `PlacedProduct.allKeys` (method): @rebuild The document's items of the product's kinds: read only when everything changed (a load, the diary forgotten).
- 270-288 `PlacedProduct.dirty` (method)
- 290-293 `PlacedProduct.compute` (method)
- 295-298 `PlacedProduct.changed` (method)
- 309-314 `furnitureProduct` (function): The furniture's product: where each placed item stands (a product of the diary), proved against one made from nothing.
