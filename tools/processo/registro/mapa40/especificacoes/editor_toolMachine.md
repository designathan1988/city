# editor:toolMachine

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/toolMachine.ts`
- Unidades próprias (só esta funcionalidade alcança): 27 em 2 arquivo(s), 270 linhas
- Alcance total: 28 unidades; usa 0 compartilhadas de outras; é dona principal de 1 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#IfStatement@3948` (5), `src/main.ts#healthPanel@3987` (5), `src/main.ts#mountInterface@1859` (2), `src/main.ts#setTool@1842` (1), `src/main.ts#freeSelection@1847` (1), `src/main.ts#pickTool@1852` (1), `src/main.ts#tools@434` (1)

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
- `tests/core/statechart.spec.ts`, `tests/editor/gameKeys.spec.ts`, `tests/editor/toolMachine.spec.ts`, `tests/ui/i18nKeys.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/core/statechart.ts
- 3-3 `MachineEvent` (interface): A STATECHART WITH THE SEMANTICS OF W3C SCXML, for the game's tools and modes
- 5-10 `Transition` (interface)
- 12-20 `StateSpec` (interface)
- 22-28 `TransitionRecord` (interface)
- 30-30 `KEPT` (variable)
- 32-138 `Machine` (class)
- 33-33 `Machine.current` (field)
- 34-34 `Machine.queue` (field)
- 35-35 `Machine.processing` (field)
- 36-36 `Machine.serial` (field)
- 37-37 `Machine.ring` (field)
- 39-51 `Machine.constructor` (method)
- 54-56 `Machine.state` (method): The active (innermost) state.
- 64-73 `Machine.send` (method): Sends an event; returns when it, and every event it caused, has been processed.
- 76-78 `Machine.latest` (method): The last `n` transitions, newest first.
- 80-90 `Machine.step` (method)
- 93-115 `Machine.take` (method): DO NOT take the shared part of the active leaf's and the target's paths as the domain
- 117-120 `Machine.record` (method)
- 123-127 `Machine.path` (method): `id`, then its parents up to the top.
- 130-137 `Machine.descend` (method): `id`, or its initial child, all the way down.

### src/editor/toolMachine.ts
- 23-23 `TOOLS` (variable): Every tool, as the interface names them.
- 26-26 `ROAD_TOOLS` (variable): The tools the Roads button holds.
- 28-30 `isTool` (function)
- 32-35 `ToolEvent` (type)
- 38-43 `ToolHands` (interface): What the machine does to the game: take a tool, let go of what Inspect picked.
- 45-49 `ToolMachine` (interface)
- 52-85 `createToolMachine` (function): A machine whose state is `start` (the store's tool).
