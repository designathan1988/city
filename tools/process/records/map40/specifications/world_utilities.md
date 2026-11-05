# world:utilities

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/utilities.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 22 linhas
- Alcance total: 41 unidades; usa 9 compartilhadas de outras; é dona principal de 30 compartilhadas

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
- Usa: `world:ids` (5), `worker:conflictTable` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/poles.spec.ts`, `tests/render/utilityWires.spec.ts`, `tests/world/roadTuning.spec.ts`, `tests/world/utilities.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `street-furniture`: portado 68

## Código próprio (linhas — declaração — comentário)

### src/world/utilities.ts
- 63-66 `wireDrop` (function): Height of a catenary above the chord, at normalised position `t` in [0, 1].
- 93-110 `poleArms` (function): Which way every pole's cross-arm lies, as line construction standards frame a pole
