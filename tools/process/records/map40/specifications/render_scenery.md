# render:scenery

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/scenery.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 1 linhas
- Alcance total: 164 unidades; usa 142 compartilhadas de outras; é dona principal de 21 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `customDepthMaterial`
- Timers: 0
- Usa: `editor:buildingPlans` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/gardenCull.spec.ts`, `tests/render/gardenFrame.spec.ts`, `tests/render/gardenPacks.spec.ts`, `tests/render/instanceRuns.spec.ts`, `tests/render/plantLevels.spec.ts`, `tests/render/poleLayer.spec.ts`, `tests/render/poleLevels.spec.ts`, `tests/render/shadowCasters.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/render/utilityWires.spec.ts`, `tests/scale/census/derivedBound.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: não se aplica 2
- porte `landscape`: já existia 58
- porte `street-furniture`: parcial 2

## Código próprio (linhas — declaração — comentário)

### src/render/scenery.ts
- 368-368 `PLANT_NEAR_ZOOM` (variable): Zoom at and above which plants are drawn with their close-up models.
