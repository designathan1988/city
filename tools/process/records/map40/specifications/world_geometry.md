# world:geometry

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/geometry.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 97 unidades; usa 80 compartilhadas de outras; é dona principal de 16 compartilhadas

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
- Lê: `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.curve`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/world/economy.spec.ts`, `tests/world/junction.spec.ts`, `tests/world/roadValueLedger.spec.ts`, `tests/world/turnOffSurface.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não portado 30

## Código próprio (linhas — declaração — comentário)

### src/world/geometry.ts
- 97-99 `PolylineCache.size` (method)
