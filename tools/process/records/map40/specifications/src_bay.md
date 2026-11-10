# src:bay

Grupo: raiz. Entrada: ligação. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/bayPool.ts`
- Unidades próprias (só esta funcionalidade alcança): 9 em 1 arquivo(s), 71 linhas
- Alcance total: 101 unidades; usa 85 compartilhadas de outras; é dona principal de 7 compartilhadas

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
- Usa: `sim:agents` (6), `worker:job` (3)
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
- porte `traffic`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/bayPool.ts
- 17-17 `SILENCE_MS` (variable): DO NOT take a silent worker for a dead one: it shares the job pool's workers and the CPU share; a failure comes as an error (`onerror`, an `error` answer).
- 20-20 `MOST_LANES` (variable): Most workers the pool opens.
- 24-24 `BayPool.constructor` (method): `ports`: the page's lanes for this thread (`jobPool.ts` jobChannel, the simulation's worker): DO NOT open a pool here when given them. None: lanes of this thread's own pool (the page).
- 28-28 `BayPool.nextId` (field)
- 30-30 `BayPool.turn` (field): The lane the next ask starts on: a few lots asked at a time go round the workers.
- 32-50 `BayPool.work` (method)
- 52-59 `BayPool.check` (method)
- 61-95 `BayPool.start` (method)
- 111-114 `BayPool.dispose` (method): Ends the workers, every job still waiting rejected.
