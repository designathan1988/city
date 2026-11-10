# render:sign

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/signLayer.ts`
- Unidades próprias (só esta funcionalidade alcança): 19 em 1 arquivo(s), 128 linhas
- Alcance total: 587 unidades; usa 557 compartilhadas de outras; é dona principal de 11 compartilhadas

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
- Usa: `world:landscape` (33), `world:changes` (20), `world:doc` (19), `world:network` (16), `world:roads` (15), `editor:barriers` (13), `worker:conflictTable` (12), `world:section` (8), `world:ribbons` (7), `world:cells` (7), `world:road` (5), `editor:buildings` (4)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`, `RoadDoc.nodes`, `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/signLayer.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `people`: já existia 2
- porte `street-furniture`: portado 96

## Código próprio (linhas — declaração — comentário)

### src/render/signLayer.ts
- 41-41 `ITEM_READS` (variable): How far round a placed sign or a street's name the footway it stands on is looked for (`streetChain`'s reach).
- 43-44 `unite` (function)
- 45-45 `around` (function)
- 46-46 `grow` (function)
- 49-54 `ribbonBox` (function): The box a segment's ribbon covers, with its casing and footways.
- 56-56 `isSignItem` (function)
- 59-59 `nodeKey` (function): A junction's key and a sign item's: one number (`CellFiling` files numbers), even and odd.
- 60-60 `itemKey` (function)
- 64-64 `SignProduct.name` (field)
- 66-66 `SignProduct.kinds` (field): DO NOT leave it out: the plates of the old standard stood after the style changed.
- 67-67 `SignProduct.byId` (field)
- 69-69 `SignProduct.filing` (field): Each key holding signs, by the ground it reads.
- 73-79 `SignProduct.allKeys` (method): @rebuild Every junction and every sign item of the map: read only when everything changed (a load, the diary forgotten).
- 81-110 `SignProduct.dirty` (method)
- 112-115 `SignProduct.compute` (method)
- 117-123 `SignProduct.equals` (method)
- 125-128 `SignProduct.changed` (method)
- 137-152 `junctionSigns` (function): A junction's signs (`signsAtNode`), reading its segments' ground.
- 155-196 `itemSigns` (function): A placed sign's plate, or a street name's plates at both ends of every segment of its street.
