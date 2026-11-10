# render:uploads

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/uploads.ts`
- Unidades próprias (só esta funcionalidade alcança): 9 em 1 arquivo(s), 57 linhas
- Alcance total: 83 unidades; usa 0 compartilhadas de outras; é dona principal de 74 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:dispose`
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
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/cellHoles.spec.ts`, `tests/render/cellPool.spec.ts`, `tests/render/gpuOnly.spec.ts`, `tests/render/platePrograms.spec.ts`, `tests/render/programsGate.spec.ts`, `tests/render/shaderCatalogue.spec.ts`, `tests/render/shadowStandIn.spec.ts`, `tests/render/textureUploads.spec.ts`, `tests/render/warmLoose.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: a portar 4
- porte `interior`: outro porte 4

## Código próprio (linhas — declaração — comentário)

### src/render/uploads.ts
- 156-156 `IfStatement` (statement)
- 161-161 `BuiltProgram` (type)
- 162-203 `programsTable` (function)
- 204-204 `IfStatement` (statement)
- 236-239 `compilesQuiet` (function): Settles at the first drain with no compile asked nor in flight: what the view does not need yet (a light's variants, `shaderCatalogue.ts`) asked after what it does, never alongside it (the driver builds a few programs at once, Chrome's limi
- 414-416 `warmQueuedBytes` (function): The bytes of the loose meshes waiting to go to the GPU (`toWarmLoose`), kept as they are queued and sent: what the producers hold back by.
- 435-437 `warmReservedBytes` (function): The bytes reserved by jobs in flight (`reserveWarm`).
- 608-608 `ExpressionStatement` (statement)
- 609-609 `ExpressionStatement` (statement)
