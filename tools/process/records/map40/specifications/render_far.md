# render:far

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/farForest.ts`, `src/render/farGround.ts`, `src/render/farWoods.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 20 linhas
- Alcance total: 239 unidades; usa 183 compartilhadas de outras; é dona principal de 53 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `onBeforeCompile`
- Timers: 0
- Usa: `world:terrain` (1), `world:ecology` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/ecologyKept.spec.ts`, `tests/render/farGround.spec.ts`, `tests/render/farWoods.spec.ts`, `tests/render/treePictures.spec.ts`, `tests/world/unboundedPlates.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: portado 98, não se aplica 7

## Código próprio (linhas — declaração — comentário)

### src/render/farForest.ts
- 33-41 `FarForestStats` (interface)
- 175-182 `FarWoodsBook.stats` (method)
- 184-186 `FarWoodsBook.landOf` (method)
