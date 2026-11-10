# world:walkways

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/walkways.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 493 unidades; usa 426 compartilhadas de outras; é dona principal de 66 compartilhadas

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
- Lê: `RoadDoc.nodes`, `RoadDoc.segments`, `RoadNode.end`, `RoadNode.heightOffset`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/sim/walkSpine.spec.ts`, `tests/world/walkLogBound.spec.ts`, `tests/world/walkwayCorners.spec.ts`, `tests/world/walkwayUpdate.spec.ts`, `tests/world/walkways.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: já existia 2
- porte `people`: já existia 4
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/world/walkways.ts
- 110-112 `WalkGraph.walkways` (method): The walkways in the graph now, in id order (a new array: for a reader of them all, as a test).
