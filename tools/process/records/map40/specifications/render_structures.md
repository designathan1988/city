# render:structures

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/structures.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 219 unidades; usa 166 compartilhadas de outras; é dona principal de 53 compartilhadas

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
- Lê: `RoadNode.heightOffset`, `RoadNode.incident`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.cutWalls`, `RoadSegment.direction`, `RoadSegment.id`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/pierOnRoad.spec.ts`, `tests/world/cutWalls.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `economy`: não se aplica 1
- porte `road-elevation`: a portar 66

## Código próprio (linhas — declaração — comentário)
