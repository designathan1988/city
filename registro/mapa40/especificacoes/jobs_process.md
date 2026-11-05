# jobs:process

Grupo: jobs. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/jobs/processWorker.ts`
- Unidades próprias (só esta funcionalidade alcança): 14 em 1 arquivo(s), 59 linhas
- Alcance total: 29 unidades; usa 0 compartilhadas de outras; é dona principal de 15 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:typeof MessagePort !== 'undefined' && po`, `envia:{ forgetKept: [id] } satisfies ForgetKep`
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
- `tests/desktop/jobs.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (arquivo:linhas — declaração — comentário)

### src/jobs/processWorker.ts
- 13-13 `ThreadFailed` (interface): What the host in the other process posts when its thread failed or stopped (`desktop/jobs.cjs`).
- 14-15 `isThreadFailed` (function)
- 105-105 `keptIds` (variable): Each buffer kept by job threads: its id, and the workers whose thread holds it.
- 106-106 `nextKept` (variable)
- 109-109 `collected` (variable): A kept buffer collected on the page: let go in every thread holding it (`ForgetKept`).
- 113-113 `ProcessWorker.onmessage` (field)
- 115-115 `ProcessWorker.onmessageerror` (field)
- 120-120 `ProcessWorker.given` (field): The kept buffers its thread holds (`KeptBuffer`).
- 123-135 `ProcessWorker.constructor` (method): `keeps`: which shared buffers are written once and never again (a record table), kept by the thread once sent (`KeptBuffer`).
- 137-141 `ProcessWorker.postMessage` (method)
- 143-149 `ProcessWorker.terminate` (method)
- 163-165 `ProcessWorker.forget` (method): A kept buffer collected on the page: its thread lets it go.
- 168-177 `ProcessWorker.kept` (method): A buffer as this thread is sent it: its token, with a copy of its bytes the first time; null for one not kept (copied with the message).
- 179-190 `ProcessWorker.send` (method)
