# src:growth

Grupo: raiz. Entrada: ligação. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/growthPool.ts`
- Unidades próprias (só esta funcionalidade alcança): 15 em 1 arquivo(s), 139 linhas
- Alcance total: 680 unidades; usa 665 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#growthPool@1993` (1), `src/main.ts#frame@2644` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:ask`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `editor:growthReplica` (11), `editor:growthFeed` (1), `worker:job` (1)
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
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/growthPool.ts
- 8-8 `SILENCE_MS` (variable): Silence past which a worker with a lot waiting is reported, once a stretch, and still waited for (one lot is milliseconds; the module's first load, some hundred).
- 10-86 `GrowthPool` (class)
- 11-11 `GrowthPool.worker` (field)
- 13-13 `GrowthPool.local` (field): No worker to be had: the replica on the page, and the ask it answers at the next frame.
- 14-14 `GrowthPool.pending` (field)
- 16-16 `GrowthPool.held` (field): An answer not taken yet (`take` said no).
- 18-18 `GrowthPool.postedAt` (field): When the lot waiting on the worker was posted; null: none waits.
- 20-20 `GrowthPool.told` (field): Whether this stretch of silence was reported.
- 23-23 `GrowthPool.constructor` (method): `take`: an answer came, false to offer it again next frame; `restarted`: the replica is new, all to be sent again.
- 25-32 `GrowthPool.post` (method)
- 35-48 `GrowthPool.pump` (method): Once a frame: an answer held offered again, the page's replica's lot answered, a silent worker reported (and still waited for).
- 51-55 `GrowthPool.dispose` (method): Ends the worker (the page going away).
- 57-59 `GrowthPool.offer` (method)
- 61-77 `GrowthPool.start` (method)
- 79-85 `GrowthPool.fail` (method)
