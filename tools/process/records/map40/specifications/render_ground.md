# render:ground

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/groundChanges.ts`, `src/render/groundCover.ts`, `src/render/groundShape.ts`, `src/render/groundShapePool.ts`, `src/render/groundShear.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 19 linhas
- Alcance total: 578 unidades; usa 492 compartilhadas de outras; é dona principal de 83 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/coverSweep.spec.ts`, `tests/render/gardenLists.spec.ts`, `tests/render/groundCover.spec.ts`, `tests/render/groundShapeRegions.spec.ts`, `tests/render/instanceRuns.spec.ts`, `tests/render/pedSlope.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/scale/buildingEdit.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: não se aplica 6
- porte `landscape`: já existia 32, portado 28, não se aplica 44
- porte `people`: já existia 1
- porte `traffic`: já existia 1

## Código próprio (linhas — declaração — comentário)

### src/render/groundChanges.ts
- 38-54 `GroundChanges.touches` (method): Whether a change after `since` reaches `area` (`null`: a dependant with nothing on the ground).
- 72-72 `isRect` (function)
- 74-74 `overlaps` (function)
