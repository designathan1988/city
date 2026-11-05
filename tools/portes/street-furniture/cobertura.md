# Cobertura do rastreamento street-furniture

Uma linha por arquivo e por declaração própria de `feature.md` (843 declarações, 63 arquivos). Gerado por `evidence/tools/cobertura.py`. Estado das declarações: já existia: 164, não se aplica: 47, parcial: 59, portado: 573.

## Arquivos

| Arquivo | Tipo | O que faz para o jogador | Onde mora | Estado | Motivo |
|---|---|---|---|---|---|
| `src/actionsWiring.ts` | part 35 | explosão derruba postes | `porte demolition` | não se aplica | dono: demolition |
| `src/editor/barriers.ts` | whole 175 | encaixe na calçada e motivo de recusa | `sim/commands/street_snap.cpp` | portado |  |
| `src/editor/bulldozer.ts` | part 16 | demolir poste, barreira e item | `sim/commands/street_commands.cpp + demolish_at` | portado |  |
| `src/editor/chunkedSave.ts` | part 14 | gravar postes, vãos e barreiras | `sim/persistence/save_write.cpp` | portado | um bloco no save |
| `src/editor/cityGenerator.ts` | part 5 | mobiliário da cidade gerada | `sim/roads/street_furniture.cpp` | já existia | derivado por regra |
| `src/editor/poleRoutes.ts` | whole 199 | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poles.ts` | whole 516 | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/roads/powerLine.ts` | part 18 | linha de energia de uma rua nova | `render/props/street_props.cpp` | parcial | fileira derivada, ligada nas juntas; a linha derivada não cruza a rua |
| `src/editor/roads/streetKit.ts` | part 11 | o kit acompanha a edição da via | `sim/roads/street_furniture.cpp + street_commands` | portado | derivado acompanha sozinho; itens são movidos |
| `src/editor/roadTool.ts` | part 19 | via nova com mobiliário | `sim/commands/road_commands.cpp` | portado |  |
| `src/editor/streetObjects.ts` | whole 106 | travessia no meio da quadra e retorno no canteiro | `sim/commands, porte roads` | não se aplica | fora do escopo; a declaração é própria do rastreamento roads, que a porta |
| `src/editor/streetscapeTool.ts` | whole 155 | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/lab/catalog/layers.ts` | part 15 | laboratório de catálogo do 4.0 | `-` | não se aplica | ferramenta de laboratório |
| `src/main.ts` | part 232 | ligação das ferramentas e prévias | `ToolController + hud_drawer` | portado |  |
| `src/render/agents.ts` | part 7 | agentes desviando do mobiliário | `render/agents` | não se aplica | dono: people e vehicles |
| `src/render/barriers.ts` | whole 258 | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/buildings/kit.ts` | part 15 | luz de porta dos prédios | `render/buildings` | não se aplica | dono: buildings |
| `src/render/cellInstances.ts` | part 66 | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/flatAtlas.ts` | part 4 | atlas plano do 4.0 | `-` | não se aplica | sem atlas plano no jogo |
| `src/render/gardens.ts` | part 78 | jardins | `porte landscape` | não se aplica | dono: landscape |
| `src/render/grass.ts` | whole 98 | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/instanceRuns.ts` | part 4 | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/lightLevels.ts` | whole 62 | níveis de luz de lentes e faróis | `render/props/signal_lamps + agents.json` | já existia | cada dono tem a sua medida |
| `src/render/plantImpostors.ts` | part 20 | quadros distantes de planta | `render/trees` | já existia |  |
| `src/render/playEffects.ts` | part 35 | destroços de poste | `porte demolition` | não se aplica | dono: demolition |
| `src/render/poleBake.ts` | whole 88 | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleCells.ts` | whole 239 | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleLayer.ts` | whole 751 | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/propCells.ts` | whole 619 | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propGeometry.ts` | whole 453 | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/renderer.ts` | part 148 | ligação das camadas | `render_world` | portado |  |
| `src/render/scenery.ts` | part 68 | kit de malhas, poça e cone | `render/props/prop_kit.cpp + lamp_glow.gdshader` | parcial | cone só com haze, que espera o clima (chuva do 4.0) |
| `src/render/signals.ts` | whole 731 | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signLayer.ts` | whole 441 | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signs.ts` | whole 466 | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/streetFurnitureLayer.ts` | whole 471 | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/utilities.ts` | whole 588 | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/wind.ts` | part 52 | vento | `props.gdshader` | parcial | fios balançam; o poste de semáforo não |
| `src/sim/agents/walk.ts` | part 10 | pedestre contorna o poste de semáforo | `sim/traffic/ped_furniture.cpp` | já existia |  |
| `src/sim/peds/signalPostIndex.ts` | part 31 | índice dos postes de semáforo | `sim/traffic/ped_furniture.cpp` | já existia |  |
| `src/ui/roads/furnitureChoice.ts` | whole 23 | escolha do conjunto de mobiliário no desenho de via | `project/ui/hud_drawer.gd _furniture_row + CityCore.road_furniture` | portado |  |
| `src/ui/toolChoices.ts` | part 7 | modo de luz lembrado | `ToolController.remembered` | portado |  |
| `src/ui/v2/shell.ts` | part 29 | aba Rua e conjunto de mobiliário | `project/ui/street_strip.gd + hud_drawer.gd` | portado |  |
| `src/world/barriers.ts` | whole 28 | tipos, medidas e regra de muro, cerca, sebe, defensa e gradil | `sim/street/street_works.h + street_furniture.json barriers` | portado |  |
| `src/world/doc.ts` | part 244 | postes, vãos e barreiras no documento | `sim/street/street_works + journal` | portado |  |
| `src/world/junction/bulb.ts` | part 7 | itens no balão | `sim/street/footway.cpp` | não se aplica | o balão do jogo não leva item |
| `src/world/landscape.ts` | part 227 | itens e encaixe na calçada | `sim/commands/street_snap.cpp` | portado |  |
| `src/world/lanelets.ts` | part 9 | conectores de retorno | `porte roads` | não se aplica | dono: roads |
| `src/world/network.ts` | part 18 | estado das linhas de postes | `sim/street` | não se aplica | sem cache regional |
| `src/world/poleIndex.ts` | whole 122 | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleLines.ts` | whole 574 | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/rasterGrid.ts` | part 7 | raster das zonas | `sim/lots` | não se aplica | sem raster |
| `src/world/roads/derivedSigns.ts` | whole 84 | placas que as regras levantam sozinhas (Pare, Preferência, velocidade) | `sim/traffic/street_signs.cpp` | parcial | Pare sem parada obrigatória: o tráfego trata como preferência |
| `src/world/roads/furnitureCatalog.ts` | whole 29 | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts` | whole 165 | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/section.ts` | part 1 | seção da via | `sim/roads/profiles.cpp` | já existia |  |
| `src/world/signalPosts.ts` | whole 77 | onde fica o poste de semáforo de cada acesso | `sim/roads/street_furniture_posts.cpp signal_post` | já existia | o jogo usa MBST V: depois do cruzamento |
| `src/world/solids.ts` | part 25 | sólidos: bomba e carro batem em poste e barreira | `sim/traffic/ped_furniture.cpp` | portado | pedestres; colisão física é do porte demolition |
| `src/world/streetFurniture.ts` | whole 124 | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/surfaces.ts` | part 3 | polígonos por nível | `sim/street/footway.cpp` | não se aplica | por perfil |
| `src/world/utilities.ts` | whole 162 | cruzeta de cada poste pela deflexão da linha, uma por linha em canto | `sim/street/pole_arms.cpp` | portado |  |
| `src/world/viewCells.ts` | part 59 | células de vista | `chunks` | já existia |  |
| `src/world/zoneGrid.ts` | part 4 | raster das zonas | `sim/lots` | não se aplica | sem raster |

