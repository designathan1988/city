# world:street

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/streetFurniture.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 10 linhas
- Alcance total: 342 unidades; usa 334 compartilhadas de outras; é dona principal de 7 compartilhadas

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
- Usa: `world:network` (2), `world:landscape` (2), `world:doc` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/world/roadFurniture.spec.ts`, `tests/world/streetFurniture.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 16
- porte `street-furniture`: portado 18

## Código próprio (linhas — declaração — comentário)

### src/world/streetFurniture.ts
- 114-123 `streetFurniture` (function): Everything standing on the footways: the items the player placed with the landscaping tool
