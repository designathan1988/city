# core:statechart

Grupo: core. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/statechart.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 21 unidades; usa 0 compartilhadas de outras; é dona principal de 20 compartilhadas

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
- `tests/core/statechart.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/statechart.ts
- 59-61 `Machine.in` (method): Whether `id` is the active state or one of its parents.
