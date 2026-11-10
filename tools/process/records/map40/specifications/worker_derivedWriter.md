# worker:derivedWriter

Grupo: worker. Entrada: worker. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/derivedWriter.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 2 arquivo(s), 52 linhas
- Alcance total: 12 unidades; usa 7 compartilhadas de outras; é dona principal de 1 compartilhadas

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
- Usa: `worker:bays` (4), `worker:conflictTable` (4)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/writeBudget.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: não se aplica 2
- porte `interior`: não se aplica 1

## Código próprio (linhas — declaração — comentário)

### src/core/derivedCache.ts
- 260-273 `keepDerivedManyCommitted` (function): As `keepDerivedMany`, resolved once the transaction has committed (or failed): what the derived writer answers on (`derivedWriter.worker.ts`).

### src/render/derivedWriter.worker.ts
- 5-7 `DerivedAsk` (type): A write to keep (answered `done` on its commit) or a read (answered `got` with the entries found).
- 10-20 `buffersOf` (function): The array buffers under `value`, each once (a shared one left: it cannot be handed over).
- 23-46 `serve` (function): This service of the job system (`job.worker.ts`): its messages answered on the scope it is given.
