# world:clouds

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/clouds.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 19 linhas
- Alcance total: 14 unidades; usa 3 compartilhadas de outras; é dona principal de 10 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#IfStatement@1703` (1), `src/main.ts#MAP_ACTIONS@1798` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
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
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/world/clouds.spec.ts`, `tests/world/weather.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: portado 28

## Código próprio (linhas — declaração — comentário)

### src/world/clouds.ts
- 51-69 `scatterClouds` (function): Clouds spread over the map, as a procedural spawner fills a volume and the placed instances are then edited one by one
