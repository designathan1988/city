# world:walkable

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/walkable.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 2 arquivo(s), 16 linhas
- Alcance total: 183 unidades; usa 141 compartilhadas de outras; é dona principal de 38 compartilhadas

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
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/regionIndex.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: já existia 2, não se aplica 36
- porte `roads`: não se aplica 57

## Código próprio (linhas — declaração — comentário)

### src/core/regionIndex.ts
- 174-178 `RegionIndex.cellCount` (method): Cells allocated over every grid.
- 297-299 `Grid.cellCount` (method)

### src/world/walkable.ts
- 39-43 `WalkableSurface.cells` (method): Cells of the grids made so far, for the performance budgets (`core/perfBudget.ts`).
- 52-54 `WalkableSurface.carriageway` (method): On the carriageway.
