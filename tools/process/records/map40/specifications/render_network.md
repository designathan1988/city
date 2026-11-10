# render:network

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/networkFarRoads.ts`, `src/render/networkFarRoadsBuild.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 19 linhas
- Alcance total: 195 unidades; usa 144 compartilhadas de outras; é dona principal de 47 compartilhadas

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
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.incident`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `earcut`, `three`

## Testes do 4.0 que a exercitam
- `tests/render/networkFarRoadsBuild.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 96

## Código próprio (linhas — declaração — comentário)

### src/render/networkFarRoads.ts
- 118-120 `NetworkFarRoads.constructor` (method)
- 127-129 `NetworkFarRoads.triangles` (method)
- 213-221 `NetworkFarRoads.covers` (method): Whether the level standing draws the cell's roads as the blocks do: made of the roads drawn, with roads there, none of it left to the detailed surfaces (`RoadResidency.network`).
- 240-243 `NetworkFarRoads.show` (method): A cell shown: its blocks' far levels let go (`roadResidency.ts` dropCell).
