# core:game

Grupo: core. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/gameState.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 9 linhas
- Alcance total: 24 unidades; usa 0 compartilhadas de outras; é dona principal de 21 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 2
- Interage com: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/gameState.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/gameState.ts
- 48-50 `GameState.get` (method)
- 123-125 `GameState.version` (method): The serial of the latest change: what a reader that has seen everything holds.
- 128-130 `GameState.since` (method): The changes after `since`, of these keys only when given; `null` if some were forgotten.
