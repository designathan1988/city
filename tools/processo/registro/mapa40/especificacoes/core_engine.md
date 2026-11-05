# core:engine

Grupo: core. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/engine.ts`
- Unidades próprias (só esta funcionalidade alcança): 6 em 1 arquivo(s), 20 linhas
- Alcance total: 24 unidades; usa 0 compartilhadas de outras; é dona principal de 18 compartilhadas

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
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/engine.ts
- 7-7 `EngineInput.serialNow` (field)
- 9-9 `EngineInput.constructor` (method)
- 12-14 `EngineInput.serial` (method): The serial of its latest change.
- 17-20 `EngineInput.touch` (method): It changed.
- 83-90 `Engine.unregister` (method): Takes a product out (its owner disposed of).
- 117-119 `Engine.idle` (method): The products of a phase that computed in its last refresh: with no input changed, each is a product computing for nothing.
