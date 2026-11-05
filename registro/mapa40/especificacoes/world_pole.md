# world:pole

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/poleIndex.ts`, `src/world/poleLines.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 7 linhas
- Alcance total: 381 unidades; usa 319 compartilhadas de outras; é dona principal de 61 compartilhadas

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
- Usa: `world:road` (4), `world:surfaces` (2), `world:network` (1), `world:landscape` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadSegment.a`, `RoadSegment.b`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `clipper2-ts`

## Testes do 4.0 que a exercitam
- `tests/editor/poleCrossing.spec.ts`, `tests/perf/budgets.spec.ts`, `tests/world/poleLinesCells.spec.ts`, `tests/world/poleLinesKept.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `street-furniture`: portado 108, não se aplica 8
- porte `build`: outro porte 4
- porte `people`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/world/poleLines.ts
- 171-177 `poleLines` (function): DO NOT keep it per network revision again: every road edit made it again for every street of the map
