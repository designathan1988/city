# render:profiler

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/profilerHooks.ts`
- Unidades próprias (só esta funcionalidade alcança): 14 em 2 arquivo(s), 100 linhas
- Alcance total: 22 unidades; usa 2 compartilhadas de outras; é dona principal de 6 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#wireProfiler@699` (13), `src/main.ts#frame@2644` (3), `src/main.ts#IfStatement@4036` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `ui:profilerHud` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/core/profiler.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/core/profiler.ts
- 27-27 `SITES_EXACT` (variable): First calls of a frame whose site is read every time; past them, one in `SITE_STRIDE`.
- 28-28 `SITE_STRIDE` (variable)
- 32-32 `STACK_LINES` (variable): Stack lines kept for the latest call.
- 35-35 `NOT_CALLER` (variable): Frames of three, Vite, the profiler itself: not the caller.
- 37-43 `stackHere` (function)
- 46-50 `shortLine` (function): `fn @ file:line` of a stack line (" at fn (http://host/src/a/b.ts?t=1:12:3)").
- 53-55 `callerLines` (function): The caller's lines of a stack, three and the profiler left out.
- 92-92 `Profiler.frameCalls` (field)
- 131-138 `Profiler.tally` (method)
- 141-155 `Profiler.count` (method): Counts one event (and its bytes), with its call site when sampled.
- 284-284 `profiler` (variable): The game's one profiler.

### src/render/profilerHooks.ts
- 6-6 `HOOKED` (variable): Set on `BufferGeometry.prototype` once the prototypes are wrapped: they are wrapped once.
- 8-55 `hookProfiler` (function)
- 57-63 `wrap` (function)
