# editor:gameKeys

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/gameKeys.ts`
- Unidades próprias (só esta funcionalidade alcança): 14 em 2 arquivo(s), 220 linhas
- Alcance total: 15 unidades; usa 1 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#gameKeys@2479` (1), `src/main.ts#ExpressionStatement@2556` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `editor:toolMachine` (4)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/commands.spec.ts`, `tests/editor/gameKeys.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: portado 20

## Código próprio (linhas — declaração — comentário)

### src/core/commands.ts
- 3-3 `CommandResult` (type): THE COMMAND REGISTRY: one table of what the player can do, behind the shortcuts, the dock, the drawers
- 5-11 `Command` (interface)
- 13-28 `KeyRule` (interface)
- 39-87 `CommandRegistry` (class)
- 40-40 `CommandRegistry.commands` (field)
- 41-41 `CommandRegistry.keys` (field)
- 43-46 `CommandRegistry.register` (method)
- 48-51 `CommandRegistry.bind` (method)

### src/editor/gameKeys.ts
- 6-52 `GameKeyDeps` (interface): What the keys act on: the game's commands, handed in by `main.ts`.
- 55-58 `TOOL_LETTERS` (variable): The letter of each tool (R is the Builder's rotation; the road tool is the number row).
- 60-60 `done` (function)
- 61-61 `used` (function)
- 63-66 `held` (function): A held key's command: when the key is taken, a frame is asked for.
- 69-148 `createGameKeys` (function): The registry with the game's commands and keys.
