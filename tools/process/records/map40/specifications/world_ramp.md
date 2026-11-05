# world:ramp

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/rampRise.ts`
- Unidades próprias (só esta funcionalidade alcança): 10 em 1 arquivo(s), 10 linhas
- Alcance total: 26 unidades; usa 5 compartilhadas de outras; é dona principal de 11 compartilhadas

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
- Usa: `world:ids` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 42

## Código próprio (linhas — declaração — comentário)

### src/world/rampRise.ts
- 33-33 `PackedRamp.constructor` (method)
- 34-34 `PackedRamp.x` (method)
- 35-35 `PackedRamp.y` (method)
- 36-36 `PackedRamp.tx` (method)
- 37-37 `PackedRamp.ty` (method)
- 38-38 `PackedRamp.nx` (method)
- 39-39 `PackedRamp.ny` (method)
- 40-40 `PackedRamp.half` (method)
- 41-41 `PackedRamp.run` (method)
- 42-42 `PackedRamp.flare` (method)
