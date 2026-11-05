# core:ring

Grupo: core. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/ring.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 29 linhas
- Alcance total: 37 unidades; usa 13 compartilhadas de outras; é dona principal de 22 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: `core:vec` (7)

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/geometry.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `road-elevation`: a portar 4
- porte `roads`: não se aplica 24

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/ring.ts
- 36-52 `Ring.constructor` (method)
- 190-201 `Ring.addToPath` (method): Appends this ring to a Path2D as a closed subpath, arcs kept exact.
