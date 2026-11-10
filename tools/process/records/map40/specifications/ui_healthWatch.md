# ui:healthWatch

Grupo: interface. Entrada: painel. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/healthWatch.ts`
- Unidades próprias (só esta funcionalidade alcança): 22 em 3 arquivo(s), 299 linhas
- Alcance total: 43 unidades; usa 19 compartilhadas de outras; é dona principal de 2 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `health.msg.budget`, `health.msg.noMessage`, `health.msg.rejected`, `health.msg.scripts`, `health.msg.slowFrame`, `health.msg.slowFrameOutside`, `health.msg.steps`, `health.msg.webglLost`, `health.msg.webglRestored`, `health.msg.workerError`, `health.msg.workerFailed`, `health.msg.workerUnreadable`
- Eventos de entrada: `ouve:error`, `ouve:messageerror`, `ouve:pagehide`, `ouve:unhandledrejection`, `ouve:webglcontextlost`, `ouve:webglcontextrestored`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#frame@2644` (8), `src/main.ts#pacedFps@4027` (2), `src/main.ts#healthWatch@173` (1), `src/main.ts#frameTimer@172` (1), `src/main.ts#noteSimulationIssues@3976` (1), `src/main.ts#IfStatement@3956` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 1
- Usa: `ui:healthPanel` (46), `ui:i18n` (13)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/cityBudget.spec.ts`, `tests/core/health.spec.ts`, `tests/core/stepBudget.spec.ts`, `tests/perf/budgets.spec.ts`, `tests/ui/healthPanel.spec.ts`, `tests/ui/healthWatch.spec.ts`, `tests/ui/visibleText.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: portado 36, não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/core/health.ts
- 37-37 `REPEAT_WINDOW_MS` (variable): A repeat of the same kind and message within this is counted on the entry already written.
- 56-81 `HealthLog.record` (method): Writes an entry, or counts a repeat on the last one of the same kind and message.
- 106-108 `HealthLog.toJSON` (method): The entries to keep across sessions, oldest first.
- 111-119 `HealthLog.restore` (method): Brings back the entries of an earlier session, marked as such (they never count in `status`).
- 123-123 `LONG_FRAME_MS` (variable): A long frame: past this the player sees a hitch (MDN, Long Animation Frames: 50 ms).
- 132-136 `FrameRecord` (interface): One frame's systems, ms each.
- 144-179 `FrameTimer` (class): Times the systems of each frame: `begin` at the top of the frame, `mark` after each system with its name
- 148-148 `FrameTimer.frames` (field)
- 171-173 `FrameTimer.between` (method): The frames that ran between `from` and `to` (a long frame's span).
- 212-216 `systemsOf` (function): The systems of these frames summed, the largest first, those under 0.5 ms left out.

### src/core/perfBudget.ts
- 3-12 `Budget` (interface): THE PERFORMANCE BUDGETS: one table for the game's own watch (the F9 monitor, `ui/healthWatch.ts`), the specs
- 15-26 `STEP_BUDGETS` (variable): A step the game measures (`performance.measure('hitch:<name>')`), ms of one occurrence
- 29-37 `stepBudget` (function): DO NOT cut at the space first: 'mutate/after edit' and 'road-edit/world rebuilt' became 'mutate/after' and

### src/ui/healthWatch.ts
- 13-18 `HealthWatch` (interface)
- 20-20 `KEPT_KEY` (variable)
- 23-29 `LongFrameEntry` (interface): A long animation frame as the browser reports it (MDN, PerformanceLongAnimationFrameTiming).
- 35-42 `longFrameWork` (function): The work in a long frame, ms: its scripts and its rendering DO NOT take the listed scripts alone: the browser lists only scripts over 5 ms, and a frame of 30 callbacks of 4 ms reported only its render (report TST-09). The game's own steps
- 45-47 `stepsWithin` (function): The code's measured steps that ran IN a frame: begun and ended within it, as a synchronous step does.
- 50-52 `textOf` (function): Text of console arguments, short.
- 53-55 `safeJSON` (function)
- 56-59 `stackOf` (function)
- 61-203 `watchHealth` (function)
