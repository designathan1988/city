# world:turn

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/turnChoice.ts`, `src/world/turnDigest.ts`, `src/world/turnPaths.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 2 arquivo(s), 47 linhas
- Alcance total: 271 unidades; usa 183 compartilhadas de outras; é dona principal de 83 compartilhadas

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
- Usa: `worker:conflictTable` (10), `world:surface` (8), `world:network` (1), `world:ids` (1), `world:junction` (1), `editor:growthFeed` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/sim/topologyOffPage.spec.ts`, `tests/world/turnOffSurface.spec.ts`, `tests/world/turnPathsEquivalence.spec.ts`, `tests/world/turnPlan.spec.ts`, `tests/world/turns.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `traffic`: já existia 3, não se aplica 2
- porte `roads`: não se aplica 44

## Código próprio (linhas — declaração — comentário)

### src/world/turnDigest.ts
- 8-12 `TurnDigest.mix` (method): Folds in another digest's two lanes.

### src/world/turnPaths.ts
- 43-43 `areas` (variable): A ring's box and digest, kept with the ring's own point list.
- 44-58 `areaOf` (function)
- 68-72 `surfaceKey` (function): A surface's key: digest of its centre and every ring point.
- 88-108 `JunctionSurface.constructor` (method): The surface round `node`, from its pieces (`surfaceParts`; a worker's, the same pieces sent)
