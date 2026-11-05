# src:conflict

Grupo: raiz. Entrada: ligação. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/conflictPool.ts`
- Unidades próprias (só esta funcionalidade alcança): 15 em 1 arquivo(s), 213 linhas
- Alcance total: 109 unidades; usa 87 compartilhadas de outras; é dona principal de 7 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:corners`, `envia:finish`, `envia:junctions`, `envia:pairs`, `envia:sweeps`, `envia:turns`
- Workers: -
- Ganchos do three.js: -
- Timers: 1
- Usa: `world:conflict` (55), `worker:job` (3), `world:walking` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `people`: não se aplica 2
- porte `traffic`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/conflictPool.ts
- 16-16 `SILENCE_MS` (variable): DO NOT take a silent worker for a dead one: it shares the job pool's workers and the CPU share; a failure comes as an error (`onerror`, an `error` answer).
- 19-19 `MOST_LANES` (variable): Most workers the pool opens: past four, a city's pairs are done before the copies are.
- 24-24 `ConflictPool.constructor` (method): `ports`: the page's lanes for this thread (`jobPool.ts` jobChannel, the simulation's worker): DO NOT open a pool here when given them. None: lanes of this thread's own pool.
- 26-28 `ConflictPool.keeps` (method)
- 32-32 `ConflictPool.nextId` (field)
- 34-59 `ConflictPool.measure` (method)
- 61-69 `ConflictPool.ask` (method)
- 72-100 `ConflictPool.chooseTurns` (method): Junction turns chosen across the workers (`conflictTable.worker.ts`, `chooseTurnOf`)
- 103-133 `ConflictPool.sampleSweeps` (method): Movements' sweeps sampled across the workers (`conflictTable.worker.ts`, `sampleSweep`)
- 136-162 `ConflictPool.finishPairs` (method): Pairs finished across the workers (`conflictTable.worker.ts`, `finishPair`)
- 165-181 `ConflictPool.buildJunctions` (method): A whole build's junctions built across the workers (`conflictTable.worker.ts`, `buildJunctionOf`)
- 184-200 `ConflictPool.roundCorners` (method): Walkways' corners rounded across the workers (`conflictTable.worker.ts`, `roundCorners`)
- 203-208 `ConflictPool.arm` (method): The lane's silence report started again, or stopped when nothing waits on it: said, and still waited for (measuring on the page would stall the simulation).
- 210-249 `ConflictPool.start` (method)
- 265-268 `ConflictPool.dispose` (method): Ends the workers, every job still waiting rejected (`fail`; the page going away).
