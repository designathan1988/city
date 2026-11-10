# editor:growthReplica

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/growthReplica.ts`
- Unidades próprias (só esta funcionalidade alcança): 9 em 1 arquivo(s), 23 linhas
- Alcance total: 676 unidades; usa 634 compartilhadas de outras; é dona principal de 33 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:changes` (8), `world:buildings` (6), `world:ids` (5), `world:cells` (2), `world:road` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/growthWorker.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: não se aplica 6

## Código próprio (linhas — declaração — comentário)

### src/editor/growthReplica.ts
- 112-112 `within` (function): Whether rectangle `a` lies within `b`.
- 155-159 `NeighbourReplica.near` (method)
- 175-175 `RoadReplica.doc` (field)
- 176-176 `RoadReplica.ribbons` (field)
- 177-177 `RoadReplica.junctions` (field)
- 178-178 `RoadReplica.segmentCells` (field)
- 179-179 `RoadReplica.junctionCells` (field)
- 181-188 `RoadReplica.ribbonsMeeting` (method)
- 190-193 `RoadReplica.check` (method)
