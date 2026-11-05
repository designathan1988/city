# ui:v2

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/v2/compass.ts`, `src/ui/v2/shell.ts`, `src/ui/v2/start.ts`, `src/ui/v2/transitLine.ts`
- Unidades próprias (só esta funcionalidade alcança): 34 em 4 arquivo(s), 2201 linhas
- Alcance total: 517 unidades; usa 483 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `action.about`, `action.newLargeWorld`, `action.newMap`, `action.newWorld`, `action.openMap`, `action.redo`, `action.saveMap`, `action.undo`, `atmo.title`, `barrier.kind.…`, `builder.advanced.off`, `builder.advanced.on`, `builder.category.models`, `builder.category.…`, `builder.city.all`, `builder.city.mine`, `builder.city.…`, `builder.drawAction.…`, `builder.field.pitch`, `builder.floor.title`, `builder.grid`, `builder.help`, `builder.hideOthers`, `builder.lot.…`, `builder.menu.app`, `builder.menu.simulation`, `builder.needsSelection`, `builder.plan.back`, `builder.plan.cancel`, `builder.plan.finish`, `builder.roof.ridge.label`, `builder.roof.ridge.…`, `builder.roof.side.label`, `builder.roof.side.…`, `builder.search`, `builder.search.none`, `builder.section.drawWhere.short`, `builder.section.styles`, `builder.section.…`, `builder.snap.label`, `builder.snap.…`, `builder.tool.…`, `builder.view.frame`, `building.finish.…`, `building.fn.…`, `building.style.…`, `camera.centre`, `camera.label`, `camera.zoom`, `cloud.clear`, `cloud.scatter`, `cloud.spread`, `cloud.…`, `compass.label`, `creator.dock.scope.…`, `creator.pattern.…`, `cutSides.row`, `el.clear`, `el.effects`, `el.kind.…` … (+145)
- Eventos de entrada: `ouve:keydown`, `ouve:pointercancel`, `ouve:pointerdown`, `ouve:pointermove`, `ouve:pointerout`, `ouve:pointerover`, `ouve:pointerup`, `ouve:resize`, `ouve:scroll`, `ouve:wheel`
- Chaves de localStorage: `escreve:roadcraft.builder.advanced`, `le:roadcraft.builder.advanced`

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#setZone@1617` (18), `src/main.ts#mountInterface@1859` (4), `src/main.ts#updateCompass@1950` (2), `src/main.ts#gameKeys@2479` (2), `src/main.ts#compass@1948` (1), `src/main.ts#mapSlider@1655` (1), `src/main.ts#brushSlider@1670` (1), `src/main.ts#weatherSlider@1675` (1), `src/main.ts#SLIDERS@1715` (1), `src/main.ts#statusListeners@3835` (1), `src/main.ts#currentStatus@3837` (1), `src/main.ts#roadClasses@1558` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 6
- Usa: `ui:i18n` (294), `ui:builder` (157), `ui:toolChoices` (100), `world:buildings` (37), `worker:conflictTable` (18), `ui:roads` (18), `world:transit` (12), `view:viewport` (9), `world:elements` (5), `world:landscape` (4), `world:terrain` (3), `ui:labels` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/ui/transitLine.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `interface`: portado 174, outra sessão 66, não se aplica 12
- porte `economy`: já existia 2
- porte `landscape`: portado 8
- porte `street-furniture`: portado 2
- porte `transit`: portado 4

## Código próprio (linhas — declaração — comentário)

### src/ui/v2/compass.ts
- 7-7 `toNorth` (function): THE COMPASS, a corner of the screen (Google Earth's and Cesium's navigation widget, Cities: Skylines' north arrow)
- 10-15 `northOnScreen` (function): Where north lies on the screen from the middle of the view: radians clockwise from straight up.
- 18-28 `groundAngleToNorth` (function): The angle on the ground, at the view's centre, from the way the screen's up points to north
- 31-33 `northTurn` (function): The turn (`Viewport.orbit`'s azimuth, radians) still wanted to bring north straight up the screen, as the view stands
- 35-40 `CompassHandlers` (interface)
- 42-45 `Compass` (interface)
- 48-48 `DRAG_TURN` (variable): Radians a CSS pixel of drag across the compass turns or tilts the camera.
- 49-49 `DRAG_TILT` (variable)
- 51-51 `CLICK_SLOP` (variable): A press that moves less than this is a click.
- 53-117 `mountCompass` (function)

### src/ui/v2/shell.ts
- 27-31 `PAINT_SWATCH` (variable): The colour of each paintable ground, for its button.
- 33-33 `BIOME_SWATCHES` (variable): The biomes, painted or chosen for the whole map (`world/ecology.ts`).
- 54-54 `Category` (type)
- 56-59 `SWATCHES` (variable)
- 60-60 `hexOf` (function)
- 62-246 `ICON` (variable)
- 247-248 `svg` (function)
- 250-255 `el` (function)
- 256-264 `button` (function)
- 267-267 `TransitToolKind` (type): What the public transport tool is set to and does, as the panel shows it (`editor/transitTools.ts`).
- 268-276 `ShellTransitTool` (interface)
- 280-307 `ShellGameValues` (interface): The game's state as the shell reads it (`main.ts` `gameState`)
- 308-311 `ShellGame` (interface)
- 313-321 `ShellSlider` (interface): A number the player sets with a slider: its range, and how the game reads and takes it.
- 323-335 `ShellStatus` (interface): The city's numbers, as `main.ts` computes them for the top bar and the simulation menu.
- 337-343 `ShellReadouts` (interface): The map's own numbers the old status bar showed (`main.ts`): its roads and nodes, the zoom
- 345-352 `ShellZonePatch` (interface): What a lot or zone setting is changed to (`main.ts` `setZone`).
- 354-361 `ShellRoadClass` (interface): A road class, as the upgrade tool shows it.
- 363-434 `ShellDeps` (interface)
- 437-444 `ShellHandle` (interface): What the game asks of the shell.
- 446-2084 `mountShell` (function)

### src/ui/v2/start.ts
- 5-12 `StartDeps` (interface): THE TITLE SCREEN: what the player sees first - play on (the map saved, or a blank one), open a map, start a new one, the settings
- 15-76 `mountStart` (function): The title screen over the game, gone at the first choice (Enter or Escape: play on).

### src/ui/v2/transitLine.ts
- 5-16 `transitLineRow` (function): DO NOT build this row with innerHTML: a name `<img src=x onerror=…>` ran its script when the panel opened.
