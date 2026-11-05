# worker:roadTile

Grupo: worker. Entrada: worker. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/roadTile.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 8 em 1 arquivo(s), 92 linhas
- Alcance total: 307 unidades; usa 299 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:road` (13), `worker:bays` (10), `render:mesh` (3), `render:derived` (2), `render:ecology` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `meshoptimizer`

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/render/roadTile.worker.ts
- 9-9 `KeepAsk` (type): Where a share's tiles are kept (`roadTilePool.ts` TileJob.keep).
- 10-13 `Message` (type)
- 16-16 `BEAT_MS` (variable): Most time between two words to the page while working: the pool hears the worker is alive.
- 18-18 `WRITES_MOST` (variable): The tiles' writes this worker has handed the store and not seen committed, bytes: past it the next tile waits for them, its answer held (DO NOT post them to the page to keep: past the page's ceiling a tile was dropped and made again next ti
- 19-19 `writes` (variable)
- 22-38 `keepTiles` (function): Each tile of `tiles` with a key kept in the store before its arrays leave (IndexedDB clones the value at `put`); the places of those kept.
- 42-99 `handler` (function): The worker's answer to the page's messages, holding the shares that wait on the page for the ground's heights.
- 102-110 `serve` (function): The road tiles' service of the job system (`job.worker.ts`).
