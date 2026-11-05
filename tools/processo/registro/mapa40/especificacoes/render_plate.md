# render:plate

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/plateFollow.ts`, `src/render/plateMesh.ts`, `src/render/platePaint.ts`, `src/render/plateTexels.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 8 linhas
- Alcance total: 49 unidades; usa 26 compartilhadas de outras; é dona principal de 22 compartilhadas

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
- Usa: `render:staged` (9)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/render/plateFollow.spec.ts`, `tests/render/plateMesh.spec.ts`, `tests/render/platePaint.spec.ts`, `tests/render/plateTexels.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: não se aplica 22
- porte `roads`: não portado 4

## Código próprio (linhas — declaração — comentário)

### src/render/plateTexels.ts
- 76-83 `lightRectInto` (function): The light as the worker sends it (a rect of corners, rows of its width) into the light store and, when given, a paint layer at its stride.
