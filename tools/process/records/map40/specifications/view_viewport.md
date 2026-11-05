# view:viewport

Grupo: view. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/view/viewport.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 53 linhas
- Alcance total: 16 unidades; usa 13 compartilhadas de outras; é dona principal de 2 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#view@608` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `view:camera` (17), `worker:conflictTable` (6)
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
- porte `build`: não se aplica 2
- porte `interface`: portado 28
- porte `interior`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/view/viewport.ts
- 52-104 `flatViewport` (function): The existing flat camera, behind the same seam.
