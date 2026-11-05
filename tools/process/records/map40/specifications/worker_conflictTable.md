# worker:conflictTable

Grupo: worker. Entrada: worker. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/conflictTable.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 152 linhas
- Alcance total: 387 unidades; usa 376 compartilhadas de outras; é dona principal de 10 compartilhadas

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
- Usa: `world:conflict` (16), `worker:bays` (6), `world:walking` (2), `world:turn` (2), `world:junction` (1)
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

### src/conflictTable.worker.ts
- 13-164 `serve` (function): This service of the job system (`job.worker.ts`): its messages answered on the scope it is given.
