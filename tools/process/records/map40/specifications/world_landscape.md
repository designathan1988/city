# world:landscape

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/landscape.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 351 unidades; usa 307 compartilhadas de outras; é dona principal de 44 compartilhadas

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
- Lê: `RoadDoc.nodes`, `RoadNode.incident`, `RoadNode.uturn`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/poles.spec.ts`, `tests/editor/streetscapeTool.spec.ts`, `tests/editor/uturn.spec.ts`, `tests/render/signLayer.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/world/cityGreen.spec.ts`, `tests/world/crossingLandings.spec.ts`, `tests/world/landscapeBatch.spec.ts`, `tests/world/medianNose.spec.ts`, `tests/world/nearSearch.spec.ts`, `tests/world/roadFurniture.spec.ts`, `tests/world/streetFurniture.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `people`: já existia 1
- porte `roads`: não se aplica 14
- porte `street-furniture`: portado 1, não se aplica 16

## Código próprio (linhas — declaração — comentário)
