# render:gpu

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/gpu/backendChoice.ts`, `src/render/gpu/compatibility.ts`, `src/render/gpu/imageCompare.ts`, `src/render/gpu/twins.ts`, `src/render/gpu/webgpuView.ts`, `src/render/gpuLedger.ts`, `src/render/gpuTimer.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 9 linhas
- Alcance total: 80 unidades; usa 8 compartilhadas de outras; é dona principal de 70 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `emite:{ type: 'dispose' }`, `ouve:webglcontextlost`, `ouve:webglcontextrestored`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `onBeforeCompile`, `onBeforeRender`
- Timers: 1
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`, `three/tsl`, `three/webgpu`

## Testes do 4.0 que a exercitam
- `tests/core/weakMachine.spec.ts`, `tests/perf/noHoles.spec.ts`, `tests/render/gpuLedger.spec.ts`, `tests/render/gpuTimer.spec.ts`, `tests/render/roadLod.spec.ts`, `tests/render/webgpuFoundation.spec.ts`, `tests/world/unboundedPlates.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: não se aplica 5
- porte `build`: a portar 2
- porte `interior`: outro porte 2

## Código próprio (linhas — declaração — comentário)

### src/render/gpuLedger.ts
- 49-54 `GpuLedger.triangles` (method): Triangles held by a class, or by all.
- 57-59 `GpuLedger.entries` (method): Every owner and what it holds now, read only (the debugging lab's consumers, `lab/resources`): nothing is copied, the book is walked as it stands.
