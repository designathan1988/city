# editor:streetscapeTool

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/streetscapeTool.ts`
- Unidades próprias (só esta funcionalidade alcança): 13 em 1 arquivo(s), 204 linhas
- Alcance total: 554 unidades; usa 541 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#drawStreetscapeHover@2808` (4), `src/main.ts#endPointer@1484` (2), `src/main.ts#streetscapeTool@502` (1), `src/main.ts#bulldozer@1105` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#pointerMove@1404` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:landscape` (57), `worker:conflictTable` (18), `world:doc` (14), `editor:barriers` (9), `world:roads` (8), `world:road` (3), `world:network` (2), `world:ribbons` (2), `editor:buildingSnap` (1), `editor:snap` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.approachRules`, `RoadNode.control`, `RoadNode.id`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/signRule.spec.ts`, `tests/editor/streetscapeTool.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não se aplica 26
- porte `street-furniture`: portado 28

## Código próprio (linhas — declaração — comentário)

### src/editor/streetscapeTool.ts
- 13-25 `StreetscapeToolHost` (interface): What the landscaping tool reads of the game and does to it.
- 28-112 `StreetscapeTool` (class): THE LANDSCAPING TOOL: benches, bins, street lights, trees, signs... placed on the footways
- 30-30 `StreetscapeTool.hover` (field): Where the chosen item would go under the pointer: a ring, red where it cannot.
- 32-32 `StreetscapeTool.row` (field): A row being dragged out from the item just placed (placement in line)
- 34-34 `StreetscapeTool.constructor` (method)
- 37-39 `StreetscapeTool.reach` (method): Pick radius for a placed item and the reach of the footway snap, world units.
- 42-84 `StreetscapeTool.down` (method): @perInput Runs once an input on one thing (a press, a release, a click): nothing it reaches may walk the map.
- 87-97 `StreetscapeTool.move` (method): @perInput `pressed`: the button still down, dragging a row out from the item just placed.
- 100-111 `StreetscapeTool.up` (method): The button let go: the row dragged out is laid, one undo step.
- 115-115 `ROW_SPACING` (variable): The spacing a row of each kind is laid at: the catalogue's (`world/roads/furnitureCatalog.ts`).
- 118-141 `rowPieces` (function): The pieces after `from` along its footway, toward the pointer, each where the tool would accept it.
- 144-151 `itemsAround` (function): DO NOT hand it `doc.landscape.values()`
- 154-154 `MEADOW_CLEAR` (variable): How far a meadow keeps from another (`snapLandscape`), and more than any gap between two items.
