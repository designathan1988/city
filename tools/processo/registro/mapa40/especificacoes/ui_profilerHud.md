# ui:profilerHud

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/profilerHud.ts`
- Unidades próprias (só esta funcionalidade alcança): 25 em 2 arquivo(s), 161 linhas
- Alcance total: 33 unidades; usa 6 compartilhadas de outras; é dona principal de 2 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:click`, `ouve:keydown`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#wireProfiler@699` (4), `src/main.ts#IfStatement@4036` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 2
- Usa: `render:profiler` (13)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/profiler.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `interface`: portado 22

## Código próprio (linhas — declaração — comentário)

### src/core/profiler.ts
- 58-65 `ProductTiming` (interface): One product's refresh as the engine timed it (`core/engine.ts`).
- 94-94 `Profiler.frames` (field)
- 100-100 `Profiler.frameP50` (field): Last window's frame time: median and worst.
- 101-101 `Profiler.frameMax` (field)
- 102-102 `Profiler.fps` (field)
- 104-104 `Profiler.products` (field): Engine products of the last refresh of each phase.
- 106-106 `Profiler.idleProducts` (field): Products that computed for nothing, ever (name → times).
- 109-109 `Profiler.lastChanges` (field)
- 111-111 `Profiler.heapMB` (field): Latest heap readings: MB, and MB per second over the last window.
- 112-112 `Profiler.heapRate` (field)
- 115-115 `Profiler.gcDropMB` (field): Biggest drop of the heap in the window: a collection, MB.
- 117-117 `Profiler.longTasks` (field): Long tasks (> 50 ms) lately, newest first: ms and when.
- 120-120 `Profiler.lastSteps` (field)
- 129-129 `Profiler.gauges` (field): Extra gauges set by the renderer (draw calls, geometries on the GPU...).
- 231-231 `Profiler.closed` (field): The last closed window, as the HUD shows it.
- 234-255 `Profiler.view` (method): Everything counted, plain data (the HUD, `__roadcraft.profiler()`, a snapshot copied out).
- 258-279 `ProfilerView` (interface)
- 281-281 `round` (function)

### src/ui/profilerHud.ts
- 4-40 `mountProfilerHud` (function): Redrawn four times a second from the last closed window of a second - DO NOT redraw it every frame
- 42-42 `esc` (function)
- 43-43 `red` (function)
- 44-44 `amber` (function)
- 45-45 `head` (function)
- 47-94 `render` (function)
- 96-100 `merge` (function)
