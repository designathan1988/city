# core:commands

Grupo: core. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/commands.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 9 linhas
- Alcance total: 15 unidades; usa 0 compartilhadas de outras; é dona principal de 12 compartilhadas

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
- `tests/core/commands.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: portado 4

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/commands.ts
- 53-55 `CommandRegistry.has` (method)
- 58-60 `CommandRegistry.all` (method): Every command, in the order registered.
- 63-65 `CommandRegistry.bindings` (method): Every key rule, in the order bound.
