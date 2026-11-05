# core:aabb

Grupo: core. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/aabb.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 13 linhas
- Alcance total: 5 unidades; usa 1 compartilhadas de outras; é dona principal de 3 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: `core:vec` (9)

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/world/buildingForkGrid.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: já existia 4
- porte `road-elevation`: a portar 2
- porte `roads`: não se aplica 2

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/aabb.ts
- 10-22 `fromPoints` (function)
