# core:assert

Grupo: core. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/assert.ts`
- Unidades próprias (só esta funcionalidade alcança): 8 em 1 arquivo(s), 26 linhas
- Alcance total: 12 unidades; usa 0 compartilhadas de outras; é dona principal de 4 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/fieldReport.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/assert.ts
- 10-10 `__SLOW_CHECKS__` (variable): RUNTIME CHECKS IN TIERS (Unreal's check/checkSlow/ensure; SQLite's asserts, ~3x slower when on, off in release; TigerBeetle's paired assertions): what no test builds - a derived index out of step with its source, a value that should never 
- 12-14 `AssertionError` (class)
- 13-13 `AssertionError.name` (field)
- 17-20 `assert` (function): Throws when `condition` is false (development only: guard the call with `import.meta.env.DEV` when the condition costs anything).
- 23-24 `slowChecks` (variable): The O(n) sweeps are run: development builds asked for them at build time or in the address.
- 28-28 `ProblemLog.told` (field)
- 37-49 `ProblemLog.ensure` (method): True when `condition` holds; otherwise reported once per message in the session and false, the caller carrying on.
- 56-56 `ensure` (function): `problems.ensure`: a condition the game survives but must hear of.