## Declarações

| Declaração | O que faz | Onde mora | Estado | Motivo |
|---|---|---|---|---|
| `src/editor/barriers.ts#BarrierTool` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.cancel` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.constructor` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.cursor` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.down` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.finish` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.gesture` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.key` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.lastClick` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.move` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.plan` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierTool.points` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#BarrierToolHost` | ferramenta Muros: ponto a ponto, duplo clique, Enter, Backspace, Esc | `project/game/tools/barrier_tool.gd` | portado |  |
| `src/editor/barriers.ts#FOOTWAY_REACH` | encaixe na calçada e motivo de recusa | `sim/commands/street_snap.cpp` | portado |  |
| `src/editor/barriers.ts#MIN_BARRIER_RUN` | encaixe na calçada e motivo de recusa | `sim/commands/street_snap.cpp` | portado |  |
| `src/editor/barriers.ts#barrierProblem` | encaixe na calçada e motivo de recusa | `sim/commands/street_snap.cpp` | portado |  |
| `src/editor/barriers.ts#onCarriageway` | encaixe na calçada e motivo de recusa | `sim/commands/street_snap.cpp` | portado |  |
| `src/editor/barriers.ts#snapBarrierPoint` | encaixe na calçada e motivo de recusa | `sim/commands/street_snap.cpp` | portado |  |
| `src/editor/poleRoutes.ts#CROSSING_COST` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#RouteGraph` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#SPAN_LANDING` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#STATION_STEP` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#buildRouteGraph` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#buildRouteGraph.stationAt` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#route` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#route.at` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#route.placedBefore` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#route.reachOf` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#route.relax` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poleRoutes.ts#stationsAround` | caminho da linha pelas calçadas, cruzando a rua quando compensa | `sim/street/pole_routes.cpp` | portado |  |
| `src/editor/poles.ts#CLOSED_PAD` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#CORNER_TOLERANCE` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#LINE_CATCH` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#MIN_POLE_GAP` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#POLE_PICK_PIXELS` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#POLE_TO_LINE` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#PlannedPole` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#PoleRunPlan` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#PoleRunPlanner` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#PoleRunRefusal` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#PoleSnap` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#PoleSnapKind` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#PoleTool` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.cancel` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.chain` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.constructor` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.down` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.down.remove` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.draft` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.gesture` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.hover` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.inProgress` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.lastMode` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.marks` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.move` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.plan` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.reach` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleTool.up` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#PoleToolHost` | ferramenta Postes: arrastar, encadear, Shift tira | `project/game/tools/pole_tool.gd` | portado |  |
| `src/editor/poles.ts#REUSE_RADIUS` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#ROUTE_MARGIN` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#ROUTE_MARGIN_MOST` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#Routing` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#TILE` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#commitPoleRun` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#commitPoleRunPoles` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#computePoleRun` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#computePoleRun.at` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#computePoleRun.clear` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#computePoleRun.clear.ok` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#computePoleRun.refuse` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#corners` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#endOff` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#graphs` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#inLanding` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#lampFor` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#lastPlan` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#placeOf` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#planPoleRun` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#planPoleRunWhole` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#poleRouteWork` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#poleRunPlanner` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#poleRunPlanner.inner` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#poleRunPlanner.plan` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#poleRunPlanner.tileAt` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#regional` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#routeGraph` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#routeRun` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#snapOn` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/poles.ts#snapPole` | plano da linha: cantos, estações, reuso e recusa | `sim/commands/pole_runs.cpp` | portado |  |
| `src/editor/streetObjects.ts#PedestrianCrossingKind` | travessia no meio da quadra e retorno no canteiro | `sim/commands, porte roads` | não se aplica | fora do escopo; a declaração é própria do rastreamento roads, que a porta |
| `src/editor/streetObjects.ts#PedestrianCrossingResult` | travessia no meio da quadra e retorno no canteiro | `sim/commands, porte roads` | não se aplica | fora do escopo; a declaração é própria do rastreamento roads, que a porta |
| `src/editor/streetObjects.ts#UturnResult` | travessia no meio da quadra e retorno no canteiro | `sim/commands, porte roads` | não se aplica | fora do escopo; a declaração é própria do rastreamento roads, que a porta |
| `src/editor/streetObjects.ts#commitPedestrianCrossing` | travessia no meio da quadra e retorno no canteiro | `sim/commands, porte roads` | não se aplica | fora do escopo; a declaração é própria do rastreamento roads, que a porta |
| `src/editor/streetObjects.ts#commitUturn` | travessia no meio da quadra e retorno no canteiro | `sim/commands, porte roads` | não se aplica | fora do escopo; a declaração é própria do rastreamento roads, que a porta |
| `src/editor/streetscapeTool.ts#MEADOW_CLEAR` | peças em volta de um ponto, sem varrer o mapa | `sim/street/street_works.cpp items_in + street_furniture.json` | portado |  |
| `src/editor/streetscapeTool.ts#ROW_SPACING` | fileira arrastada ao longo da calçada, no espaçamento de cada peça | `sim/commands/street_commands.cpp place_item_row + drawers.json row_m` | portado |  |
| `src/editor/streetscapeTool.ts#StreetscapeTool` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#StreetscapeTool.constructor` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#StreetscapeTool.down` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#StreetscapeTool.hover` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#StreetscapeTool.move` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#StreetscapeTool.reach` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#StreetscapeTool.row` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#StreetscapeTool.up` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#StreetscapeToolHost` | colocar, remover com Shift e arrastar uma fileira | `project/game/tools/streetscape_tool.gd` | portado | a peça sai ao soltar o botão, no 4.0 sai ao apertar |
| `src/editor/streetscapeTool.ts#itemsAround` | peças em volta de um ponto, sem varrer o mapa | `sim/street/street_works.cpp items_in + street_furniture.json` | portado |  |
| `src/editor/streetscapeTool.ts#rowPieces` | fileira arrastada ao longo da calçada, no espaçamento de cada peça | `sim/commands/street_commands.cpp place_item_row + drawers.json row_m` | portado |  |
| `src/render/barriers.ts#BarrierPlace` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.allKeys` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.byId` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.changed` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.compute` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.constructor` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.dirty` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.equals` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.filing` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.kinds` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierProduct.name` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#BarrierSource` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#Barriers` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#Box` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#KINDS` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#PARTS` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#PIECE` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#POST_STEP` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#PartList` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#READ_REACH` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#SINK` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#barrierInto` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#barrierParts` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#barrierParts.material` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#createBarriers` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#createBarriers.cull` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#createBarriers.dispose` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#createBarriers.sync` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#placeBarrier` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/barriers.ts#placeKerbBarrier` | peças de cada barreira no terreno | `render/props/barrier_boxes.cpp` | parcial | caixas instanciadas por chunk; defensa sem o metal do aço galvanizado |
| `src/render/cellInstances.ts#batchMesh` | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/cellInstances.ts#createCellInstances.setMap` | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/cellInstances.ts#releaseBatchMesh` | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/cellInstances.ts#warmMesh` | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/cellInstances.ts#watchScreen` | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/grass.ts#CLUMP_REACH` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#CLUMP_READ_REACH` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#Clump` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#FLOWER_COLOURS` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#FLOWER_MIN` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#FLOWER_RANGE` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#NONE` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#SLOPE_PROBE` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#TUFT_MIN` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#TUFT_RANGE` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#Tuft` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/grass.ts#grassClump` | moita de mato colocada | `render/props/street_props.cpp` | portado |  |
| `src/render/instanceRuns.ts#nearCode` | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/instanceRuns.ts#warmMeshes` | instâncias por célula | `MultiMesh por chunk` | já existia |  |
| `src/render/lightLevels.ts#BLOOM_THRESHOLD` | níveis de luz de lentes e faróis | `render/props/signal_lamps + agents.json` | já existia | cada dono tem a sua medida |
| `src/render/lightLevels.ts#NIGHT_LUMINANCE` | níveis de luz de lentes e faróis | `render/props/signal_lamps + agents.json` | já existia | cada dono tem a sua medida |
| `src/render/lightLevels.ts#fromCandela` | níveis de luz de lentes e faróis | `render/props/signal_lamps + agents.json` | já existia | cada dono tem a sua medida |
| `src/render/lightLevels.ts#lightPoolTexture` | queda da poça de luz | `project/render/shaders/lamp_pool.gdshaderinc + lamp_glow.gdshader` | portado | no shader, sem textura; disco por lâmpada |
| `src/render/lightLevels.ts#luminanceOf` | níveis de luz de lentes e faróis | `render/props/signal_lamps + agents.json` | já existia | cada dono tem a sua medida |
| `src/render/lightLevels.ts#nightGain` | níveis de luz de lentes e faróis | `render/props/signal_lamps + agents.json` | já existia | cada dono tem a sua medida |
| `src/render/lightLevels.ts#scratch` | níveis de luz de lentes e faróis | `render/props/signal_lamps + agents.json` | já existia | cada dono tem a sua medida |
| `src/render/poleBake.ts#PartBounds` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#PartShape` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#PoleSource` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#WIRE_BLOCK` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#WireBounds` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#emptyBox` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#framePole` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#growPart` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#growWires` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#partShape` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleBake.ts#wireEnd` | moldura do poste e limites | `render/props/prop_kit.cpp` | portado |  |
| `src/render/poleCells.ts#BLOCK_OVERHEAD` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#HeldBlock` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#POLE_CHANGES` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#POLE_LOW,POLE_HIGH` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#POLE_SHADOW_REACH` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.box` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.build` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.build.frameOf` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.busy` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.cells` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.constructor` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.derive` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.dispose` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.follow` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.from` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.group` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.live` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.look` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.release` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.scale` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.scratch` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.source` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.specs` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.triangles` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.view` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.wantedKeys` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.wantedOrder` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleCells.wantedPool` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#PoleView` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleCells.ts#SPAN_REACH` | postes guardados em volta da câmera | `render_world: props_stream_` | já existia | streaming por chunk já existe |
| `src/render/poleLayer.ts#BLOCK` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#Block` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#POLE_REACH` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.add` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.bounds` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.boundsMoved` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.box` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.capacity` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.constructor` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.count` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.dispose` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.drawn` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.flush` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.level` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.levelFor` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.matrices` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.mesh` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.meshOf` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.owner` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.perInstance` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.remove` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.reserve` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.seal` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.shape` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.shown` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.uncast` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.view` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.whole` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.write` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PartStore.written` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.block` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.blocks` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.constructor` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.dispose` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.fill` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.flush` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.follow` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.group` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.ids` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.index` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.place` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.poles` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.polesIn` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.redo` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.scale` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.scratch` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.spans` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.specs` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.string` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.triangles` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.unplace` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.unstring` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PoleLayer.view` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#PolePreviewInput` | prévia 3D da linha sob o ponteiro | `render_world set_pole_preview + city_core_street.cpp` | portado |  |
| `src/render/poleLayer.ts#PoleState` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.ahead` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.at` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.atBox` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.atPoint` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.eye` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.perspective` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.pixels` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.set` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ScreenScale.texel` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ViewedRoot` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ViewedRoot.constructor` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#ViewedRoot.update` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.BLOCK` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.add` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.bounds` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.boundsMoved` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.box` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.buffer` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.capacity` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.constructor` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.count` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.data` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.dispose` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.drawn` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.dropLevels` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.flush` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.geometry` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.instances` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.level` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.levels` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.makeMesh` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.mesh` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.owner` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.remove` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.reserve` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.seal` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.sphere` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.uncast` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.view` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.whole` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#WireStore.written` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#blockKey` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#buildPolePreview` | prévia 3D da linha sob o ponteiro | `render_world set_pole_preview + city_core_street.cpp` | portado |  |
| `src/render/poleLayer.ts#buildUtilities` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#docPoles` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#drawingBuffer` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#noteScreen` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#noteShadow` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/poleLayer.ts#screen` | camada instanciada por blocos, com LOD | `render/props/prop_renderer.cpp (MultiMesh por chunk, faixas de visibilidade)` | portado | o streaming por chunk substitui blocos, slots e ViewedRoot |
| `src/render/propCells.ts#BLOCK_LOW,BLOCK_HIGH` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#FAR_CELLS` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#FAR_OVERHEAD` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#FAR_SIDE` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#FIRST_LARGEST` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#FRAME_MS` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#FarBlock` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#FarClass` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#GROUND_SPAN` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#MOST_NEAR` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#NEAR_OVERHEAD` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#NearCell` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#PROP_CELL` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#PropLayer` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#PropLayerSpec` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#PropPlacement` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#PropSquare` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#PropView` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#columnOf` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#corner,far,inverse` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.batches` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.batches.boundsOf` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.batches.make` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.batches.own` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.cull` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.dispose` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.follow` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.forgetGrowing` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.inSquare` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.note` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.ringsIn` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.setMap` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.show` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#createPropLayer.wantedBlocks` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#keyOfCell` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#lookProps` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#nearSquareOf` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#packBytes` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#propView` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#reachFor` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#rowOf` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propCells.ts#within` | células de props em volta da câmera, com quadro distante | `render_world + render/trees` | já existia | streaming por chunk já existe |
| `src/render/propGeometry.ts#BUSH_KINDS` | corpo das plantas | `render/trees, porte landscape` | não se aplica | dono: landscape |
| `src/render/propGeometry.ts#BushKind` | corpo das plantas | `render/trees, porte landscape` | não se aplica | dono: landscape |
| `src/render/propGeometry.ts#IRON` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#LAMP_HEIGHT` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#LAMP_METAL` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#LAMP_OUTREACH` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#LAMP_PLINTH` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#Placement` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#Rgb` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#TREE_KIND` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#TREE_SPECIES` | corpo das plantas | `render/trees, porte landscape` | não se aplica | dono: landscape |
| `src/render/propGeometry.ts#TreeSpecies` | corpo das plantas | `render/trees, porte landscape` | não se aplica | dono: landscape |
| `src/render/propGeometry.ts#WOOD` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#benchGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#benchGeometry.slat` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#binGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#boxWithout` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#bushParts` | corpo das plantas | `render/trees, porte landscape` | não se aplica | dono: landscape |
| `src/render/propGeometry.ts#drainGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#faceNoise` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#grassTuftGeometry` | corpo das plantas | `render/trees, porte landscape` | não se aplica | dono: landscape |
| `src/render/propGeometry.ts#hydrantGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#lampFarGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#lampGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#lampLensGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#merge` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#part` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#phoneGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#postboxGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#rgb` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#treeParts` | corpo das plantas | `render/trees, porte landscape` | não se aplica | dono: landscape |
| `src/render/propGeometry.ts#treePitGeometry` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#trianglesOf` | malha de banco, lixeira, hidrante, caixa de correio, orelhão, boca de lobo, caldeira e luminária | `data/render/streets.json props + render/props/prop_kit.cpp` | portado |  |
| `src/render/propGeometry.ts#wildflowerGeometry` | corpo das plantas | `render/trees, porte landscape` | não se aplica | dono: landscape |
| `src/render/signLayer.ts#ITEM_READS` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#POST_STORES` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#PlateStore` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#QUAD_3,QUAD_2` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignLayerKept` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.allKeys` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.byId` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.changed` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.compute` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.dirty` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.equals` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.filing` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.kinds` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignProduct.name` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignSource` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#SignValue` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#Stand` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#Standing` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#around` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.cull` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.dispose` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.freeQuad` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.letGo` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.putQuad` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.refill` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.rewriteUv` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.sized` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.stand` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.standOf` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.storeOf` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.sync` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.take` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.touch` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.useCopier` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.warm` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#createSignLayer.writeUv` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#grow` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#isSignItem` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#itemKey` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#itemSigns` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#junctionSigns` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#nodeKey` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#ribbonBox` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#signProduct` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signLayer.ts#unite` | placas em pé: postes, quadros, nomes nas esquinas | `render/props/street_signs.cpp` | portado | no mesh do chunk, sem produto derivado |
| `src/render/signals.ts#ARM_LENGTH` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#ARM_RADIUS` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#BatchName` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#Cell` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#FACES` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#HEAD_CENTRE` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#HEAD_DEPTH` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#HEAD_HEIGHT` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#HEAD_REACH` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#HEAD_WIDTH` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#Head` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#Junction` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#LAMPS` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#LAMP_PITCH` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#LAMP_RADIUS` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#Lamp` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#POST_HEIGHT` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#POST_RADIUS` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SIGNAL_CELL` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SIGNAL_SCALE` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.add` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.capacity` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.colours` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.constructor` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.count` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.data` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.dirty` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.dispose` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.flush` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.geometry` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.glows` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.holds` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.holds.same` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.makeMesh` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.mesh` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.remove` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.reserve` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.runs` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalBatch.sphere` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#SignalHeads` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#VISOR_DEPTH` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.at` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.batchesOf` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.buildCell` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.cellKeyOf` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.countLamps` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.dispose` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.dropCell` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.dropCells` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.facing` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.framePart` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.junctionOf` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.placeAll` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.placeSome` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.placeSome.cellJunctions` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.put` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.rootOf` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.sharing` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.sync` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#createSignalHeads.writeLenses` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signals.ts#u` | cabeças de semáforo, lentes acesas e apagadas por estado | `render/props/signal_lamps + streets.json signal` | já existia | o jogo tem as lentes por MUTCD e EN 12368 e o foco de pedestre |
| `src/render/signs.ts#BLADE_HALF` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#CELL` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#CELL_HEIGHTS` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#COLUMNS` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#FACE_OUT` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#FIRST_ROWS` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#GUTTER` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#PAGE` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#PITCH` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#PLATE` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#POST` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#POST_RADIUS` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#Placed` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#QUAD` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#QUAD_ORDER` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#Sheet` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SheetCopier` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.begin` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.builds` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.cellCanvas` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.cellTexture` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.constructor` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.counts` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.dispose` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.drop` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.flush` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.growSheet` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.newSheet` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.paint` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.paintCells` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.paintCells.send` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.pending` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.placed` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.plateCell` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.refill` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.sheetWithRoom` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.sheets` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.sizes` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.take` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#SignSheets.useCopier` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#cellUv` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#fitSize` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#fitSize.ok` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#maxRows` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#onTopOfPost` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#pitchOf` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#plateCanvas` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#plateCanvas.circle` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#plateCanvas.fit` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#plateFaces` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#plateFaces.face` | desenho e medida de cada placa | `project/render/sign_sheet.gd + streets.json styles` | portado |  |
| `src/render/signs.ts#plateMaterial` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#range` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#sheetTexture` | folha de placas na GPU | `render/props/sign_atlas + project/render/sign_sheet.gd` | portado | uma folha, células por tipo e texto |
| `src/render/signs.ts#streetChain` | a rua inteira de um nome | `sim/street/street_spots.cpp street_chain` | já existia | mesma função já existia; movida para o render poder ler |
| `src/render/signs.ts#streetChain.dirAt` | a rua inteira de um nome | `sim/street/street_spots.cpp street_chain` | já existia | mesma função já existia; movida para o render poder ler |
| `src/render/signs.ts#streetChain.walk` | a rua inteira de um nome | `sim/street/street_spots.cpp street_chain` | já existia | mesma função já existia; movida para o render poder ler |
| `src/render/streetFurnitureLayer.ts#DECK_REACH` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#FURNITURE_MODELS` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#FURNITURE_SPREAD` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#FurnitureModel` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#FurniturePlace` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#FurnitureSource` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#LampLens` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#MEADOW_STORES` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#MODEL` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#MeadowPlace` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PROP_FAR_PX` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#Placed` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.byId` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.changed` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.compute` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.constructor` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.filing` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.kinds` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#ROAD_REACH` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#STREET_TREE_MIN` | espécie e porte da árvore de rua | `render/trees` | já existia | dono: render/trees, porte landscape |
| `src/render/streetFurnitureLayer.ts#STREET_TREE_RANGE` | espécie e porte da árvore de rua | `render/trees` | já existia | dono: render/trees, porte landscape |
| `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#bushStore` | espécie e porte da árvore de rua | `render/trees` | já existia | dono: render/trees, porte landscape |
| `src/render/streetFurnitureLayer.ts#createStreetFurniture` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#createStreetFurniture.setMap` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#createStreetFurniture.warm` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#farBox` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#furnitureAbout` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#furniturePlace` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#furniturePlace.put` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#furnitureProduct` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#furnitureStores` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#furnitureStores.plant` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#furnitureStores.prop` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#isFurniture` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#meadowInto` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#meadowInto.add` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#meadowProduct` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#placeInto` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#radiusOf` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#sameColour` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#samePlace` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#stillGrowing` | cada item desenhado no chão certo, de frente para o lugar certo | `render/props/street_props.cpp` | portado |  |
| `src/render/streetFurnitureLayer.ts#streetSpecies` | espécie e porte da árvore de rua | `render/trees` | já existia | dono: render/trees, porte landscape |
| `src/render/streetFurnitureLayer.ts#treeStore` | espécie e porte da árvore de rua | `render/trees` | já existia | dono: render/trees, porte landscape |
| `src/render/utilities.ts#BRACE_DROP` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#BRACE_THICK` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#CAN_RADIUS` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#INSULATOR_RADIUS` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#INSULATOR_TALL` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#LAMP_TILT` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#PARTS` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#POLE_PART` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#POLE_PART_COUNT` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#PartLevel` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#Placement` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#PolePartSpec` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#SPAN_WIRES` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#Utilities` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#UtilityParts` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#WIRE_FADE` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WIRE_GLSL` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WIRE_HALF_WIDTH` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WIRE_RADIUS` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WIRE_STRIDE` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WIRE_SWING` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WIRE_TEMPLATES` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WIRE_TUBE_PIXELS` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WIRE_WIDTH` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#WireTemplate` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#madeUtilityParts` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#makeUtilityParts` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#mastShape` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#placePole` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#placing` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#poleGroundAt` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#polePartSpecs` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#polePartSpecs.light` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#polePartSpecs.solid` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#releaseWireGeometry` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#spanWires` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#stripTemplate` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#tubeTemplate` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#tubeTemplate.put` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#unlit` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/utilities.ts#wireGeometry` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#wireLevel` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#wireMaterials` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#wireParts` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#wireScreen` | nove fios por vão, com flecha e vento, tubo perto e fita longe | `render/props/prop_kit.cpp + street_props.cpp + props.gdshader` | portado | MultiMesh com a curva no vértice |
| `src/render/utilities.ts#woodCache` | madeira do poste | `project/render/shaders/props.gdshader` | portado | grão no shader, sem textura de canvas |
| `src/render/utilities.ts#woodTexture` | madeira do poste | `project/render/shaders/props.gdshader` | portado | grão no shader, sem textura de canvas |
| `src/render/utilities.ts#woodTexture.rnd` | madeira do poste | `project/render/shaders/props.gdshader` | portado | grão no shader, sem textura de canvas |
| `src/render/utilities.ts#writePlacement` | peças do poste: mastro, cruzeta, mão francesa, isoladores, transformador, braço, luminária | `render/props/prop_kit.cpp + streets.json` | portado | uma malha com máscara por instância |
| `src/render/wind.ts#applyPartSway` | fios e postes balançam no vento | `props.gdshader` | parcial | fios balançam; o poste de semáforo não |
| `src/render/wind.ts#applyStructureSway` | fios e postes balançam no vento | `props.gdshader` | parcial | fios balançam; o poste de semáforo não |
| `src/render/wind.ts#installStructureSway` | fios e postes balançam no vento | `props.gdshader` | parcial | fios balançam; o poste de semáforo não |
| `src/render/wind.ts#wireWindGlsl` | fios e postes balançam no vento | `props.gdshader` | parcial | fios balançam; o poste de semáforo não |
| `src/ui/roads/furnitureChoice.ts#STORAGE_KEY` | escolha do conjunto de mobiliário no desenho de via | `project/ui/hud_drawer.gd _furniture_row + CityCore.road_furniture` | portado |  |
| `src/ui/roads/furnitureChoice.ts#chosen` | escolha do conjunto de mobiliário no desenho de via | `project/ui/hud_drawer.gd _furniture_row + CityCore.road_furniture` | portado |  |
| `src/ui/roads/furnitureChoice.ts#furnitureChosen` | escolha do conjunto de mobiliário no desenho de via | `project/ui/hud_drawer.gd _furniture_row + CityCore.road_furniture` | portado |  |
| `src/ui/roads/furnitureChoice.ts#read` | escolha do conjunto de mobiliário no desenho de via | `project/ui/hud_drawer.gd _furniture_row + CityCore.road_furniture` | portado |  |
| `src/ui/roads/furnitureChoice.ts#setFurnitureChosen` | escolha do conjunto de mobiliário no desenho de via | `project/ui/hud_drawer.gd _furniture_row + CityCore.road_furniture` | portado |  |
| `src/world/barriers.ts#BARRIER_KINDS` | tipos, medidas e regra de muro, cerca, sebe, defensa e gradil | `sim/street/street_works.h + street_furniture.json barriers` | portado |  |
| `src/world/barriers.ts#BARRIER_SIZE` | tipos, medidas e regra de muro, cerca, sebe, defensa e gradil | `sim/street/street_works.h + street_furniture.json barriers` | portado |  |
| `src/world/barriers.ts#Barrier` | tipos, medidas e regra de muro, cerca, sebe, defensa e gradil | `sim/street/street_works.h + street_furniture.json barriers` | portado |  |
| `src/world/barriers.ts#BarrierKind` | tipos, medidas e regra de muro, cerca, sebe, defensa e gradil | `sim/street/street_works.h + street_furniture.json barriers` | portado |  |
| `src/world/barriers.ts#KERB_BARRIERS` | tipos, medidas e regra de muro, cerca, sebe, defensa e gradil | `sim/street/street_works.h + street_furniture.json barriers` | portado |  |
| `src/world/barriers.ts#isBarrierKind` | tipos, medidas e regra de muro, cerca, sebe, defensa e gradil | `sim/street/street_works.h + street_furniture.json barriers` | portado |  |
| `src/world/doc.ts#RoadDoc.barriersInAny` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/doc.ts#RoadDoc.landscapeInAny` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/doc.ts#RoadDoc.pole` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/doc.ts#RoadDoc.setNodeCrossing` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/doc.ts#RoadDoc.setNodeUturn` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#SnapLocal` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#crossingAccessTester` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#crossingAccesses` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#crossingAccessesAt` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#crossingAccessesNow` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#depthFor` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#footwayAt` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#medianAt` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#nearCrossingAccess` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#onAnyRoad` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#onCrossingAccess` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#plantGrowth` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#plantedMedian` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#ribbonsNear` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#snapLandscape` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/landscape.ts#snapLandscape.others` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/lanelets.ts#LaneletGraph.junctionConnectors` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/network.ts#Network.crossingLandingAt` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/network.ts#Network.poleLines` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/poleIndex.ts#CELL` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.cellOfPole` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.cells` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.clear` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.deletePole` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.deleteSpan` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.near` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.over` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.own` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.putPole` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.putSpan` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.spansAt` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.spansOfPole` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#PoleIndex.unfile` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#addTo` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#cellOf` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#keyOf` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleIndex.ts#takeFrom` | achar postes por lugar e vãos por poste sem varrer o mapa | `sim/street/street_works.cpp (SpatialHash e vãos por poste)` | portado | ForkMap não se aplica: o jogo tem journal, sem cópia de trabalho |
| `src/world/poleLines.ts#ContourEdges` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#ContourScratch` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#EDGE_BUCKET` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#FootRings` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#LINES_HALO` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#OnContour` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#POLE_KERB_INSET` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#Placed` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#PoleLineState` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#PoleLineState.checkedAt` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#PoleLineState.letGoStale` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#PoleLineState.linesIn` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#PoleLineState.regions` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#PoleLineState.rings` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#PoleLines` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#WindingRing` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#boxesOf` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#contourEdges` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#contoursOf` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#edgeBucket` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#insideLevels` | o ponto está na calçada? | `sim/street/footway.cpp` | portado | por perfil do segmento, sem união de polígonos |
| `src/world/poleLines.ts#insideLevels.add` | o ponto está na calçada? | `sim/street/footway.cpp` | portado | por perfil do segmento, sem união de polígonos |
| `src/world/poleLines.ts#junctionRings` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#kerbWork` | direção do meio-fio mais próximo: a luz sai para a rua | `sim/street/footway.cpp (o quadro do segmento já dá a direção)` | portado |  |
| `src/world/poleLines.ts#kerbward` | direção do meio-fio mais próximo: a luz sai para a rua | `sim/street/footway.cpp (o quadro do segmento já dá a direção)` | portado |  |
| `src/world/poleLines.ts#kerbwardOfUnion` | direção do meio-fio mais próximo: a luz sai para a rua | `sim/street/footway.cpp (o quadro do segmento já dá a direção)` | portado |  |
| `src/world/poleLines.ts#localRings` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#markPlates` | marca o pavimento na grade de zonas | `sim/lots` | não se aplica | o jogo acha lotes por testada, sem raster do 4.0 |
| `src/world/poleLines.ts#markPlates.cx` | marca o pavimento na grade de zonas | `sim/lots` | não se aplica | o jogo acha lotes por testada, sem raster do 4.0 |
| `src/world/poleLines.ts#markPlates.cy` | marca o pavimento na grade de zonas | `sim/lots` | não se aplica | o jogo acha lotes por testada, sem raster do 4.0 |
| `src/world/poleLines.ts#markPlates.place` | marca o pavimento na grade de zonas | `sim/lots` | não se aplica | o jogo acha lotes por testada, sem raster do 4.0 |
| `src/world/poleLines.ts#nearestOnContour` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#onFootway` | o ponto está na calçada? | `sim/street/footway.cpp` | portado | por perfil do segmento, sem união de polígonos |
| `src/world/poleLines.ts#onPaving` | o ponto está na calçada? | `sim/street/footway.cpp` | portado | por perfil do segmento, sem união de polígonos |
| `src/world/poleLines.ts#poleLines` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#poleLinesAt` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#poleLinesIn` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#poleLinesOf` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#poleLinesWork` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#polesInclude` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#regionStamp` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#ribbonRings` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#ringBoxes` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#scratchOf` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#stateOf` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#windingOf` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/poleLines.ts#windingRings` | linha de guia dos postes atrás do meio-fio | `sim/street/pole_routes.cpp` | portado | linhas por lado de segmento em vez de contorno Clipper |
| `src/world/rasterGrid.ts#firstTrue` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/roads/derivedSigns.ts#APPROACH_REACH` | placas que as regras levantam sozinhas (Pare, Preferência, velocidade) | `sim/traffic/street_signs.cpp` | parcial | Pare sem parada obrigatória: o tráfego trata como preferência |
| `src/world/roads/derivedSigns.ts#DerivedSign` | placas que as regras levantam sozinhas (Pare, Preferência, velocidade) | `sim/traffic/street_signs.cpp` | parcial | Pare sem parada obrigatória: o tráfego trata como preferência |
| `src/world/roads/derivedSigns.ts#SPEED_MIN_LENGTH` | placas que as regras levantam sozinhas (Pare, Preferência, velocidade) | `sim/traffic/street_signs.cpp` | parcial | Pare sem parada obrigatória: o tráfego trata como preferência |
| `src/world/roads/derivedSigns.ts#SPEED_PAST_MOUTH` | placas que as regras levantam sozinhas (Pare, Preferência, velocidade) | `sim/traffic/street_signs.cpp` | parcial | Pare sem parada obrigatória: o tráfego trata como preferência |
| `src/world/roads/derivedSigns.ts#signApproach` | placa de Pare ou Preferência no acesso vira a regra do cruzamento | `sim/commands/street_commands.cpp` | portado |  |
| `src/world/roads/derivedSigns.ts#signsAtNode` | placas que as regras levantam sozinhas (Pare, Preferência, velocidade) | `sim/traffic/street_signs.cpp` | parcial | Pare sem parada obrigatória: o tráfego trata como preferência |
| `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureCatalog.ts#FurnitureEntry` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureCatalog.ts#furnitureEntry` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#CELL` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#FURNITURE_SETS` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#FurniturePlacement` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#FurnitureSet` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#NBR9050_CLEAR_WALK` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#NEAR_CELL` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#Pattern` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#SETS` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#furnitureFor` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#furnitureFor.cellOf` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#furnitureFor.file` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#isFurnitureSet` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#nearGrid` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#nearGrid.around` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#nearGrid.file` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/roads/furnitureSets.ts#patternsOf` | conjunto completo, básico ou nenhum de uma via nova | `sim/roads/street_furniture.cpp (nível do segmento) + catálogo de vias` | parcial | peças por regra com norma, não por lista; colunas na mesma estação dos postes, o 4.0 alterna |
| `src/world/section.ts#zonesOn` | faixas da seção da via | `sim/roads/profiles.cpp` | já existia |  |
| `src/world/signalPosts.ts#SIGNAL_POST_KERB` | onde fica o poste de semáforo de cada acesso | `sim/roads/street_furniture_posts.cpp signal_post` | já existia | o jogo usa MBST V: depois do cruzamento |
| `src/world/signalPosts.ts#SIGNAL_POST_RADIUS` | onde fica o poste de semáforo de cada acesso | `sim/roads/street_furniture_posts.cpp signal_post` | já existia | o jogo usa MBST V: depois do cruzamento |
| `src/world/signalPosts.ts#SignalJunctions` | onde fica o poste de semáforo de cada acesso | `sim/roads/street_furniture_posts.cpp signal_post` | já existia | o jogo usa MBST V: depois do cruzamento |
| `src/world/signalPosts.ts#SignalPost` | onde fica o poste de semáforo de cada acesso | `sim/roads/street_furniture_posts.cpp signal_post` | já existia | o jogo usa MBST V: depois do cruzamento |
| `src/world/signalPosts.ts#signalPostPlace` | onde fica o poste de semáforo de cada acesso | `sim/roads/street_furniture_posts.cpp signal_post` | já existia | o jogo usa MBST V: depois do cruzamento |
| `src/world/signalPosts.ts#signalPosts` | onde fica o poste de semáforo de cada acesso | `sim/roads/street_furniture_posts.cpp signal_post` | já existia | o jogo usa MBST V: depois do cruzamento |
| `src/world/streetFurniture.ts#FURNITURE_OF` | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/streetFurniture.ts#FurnitureItem` | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/streetFurniture.ts#FurnitureKind` | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/streetFurniture.ts#STAND_REACH` | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/streetFurniture.ts#furnitureOf` | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/streetFurniture.ts#hash01` | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/streetFurniture.ts#standing` | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/streetFurniture.ts#streetFurniture` | cada item colocado virando peça em pé na calçada | `sim/commands/street_snap.cpp + traffic::street_furniture_of` | portado |  |
| `src/world/surfaces.ts#levelPolygons` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/utilities.ts#CORNER_DEFLECTION` | cruzeta de cada poste pela deflexão da linha, uma por linha em canto | `sim/street/pole_arms.cpp` | portado |  |
| `src/world/utilities.ts#DEFAULT_POLE_SPACING` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#MAX_POLE_SPACING` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#MIN_POLE_SPACING` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_ARM_DROP` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_ARM_HALF` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_ARM_THICK` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_BASE_RADIUS` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_HEIGHT` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_LAMP_DROP` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_LAMP_LONG` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_LAMP_MODES` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_LAMP_REACH` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_LAMP_TALL` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_LAMP_WIDE` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#POLE_TOP_RADIUS` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#PoleArms` | cruzeta de cada poste pela deflexão da linha, uma por linha em canto | `sim/street/pole_arms.cpp` | portado |  |
| `src/world/utilities.ts#PoleLampMode` | luz em nenhum, em postes alternados ou em todos | `sim/commands/pole_runs.cpp + project/ui/street_strip.gd` | portado |  |
| `src/world/utilities.ts#UtilityPole` | poste e vão que o jogador constrói | `sim/street/street_works.h` | portado |  |
| `src/world/utilities.ts#UtilitySpan` | poste e vão que o jogador constrói | `sim/street/street_works.h` | portado |  |
| `src/world/utilities.ts#WIRE_COURSES` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#WIRE_MAX_SAG` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#WIRE_OFFSETS` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#WIRE_SAG_RATIO` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#WIRE_SEGMENTS` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#perp` | cruzeta de cada poste pela deflexão da linha, uma por linha em canto | `sim/street/pole_arms.cpp` | portado |  |
| `src/world/utilities.ts#poleArms` | cruzeta de cada poste pela deflexão da linha, uma por linha em canto | `sim/street/pole_arms.cpp` | portado |  |
| `src/world/utilities.ts#poleArmsAt` | cruzeta de cada poste pela deflexão da linha, uma por linha em canto | `sim/street/pole_arms.cpp` | portado |  |
| `src/world/utilities.ts#poleArmsAt.oriented` | cruzeta de cada poste pela deflexão da linha, uma por linha em canto | `sim/street/pole_arms.cpp` | portado |  |
| `src/world/utilities.ts#poleCarriesLamp` | luz em nenhum, em postes alternados ou em todos | `sim/commands/pole_runs.cpp + project/ui/street_strip.gd` | portado |  |
| `src/world/utilities.ts#spanSag` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/utilities.ts#unit` | cruzeta de cada poste pela deflexão da linha, uma por linha em canto | `sim/street/pole_arms.cpp` | portado |  |
| `src/world/utilities.ts#wireDrop` | medidas do poste, da cruzeta, dos fios e da flecha | `data/catalog/street_furniture.json poles + data/render/streets.json` | portado |  |
| `src/world/viewCells.ts#ViewCells.account` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/viewCells.ts#ViewCells.forget` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/viewCells.ts#ViewCells.peek` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
| `src/world/viewCells.ts#ViewCells.want` | infraestrutura do documento e das células do 4.0 | `CityState + ChangeLog + chunks` | não se aplica | o jogo tem journal, ChangeLog e streaming por chunk |
