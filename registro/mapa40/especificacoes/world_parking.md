# world:parking

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/parking.ts`, `src/world/parkingLayout.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 11 linhas
- Alcance total: 409 unidades; usa 371 compartilhadas de outras; é dona principal de 37 compartilhadas

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
- Usa: `worker:conflictTable` (5), `editor:buildings` (4), `editor:barriers` (1), `editor:commit` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadNode.crossing`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/sim/agents/lotGate.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `roads`: portado 13, não portado 48
- porte `traffic`: portado 3

## Código próprio (linhas — declaração — comentário)

### src/world/parkingLayout.ts
- 77-87 `gatesNear` (function): The entrances of `gates` (`gatesOf`) a street of half width `half` along `line` may read: those `entrancesNear` would hand it, for a road bake without the buildings (`render/nearRoadTiles.ts`).
