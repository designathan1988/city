# editor:transitTools

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/transitTools.ts`
- Unidades próprias (só esta funcionalidade alcança): 31 em 1 arquivo(s), 409 linhas
- Alcance total: 291 unidades; usa 259 compartilhadas de outras; é dona principal de 1 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#drawOverlayScreen@2942` (2), `src/main.ts#transitEditor@854` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#pointerMove@1404` (1), `src/main.ts#doubleClick@1207` (1), `src/main.ts#gameKeys@2479` (1), `src/main.ts#mountInterface@1859` (1), `src/main.ts#ExpressionStatement@861` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `recebe:line`, `recebe:station`, `recebe:stop`, `recebe:terminal`, `recebe:track`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:transit` (52), `worker:conflictTable` (41), `editor:barriers` (5), `world:buildings` (4), `world:doc` (3), `world:network` (1), `world:footway` (1), `world:carriageway` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadDoc.transit`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: não se aplica 2
- porte `transit`: portado 50, não se aplica 12

## Código próprio (linhas — declaração — comentário)

### src/editor/transitTools.ts
- 16-16 `TransitToolKind` (type): The public transport tool (`world/transit.ts`), as Cities: Skylines II lays it out
- 18-25 `TransitToolDeps` (interface)
- 28-28 `PICK` (variable): How near a click must be to a stop or a track to pick it, u.
- 30-30 `STOP_REACH` (variable): How near a street a bus stop can be put.
- 32-244 `TransitTool` (class)
- 33-33 `TransitTool.kind` (field)
- 34-34 `TransitTool.rail` (field)
- 36-36 `TransitTool.points` (field): The run of a track being laid.
- 38-38 `TransitTool.lineStops` (field): The stops of a line being made.
- 39-39 `TransitTool.cursor` (field)
- 41-41 `TransitTool.constructor` (method)
- 43-49 `TransitTool.setKind` (method)
- 51-51 `TransitTool.data` (method)
- 54-61 `TransitTool.built` (method): DO NOT walk `buildings.all()`: the metro's way down asked it of every footway point of the map against every building.
- 64-66 `TransitTool.footwayAt` (method): DO NOT build the walking network of the whole map for a click
- 69-69 `TransitTool.onRoad` (method): Whether a point is on a road's carriageway.
- 72-74 `TransitTool.stopAt` (method): The stop or station nearest a point within reach, of a mode if given (the stops round it, `stopNear`).
- 77-83 `TransitTool.snapTrack` (method): @perInput A track point (an end or a corner) within joining reach, or the point itself.
- 86-137 `TransitTool.click` (method): @perInput Runs once an input on one thing (a press, a release, a click): nothing it reaches may walk the map.
- 139-139 `TransitTool.move` (method)
- 142-149 `TransitTool.key` (method): Enter ends a run or a line, Backspace takes the last point back, Escape drops it. True when the key was taken.
- 151-164 `TransitTool.finishTrack` (method)
- 166-173 `TransitTool.finishLine` (method)
- 176-179 `TransitTool.preview` (method): The run being laid, to the pointer, for the scene to build as track; null when none.
- 182-182 `TransitTool.lines` (method): The lines laid out, for the panel.
- 184-184 `TransitTool.setVehicles` (method)
- 185-185 `TransitTool.removeLine` (method)
- 188-243 `TransitTool.draw` (method): The plan over the map: tracks, lines, stops and stations, and what is being laid.
- 246-246 `current` (variable)
- 248-248 `transitTool` (function): The tool, for the game's panel (`ui/v2/shell.ts`).
- 249-249 `setTransitTool` (function)
