# worker:job

Grupo: worker. Entrada: worker. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/job.worker.ts`, `src/jobs/jobPool.ts`
- Unidades próprias (só esta funcionalidade alcança): 21 em 3 arquivo(s), 163 linhas
- Alcance total: 139 unidades; usa 15 compartilhadas de outras; é dona principal de 103 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#IfStatement@209` (1), `src/main.ts#IfStatement@210` (1), `src/main.ts#IfStatement@4036` (1), `src/main.ts#simLanes@662` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`, `envia:data`, `envia:message`, `envia:{ adopt: service, port: channel.port1 } `, `envia:{ close: this.id } satisfies CloseLane`
- Workers: `src/job.worker.ts`
- Ganchos do three.js: -
- Timers: 1
- Usa: `worker:bays` (9), `editor:workerParts` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/conflictPool.spec.ts`, `tests/core/jobPool.spec.ts`, `tests/render/cellPool.spec.ts`, `tests/render/landStroke.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `build`: não se aplica 2
- porte `interior`: não se aplica 2
- porte `landscape`: já existia 10
- porte `roads`: não se aplica 4

## Código próprio (linhas — declaração — comentário)

### src/job.worker.ts
- 6-6 `Serve` (type): ONE WORKER OF THE JOB SYSTEM (`jobs/jobPool.ts`): at the root with the other entries, since it puts every service together: every service's ports in it, each service's module loaded on its first message
- 8-35 `LOADERS` (variable)
- 37-37 `host` (variable)
- 40-53 `adopt` (function): A lane another thread holds the other end of (`jobPool.ts` jobChannel): its service served on the port; a service that cannot load says so on it (`laneFailed`: the pool falls back).
- 55-55 `Port` (interface): A port's scope once its service is loaded; the messages that came before, in order; the queued job it is on (`seq`); why its service never loaded.
- 56-56 `ports` (variable)
- 58-58 `openPorts` (function): How many ports this worker holds an entry for (the tests: a lane let go leaves none).
- 61-62 `answer` (function): A port's answer, with the queued job it is for: a queue port runs one job at a time (`jobPool.ts` `submitJob`).
- 64-94 `ExpressionStatement` (statement)

### src/jobs/jobPool.ts
- 83-85 `JobQueue.waiting` (method): Jobs waiting for a worker.
- 88-90 `JobQueue.submitted` (method): Jobs ever asked of the queue (read only: the debugging lab).
- 247-247 `JobSystem.tilesWaiting` (field): Tiles of the map waiting for the long side's lanes (`tilesWaiting`): its workers take no queued job meanwhile.
- 252-254 `setJobWorkerMaker` (function): Where the pool's workers are made, by side (`long`: tiles and growth); null from it: the page's own Worker. On the desktop the long side goes to a utility process outside the renderer's V8 cage (`jobs/processWorker.ts`), whose `terminate` e
- 257-260 `setJobWorkerSetup` (function): Told to every worker of the pool, those made and those to come, before its first job: on the desktop, the port it reads and writes the tiles through (`editor/workerParts.ts`).
- 494-508 `JobWorkerStats` (interface): One worker of the pool as the debugging lab reads it (`lab/resources`).
- 511-531 `jobPoolStats` (function): The pool as it stands, read only: its workers, the queue's waiting jobs, the most it may hold (the debugging lab's consumers; nothing is changed).
- 614-620 `JobPort.postMessage` (method)
- 644-658 `JobPort.terminate` (method): The lane let go (its service's state stays in the worker for the next lane of the kind).
- 674-682 `jobChannel` (function): A LANE OF THIS PAGE'S POOL HANDED TO ANOTHER THREAD (the simulation's worker): a channel, one end served by a worker of the pool (`job.worker.ts` adopt), the other returned to be transferred; that thread and the worker talk straight. DO NOT

### src/jobs/jobScope.ts
- 15-15 `AdoptLane` (interface): Page to job worker: serve `adopt` on `port`, one end of a channel whose other end another thread holds (`jobPool.ts` jobChannel): that thread talks to this worker straight, no page in between.
- 17-17 `CloseLane` (interface): Page to job worker: a lane let go (`JobPort.terminate`), its port's entry dropped there.
