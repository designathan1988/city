# editor:blocks

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/blocks.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 36 linhas
- Alcance total: 1203 unidades; usa 1200 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#drawOverlayScreen@2942` (1), `src/main.ts#pointerDown@1217` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (10), `editor:snap` (5), `editor:commit` (2), `editor:barriers` (1), `world:doc` (1), `world:network` (1), `world:parking` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `traffic`: de outra funcionalidade 2

## Código próprio (linhas — declaração — comentário)

### src/editor/blocks.ts
- 10-17 `BlockGrid` (interface): Several blocks at once: a grid of streets laid in one gesture
- 20-29 `blockGridLines` (function): The street lines of a grid, as pairs of world points.
- 32-49 `commitBlockGrid` (function): DO NOT make a network per line: each was built from nothing over the whole map, as many times as the grid has lines.
