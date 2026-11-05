# worker:citizenBake

Grupo: worker. Entrada: worker. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/citizenBake.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 10 em 1 arquivo(s), 111 linhas
- Alcance total: 353 unidades; usa 340 compartilhadas de outras; é dona principal de 3 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:{ id, clips: [clip ? sent(clip, transfer`, `envia:{ id, clips: out } satisfies BakeReply`, `envia:{ id, error: String(error) } satisfies B`, `envia:{ id, heights, face: rows } satisfies Ba`, `envia:{ id, klass: made } satisfies BakeReply`, `envia:{ id, person } satisfies BakeReply`, `envia:{ id, pixels } satisfies BakeReply`, `envia:{ id, worn } satisfies BakeReply`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:people` (21), `render:citizen` (20), `people:body` (17), `worker:bays` (12), `people:spec` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): `uncook`
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `people`: não se aplica 28

## Código próprio (linhas — declaração — comentário)

### src/render/citizenBake.worker.ts
- 20-20 `ExpressionStatement` (statement): Nothing to yield to here: a bake runs to its end.
- 23-39 `Job` (interface): The core clips of a body; one clip baked on first use (`at`), timed by the body's shuffle (`facts`); or a cooked person's file to read (`uncook`).
- 65-65 `morpher` (variable): The MakeHuman packs, on the people's worker only.
- 67-67 `bakeEnv` (variable): The procedural classes' rigs made here, kept for the items fitted to them (`wear`).
- 68-68 `classRigs` (variable)
- 69-83 `classRigOf` (function)
- 86-86 `libraries` (variable): Reuse decoded clips across this worker's jobs.
- 89-91 `serve` (function): The bakes' service of the job system (`job.worker.ts`).
- 94-98 `sent` (function): A clip as it goes to the page: its arrays moved, its head bone as numbers.
- 100-165 `bakeJob` (function)
