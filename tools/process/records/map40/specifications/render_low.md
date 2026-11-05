# render:low

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/lowPolyTrees.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 76 unidades; usa 16 compartilhadas de outras; é dona principal de 60 compartilhadas

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
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/leafDetail.spec.ts`, `tests/render/sharedPlantPrograms.spec.ts`, `tests/render/treeModelsOnce.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: já existia 70

## Código próprio (linhas — declaração — comentário)
