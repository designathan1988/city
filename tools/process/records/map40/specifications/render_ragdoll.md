# render:ragdoll

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/ragdoll.ts`, `src/render/ragdollAxes.ts`, `src/render/ragdollGround.ts`, `src/render/ragdollJolt.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 1 arquivo(s), 36 linhas
- Alcance total: 166 unidades; usa 15 compartilhadas de outras; é dona principal de 146 compartilhadas

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
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `jolt-physics/wasm-compat`, `three`

## Testes do 4.0 que a exercitam
- `tests/render/ragdollAxes.spec.ts`, `tests/render/ragdollFrame.spec.ts`, `tests/render/ragdollGround.spec.ts`, `tests/render/ragdollJolt.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: já existia 2, não portado (pendência) 1, não se aplica 2
- porte `people`: não se aplica 1
- porte `traffic`: de outra funcionalidade 1

## Código próprio (linhas — declaração — comentário)

### src/render/ragdollJolt.ts
- 14-14 `MAX_PARTS` (variable): Parts a ragdoll has (`RagdollSpec.parts`), for its collision groups.
- 76-97 `JoltWorld.constructor` (method)
- 263-267 `JoltWorld.pointVelocity` (method)
- 288-288 `JoltWorld.liveConstraints` (method): How many joints are in the physics now.
- 314-320 `JoltRagdoll.velocity` (method): How fast particle `k` moves (metres a second, world).
