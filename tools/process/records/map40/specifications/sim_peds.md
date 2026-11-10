# sim:peds

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/peds/sidewalk.ts`, `src/sim/peds/signalPostIndex.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 337 unidades; usa 296 compartilhadas de outras; é dona principal de 41 compartilhadas

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
- Lê: `RoadDoc.nodes`, `RoadDoc.segments`, `RoadNode.incident`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/sim/indexes/footwayIndex.spec.ts`, `tests/sim/sidewalkUpdate.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: já existia 2
- porte `people`: já existia 2
- porte `traffic`: já existia 2
- porte `street-furniture`: já existia 1

## Código próprio (linhas — declaração — comentário)
