# render:transit

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/transit.ts`
- Unidades próprias (só esta funcionalidade alcança): 25 em 1 arquivo(s), 209 linhas
- Alcance total: 260 unidades; usa 215 compartilhadas de outras; é dona principal de 20 compartilhadas

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
- Usa: `editor:barriers` (126), `world:transit` (99), `world:doc` (23), `worker:conflictTable` (16), `render:instance` (10), `world:changes` (8), `world:cells` (5)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`, `RoadDoc.nodes`, `RoadDoc.segments`, `RoadDoc.transit`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `TrainCar.heading`, `TrainCar.x`, `TrainCar.y`, `TrainState.cars`, `TrainState.colour`, `TrainState.metro`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/pageGarbage.spec.ts`, `tests/render/pinned.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `transit`: não se aplica 20, portado 98

## Código próprio (linhas — declaração — comentário)

### src/render/transit.ts
- 7-7 `EmptyStatement` (statement)
- 97-97 `VISIBLE` (variable): Every kind of box the transport draws, by the store it is drawn in.
- 98-98 `XRAY` (variable)
- 99-99 `TransitParts` (type)
- 100-100 `emptyParts` (function)
- 101-101 `TransitStop` (type)
- 102-102 `TransitTrack` (type)
- 105-109 `trackParts` (function): A track's boxes: the train's on the ground, the metro's in its cut.
- 112-218 `stopParts` (function): One stop's boxes (a station, a metro entrance with its hall below, a bus stop), into `out`.
- 231-231 `STOP_READS` (variable): How far round a stop its boxes and what they read reach (a station's 60 m platform and hall, an entrance's search).
- 233-233 `TRACK_READS` (variable): How far round a track's points it reads the ground and the paving.
- 234-234 `trackKey` (function)
- 235-235 `stopKey` (function)
- 239-239 `TransitProduct.name` (field)
- 240-240 `TransitProduct.kinds` (field)
- 241-241 `TransitProduct.byId` (field)
- 242-242 `TransitProduct.filing` (field)
- 244-244 `TransitProduct.constructor` (method)
- 247-253 `TransitProduct.allKeys` (method): @rebuild Every track and stop: read only when everything changed (a load, the diary forgotten).
- 255-269 `TransitProduct.dirty` (method)
- 271-302 `TransitProduct.compute` (method)
- 304-306 `TransitProduct.equals` (method)
- 308-311 `TransitProduct.changed` (method)
- 315-320 `pointsBox` (function): The box round some points, grown by `pad`; null for none.
- 470-483 `entranceSpot` (function): Where a metro entrance stands for a station at (x, y)
