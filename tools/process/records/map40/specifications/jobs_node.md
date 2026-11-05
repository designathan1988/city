# jobs:node

Grupo: jobs. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/jobs/nodeJobHost.ts`
- Unidades próprias (só esta funcionalidade alcança): 12 em 1 arquivo(s), 49 linhas
- Alcance total: 19 unidades; usa 7 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:message`
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:unshared(message)`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: `jobs:process` (4)

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `node:worker_threads`

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (arquivo:linhas — declaração — comentário)

### src/jobs/nodeJobHost.ts
- 6-6 `Listener` (type): A JOB WORKER AS A THREAD OF THE DESKTOP'S JOB PROCESS (`desktop/jobs.cjs`): the page's `job.worker.ts` as it is, its `self` the thread's port (Node `worker_threads` parentPort), built for Node (`scripts/vite/desktop-jobs.config.ts`)
- 9-34 `ThreadSelf` (class): The thread's `self` (`job.worker.ts` reads it as a worker's): its listener, and the messages that came before the worker's module set it, handed to it in order once it does.
- 10-10 `ThreadSelf.listener` (field)
- 11-11 `ThreadSelf.early` (field)
- 13-13 `ThreadSelf.kept` (field): The buffers the page sent once and names since (`processWorker.ts` KeptBuffer): its record tables.
- 15-22 `ThreadSelf.constructor` (method)
- 24-24 `ThreadSelf.onmessage` (method)
- 25-28 `ThreadSelf.onmessage` (method)
- 31-33 `ThreadSelf.postMessage` (method): DO NOT post shared memory to the page: the shell's port carries none (a thread's SharedArrayBuffer is its process's, HTML "StructuredSerialize"), the city's table (`recordTable.ts` allocate, shared where `crossOriginIsolated` is not false -
- 36-36 `IfStatement` (statement)
- 37-37 `ExpressionStatement` (statement)
- 38-38 `ExpressionStatement` (statement)
