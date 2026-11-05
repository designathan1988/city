# worker:groundShape

Grupo: worker. Entrada: worker. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/groundShape.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 17 linhas
- Alcance total: 419 unidades; usa 418 compartilhadas de outras; é dona principal de 0 compartilhadas

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
- Usa: `worker:bays` (3), `render:ground` (3)
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
- porte `landscape`: não se aplica 4

## Código próprio (linhas — declaração — comentário)

### src/render/groundShape.worker.ts
- 5-21 `serve` (function): This service of the job system (`job.worker.ts`): its messages answered on the scope it is given.
