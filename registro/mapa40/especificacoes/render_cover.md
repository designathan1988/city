# render:cover

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/coverRaster.ts`, `src/render/coverSweep.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 15 linhas
- Alcance total: 78 unidades; usa 38 compartilhadas de outras; é dona principal de 37 compartilhadas

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
- Usa: `world:terrain` (8)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/render/coverSweep.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: portado em parte 22

## Código próprio (linhas — declaração — comentário)

### src/render/coverRaster.ts
- 38-42 `PlateCover` (interface): A plate's cover densities and rock at its corners, as the plate lays them from its dabs (`paintStep`) and a tile's cover bake does (`coverTiles.ts`).
- 45-50 `plateCover` (function): A plate's cover and rock with nothing laid.
- 53-56 `layCover` (function): One dab laid on a plate's cover and rock, in the plate's own frame, as the plate lays it (`paintStep`; a biome dab lays neither).
