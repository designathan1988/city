# Cobertura do rastreamento: landscape

Fonte: `converter/out/features/landscape/` (`feature.md`, `source/`, `boundary.json`). Uma linha por arquivo, por declaração própria (1.371) e por fatia de código compartilhado (411). Estados: portado, já existia, não se aplica, outro dono (funcionalidade na fila, ver `registro/fila.md`), pendente. A linha de arquivo vale para as declarações dele, salvo a linha da declaração dizer outra coisa.

## Arquivos

| Arquivo | Estado | Dono no jogo | Para o jogador / motivo |
|---|---|---|---|
| `src/actionsWiring.ts` | outro dono | demolition | explodeAt: explosão do item demolido; a parte de paisagem é só remover árvore ou mobiliário |
| `src/core/aabb.ts` | já existia | core/math.h | caixa 2D |
| `src/core/rerootMap.ts` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/editor/bulldozer.ts` | outro dono | demolition | demolir itens de paisagem (fila: porte-4 demolition) |
| `src/editor/chunkedSave.ts` | já existia | sim/persistence/save_landscape.cpp | save por blocos: aqui um arquivo por cidade |
| `src/editor/cityGenerator.ts` | já existia | sim/citygen | cidade verde gerada |
| `src/editor/cloudTool.ts` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | pôr, mover, editar e tirar nuvens |
| `src/editor/commit.ts` | outro dono | road-elevation / roads | greide das vias sobre o terreno |
| `src/editor/editRules.ts` | outro dono | roads | regras do greide |
| `src/editor/growthFeed.ts` | não se aplica | - | envia o chão ao worker do crescimento: aqui o crescimento lê o Heightfield sob trava |
| `src/editor/growthReplica.ts` | não se aplica | - | réplica no worker: idem |
| `src/editor/history.ts` | já existia | sim/commands/edit_history.cpp | desfazer |
| `src/editor/persistence.ts` | portado | sim/persistence/save_nature.cpp | validação do save da paisagem |
| `src/editor/poles.ts` | outro dono | street-furniture | postes |
| `src/editor/roadTool.ts` | outro dono | roads | paisagem ao longo da via |
| `src/editor/roads/streetKit.ts` | outro dono | street-furniture | kit de rua |
| `src/editor/smallCityMap.ts` | já existia | sim/citygen | cidade pequena |
| `src/editor/streetObjects.ts` | outro dono | street-furniture | objetos de rua |
| `src/editor/streetscapeTool.ts` | outro dono | street-furniture | aba Rua da Paisagem: fila porte-6 |
| `src/editor/terrainBrush.ts` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | o que cada pincel de terreno faz |
| `src/jobs/jobPool.ts` | já existia | core/job_pool.h | pool de jobs |
| `src/lab/catalog/layers.ts` | não se aplica | - | laboratório do 4.0, não é jogo |
| `src/lab/catalog/layout.ts` | não se aplica | - | laboratório do 4.0, não é jogo |
| `src/main.ts` | portado | project/ui/land_strips.gd, project/game/tools/* | sliders e ações da aba Paisagem, pincéis |
| `src/render/agents.ts` | já existia | render/agents | ganho das lanternas à noite |
| `src/render/bake/detailRecipes.ts` | portado | project/assets/surfaces/grass.detail*.png, terrain_layers.gdshaderinc | grão de perto da grama, a textura cozida do 4.0 convertida por tools/assets/ktx2_to_png.mjs; a receita não roda no jogo; solo fino não ligado |
| `src/render/bake/terrainRecipes.ts` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/buildings/buildingMesh.ts` | já existia | render/buildings | gramado e bandeiras dos prédios |
| `src/render/buildings/flagLayer.ts` | já existia | render/buildings | gramado e bandeiras dos prédios |
| `src/render/buildings/kit.ts` | já existia | render/buildings | gramado e bandeiras dos prédios |
| `src/render/canyonLight.ts` | dividido | chave e painel na Porte 4; valores de oclusão na Porte 3 | o passe que escurece o céu no fundo das ruas não foi portado; a oclusão de tela (`daylight.json` occlusion) cobre só a parte GTAO |
| `src/render/cellInstances.ts` | já existia | render/rs_util.h MultiMeshDraw | instâncias por célula |
| `src/render/clearingCells.ts` | portado | sim/nature/planted_trees.cpp | clareiras por célula |
| `src/render/cloudShadowStamp.ts` | não se aplica | - | carimbo do mapa de sombra das nuvens: aqui a sombra é analítica por puff no passe de atmosfera, sem mapa |
| `src/render/coverRaster.ts` | portado em parte | sim/nature/ground_paint.cpp, render/cover/cover_renderer.cpp | mato e flores ao pintar; a rocha do lugar nao tinge a pedra |
| `src/render/coverSweep.ts` | portado em parte | render/cover/cover_renderer.cpp, render/trees/tree_place.cpp | mata, mato e rochas pintados; pedras da costa (shoreIn) não portadas |
| `src/render/densityCars.ts` | outro dono | traffic | carros distantes |
| `src/render/ecology.worker.ts` | já existia | core/job_pool.h + render/trees | ecologia em job |
| `src/render/ecologyKept.ts` | não se aplica | - | guarda ecologia no worker; aqui cada chunk calcula a sua no job |
| `src/render/ecologyPool.ts` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/elements.ts` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/environment.ts` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/exhaust.ts` | outro dono | traffic | fumaça |
| `src/render/farForest.ts` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farGround.ts` | não se aplica | - | chão além do mapa: Terrain3D desenha o mapa todo |
| `src/render/farWoods.ts` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/flatAtlas.ts` | não se aplica | - | placas do chão infinito: aqui Terrain3D (a parte de pintura foi para o mapa de controle) |
| `src/render/flatGround.worker.ts` | não se aplica | - | worker das placas |
| `src/render/flatGroundPool.ts` | não se aplica | - | pool das placas |
| `src/render/fogLayer.ts` | portado | render/atmosphere/fog_field.cpp | janela da neblina pintada |
| `src/render/gardens.ts` | já existia | sim/buildings/b40/lot_yard + render/buildings/lot_props | jardins dos lotes |
| `src/render/gpu/twins.ts` | não se aplica | - | gêmeos WebGPU |
| `src/render/grass.ts` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | tufos e flores nas áreas pintadas; o item de capim alto posto pelo jogador é da aba Rua (elements) |
| `src/render/groundChanges.ts` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundCover.ts` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundShape.ts` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.worker.ts` | não se aplica | - | worker das placas |
| `src/render/groundShapePool.ts` | não se aplica | - | pool das placas |
| `src/render/groundShear.ts` | não se aplica | - | cisalhamento de pessoas no declive: dono people |
| `src/render/impostorShaders.ts` | portado | project/render/shaders/vegetation_cards.gdshader | retrato das árvores no shader |
| `src/render/instanceRuns.ts` | já existia | render/rs_util.h | corridas de instâncias |
| `src/render/postprocess.ts#sharpen` | sem diferença a portar | - | CAS: `quality.sharpen` é 0 nos quatro níveis (`src/render/quality.ts:132`, `:165`, `:200`, `:241`) |
| `src/render/landStroke.ts` | em parte | render/water/shore_field.cpp | nível da água ao lado da terra portado; o traço no worker das placas não se aplica, aqui o comando escreve o Heightfield |
| `src/render/landStrokePool.ts` | não se aplica | - | pool do traço |
| `src/render/lightLevels.ts` | já existia | project/render/daylight.gd, data/game/daylight.json | níveis de luz noturna |
| `src/render/lightning.ts` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | raios |
| `src/render/lowPolyTrees.ts` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/materials.ts` | já existia | project/data/render/materials.json | materiais |
| `src/render/mesh/detailLayer.ts` | portado | project/render/shaders/terrain_layers.gdshaderinc | camada de grão de perto e grão por pixel |
| `src/render/mesh/textureBaker.ts` | não se aplica | - | assador em tempo de build do 4.0; aqui entram as texturas já cozidas |
| `src/render/natureNoise.ts` | já existia | render/trees/tree_place.cpp field() | ruído da densidade da mata |
| `src/render/natureTrees.ts` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/optimizationsApply.ts` | outro dono | interface (gráficos) | interruptores de qualidade |
| `src/render/plantCells.ts` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantImpostors.ts` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plateMesh.ts` | não se aplica | - | malha das placas |
| `src/render/platePaint.ts` | não se aplica | - | pintura por placa: aqui mapa de controle |
| `src/render/plateTexels.ts` | não se aplica | - | texels por placa |
| `src/render/poleCells.ts` | outro dono | street-furniture | postes |
| `src/render/poleLayer.ts` | outro dono | street-furniture | postes |
| `src/render/shadowCasters.ts` | portado em parte | project/render/daylight.gd, `daylight.json` shadow_* | alcance das cascatas segue a câmera (800 m além do ponto olhado, até 8 km); a escolha de quem projeta sombra por tamanho e o redesenho por faixas ficam com o motor |
| `src/render/sunCascades.ts` | portado em parte | project/render/daylight.gd | sol em cascatas pelo Godot, quatro em vez de três, sem as otimizações de armazenar cascata |
| `src/render/effectSwitches.ts` | outro dono | Porte 4 | as chaves e o painel; os valores de oclusão, SMAA e brilho estão em `daylight.json` e `settings.json` |
| `src/render/postprocess.ts` | portado | project/render/shaders/atmosphere.gdshader, sky.gdshader, render/film_grade.cpp, project/render/film_look.gd | nuvens, sombra, névoa, neblina, céu visto do chão, gradação de filme e vinheta; o 4.0 não tem Henyey-Greenstein (aquilo era da fonte Schneider): o halo do sol dele é `sunSide`, portado em `sky.gdshader`; sem nitidez CAS |
| `src/render/propCells.ts` | já existia | render/props | células de props |
| `src/render/propGeometry.ts` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/rain.ts` | portado | project/render/weather_view.gd, shaders/rain.gdshader | chuva |
| `src/render/renderer.ts` | portado | render/render_world.cpp, project/render/* | liga clima, nuvens e natureza ao desenho |
| `src/render/riggedCitizens.ts` | outro dono | people | pessoas |
| `src/render/roadStations.ts` | outro dono | road-elevation | estações da via |
| `src/render/roadStations.worker.ts` | outro dono | road-elevation | estações da via |
| `src/render/roadSurfacePass.ts` | outro dono | road-elevation | superfícies da via |
| `src/render/roadSurfaceRings.ts` | outro dono | road-elevation | superfícies da via |
| `src/render/roadSurfaces.ts` | outro dono | road-elevation | superfícies da via |
| `src/render/scenery.ts` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/sharedGround.ts` | não se aplica | - | chão compartilhado com workers |
| `src/render/signLayer.ts` | outro dono | street-furniture | placas |
| `src/render/signals.ts` | já existia | render/props/signal_lamps | semáforos |
| `src/render/signs.ts` | outro dono | street-furniture | placas |
| `src/render/stagedTexels.ts` | não se aplica | - | envio escalonado de texels |
| `src/render/stillTree.ts` | não se aplica | - | congela a árvore de cena three.js |
| `src/render/streetFurnitureLayer.ts` | outro dono | street-furniture | mobiliário e capim do canteiro |
| `src/render/structureCells.ts` | outro dono | road-elevation | estruturas |
| `src/render/structures.ts` | outro dono | road-elevation | estruturas |
| `src/render/sunCascades.ts` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/terrain.ts` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrainGrid.ts` | já existia | sim/terrain/heightfield.h | grade do terreno |
| `src/render/terrainLevels.ts` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLight.worker.ts` | não se aplica | - | luz no worker: sol do Godot |
| `src/render/terrainLightCompute.ts` | não se aplica | - | luz assada no chão: sol do Godot |
| `src/render/terrainLightPool.ts` | não se aplica | - | pool da luz |
| `src/render/terrainLod.ts` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainShaping.ts` | não se aplica | - | forma do chão por vias: Heightfield modifiers |
| `src/render/torusWindow.ts` | portado | render/atmosphere/fog_field.cpp | janela que dá a volta |
| `src/render/transit.ts` | outro dono | transit | transporte |
| `src/render/utilities.ts` | outro dono | street-furniture | postes e fios (vento nos fios) |
| `src/render/water.ts` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/wind.ts` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/woodsBase.ts` | já existia | render/trees/tree_place.cpp field() | base da densidade da mata |
| `src/ui/toolChoices.ts` | não portado | project/game/tools/brush_defaults.gd | os pincéis abrem nos valores de land_panels.json; a escolha não é lembrada entre usos |
| `src/ui/v2/shell.ts` | portado | project/ui/land_strips.gd, project/data/ui/land_panels.json | abas Solo, Natureza e Céu; Rua é outro dono |
| `src/world/buildings/cameraSolids.ts` | já existia | sim/buildings | sólidos da câmera e pisos dos prédios |
| `src/world/buildings/pads.ts` | já existia | sim/buildings | sólidos da câmera e pisos dos prédios |
| `src/world/cityGen/field.ts` | já existia | sim/citygen | ruído do plano da cidade |
| `src/world/cityGen/plan.ts` | já existia | sim/citygen | ruído do plano da cidade |
| `src/world/cityGen/squares.ts` | já existia | sim/citygen | ruído do plano da cidade |
| `src/world/clouds.ts` | portado | sim/nature/sky.cpp | nuvens colocadas |
| `src/world/doc.ts` | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp | estado da paisagem, desfazer e save |
| `src/world/ecology.ts` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/elements.ts` | portado | sim/nature/elements.cpp, sim/commands/element_commands.cpp, project/data/ui/land_elements.json | elementos do pincel: tipos, pincel de cada tipo, espalhar e apagar |
| `src/world/elevation.ts` | outro dono | road-elevation | greide das vias |
| `src/world/fogPaint.ts` | portado | sim/nature/nature_state.cpp | neblina pintada |
| `src/world/groundPatch.ts` | já existia | sim/terrain/height_patch.h | leitura do chão fora da trava |
| `src/world/gullies.ts` | portado | sim/nature/ground_paint.cpp | ravinas pintadas |
| `src/world/junction/bulb.ts` | outro dono | roads | bulbos e níveis de cruzamento |
| `src/world/junction/levels.ts` | outro dono | roads | bulbos e níveis de cruzamento |
| `src/world/landscape.ts` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/network.ts` | já existia | sim/roads/road_network | rede viária |
| `src/world/poleLines.ts` | outro dono | street-furniture | linhas de poste |
| `src/world/roadField.ts` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roads/buildMode.ts` | outro dono | street-furniture | mobiliário e nomes de rua |
| `src/world/roads/derivedSigns.ts` | outro dono | street-furniture | mobiliário e nomes de rua |
| `src/world/roads/furnitureCatalog.ts` | outro dono | street-furniture | mobiliário e nomes de rua |
| `src/world/roads/furnitureSets.ts` | outro dono | street-furniture | mobiliário e nomes de rua |
| `src/world/roads/streetNames.ts` | outro dono | street-furniture | mobiliário e nomes de rua |
| `src/world/solids.ts` | já existia | sim/buildings | sólidos |
| `src/world/streetFurniture.ts` | outro dono | street-furniture | mobiliário |
| `src/world/structures.ts` | outro dono | road-elevation | estruturas |
| `src/world/terrain.ts` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrainPaint.ts` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/trees.ts` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/weather.ts` | portado | sim/nature/sky.cpp, nature_types.h | clima do mapa |

## Declarações próprias

| Declaração | Estado | Dono no jogo | O que é |
|---|---|---|---|
| `src/core/aabb.ts#expand` | já existia | core/math.h | caixa 2D |
| `src/core/rerootMap.ts#GONE` | não se aplica | - | A key absent from a version (a diff's entry). |
| `src/core/rerootMap.ts#RerootMap` | não se aplica | - | One version of a map: the root holds the values; any other holds its differences from the version it points to |
| `src/core/rerootMap.ts#RerootMap.data` | não se aplica | - | Root: the values (never `GONE`). Otherwise: this version's value of each key where it differs from `up`'s. |
| `src/core/rerootMap.ts#RerootMap.delete` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/core/rerootMap.ts#RerootMap.derive` | não se aplica | - | A new version holding this one's values, to be written; this one keeps its values (read through the new one). |
| `src/core/rerootMap.ts#RerootMap.entries` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/core/rerootMap.ts#RerootMap.get` | não se aplica | - | `V` must not be `undefined`: an absent key reads as undefined. |
| `src/core/rerootMap.ts#RerootMap.note` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/core/rerootMap.ts#RerootMap.prev` | não se aplica | - | While this version is being written: the version it was derived from, told of each key before its first write. |
| `src/core/rerootMap.ts#RerootMap.reroot` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/core/rerootMap.ts#RerootMap.seal` | não se aplica | - | No more writes: the version is what it holds now, and others may be derived from it. |
| `src/core/rerootMap.ts#RerootMap.sealed` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/core/rerootMap.ts#RerootMap.set` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/core/rerootMap.ts#RerootMap.up` | não se aplica | - | The version this one is read through; null for the root. |
| `src/core/rerootMap.ts#RerootMap.writable` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/core/rerootMap.ts#Slot` | não se aplica | - | reancora o mapa em outro centro do mundo sem fim; aqui o mapa tem borda fixa |
| `src/editor/cloudTool.ts#CloudTool` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | THE CLOUD TOOL (Landscape > Terrain > Clouds) |
| `src/editor/cloudTool.ts#CloudTool.cancel` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | pôr, mover, editar e tirar nuvens |
| `src/editor/cloudTool.ts#CloudTool.constructor` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | pôr, mover, editar e tirar nuvens |
| `src/editor/cloudTool.ts#CloudTool.down` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | pôr, mover, editar e tirar nuvens |
| `src/editor/cloudTool.ts#CloudTool.drag` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | pôr, mover, editar e tirar nuvens |
| `src/editor/cloudTool.ts#CloudTool.gesture` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | pôr, mover, editar e tirar nuvens |
| `src/editor/cloudTool.ts#CloudTool.move` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | @perInput The pointer moved to (px, py): the cloud being dragged follows it. |
| `src/editor/cloudTool.ts#CloudTool.pointer` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | pôr, mover, editar e tirar nuvens |
| `src/editor/cloudTool.ts#CloudTool.up` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | pôr, mover, editar e tirar nuvens |
| `src/editor/cloudTool.ts#CloudToolHost` | portado | project/game/tools/cloud_tool.gd, sim/nature/sky.cpp | What the cloud tool reads of the game and does to it. |
| `src/editor/streetscapeTool.ts#MEADOW_CLEAR` | outro dono | street-furniture | aba Rua da Paisagem: fila porte-6 |
| `src/editor/streetscapeTool.ts#ROW_SPACING` | outro dono | street-furniture | aba Rua da Paisagem: fila porte-6 |
| `src/editor/streetscapeTool.ts#StreetscapeTool` | outro dono | street-furniture | THE LANDSCAPING TOOL: benches, bins, street lights, trees, signs... placed on the footways |
| `src/editor/streetscapeTool.ts#StreetscapeTool.constructor` | outro dono | street-furniture | aba Rua da Paisagem: fila porte-6 |
| `src/editor/streetscapeTool.ts#StreetscapeTool.down` | outro dono | street-furniture | @perInput Runs once an input on one thing (a press, a release, a click): nothing it reaches may walk the map. |
| `src/editor/streetscapeTool.ts#StreetscapeTool.hover` | outro dono | street-furniture | Where the chosen item would go under the pointer: a ring, red where it cannot. |
| `src/editor/streetscapeTool.ts#StreetscapeTool.move` | outro dono | street-furniture | @perInput `pressed`: the button still down, dragging a row out from the item just placed. |
| `src/editor/streetscapeTool.ts#StreetscapeTool.reach` | outro dono | street-furniture | Pick radius for a placed item and the reach of the footway snap, world units. |
| `src/editor/streetscapeTool.ts#StreetscapeTool.row` | outro dono | street-furniture | A row being dragged out from the item just placed (placement in line) |
| `src/editor/streetscapeTool.ts#StreetscapeTool.up` | outro dono | street-furniture | The button let go: the row dragged out is laid, one undo step. |
| `src/editor/streetscapeTool.ts#StreetscapeToolHost` | outro dono | street-furniture | What the landscaping tool reads of the game and does to it. |
| `src/editor/streetscapeTool.ts#itemsAround` | outro dono | street-furniture | aba Rua da Paisagem: fila porte-6 |
| `src/editor/streetscapeTool.ts#rowPieces` | outro dono | street-furniture | aba Rua da Paisagem: fila porte-6 |
| `src/editor/terrainBrush.ts#DabSettings` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | Everything a dab reads of the brush in hand, read once per dab. |
| `src/editor/terrainBrush.ts#Landform` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | A landform: a shape and its rock in one tool (`main.ts` LANDFORMS). |
| `src/editor/terrainBrush.ts#NOISE_ORIGIN_REACH` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | How far a dab may be from its stroke's noise origin (`TerrainStamp.ox`) before the stroke takes a new one |
| `src/editor/terrainBrush.ts#Stroke` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | A stroke being painted. |
| `src/editor/terrainBrush.ts#TERRAIN_MIN_MS` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | The shortest time between dabs, ms. |
| `src/editor/terrainBrush.ts#TERRAIN_SPACING` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | Spacing between dabs along a stroke, a fraction of the radius. |
| `src/editor/terrainBrush.ts#TERRAIN_STAMP_MARK` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | Where the terrain stamps' allocator stands in `RoadDoc.allocatorMarks`. |
| `src/editor/terrainBrush.ts#TerrainBrush` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | o que cada pincel de terreno faz |
| `src/editor/terrainBrush.ts#TerrainBrush.at` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | A stroke's noise origin for a dab at `at`, on `at`'s map (moved on to `at` when it has gone too far). |
| `src/editor/terrainBrush.ts#TerrainBrush.begin` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | Starts a stroke, capturing the level target and arming the held repeat. |
| `src/editor/terrainBrush.ts#TerrainBrush.constructor` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | o que cada pincel de terreno faz |
| `src/editor/terrainBrush.ts#TerrainBrush.disarm` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | o que cada pincel de terreno faz |
| `src/editor/terrainBrush.ts#TerrainBrush.end` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | Ends the stroke; true when one was being painted. |
| `src/editor/terrainBrush.ts#TerrainBrush.level` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | False: the dab's tile holds as many of its kind as it may (decision 5.1): nothing laid, the player told by the |
| `src/editor/terrainBrush.ts#TerrainBrush.paint` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | o que cada pincel de terreno faz |
| `src/editor/terrainBrush.ts#TerrainBrush.pointer` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | o que cada pincel de terreno faz |
| `src/editor/terrainBrush.ts#TerrainBrush.repeat` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | A flatten held: it levels on at every frame its interval allows (`tick`). |
| `src/editor/terrainBrush.ts#TerrainBrush.stroke` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | A stroke's noise origin for a dab at `at`, on `at`'s map (moved on to `at` when it has gone too far). |
| `src/editor/terrainBrush.ts#TerrainBrush.stroking` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | The shortest time between dabs now, ms: what the last rebuild cost, during a stroke or not. |
| `src/editor/terrainBrush.ts#TerrainBrush.tick` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | Run from the frame (`main.ts` frame), as the game's other timed work |
| `src/editor/terrainBrush.ts#TerrainBrushHost` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | What the stroke reads of the game and does to it. |
| `src/editor/terrainBrush.ts#noiseOrigin` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | A stroke's noise origin for a dab at `at`, on `at`'s map (moved on to `at` when it has gone too far). |
| `src/editor/terrainBrush.ts#stampTerrain` | portado | sim/terrain/terrain_brush.cpp, sim/commands/land_commands.cpp, project/game/tools/terrain_tool.gd | False: the dab's tile holds as many of its kind as it may (decision 5.1): nothing laid, the player told by the |
| `src/jobs/jobPool.ts#JobPort.constructor` | já existia | core/job_pool.h | pool de jobs |
| `src/jobs/jobPool.ts#JobPort.id` | já existia | core/job_pool.h | pool de jobs |
| `src/jobs/jobPool.ts#JobPort.postSetup` | já existia | core/job_pool.h | pool de jobs |
| `src/jobs/jobPool.ts#postSetup` | já existia | core/job_pool.h | pool de jobs |
| `src/lab/catalog/layers.ts#waterItems.basin` | não se aplica | - | laboratório do 4.0, não é jogo |
| `src/lab/catalog/layers.ts#waterItems.bed` | não se aplica | - | laboratório do 4.0, não é jogo |
| `src/render/bake/detailRecipes.ts#DETAIL_WORLD_SIZE` | portado | project/assets/surfaces/grass.detail*.png, terrain_layers.gdshaderinc | World units one tile of each detail family covers. |
| `src/render/bake/detailRecipes.ts#DetailKind` | portado | project/assets/surfaces/grass.detail*.png, terrain_layers.gdshaderinc | grão de perto da grama, a textura cozida do 4.0 convertida por tools/assets/ktx2_to_png.mjs; a receita não roda no jogo; solo fino não ligado |
| `src/render/bake/detailRecipes.ts#SIZE` | portado | project/assets/surfaces/grass.detail*.png, terrain_layers.gdshaderinc | Texels across a detail texture. |
| `src/render/bake/detailRecipes.ts#bake` | portado | project/assets/surfaces/grass.detail*.png, terrain_layers.gdshaderinc | Bakes one detail family. |
| `src/render/bake/detailRecipes.ts#blade` | portado | project/assets/surfaces/grass.detail*.png, terrain_layers.gdshaderinc | Draws a thin tapered stroke - one grass blade seen from above. |
| `src/render/bake/detailRecipes.ts#detailTexels` | portado | project/assets/surfaces/grass.detail*.png, terrain_layers.gdshaderinc | One detail family's texels, rows top first as a canvas holds them: its albedo (linear data around 0.5, a multi |
| `src/render/bake/detailRecipes.ts#stamp` | portado | project/assets/surfaces/grass.detail*.png, terrain_layers.gdshaderinc | Adds a soft round mark into a wrapped float field. |
| `src/render/bake/terrainRecipes.ts#GRASS_TONES` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#ROCK_RECIPES` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | The rocks, two layers each - its FACE, read by the side projections of the triplanar mapping |
| `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.basaltFace` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.basaltTop` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.graniteFace` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.graniteTop` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.layer` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.sandstoneFace` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.sandstoneTop` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#ROCK_SIZE` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | Texels across a rock layer; every layer of the array is this size. |
| `src/render/bake/terrainRecipes.ts#ROCK_WORLD` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | World units one rock tile covers (`uRockScale`). |
| `src/render/bake/terrainRecipes.ts#cell` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | The nearest and second-nearest jittered points of an n x n tiling grid. |
| `src/render/bake/terrainRecipes.ts#cellular` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#latticeHash` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | 0..1 hash of an integer lattice point. |
| `src/render/bake/terrainRecipes.ts#makeNoiseXY` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | Value noise tiling with its own period on each axis: a streak long down a face and narrow across it, still sea |
| `src/render/bake/terrainRecipes.ts#makeNoiseXY.smooth` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#smoothstep` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | texturas do chão e das rochas; o mapa nunca chegava ao shader (sampler2D sobre array) e agora chega |
| `src/render/bake/terrainRecipes.ts#streakNoise` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | Tiling noise stretched along v: the mean of samples stacked up the column |
| `src/render/bake/terrainRecipes.ts#terrainRecipes` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | The land's surfaces: the grass, the rock layers (in `ROCK_RECIPES` order: one texture array) and the dirt. |
| `src/render/bake/terrainRecipes.ts#tintTowards` | portado | project/assets/surfaces/terrain-land.map.png, terrain.gdshader | Mixes `out`'s colour towards an sRGB colour by `w`. |
| `src/render/canyonLight.ts#CELL` | outro dono | interface (gráficos) | oclusão na rua (quality.ambientOcclusion): Godot SSAO/SSIL |
| `src/render/canyonLight.ts#CanyonLight` | outro dono | interface (gráficos) | oclusão na rua (quality.ambientOcclusion): Godot SSAO/SSIL |
| `src/render/canyonLight.ts#FRAGMENT` | outro dono | interface (gráficos) | oclusão na rua (quality.ambientOcclusion): Godot SSAO/SSIL |
| `src/render/canyonLight.ts#MASK` | outro dono | interface (gráficos) | The field is a torus: world cell (i, j) is texel (i & MASK, j & MASK), so a window moved keeps every cell it s |
| `src/render/canyonLight.ts#OFFSET` | outro dono | interface (gráficos) | oclusão na rua (quality.ambientOcclusion): Godot SSAO/SSIL |
| `src/render/canyonLight.ts#SIDE` | outro dono | interface (gráficos) | oclusão na rua (quality.ambientOcclusion): Godot SSAO/SSIL |
| `src/render/canyonLight.ts#SNAP` | outro dono | interface (gráficos) | The window's corner moves in steps of this many cells: what a step brings in is a strip, laid in the frame it  |
| `src/render/canyonLight.ts#STEP` | outro dono | interface (gráficos) | Heights stored in 16 bits: (h + OFFSET) / STEP. |
| `src/render/canyonLight.ts#WHOLE_ROWS` | outro dono | interface (gráficos) | Past this many rows written, the whole field goes up in one call (three sends one call a row of ranges). |
| `src/render/canyonLight.ts#WINDOW` | outro dono | interface (gráficos) | The field's side, world units, and its cell. |
| `src/render/canyonLight.ts#createCanyonLight` | outro dono | interface (gráficos) | `sunsOf`: the sun's cascades, nearest first (`sunCascades.ts`); the two nearest are read. |
| `src/render/canyonLight.ts#createCanyonLight.clearCells` | outro dono | interface (gráficos) | The window's cells i0..i1, j0..j1 (world cells, clipped to the window) cleared; their world rectangle, or null |
| `src/render/canyonLight.ts#createCanyonLight.dispose` | outro dono | interface (gráficos) | oclusão na rua (quality.ambientOcclusion): Godot SSAO/SSIL |
| `src/render/canyonLight.ts#createCanyonLight.follow` | outro dono | interface (gráficos) | @rebuild The view's centre (map x, y), the town's solids, and where they changed since the last call |
| `src/render/canyonLight.ts#createCanyonLight.layVolume` | outro dono | interface (gráficos) | One volume laid into the window's cells it covers (by the cell's centre), the highest kept. |
| `src/render/canyonLight.ts#createCanyonLight.markRow` | outro dono | interface (gráficos) | oclusão na rua (quality.ambientOcclusion): Godot SSAO/SSIL |
| `src/render/canyonLight.ts#createCanyonLight.setOcclusion` | outro dono | interface (gráficos) | The ambient occlusion multiplied into the picture first (GTAO's denoised target, its blend intensity), or none |
| `src/render/canyonLight.ts#createCanyonLight.setSun` | outro dono | interface (gráficos) | The sun's direction (world, to the sun) and its share of the light on the ground. |
| `src/render/canyonLight.ts#createCanyonLight.upload` | outro dono | interface (gráficos) | The rows written go up: each a range, or the whole field past `WHOLE_ROWS`. |
| `src/render/cloudShadowStamp.ts#CloudShadowStamp` | não se aplica | - | carimbo do mapa de sombra das nuvens: aqui a sombra é analítica por puff no passe de atmosfera, sem mapa |
| `src/render/cloudShadowStamp.ts#CloudShadowStamp.changed` | não se aplica | - | Whether the map drawn from these would differ from the one held; when it would, these become the held. |
| `src/render/cloudShadowStamp.ts#CloudShadowStamp.held` | não se aplica | - | carimbo do mapa de sombra das nuvens: aqui a sombra é analítica por puff no passe de atmosfera, sem mapa |
| `src/render/cloudShadowStamp.ts#CloudShadowStamp.invalidate` | não se aplica | - | The map no longer holds what was drawn (its target lost or made again): the next call draws. |
| `src/render/cloudShadowStamp.ts#CloudShadowStamp.next` | não se aplica | - | carimbo do mapa de sombra das nuvens: aqui a sombra é analítica por puff no passe de atmosfera, sem mapa |
| `src/render/cloudShadowStamp.ts#CloudShadowStamp.valid` | não se aplica | - | carimbo do mapa de sombra das nuvens: aqui a sombra é analítica por puff no passe de atmosfera, sem mapa |
| `src/render/cloudShadowStamp.ts#SAME` | não se aplica | - | Below this (world units) two inputs of the clouds' shadow map are the same: far under a float32 step of the sh |
| `src/render/coverRaster.ts#FOREST_RES` | portado em parte | sim/nature/ground_paint.cpp, render/cover/cover_renderer.cpp | mato e flores ao pintar; a rocha do lugar nao tinge a pedra |
| `src/render/coverRaster.ts#GRID` | portado em parte | sim/nature/ground_paint.cpp, render/cover/cover_renderer.cpp | mato e flores ao pintar; a rocha do lugar nao tinge a pedra |
| `src/render/coverRaster.ts#coverOf` | portado em parte | sim/nature/ground_paint.cpp, render/cover/cover_renderer.cpp | mato e flores ao pintar; a rocha do lugar nao tinge a pedra |
| `src/render/coverRaster.ts#geologyOf` | portado em parte | sim/nature/ground_paint.cpp, render/cover/cover_renderer.cpp | mato e flores ao pintar; a rocha do lugar nao tinge a pedra |
| `src/render/coverRaster.ts#rasterCover` | portado em parte | sim/nature/ground_paint.cpp, render/cover/cover_renderer.cpp | mato e flores ao pintar; a rocha do lugar nao tinge a pedra |
| `src/render/coverRaster.ts#rasterGeology` | portado em parte | sim/nature/ground_paint.cpp, render/cover/cover_renderer.cpp | mato e flores ao pintar; a rocha do lugar nao tinge a pedra |
| `src/render/ecology.worker.ts#serve` | já existia | core/job_pool.h + render/trees | This service of the job system (`job.worker.ts`): its messages answered on the scope it is given. |
| `src/render/ecologyKept.ts#ECOLOGY_HASH` | não se aplica | - | The fingerprints of the code they are worked out by (`cook-plugin.ts`): kept under them; none (tests): nothing |
| `src/render/ecologyKept.ts#ECOLOGY_PREFIX` | não se aplica | - | guarda ecologia no worker; aqui cada chunk calcula a sua no job |
| `src/render/ecologyKept.ts#ECOLOGY_WRITES_MOST` | não se aplica | - | Bytes of plates' fields this thread may have handed IndexedDB and not seen committed (a near plate ~5 MB): the |
| `src/render/ecologyKept.ts#EcologyCache` | não se aplica | - | Where they are kept: one entry a plate (by its centre), its land's digest beside it, written over when the lan |
| `src/render/ecologyKept.ts#Kept` | não se aplica | - | guarda ecologia no worker; aqui cada chunk calcula a sua no job |
| `src/render/ecologyKept.ts#WOODS_HASH` | não se aplica | - | guarda ecologia no worker; aqui cada chunk calcula a sua no job |
| `src/render/ecologyKept.ts#WOODS_PREFIX` | não se aplica | - | guarda ecologia no worker; aqui cada chunk calcula a sua no job |
| `src/render/ecologyKept.ts#__ECOLOGY_HASH__` | não se aplica | - | guarda ecologia no worker; aqui cada chunk calcula a sua no job |
| `src/render/ecologyKept.ts#__FAR_WOODS_HASH__` | não se aplica | - | guarda ecologia no worker; aqui cada chunk calcula a sua no job |
| `src/render/ecologyKept.ts#derivedEcologyCache` | não se aplica | - | The derived cache under a write budget of its own: one a thread that works the plates out (`ecology.worker.ts` |
| `src/render/ecologyKept.ts#derivedEcologyCache.get` | não se aplica | - | guarda ecologia no worker; aqui cada chunk calcula a sua no job |
| `src/render/ecologyKept.ts#derivedEcologyCache.put` | não se aplica | - | Resolved once the store took its copy (the arrays may be transferred after). |
| `src/render/ecologyKept.ts#digestKey` | não se aplica | - | Where an entry's digest is kept beside it, a few bytes: read first, so a plate whose land changed is not read  |
| `src/render/ecologyKept.ts#ecologyDigest` | não se aplica | - | Everything a plate's field is read from: the grid, its place, the settings, the land, the water, the rock and  |
| `src/render/ecologyKept.ts#keep` | não se aplica | - | Keeps an entry and its digest beside it. |
| `src/render/ecologyKept.ts#keptEcology` | não se aplica | - | A plate's field: the one kept for the same land when there is one, else worked out and kept. |
| `src/render/ecologyKept.ts#keptFor` | não se aplica | - | The entry kept for `digest`, or undefined: the whole entry read only when its digest beside it is the same, or |
| `src/render/ecologyKept.ts#keptWoods` | não se aplica | - | A far plate's woods: the ones kept for the same ask (its land is the relief's, its ecosystem the settings') wh |
| `src/render/ecologyPool.ts#FarJob` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#Job` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#Running` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#broken` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#busy` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#ecologyOffPage` | já existia | core/job_pool.h | Whether ecosystems can be read off the page here (a browser with workers). |
| `src/render/ecologyPool.ts#listeners` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#nextId` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#onEcologyRead` | já existia | core/job_pool.h | Called each time a field comes back (the scene draws again). |
| `src/render/ecologyPool.ts#pump` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#queue` | já existia | core/job_pool.h | The plates' ecosystems first, in the order asked; the far woods after all of them. |
| `src/render/ecologyPool.ts#readEcology` | já existia | core/job_pool.h | Reads plate `key`'s ecosystem in the worker: `done` with the field, or `failed` (the caller reads it on the pa |
| `src/render/ecologyPool.ts#readFarWoods` | já existia | core/job_pool.h | Works out plate `key`'s far woods in the worker (`farWoods.ts`), after every plate's ecosystem asked |
| `src/render/ecologyPool.ts#start` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/ecologyPool.ts#worker` | já existia | core/job_pool.h | pool da ecologia |
| `src/render/elements.ts#CASTS` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | The props that throw a shadow worth its pass. |
| `src/render/elements.ts#EFFECTS` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | Effects: the particle each gives off, how many a second at full intensity, a puff's size (times the element's) |
| `src/render/elements.ts#EFFECT_REACH` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | Effects farther than this from the camera give off nothing (a budget for the one particle pool). |
| `src/render/elements.ts#ELEMENT_SHADOW_REACH` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | How far off the view a prop still throws a shadow into it. |
| `src/render/elements.ts#ElementKit` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#ElementLayer` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#ElementSource` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | What the layer reads of the document: every element (a whole build only) and the elements by place. |
| `src/render/elements.ts#Emitter` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | An effect giving off its particles: where it stands, and the particles owed since the last frame. |
| `src/render/elements.ts#MODELS` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | Each prop kind's models (several variants, picked by an instance's seed). |
| `src/render/elements.ts#Part` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | One part of a model: its geometry (one unit across, standing on y = 0) and its material. |
| `src/render/elements.ts#allByCell` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | @rebuild Every element of the map by the cell it stands in, and every cell drawn before: a map opened or repla |
| `src/render/elements.ts#createElementLayer` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | DO NOT build every element of the map again when one is laid: the elements kept cell by cell, instanced per ki |
| `src/render/elements.ts#createElementLayer.cellOf` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | One cell's elements: its props into a run, its effects as emitters. |
| `src/render/elements.ts#createElementLayer.cull` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | Keeps the instances the camera can see (`InstanceRuns.cull`). |
| `src/render/elements.ts#createElementLayer.dispose` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#createElementLayer.emit` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | Gives off the effects' particles near the camera (three's x, y, z), `dt` seconds on. |
| `src/render/elements.ts#createElementLayer.give` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | An effect giving off near the eye. |
| `src/render/elements.ts#createElementLayer.sync` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | The cells (`cellKeeps.ts` keep cells) made again from the elements standing in them (null: every cell); whethe |
| `src/render/elements.ts#elementsInto` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | Elements put in a run of the layer's stores (`storesOf`), each on the ground at its point, turned and sized, t |
| `src/render/elements.ts#fallenLeavesTexture` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | A card of fallen leaves - a dozen leaves in autumn browns, ochres and dull reds, veined - on a clear ground. |
| `src/render/elements.ts#fallenLeavesTexture.rand` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#loadElementKit` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#loadElementKit.dispose` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#loadElementKit.materialFor` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#loadElementKit.textureOf` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#readParts` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#readParts.read` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | elementos colocados: modelos por MultiMesh e efeitos por emissores |
| `src/render/elements.ts#url` | portado | render/cover/element_models.cpp, render/cover/cover_renderer.cpp, project/render/element_effects.gd | The files' served URLs (the game serves no public folder): one module for the folder (`model-urls-plugin.ts`) |
| `src/render/environment.ts#DAY_HORIZON` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#DAY_SUN` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#DAY_ZENITH` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#DUSK_HORIZON` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#DUSK_SUN` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#EnvironmentQuality` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#MOON` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#NIGHT_HORIZON` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#NIGHT_ZENITH` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#SKY_FRAGMENT` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#SKY_VERTEX` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#SUN_AZIMUTH` | portado | project/render/daylight.gd, sky_view.gd | Sun bearing — and the one number that decides whether shadows are VISIBLE. |
| `src/render/environment.ts#SUN_ELEVATION` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#SceneEnvironment` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#createEnvironment` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#createEnvironment.directShare` | portado | project/render/daylight.gd, sky_view.gd | How much of the light on level ground comes straight from the sun, 0..1 |
| `src/render/environment.ts#createEnvironment.dispose` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#createEnvironment.follow` | portado | project/render/daylight.gd, sky_view.gd | Fits the sun's cascades to what the camera sees (`sunCascades.ts`). |
| `src/render/environment.ts#createEnvironment.remakeShadowMaps` | portado | project/render/daylight.gd, sky_view.gd | The cascades' maps made again for the renderer's shadow map type (`SunCascades.remakeMaps`). |
| `src/render/environment.ts#createEnvironment.setCasters` | portado | project/render/daylight.gd, sky_view.gd | What chooses its casters by each cascade's texel (`SunCascades.setCasters`). |
| `src/render/environment.ts#createEnvironment.setFarCascadeEvery` | portado | project/render/daylight.gd, sky_view.gd | The far cascades drawn at most every `n` passes (`SunCascades.setFarEvery`). |
| `src/render/environment.ts#createEnvironment.setMovers` | portado | project/render/daylight.gd, sky_view.gd | What moves and is drawn into the shadows (`sunCascades.ts` `Movers`). |
| `src/render/environment.ts#createEnvironment.setQuality` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#createEnvironment.setSmog` | portado | project/render/daylight.gd, sky_view.gd | Smoke in the air, 0..1: the fog closes in and browns, the light dims. |
| `src/render/environment.ts#createEnvironment.setTimeOfDay` | portado | project/render/daylight.gd, sky_view.gd | The time of day, minutes after midnight: the sun's path, the sky, the light; the moon at night. |
| `src/render/environment.ts#createEnvironment.setWeather` | portado | project/render/daylight.gd, sky_view.gd | The weather on the light (`world/weather.ts`): how overcast |
| `src/render/environment.ts#createEnvironment.shadowCover` | portado | project/render/daylight.gd, sky_view.gd | The world box the sun's cascades cover, grown by `margin` (`SunCascades.coverBox`): false without shadows. |
| `src/render/environment.ts#createEnvironment.shadowMaps` | portado | project/render/daylight.gd, sky_view.gd | The cascades' maps, for the video memory count. |
| `src/render/environment.ts#createEnvironment.shadowNearTexel` | portado | project/render/daylight.gd, sky_view.gd | The near cascade's texel, world units (`SunCascades.nearTexel`). |
| `src/render/environment.ts#createEnvironment.shadowPass` | portado | project/render/daylight.gd, sky_view.gd | Marks the cascades to draw this frame; true when any is (the renderer's `shadowMap.needsUpdate`). |
| `src/render/environment.ts#createEnvironment.shadowStats` | portado | project/render/daylight.gd, sky_view.gd | The cascades' passes since the last call (`SunCascades.takeStats`). |
| `src/render/environment.ts#slopeScaledShadowOffset` | portado | project/render/daylight.gd, sky_view.gd | céu, sol e tempo fechado; sombras são do Godot |
| `src/render/environment.ts#sunShadowChunks` | portado | project/render/daylight.gd, sky_view.gd | The sun's shadow in three's shaders: its cascades (`sunCascades.ts`) and its normal offset by the slope of the |
| `src/render/farForest.ts#DISSOLVE` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarForest` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarForestStats` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarWoodsBook` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | The far woods' bookkeeping, no `three`: which plates' woods to ask, which are in, each plate's fade |
| `src/render/farForest.ts#FarWoodsBook.add` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | A far plate come into the view's ring, in `slot`: its woods asked once the land is known. |
| `src/render/farForest.ts#FarWoodsBook.arrived` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | The keys of the plates whose woods came in (or went with the land): the instances to write. |
| `src/render/farForest.ts#FarWoodsBook.ask` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Asks the worker for a plate's woods (`ecologyPool.ts` readFarWoods): injected, so a spec counts the asks. |
| `src/render/farForest.ts#FarWoodsBook.asks` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Plates whose woods were asked of the worker, and how many times (each land and ecosystem once). |
| `src/render/farForest.ts#FarWoodsBook.budget` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | The trees per 1920 m square the quality tier allows (`setVegetation`). |
| `src/render/farForest.ts#FarWoodsBook.constructor` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarWoodsBook.drop` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | A far plate gone out of the ring: its woods let go (asked again if it comes back). |
| `src/render/farForest.ts#FarWoodsBook.gone` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | The keys of the plates let go: their meshes to drop. |
| `src/render/farForest.ts#FarWoodsBook.land` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarWoodsBook.landKey` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarWoodsBook.landOf` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarWoodsBook.plates` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | The far plates there are now, by key (`add`, `drop`): those round the view, never a list of the world's. |
| `src/render/farForest.ts#FarWoodsBook.rescaled` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | The keys of the plates whose scale alone changed (the quality tier): their picks sent again, their meshes kept |
| `src/render/farForest.ts#FarWoodsBook.scaleOf` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarWoodsBook.setFull` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Whether plate `key` is drawn in full, and whether its own ecosystem is in (its near trees can grow); a plate n |
| `src/render/farForest.ts#FarWoodsBook.setVegetation` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | The vegetation the quality tier allows, as the near sweep reads it |
| `src/render/farForest.ts#FarWoodsBook.stats` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#FarWoodsBook.step` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Moves the fades to `now`, seconds; true when any changed. |
| `src/render/farForest.ts#FarWoodsBook.sync` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | The land and the ecosystem: when either changed, every far plate's woods asked again (in the background, the n |
| `src/render/farForest.ts#FarWoodsReader` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Asks the worker for a plate's woods (`ecologyPool.ts` readFarWoods): injected, so a spec counts the asks. |
| `src/render/farForest.ts#NEAR_GROWS` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | How long after a plate's own trees could grow its far ones start to go, and how long they take, seconds. |
| `src/render/farForest.ts#PlateState` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#SLOT` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Most trees a plate draws: its budget at the map's scale with room (`farWoods.ts` HEADROOM). |
| `src/render/farForest.ts#THIN_LEAST` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#THIN_SHARE` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Under this share of the screen's height a tree's wood thins (about 6 px at 1080) |
| `src/render/farForest.ts#createFarForest` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | `slots`: the most far plates there are at once (`flatAtlas.ts` the far ring), each fading by its slot. |
| `src/render/farForest.ts#createFarForest.arraysOf` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | A plate's trees as its instances: x, height, three's z, size; the model and the plate's slot, the pick, the ya |
| `src/render/farForest.ts#createFarForest.dispose` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | matas distantes por retrato |
| `src/render/farForest.ts#createFarForest.quadOf` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | A quad drawn once a tree: the instances' corners. |
| `src/render/farForest.ts#createFarForest.unwrite` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Plate `key`'s mesh let go. |
| `src/render/farForest.ts#createFarForest.update` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Each frame: the pictures taken once, the instances of plates come in written, the fades moved. |
| `src/render/farForest.ts#createFarForest.write` | portado | render/trees/far_cards.cpp, tree_renderer.cpp | Plate `key`'s mesh made again from its woods (or let go). |
| `src/render/farGround.ts#FarNode` | não se aplica | - | One node of the far ground chosen for the view: its level, its key, its square (world units; clipped to the ma |
| `src/render/farGround.ts#SPLIT_ABOVE` | não se aplica | - | chão além do mapa: Terrain3D desenha o mapa todo |
| `src/render/farGround.ts#SPLIT_PLATES` | não se aplica | - | A node is split while the view is nearer its box than this many times its side: level 1's (from 2 x 2 plates d |
| `src/render/farGround.ts#farExtent` | não se aplica | - | The map's plates: `ring` plates each way round the origin's plate (`flatAtlas.ts` FAR_RING), its edge half a p |
| `src/render/farGround.ts#farNodesFor` | não se aplica | - | chão além do mapa: Terrain3D desenha o mapa todo |
| `src/render/farGround.ts#farNodesFor.visit` | não se aplica | - | chão além do mapa: Terrain3D desenha o mapa todo |
| `src/render/farWoods.ts#BUDGET_AREA` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FAR_WOODS_BUDGET` | portado | render/trees/tree_place.cpp | The woods the ecosystem grows at full vegetation, per 1920 m square (`renderer.ts` NATURE_TREES). |
| `src/render/farWoods.ts#FarWoods` | portado | render/trees/tree_place.cpp | One plate's far trees, as parallel arrays (transferred from the worker, not copied). |
| `src/render/farWoods.ts#FarWoodsAsk` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FarWoodsGrid` | portado | render/trees/tree_place.cpp | The terrain's corners per plate side, the cell between them, the plate's half side (`terrain.ts` GRID, TERRAIN |
| `src/render/farWoods.ts#FarWoodsWork` | portado | render/trees/tree_place.cpp | What the far woods work in, plate after plate, owned by the worker that works them (`ecology.worker.ts`): grow |
| `src/render/farWoods.ts#FarWoodsWork.band` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FarWoodsWork.dry` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FarWoodsWork.ecology` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FarWoodsWork.far` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FarWoodsWork.heights` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FarWoodsWork.list` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FarWoodsWork.odds` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#FarWoodsWork.zeros` | portado | render/trees/tree_place.cpp | No rock painted and no water on a far plate: zeros, never written. |
| `src/render/farWoods.ts#HEADROOM` | portado | render/trees/tree_place.cpp | Trees kept past the plate's own scale: room for the map's scale, an average over the plates, to be larger. |
| `src/render/farWoods.ts#MODELS` | portado | render/trees/tree_place.cpp | The trees' models, in seed order (`natureTrees.ts` TREE_MODELS): oak, oak, cypress, cypress, palm, palm. |
| `src/render/farWoods.ts#SLOTS` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#cellHash` | portado | render/trees/tree_place.cpp | A cell's hash in 0..1 (`renderer.ts` cellHash): the same numbers, so the far trees are the near ones. |
| `src/render/farWoods.ts#farWoods` | portado | render/trees/tree_place.cpp | Works out one plate's far trees, in `work` (one of its own unless handed). |
| `src/render/farWoods.ts#farWoods.farGround` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#farWoods.natural` | portado | render/trees/tree_place.cpp | árvores distantes de um trecho |
| `src/render/farWoods.ts#seedOf` | portado | render/trees/tree_place.cpp | `natureTrees.ts` seedOfKind, without its `three`: a seed (0..1) that grows as this kind, `r` choosing among it |
| `src/render/farWoods.ts#woodsScale` | portado | render/trees/tree_place.cpp | The woods' scale at which a plate grows its budget (`renderer.ts` natureSweep treeScale), from the plates' ave |
| `src/render/fogLayer.ts#FOG_BLOCK` | portado | render/atmosphere/fog_field.cpp | Texels a block of a row: each block's slab from its half floats kept, read back only when texels of it were wr |
| `src/render/fogLayer.ts#FOG_BLOCKS` | portado | render/atmosphere/fog_field.cpp | janela da neblina pintada |
| `src/render/fogLayer.ts#FOG_CELL` | portado | render/atmosphere/fog_field.cpp | janela da neblina pintada |
| `src/render/fogLayer.ts#FOG_HOLD` | portado | render/atmosphere/fog_field.cpp | How far the view goes from the window's middle before it follows (texels): the texels that came in written, no |
| `src/render/fogLayer.ts#FOG_RES` | portado | render/atmosphere/fog_field.cpp | Texels across the window: 20 m a texel over its 10,240 m (the old map's, so the same fog where it was). |
| `src/render/fogLayer.ts#FogField` | portado | render/atmosphere/fog_field.cpp | DO NOT raster the whole window at every dab: 262 144 texels and a ground height each, for a brush's few metres |
| `src/render/fogLayer.ts#FogLayer` | portado | render/atmosphere/fog_field.cpp | janela da neblina pintada |
| `src/render/fogLayer.ts#Rect` | portado | render/atmosphere/fog_field.cpp | A world rectangle: [minX, minY, maxX, maxY]. |
| `src/render/fogLayer.ts#createFogField` | portado | render/atmosphere/fog_field.cpp | janela da neblina pintada |
| `src/render/fogLayer.ts#createFogField.clear` | portado | render/atmosphere/fog_field.cpp | No fog anywhere: the window left as it is, every row empty. |
| `src/render/fogLayer.ts#createFogField.follow` | portado | render/atmosphere/fog_field.cpp | The window kept round where the view looks (world x, y): only the texels that came in written; null when it st |
| `src/render/fogLayer.ts#createFogField.layer` | portado | render/atmosphere/fog_field.cpp | janela da neblina pintada |
| `src/render/fogLayer.ts#createFogField.raster` | portado | render/atmosphere/fog_field.cpp | World texels [gx0, gx1] x [gy0, gy1] of the window worked out from `dabs` (those that can reach them); their r |
| `src/render/fogLayer.ts#createFogField.readBack` | portado | render/atmosphere/fog_field.cpp | janela da neblina pintada |
| `src/render/fogLayer.ts#createFogField.update` | portado | render/atmosphere/fog_field.cpp | Only the texels inside `rects`. |
| `src/render/fogLayer.ts#createFogField.whole` | portado | render/atmosphere/fog_field.cpp | Every texel, from these dabs and this ground (the first time, or when the diary cannot say where). |
| `src/render/fogLayer.ts#createFogTexture` | portado | render/atmosphere/fog_field.cpp | janela da neblina pintada |
| `src/render/grass.ts#CLUMP_REACH` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | The largest clump's radius (`grassClump`: one to three metres). |
| `src/render/grass.ts#CLUMP_READ_REACH` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | How far round its item a clump reads the ground and the roads: its radius and the slope's probe. |
| `src/render/grass.ts#Clump` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | A clump's grass: its tufts and its wildflowers. |
| `src/render/grass.ts#FLOWER_COLOURS` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | tufos e flores nas áreas pintadas; o item de capim alto posto pelo jogador é da aba Rua (elements) |
| `src/render/grass.ts#FLOWER_MIN` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | tufos e flores nas áreas pintadas; o item de capim alto posto pelo jogador é da aba Rua (elements) |
| `src/render/grass.ts#FLOWER_RANGE` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | tufos e flores nas áreas pintadas; o item de capim alto posto pelo jogador é da aba Rua (elements) |
| `src/render/grass.ts#NONE` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | tufos e flores nas áreas pintadas; o item de capim alto posto pelo jogador é da aba Rua (elements) |
| `src/render/grass.ts#SLOPE_PROBE` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | How far round a clump's centre the slope is read (`grassClump`). |
| `src/render/grass.ts#TUFT_MIN` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | Real turf height, root to tip. |
| `src/render/grass.ts#TUFT_RANGE` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | tufos e flores nas áreas pintadas; o item de capim alto posto pelo jogador é da aba Rua (elements) |
| `src/render/grass.ts#Tuft` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | tufos e flores nas áreas pintadas; o item de capim alto posto pelo jogador é da aba Rua (elements) |
| `src/render/grass.ts#grassClump` | portado em parte | render/cover/cover_kit.cpp, cover_renderer.cpp | The clump at a placed item, from its own generator (seeded by its id, so an edit elsewhere moves none of it) |
| `src/render/groundChanges.ts#Area` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundChanges.ts#GroundChanges` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundChanges.ts#GroundChanges.forgotten` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundChanges.ts#GroundChanges.log` | já existia | sim/state/change_log.h | Changes after `forgotten`, oldest first; `null` is the whole map. |
| `src/render/groundChanges.ts#GroundChanges.mark` | já existia | sim/state/change_log.h | The ground changed in these rectangles (`null`: everywhere). An empty list changes nothing. |
| `src/render/groundChanges.ts#GroundChanges.rectsSince` | já existia | sim/state/change_log.h | Where the ground changed after `since`; null when the record cannot say (the whole map, or forgotten). |
| `src/render/groundChanges.ts#GroundChanges.serial` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundChanges.ts#GroundChanges.touches` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundChanges.ts#GroundChanges.version` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundChanges.ts#GroundRecord` | já existia | sim/state/change_log.h | What a dependant reads: the serial of the latest change, and whether a change since one reaches an area. |
| `src/render/groundChanges.ts#KEPT` | já existia | sim/state/change_log.h | How many changes are remembered; a dependant older than that is built again. |
| `src/render/groundChanges.ts#Rect` | já existia | sim/state/change_log.h | A world rectangle: [minX, minY, maxX, maxY]. |
| `src/render/groundChanges.ts#isRect` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundChanges.ts#overlaps` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundChanges.ts#unionRect` | já existia | sim/state/change_log.h | canal de mudanças do chão |
| `src/render/groundCover.ts#COVER_POSE` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A cover kept as its pose (`coverPoseInto`): place in the scene's frame, yaw, lean and scale on three axes (8 f |
| `src/render/groundCover.ts#COVER_STANCE` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | How each kind of cover stands: sunk by a share of its height, tilted, and whether it stands straight (a tree). |
| `src/render/groundCover.ts#CoverPlacement` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#FOREST_KIND` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | Each forest species as the game's one tree style grows it (`lowPolyTrees.ts`). |
| `src/render/groundCover.ts#FOREST_SPECIES` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#FOREST_WIND` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | The forest's sway: the crown more than the trunk, as the garden trees. |
| `src/render/groundCover.ts#ForestSpecies` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | The trees a painted forest grows. |
| `src/render/groundCover.ts#GroundCoverKit` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#ROCK_VARIANTS` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#SCRUB_VARIANTS` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#TreeModel` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | One tree model: its body and its foliage cards (none on a conifer). |
| `src/render/groundCover.ts#TreePlacement` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A tree: `size` is its height. |
| `src/render/groundCover.ts#coverPoseInto` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | One placement's pose, into `out` at `at` (`COVER_POSE`): on its ground, turned, leaning a little, sized by its |
| `src/render/groundCover.ts#coverVariant` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A cover's variant among `variants`, by its seed. |
| `src/render/groundCover.ts#createGroundCoverKit` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#createGroundCoverKit.dispose` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#leanedMatrixInto` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | Three's `compose` (r186 `Matrix4.compose`) of a turn by `yaw` about the up axis times a lean by `lean` about t |
| `src/render/groundCover.ts#noise3` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A deterministic 3-D value noise in -1..1, enough for a stone's or a bush's shape. |
| `src/render/groundCover.ts#noise3.hash` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#noise3.lerp` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#noise3.plane` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | pedras, moitas e floresta pintada |
| `src/render/groundCover.ts#rockGeometry` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A stone: 36 triangles, non-indexed so every face is flat. |
| `src/render/groundCover.ts#rockTint` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A stone's colour: warm grey with some brown and some moss-green ones. |
| `src/render/groundCover.ts#scrubGeometry` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A low bush: three lumps of foliage |
| `src/render/groundCover.ts#scrubTint` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A bush's colour: greens from olive to deep, a few going yellow. Linear. |
| `src/render/groundCover.ts#treeModel` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A tree one unit tall, in the game's one style (`lowPolyTrees.ts`) |
| `src/render/groundCover.ts#treeTint` | portado | render/cover/cover_kit.cpp, render/trees/tree_place.cpp | A tree's tint: its own colour is in its vertices; this only varies it a little. |
| `src/render/groundShape.ts#BlockDigestAsk` | não se aplica | - | Blocks of two solves (or one) digested off the page: each solve's roads near its blocks, and the ground under  |
| `src/render/groundShape.ts#BlockRoads` | não se aplica | - | What `blockDigest` reads of a solve of the roads: a `RoadElevation`, or a cut's reader (`roadShaper`). |
| `src/render/groundShape.ts#GroundJob` | não se aplica | - | What the page asks the ground's worker. |
| `src/render/groundShape.ts#GroundJobAnswer` | não se aplica | - | What the worker answers each. |
| `src/render/groundShape.ts#GroundShapeAsk` | não se aplica | - | A shaping pass asked of the worker: the roads and the platforms its corners read, and its plates. |
| `src/render/groundShape.ts#NO_BOUNDS` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#NaturalPlate` | não se aplica | - | A plate's natural ground as `naturalRenderedHeightAt` reads it: its corners and the diagonals they are drawn w |
| `src/render/groundShape.ts#OffTheCut` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#PadsAsk` | não se aplica | - | The platforms of some buildings asked off the page (`renderer.ts` `shapeBlocksSteps`): each building's record  |
| `src/render/groundShape.ts#PlateShapeAnswer` | não se aplica | - | The corners a plate's pass moved, in the order the pass met them, and their new heights. |
| `src/render/groundShape.ts#PlateShapeAsk` | não se aplica | - | One plate's part of a shaping pass: its corners asked, as its mesh stands them. |
| `src/render/groundShape.ts#ShapeScratch` | não se aplica | - | A worker's scratch for its passes, sized to a plate once: where each corner stands and its land, and the corne |
| `src/render/groundShape.ts#ShapeScratch.heights` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#ShapeScratch.lands` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#ShapeScratch.pass` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#ShapeScratch.shaped` | não se aplica | - | The corners a pass shaped and their heights, in the order shaped (a corner once a pass: never more than the pl |
| `src/render/groundShape.ts#ShapeScratch.visited` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#ShapeScratch.xs` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#ShapeScratch.ys` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#askPlate` | não se aplica | - | A plate's part of a pass as the worker takes it (`terrain.ts` `askShape`) |
| `src/render/groundShape.ts#blockDigest` | não se aplica | - | What a block of ground reads of a solve, digested (`renderer.ts` `changedBlocks`) |
| `src/render/groundShape.ts#blockDigest.quantum` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#cornerBoxes` | não se aplica | - | The world boxes a pass's corners stand in, a cell round (`cell`, `half`: the plates' grid) |
| `src/render/groundShape.ts#digestBlocks` | não se aplica | - | Each solve's blocks digested as the page digests them (`blockDigest`) |
| `src/render/groundShape.ts#digestBlocks.naturalAt` | não se aplica | - | What a block of ground reads of a solve, digested (`renderer.ts` `changedBlocks`) |
| `src/render/groundShape.ts#padsOfAsk` | não se aplica | - | Each building's platform as the page works it out (`siteGrading.ts` `refresh`: `padOf` on its kept floor), in  |
| `src/render/groundShape.ts#padsOfAsk.pavedAt` | não se aplica | - | forma das placas: Heightfield + Terrain3D |
| `src/render/groundShape.ts#runGroundJob` | não se aplica | - | Runs a job as the worker does (`groundShape.worker.ts`; on the page when it fails), and the buffers its answer |
| `src/render/groundShape.ts#shapeGroundAsk` | não se aplica | - | A shaping pass as `terrain.ts` `shapeToRoads` runs it (`shapeCorners`), for each plate asked. |
| `src/render/groundShear.ts#Gradient` | não se aplica | - | FEET ON A SLOPE, WITHOUT IK. |
| `src/render/groundShear.ts#MAX_SHEAR_GRADIENT` | não se aplica | - | Steepest gradient a body is sheared to; beyond it the ground is a step, not a slope. |
| `src/render/groundShear.ts#groundGradient` | não se aplica | - | The ground's gradient at a point, from two forward differences off the height already known there. |
| `src/render/groundShear.ts#groundGradient.clampGradient` | não se aplica | - | cisalhamento de pessoas no declive: dono people |
| `src/render/groundShear.ts#shearMatrix` | não se aplica | - | Premultiplies `matrix` (three's axes: world y is -Z) by the shear about the body's centre (X0, Z0): Y += gx (X |
| `src/render/impostorShaders.ts#BAKE_FRAGMENT_OUT` | portado | project/render/shaders/vegetation_cards.gdshader | retrato das árvores no shader |
| `src/render/impostorShaders.ts#FRAMES` | portado | project/render/shaders/vegetation_cards.gdshader | Pictures along each side of a model's square. |
| `src/render/impostorShaders.ts#FRAME_PX` | portado | project/render/shaders/vegetation_cards.gdshader | Pixels of one picture on the atlas |
| `src/render/impostorShaders.ts#IMPOSTOR_FRAGMENT_MAP` | portado | project/render/shaders/vegetation_cards.gdshader | In place of three's \`map_fragment\`: the three pictures' colour, blended; the cover kept; the fade dissolved  |
| `src/render/impostorShaders.ts#IMPOSTOR_FRAGMENT_NORMAL` | portado | project/render/shaders/vegetation_cards.gdshader | After three's \`normal_fragment_begin\`: the pictures' normal, from the model's frame to the view's. |
| `src/render/impostorShaders.ts#IMPOSTOR_FRAGMENT_PARS` | portado | project/render/shaders/vegetation_cards.gdshader | retrato das árvores no shader |
| `src/render/impostorShaders.ts#IMPOSTOR_FRAMES_GLSL` | portado | project/render/shaders/vegetation_cards.gdshader | The pictures' directions and their planes, for a vertex shader: the far woods' and the near plants' (`plantImp |
| `src/render/impostorShaders.ts#IMPOSTOR_READ_GLSL` | portado | project/render/shaders/vegetation_cards.gdshader | A picture read off an atlas `blocks` models wide and high, for a fragment shader: the far woods' and the near  |
| `src/render/impostorShaders.ts#IMPOSTOR_VERTEX` | portado | project/render/shaders/vegetation_cards.gdshader | In place of three's \`begin_vertex\`: \`transformed\` is the corner in the world (the mesh stands at the origi |
| `src/render/impostorShaders.ts#IMPOSTOR_VERTEX_PARS` | portado | project/render/shaders/vegetation_cards.gdshader | retrato das árvores no shader |
| `src/render/impostorShaders.ts#bakeMaterials` | portado | project/render/shaders/vegetation_cards.gdshader | The materials the pictures are taken with, made at load: the body's faceted colour and face normal |
| `src/render/impostorShaders.ts#bakeShadowMaterials` | portado | project/render/shaders/vegetation_cards.gdshader | The materials a model's shadow outline is taken with: its body, and the cards of a plant whose cards throw its |
| `src/render/impostorShaders.ts#frameBasis` | portado | project/render/shaders/vegetation_cards.gdshader | The picture's own axes for a direction: right and up in it (the shader's `frameBasis`, the same numbers). |
| `src/render/impostorShaders.ts#hemiOctaDirection` | portado | project/render/shaders/vegetation_cards.gdshader | The direction of the picture at (u, v), each 0..1, on the hemi-octahedron (y up). |
| `src/render/instanceRuns.ts#RunBuilder.add` | já existia | render/rs_util.h | corridas de instâncias |
| `src/render/instanceRuns.ts#createInstanceRuns.setMap` | já existia | render/rs_util.h | corridas de instâncias |
| `src/render/instanceRuns.ts#leastPixel` | já existia | render/rs_util.h | corridas de instâncias |
| `src/render/landStroke.ts#unionBox` | não se aplica | - | traço no worker das placas; aqui o comando escreve o Heightfield |
| `src/render/landStroke.ts#unionRect` | não se aplica | - | traço no worker das placas; aqui o comando escreve o Heightfield |
| `src/render/landStroke.ts#writeWoodsBlock` | não se aplica | - | traço no worker das placas; aqui o comando escreve o Heightfield |
| `src/render/landStrokePool.ts#landStrokeOffPage` | não se aplica | - | pool do traço |
| `src/render/lightLevels.ts#BLOOM_THRESHOLD` | já existia | project/render/daylight.gd, data/game/daylight.json | HOW BRIGHT EACH KIND OF LIGHT IS AFTER DARK, in one place. |
| `src/render/lightLevels.ts#NIGHT_LUMINANCE` | já existia | project/render/daylight.gd, data/game/daylight.json | níveis de luz noturna |
| `src/render/lightLevels.ts#fromCandela` | já existia | project/render/daylight.gd, data/game/daylight.json | I in candela as luminance on the scale above. |
| `src/render/lightLevels.ts#lightPoolTexture` | já existia | project/render/daylight.gd, data/game/daylight.json | A soft round falloff, white in the middle, for a pool or a splash of lamplight laid additively on the ground o |
| `src/render/lightLevels.ts#luminanceOf` | já existia | project/render/daylight.gd, data/game/daylight.json | The luminance of an sRGB hex colour, in the working (linear) space. |
| `src/render/lightLevels.ts#nightGain` | já existia | project/render/daylight.gd, data/game/daylight.json | The factor a colour is multiplied by after dark to reach `target` luminance, over the 1 it is drawn with by da |
| `src/render/lightLevels.ts#scratch` | já existia | project/render/daylight.gd, data/game/daylight.json | níveis de luz noturna |
| `src/render/lightning.ts#Lightning` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | raios |
| `src/render/lightning.ts#MAX_STRIKES` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | Strikes drawn at once at most (`createLightning`): each lives under half a second. |
| `src/render/lightning.ts#STRIKE_LIFE` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | Seconds a strike lasts, from the first stroke to the last glow. |
| `src/render/lightning.ts#boltSegments` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | The bolt's channel: midpoint displacement between two points, then branches. Pairs of points, one pair a segme |
| `src/render/lightning.ts#boltSegments.pairs` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | raios |
| `src/render/lightning.ts#boltSegments.split` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | raios |
| `src/render/lightning.ts#createLightning` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | raios |
| `src/render/lightning.ts#createLightning.dispose` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | raios |
| `src/render/lightning.ts#createLightning.end` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | raios |
| `src/render/lightning.ts#createLightning.make` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | raios |
| `src/render/lightning.ts#createLightning.strike` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | A bolt from `from` (in the cloud) to `to` (the ground), three coordinates. |
| `src/render/lightning.ts#createLightning.update` | portado | project/render/weather_view.gd, shaders/lightning.gdshader | Ages the strikes; returns how bright their flash is now (0 none .. ~1). |
| `src/render/lowPolyTrees.ts#BARK` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#BLOOM_PINK` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#BLOOM_YELLOW` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | Triangles laid out flat - each its own three corners, so each face takes its own normal and colour |
| `src/render/lowPolyTrees.ts#Builder.add` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder.centre` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | How much light a face keeps, from where its centre lies (1 open, lower in shade). |
| `src/render/lowPolyTrees.ts#Builder.col` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder.constructor` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder.e` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder.finish` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | The body and `count` cards of `size` on the leaf points, both stood on the origin and scaled to one unit tall. |
| `src/render/lowPolyTrees.ts#Builder.frond` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A palm frond's card: a strip along `spine` |
| `src/render/lowPolyTrees.ts#Builder.frond.at` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder.leaves` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder.mass` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A crown mass: an 80-face sphere |
| `src/render/lowPolyTrees.ts#Builder.n` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder.pos` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Builder.strip` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | Frond strips (`frond`): each corner's position, normal, colour and atlas uv once, and the triangles over them. |
| `src/render/lowPolyTrees.ts#Builder.tri` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | One face: lighter facing the sky, a little shade of its own, times its occlusion. |
| `src/render/lowPolyTrees.ts#FAR_LEAVES` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | Leaves in the far clump, and the near clump's times as many. |
| `src/render/lowPolyTrees.ts#FLOWERS` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#FROND` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#HEART_SHRINK` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | How far the shaded heart under a crown's foliage cards is drawn, of its full size: hidden behind the cards, se |
| `src/render/lowPolyTrees.ts#HEDGE` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#LEAF` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#LEAF_FAR` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#LEAF_NEAR` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | World distance from the eye over which a card goes from the near clump to the far one, metres. |
| `src/render/lowPolyTrees.ts#LEAF_TALL` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#LeafPoint` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A point of a crown's surface the foliage cards stand on: where, which way is out, its leaf colour, its height  |
| `src/render/lowPolyTrees.ts#LowPolyKind` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#LowPolyParts` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A tree's two parts: its body (trunk, inner crown) and its foliage cards (a palm's: its frond strips). |
| `src/render/lowPolyTrees.ts#NEAR_LEAVES` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#NEEDLE` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#NUT` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#OPEN` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#Occlusion` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | How much light a face keeps, from where its centre lies (1 open, lower in shade). |
| `src/render/lowPolyTrees.ts#PALM_BARK` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#SHRUB` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#THIRD` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | The atlas' thirds: the far clump, the near clump, the frond. |
| `src/render/lowPolyTrees.ts#Tone` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#UP` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#along` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#atlasTexture` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | The foliage atlas, one for the whole game, grey |
| `src/render/lowPolyTrees.ts#billboardCards` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | Turns a material's foliage cards to face the camera - or the light, in a shadow pass |
| `src/render/lowPolyTrees.ts#broadleafTall` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#bush` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#crownShade` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A crown's baked occlusion: dark low in it and deep inside it, where the leaves above shade it. |
| `src/render/lowPolyTrees.ts#cypress` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#cypress.shade` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#foliageAtlas` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#foliageAtlas.clump` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#hedge` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#ipe` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | The flowering ipê: one wide, flat crown of cards, flower over the top and the last leaves' green underneath. |
| `src/render/lowPolyTrees.ts#ipe.pick` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A three primitive, moved by `m`, its faces each their own shade; `pick` may give a face another tone. |
| `src/render/lowPolyTrees.ts#keepLeafCoverage` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | Keeps the leaves as covered at a distance as up close |
| `src/render/lowPolyTrees.ts#leafCardMaterials` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | The foliage cards' material (leaf-cluster texture, alpha to coverage, billboards, the wind) and the depth mate |
| `src/render/lowPolyTrees.ts#limbs` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | Limbs from near the top of the trunk out into the crown, each `r0` thick at the trunk: seen between the cards. |
| `src/render/lowPolyTrees.ts#lowPolyTreeParts` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A tree or bush of this kind, grown from `seed` (the same seed, the same model): its body and its foliage cards |
| `src/render/lowPolyTrees.ts#nearLeaves` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | The near clump on cards close to the eye (`NEAR_LEAVES`) |
| `src/render/lowPolyTrees.ts#noise3` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A deterministic 3-D value noise in -1..1, of the ORIGINAL corner, so the copies of a corner move together. |
| `src/render/lowPolyTrees.ts#noise3.hash` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#noise3.lerp` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#noise3.plane` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#oak` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#palm` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#palm.at` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#palm.shade` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | árvores low-poly; coração da copa corrigido |
| `src/render/lowPolyTrees.ts#random` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A small seeded random (mulberry32): a model is the same every time it is made. |
| `src/render/lowPolyTrees.ts#tone` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | Two sRGB hex colours, converted once into the linear working space by `Color`. |
| `src/render/lowPolyTrees.ts#trunk` | já existia | render/trees/tree_kit.cpp, tree_parts.cpp | A six-sided trunk from the ground to `top`, tapering from `r0` to `r1`, open at its ends (in the ground and th |
| `src/render/mesh/detailLayer.ts#DETAIL_GLSL` | portado | project/render/shaders/terrain_layers.gdshaderinc | GLSL for sampling a detail layer. |
| `src/render/mesh/detailLayer.ts#DetailSpec` | portado | project/render/shaders/terrain_layers.gdshaderinc | camada de grão de perto e grão por pixel |
| `src/render/mesh/detailLayer.ts#DetailTextures` | portado | project/render/shaders/terrain_layers.gdshaderinc | camada de grão de perto e grão por pixel |
| `src/render/mesh/detailLayer.ts#applyDetail` | portado | project/render/shaders/terrain_layers.gdshaderinc | Installs the detail layer on a standard material whose UVs are in macro tiles |
| `src/render/mesh/detailLayer.ts#detailSwitch` | portado | project/render/shaders/terrain_layers.gdshaderinc | The shared on/off switch: off, the layer reads none of its textures (its weight 0, `detailW`). |
| `src/render/mesh/detailLayer.ts#detailTextures` | portado | project/render/shaders/terrain_layers.gdshaderinc | The detail textures for one family, baked at build time (`bake/detailRecipes.ts`) and loaded once (`surfaceBak |
| `src/render/mesh/textureBaker.ts#normalTexels` | não se aplica | - | assador em tempo de build do 4.0; aqui entram as texturas já cozidas |
| `src/render/natureNoise.ts#hash2` | já existia | render/trees/tree_place.cpp field() | ruído da densidade da mata |
| `src/render/natureNoise.ts#natureDensity` | já existia | render/trees/tree_place.cpp field() | The woods' density at a place (`natureSweep`'s Horizon Zero Dawn curve) |
| `src/render/natureNoise.ts#natureNoise` | já existia | render/trees/tree_place.cpp field() | The woods' masses: smooth value noise over the map's own coordinates, one octave at a scale, by `salt` |
| `src/render/natureTrees.ts#FOREST_WIND` | portado | render/trees/tree_place.cpp | The forest's wind (`groundCover.ts`): a slow sway and a leaf flutter. |
| `src/render/natureTrees.ts#Group` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#LOD_SLACK` | portado | render/trees/tree_place.cpp | How far the view moves before the trees are sorted again (one level now: kept for the API). |
| `src/render/natureTrees.ts#NatureForest` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#NatureTreeKit` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#SLOTS_OF_KIND` | portado | render/trees/tree_place.cpp | Each kind's models' places in `TREE_MODELS`, worked out once (VEG-06: a list made for every tree planted). |
| `src/render/natureTrees.ts#STAND_REACH` | portado | render/trees/tree_place.cpp | Neighbours within this reach (18 m) make a tree's stand. |
| `src/render/natureTrees.ts#TREE_MODELS` | portado | render/trees/tree_place.cpp | The models, in seed order: each takes its share of the seed's range (`world/trees.ts` KIND_SLOTS). |
| `src/render/natureTrees.ts#TREE_POSE` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#TREE_SCRATCH` | portado | render/trees/tree_place.cpp | `treeMatrixInto`'s pose in doubles (the forest's matrices as they were, not rounded to floats first). |
| `src/render/natureTrees.ts#TreeModelKind` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#Variant` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#buildNatureForest` | portado | render/trees/tree_place.cpp | The trees, instanced per variety: one mesh each, the whole model at every distance. |
| `src/render/natureTrees.ts#buildNatureForest.dispose` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#buildNatureForest.place` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#buildNatureForest.updateLod` | portado | render/trees/tree_place.cpp | Writes the trees into their meshes when the camera has moved (one level: the same model at every distance). |
| `src/render/natureTrees.ts#forestRoom` | portado | render/trees/tree_place.cpp | How many of these trees (or places a tree may stand, by seed) each variety of the kit takes: the room a forest |
| `src/render/natureTrees.ts#leaningMatrixInto` | portado | render/trees/tree_place.cpp | A tree's matrix from its pose's numbers, into `m` at `o`: turned by `yaw`, `wide` across, `tall`, its up axis  |
| `src/render/natureTrees.ts#loadNatureTrees` | portado | render/trees/tree_place.cpp | Makes the trees; their materials share the forest's wind. |
| `src/render/natureTrees.ts#loadNatureTrees.dispose` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#natureTreePictures` | portado | render/trees/tree_place.cpp | `grown`: the models already grown from the same seeds (`loadNatureTrees`), copied rather than grown again (VEG |
| `src/render/natureTrees.ts#natureTreePictures.release` | portado | render/trees/tree_place.cpp | `grown`: the models already grown from the same seeds (`loadNatureTrees`), copied rather than grown again (VEG |
| `src/render/natureTrees.ts#seedOfKind` | portado | render/trees/tree_place.cpp | A seed (0..1) that grows as this kind of tree, `r` (0..1) choosing among its models. |
| `src/render/natureTrees.ts#standKey` | portado | render/trees/tree_place.cpp | A cell of the stand grid as one number (a string key per tree and neighbour was most of the cost). |
| `src/render/natureTrees.ts#stands` | portado | render/trees/tree_place.cpp | How each tree stands among its neighbours: how crowded it is |
| `src/render/natureTrees.ts#standsAmong` | portado | render/trees/tree_place.cpp | árvores do campo e plantadas |
| `src/render/natureTrees.ts#treeColourInto` | portado | render/trees/tree_place.cpp | A tree's own green, into `col` at `o` (it multiplies the model's linear colours, three's color_vertex) |
| `src/render/natureTrees.ts#treeMatrixInto` | portado | render/trees/tree_place.cpp | A tree's matrix, into `m` at `o`: its pose (`treePoseInto`) written out |
| `src/render/natureTrees.ts#treePictures` | portado | render/trees/tree_place.cpp | The trees' pictures and who holds them. |
| `src/render/natureTrees.ts#treePoseInto` | portado | render/trees/tree_place.cpp | A tree's pose, into `out` at `at` (`TREE_POSE`): deep in a wood taller and narrower, at a wood's edge leaning  |
| `src/render/natureTrees.ts#treeSeed` | portado | render/trees/tree_place.cpp | A model's seed: the near trees and their pictures grow the same tree from it. |
| `src/render/natureTrees.ts#varietyOf` | portado | render/trees/tree_place.cpp | The variety a tree is grown as, from its seed. |
| `src/render/plantCells.ts#COVER_SHADOW_REACH` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#CoverLayout` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#PlantCells` | já existia | render/trees/tree_renderer.cpp | DO NOT write every tree of the map into its meshes again when one changed (`NatureForest.update`, `buildGround |
| `src/render/plantCells.ts#TREE_SHADOW_REACH` | já existia | render/trees/tree_renderer.cpp | How far off the view a tree (up to 19 m tall) and a stone or bush still throw a shadow into it, low sun. |
| `src/render/plantCells.ts#coverInto` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#coverInto.put` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#createCoverCells` | já existia | render/trees/tree_renderer.cpp | The painted covers' stones and bushes, the rivers' stones, and the woods' card trees where the low-poly ones a |
| `src/render/plantCells.ts#createCoverCells.shown` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#createTreeCells` | já existia | render/trees/tree_renderer.cpp | The countryside's low-poly trees (`natureTrees.ts`), cell by cell: a body and its leaf cards near, its picture |
| `src/render/plantCells.ts#createTreeCells.treesOnce` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#plantLayer` | já existia | render/trees/tree_renderer.cpp | `syncing`: told as a sync starts and ends, round the runs it makes. |
| `src/render/plantCells.ts#plantLayer.cull` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#plantLayer.dispose` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#plantLayer.setMap` | já existia | render/trees/tree_renderer.cpp | The map zoom: the leaf cards left out (`InstanceRuns.setMap`). |
| `src/render/plantCells.ts#plantLayer.sync` | já existia | render/trees/tree_renderer.cpp | The cells whose places changed (`CandidateKeep.takeChangedCells`) written again; null |
| `src/render/plantCells.ts#treesInto` | já existia | render/trees/tree_renderer.cpp | células das plantas |
| `src/render/plantCells.ts#treesRound` | já existia | render/trees/tree_renderer.cpp | The trees of a cell and the cells round it (a tree's shape reads its neighbours). |
| `src/render/plantImpostors.ts#COLOUR_MAP` | portado | render/trees/tree_pictures.cpp | In place of three's `map_fragment`: the pictures' colour, the cover kept as the mips thin it (`farForest.ts`). |
| `src/render/plantImpostors.ts#COLOUR_NORMAL` | portado | render/trees/tree_pictures.cpp | After three's `normal_fragment_begin`: the pictures' normal, from the model's frame to the view's. |
| `src/render/plantImpostors.ts#FRAGMENT_PARS` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#FRAGMENT_SLOPES` | portado | render/trees/tree_pictures.cpp | Each picture's uv change across the pixel, on the atlas, taken before anything is discarded. |
| `src/render/plantImpostors.ts#ImpostorModel` | portado | render/trees/tree_pictures.cpp | A model to take pictures of: its body, its foliage cards, and whether the cards throw its shadow (a palm's fro |
| `src/render/plantImpostors.ts#PICK_GLSL` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#PICTURE_PX` | portado | render/trees/tree_pictures.cpp | Device pixels across under which a plant is drawn as its picture (the frames are `FRAME_PX` wide: never magnif |
| `src/render/plantImpostors.ts#PICTURE_WIND` | portado | render/trees/tree_pictures.cpp | The trees' wind on their pictures (`scenery.ts` TREE_WIND, `natureTrees.ts` FOREST_WIND: the same breeze). |
| `src/render/plantImpostors.ts#Picture` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#PlantImpostors` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#PlantModels` | portado | render/trees/tree_pictures.cpp | The plants of the scenery's kit the pictures are made of (`scenery.ts` SceneryKit): only these, so this file d |
| `src/render/plantImpostors.ts#SHADOW_CUT` | portado | render/trees/tree_pictures.cpp | Before three's `alphatest_fragment` in the shadow pass: the outline that throws the shadow (the body's, `bakeS |
| `src/render/plantImpostors.ts#SceneryPictures` | portado | render/trees/tree_pictures.cpp | The gardens' and the street's plants' pictures: the scenery's models, its trees then its bushes. |
| `src/render/plantImpostors.ts#THIN_MARGIN` | portado | render/trees/tree_pictures.cpp | The margin of the share kept, over which a plant shrinks away rather than vanishing. |
| `src/render/plantImpostors.ts#THIN_PX` | portado | render/trees/tree_pictures.cpp | Device pixels across under which a picture is thinned: a share kept, each grown back to this size. |
| `src/render/plantImpostors.ts#VARYINGS` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#createPlantImpostors` | portado | render/trees/tree_pictures.cpp | The pictures of these models, taken at the next `takeWaitingPictures`; `key` names their programs; `ownModels` |
| `src/render/plantImpostors.ts#createPlantImpostors.attach` | portado | render/trees/tree_pictures.cpp | A mesh drawn with the pictures: the shadow map's size read before its shadow. |
| `src/render/plantImpostors.ts#createPlantImpostors.bake` | portado | render/trees/tree_pictures.cpp | The pictures taken, once, between draws (the far woods' own way, `farForest.ts` `takePictures`). |
| `src/render/plantImpostors.ts#createPlantImpostors.dispose` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#createPlantImpostors.ready` | portado | render/trees/tree_pictures.cpp | Whether the pictures are taken (`takeWaitingPictures`): until then the plants are drawn whole. |
| `src/render/plantImpostors.ts#createPlantImpostors.setView` | portado | render/trees/tree_pictures.cpp | The view the instances were culled for and half the screen's height, device pixels: what the pictures thin by. |
| `src/render/plantImpostors.ts#impostorPick` | portado | render/trees/tree_pictures.cpp | A plant's pick (0..1): the bits of its place hashed, as `impostorPick` hashes them on the GPU (the same float3 |
| `src/render/plantImpostors.ts#layoutOf` | portado | render/trees/tree_pictures.cpp | Blocks across and down for `n` models: the fewest texels, no side over 4 blocks (2,048 texels). |
| `src/render/plantImpostors.ts#pickBits` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#pickFloats` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#pictureOf` | portado | render/trees/tree_pictures.cpp | A model's sphere: round the box of its body and its cards reaching out from their centres. |
| `src/render/plantImpostors.ts#pictureOf.grow` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#readOf` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#sceneryPictures` | portado | render/trees/tree_pictures.cpp | One set of pictures for the gardens and the street (`gardens.ts`, `streetFurnitureLayer.ts`): one atlas, one p |
| `src/render/plantImpostors.ts#sceneryPictures.bushAt` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#sceneryPictures.release` | portado | render/trees/tree_pictures.cpp | Let go by a layer: the pictures go with the last one. |
| `src/render/plantImpostors.ts#sceneryPictures.treeAt` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#sceneryShared` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#takeWaitingPictures` | portado | render/trees/tree_pictures.cpp | Takes the pictures of every set made since, between draws (`farForest.ts` `update`, as it takes its own): neve |
| `src/render/plantImpostors.ts#thinWant` | portado | render/trees/tree_pictures.cpp | The share of plants this many pixels across that the pictures keep, with its margin: a plant whose pick is und |
| `src/render/plantImpostors.ts#vertexMain` | portado | render/trees/tree_pictures.cpp | retratos das árvores, rasterizados na CPU ao iniciar |
| `src/render/plantImpostors.ts#vertexPars` | portado | render/trees/tree_pictures.cpp | The vertex shader's declarations; `shadow`: the shadow pass's. |
| `src/render/plantImpostors.ts#waiting` | portado | render/trees/tree_pictures.cpp | Picture sets not taken yet: taken at the next `takeWaitingPictures`. |
| `src/render/plateTexels.ts#CornerCover` | não se aplica | - | texels por placa |
| `src/render/plateTexels.ts#CornerStore` | não se aplica | - | texels por placa |
| `src/render/plateTexels.ts#byte` | não se aplica | - | texels por placa |
| `src/render/plateTexels.ts#ecologyCornersInto` | não se aplica | - | texels por placa |
| `src/render/plateTexels.ts#groundCornersInto` | não se aplica | - | texels por placa |
| `src/render/plateTexels.ts#landTexelsInto` | não se aplica | - | texels por placa |
| `src/render/plateTexels.ts#lightLayerInto` | não se aplica | - | texels por placa |
| `src/render/propGeometry.ts#Placement` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#benchGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#benchGeometry.slat` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#binGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#boxWithout` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#bushParts` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#drainGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#faceNoise` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#grassTuftGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#hydrantGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#lampFarGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#lampGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#lampLensGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#merge` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#part` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#phoneGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#postboxGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#treeParts` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#treePitGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/propGeometry.ts#wildflowerGeometry` | já existia | render/props/prop_kit.cpp | geometria dos props |
| `src/render/rain.ts#MAX_DROPS` | portado | project/render/weather_view.gd, shaders/rain.gdshader | Streaks at a downpour; a lighter rain draws the first part of them. |
| `src/render/rain.ts#Rain` | portado | project/render/weather_view.gd, shaders/rain.gdshader | chuva |
| `src/render/rain.ts#RainField` | portado | project/render/weather_view.gd, shaders/rain.gdshader | Where the rain is this frame: what the vertex shader places the drops by (`dropAt`). |
| `src/render/rain.ts#STREAK` | portado | project/render/weather_view.gd, shaders/rain.gdshader | The streak's length in seconds of fall. |
| `src/render/rain.ts#advanceRain` | portado | project/render/weather_view.gd, shaders/rain.gdshader | The rain one frame on: the box over `ground` as wide as the view |
| `src/render/rain.ts#createRain` | portado | project/render/weather_view.gd, shaders/rain.gdshader | chuva |
| `src/render/rain.ts#createRain.dispose` | portado | project/render/weather_view.gd, shaders/rain.gdshader | chuva |
| `src/render/rain.ts#createRain.next` | portado | project/render/weather_view.gd, shaders/rain.gdshader | chuva |
| `src/render/rain.ts#createRain.update` | portado | project/render/weather_view.gd, shaders/rain.gdshader | The rain this frame: how hard (0..1), the ground point under the view and the view's width there |
| `src/render/rain.ts#createRainField` | portado | project/render/weather_view.gd, shaders/rain.gdshader | chuva |
| `src/render/rain.ts#dropAt` | portado | project/render/weather_view.gd, shaders/rain.gdshader | A drop's place - `seed` its hash in [0, 1) on each axis, `tip` 0 at its head, 1 at its tail |
| `src/render/rain.ts#wrap` | portado | project/render/weather_view.gd, shaders/rain.gdshader | chuva |
| `src/render/scenery.ts#BEAM_FADE` | já existia | render/props, render/trees | VLB's Fade Out: the cone dims from `BEAM_FADE[0]` from the eye and is gone at `BEAM_FADE[1]`. |
| `src/render/scenery.ts#BEAM_INTENSITY` | já existia | render/props, render/trees | Brightness of the air lit at the lens, at full dark |
| `src/render/scenery.ts#BEAM_SEGMENTS` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/scenery.ts#BEAM_SIDES` | já existia | render/props, render/trees | Sides and segments of the cone (VLB: 18 sides; 3 segments at least, for a smooth falloff). |
| `src/render/scenery.ts#BEAM_SIDE_SOFTNESS` | já existia | render/props, render/trees | How hard the cone's edges are seen from the side (VLB's Side Thickness): higher, softer. |
| `src/render/scenery.ts#BEAM_SOURCE` | já existia | render/props, render/trees | Radius of the cut apex, over the radius at the ground: the lens (about 0.28 m of 3.7). |
| `src/render/scenery.ts#BUSH_WIND` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/scenery.ts#GRASS_WIND` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/scenery.ts#LAMP_BEAM_RADIUS` | já existia | render/props, render/trees | The cone's radius at the ground: the bright core of the pool. |
| `src/render/scenery.ts#LAMP_POOL_RADIUS` | já existia | render/props, render/trees | Radius of the bright core under a street lamp (the cone's foot). |
| `src/render/scenery.ts#PLANT_MAP_ZOOM` | já existia | render/props, render/trees | Below this zoom the plants' leaf cards are left out (`Scenery.setMap`). |
| `src/render/scenery.ts#PLANT_NEAR_ZOOM` | já existia | render/props, render/trees | Zoom at and above which plants are drawn with their close-up models. |
| `src/render/scenery.ts#POOL_ALONG` | já existia | render/props, render/trees | The light a street lamp lays on the road: not a spotlight's disc but the long patch of a road luminaire's spre |
| `src/render/scenery.ts#SHADOW_REACH` | já existia | render/props, render/trees | How far outside the view an instance is still drawn, world units |
| `src/render/scenery.ts#STREET_LED` | já existia | render/props, render/trees | The street lights' colour: white LED, some 4500 K. |
| `src/render/scenery.ts#Scenery` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/scenery.ts#SceneryKit` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/scenery.ts#TREE_WIND` | já existia | render/props, render/trees | How each kind of plant answers the wind; see `wind.ts`. |
| `src/render/scenery.ts#beamMaterial` | já existia | render/props, render/trees | The cone's material. |
| `src/render/scenery.ts#createSceneryKit` | já existia | render/props, render/trees | Builds every model and material once, at renderer start. |
| `src/render/scenery.ts#createSceneryKit.dispose` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/scenery.ts#createSceneryKit.lightBeams` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/scenery.ts#createSceneryKit.poolMaterial` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/scenery.ts#createSceneryKit.setHaze` | já existia | render/props, render/trees | How thick the air is, 0 clear to 1 rain or fog: a lamp's cone of light is seen only in air that scatters it. |
| `src/render/scenery.ts#createSceneryKit.setNight` | já existia | render/props, render/trees | Lamps lit as night falls: 0 by day, 1 at night. |
| `src/render/scenery.ts#foliageTint` | já existia | render/props, render/trees | A near-white multiplier, so no two plants of one species are identical. |
| `src/render/scenery.ts#plantSamples` | já existia | render/props, render/trees | One of every plant as the gardens and the street draw them, with and without a tint of their own |
| `src/render/scenery.ts#plantSamples.dispose` | já existia | render/props, render/trees | kit de cenário: postes são de street-furniture |
| `src/render/stagedTexels.ts#stageTexels.hold` | não se aplica | - | envio escalonado de texels |
| `src/render/stagedTexels.ts#stageTexels.letGoWhenSent` | não se aplica | - | envio escalonado de texels |
| `src/render/stillTree.ts#holdStill` | não se aplica | - | DO NOT move an object inside a still tree |
| `src/render/stillTree.ts#holdStill.onAdded` | não se aplica | - | congela a árvore de cena three.js |
| `src/render/stillTree.ts#holdStill.settle` | não se aplica | - | congela a árvore de cena three.js |
| `src/render/stillTree.ts#movedInStill` | não se aplica | - | One object's matrix composed and its world matrices (its own and its children's) made at once, from its parent |
| `src/render/stillTree.ts#stillRoots` | não se aplica | - | Still trees under the scene: the frame walks them never (`updateMoving`), their world matrices made where some |
| `src/render/stillTree.ts#updateMoving` | não se aplica | - | The scene's world matrices for a frame (the scene's `matrixWorldAutoUpdate` is off): every child that is not a |
| `src/render/sunCascades.ts#CASCADE_LIGHTS` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#CASCADE_PARS` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#CascadeShadow` | não se aplica | - | One cascade's shadow; three's `WebGLShadowMap` calls `updateMatrices` right before it draws that light's map,  |
| `src/render/sunCascades.ts#CascadeShadow.beforeDraw` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#CascadeShadow.constructor` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#CascadeShadow.isDirectionalLightShadow` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#CascadeShadow.updateMatrices` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#Fit` | não se aplica | - | Where a cascade stands in the light's axes: centre (right, up), half-side, and the depth range along the sun i |
| `src/render/sunCascades.ts#GROUND_DIP` | não se aplica | - | The ground below the lowest ground sampled that is still taken as ground (a dip between the samples). |
| `src/render/sunCascades.ts#KEEP_CASCADES` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#MOST_STRIPS` | não se aplica | - | The most strips a cascade clears in one pass for its changed casters (`rectsOf`): each is a scissored clear, t |
| `src/render/sunCascades.ts#Movers` | não se aplica | - | What moves and is drawn into the shadows: when it moves, every cascade it is drawn into is drawn again. |
| `src/render/sunCascades.ts#NDC` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#NEAR_DEFER_REACH` | não se aplica | - | How far a change's shadow may reach past its box, world units: a change farther than this from the view is pat |
| `src/render/sunCascades.ts#ROOM` | não se aplica | - | A cascade's half-side is one of the steps 2^(k/4) m; it is fitted to what it must hold with this much room (`f |
| `src/render/sunCascades.ts#SAFETY_FRAMES` | não se aplica | - | A cascade drawn whole after this many frames however still, a last resort for a change nothing told it of (ten |
| `src/render/sunCascades.ts#SAFETY_STAGGER` | não se aplica | - | DO NOT let the cascades' safety draws fall in one frame: three whole passes at once every half second (RND-03) |
| `src/render/sunCascades.ts#SCROLL_MOST` | não se aplica | - | A cascade whose square only moved by whole texels keeps the store's depth where the squares overlap and draws  |
| `src/render/sunCascades.ts#SHADOW_BIAS_WORLD` | não se aplica | - | Depth offset of the shadow test, world units (4 cm). |
| `src/render/sunCascades.ts#SHADOW_DEPTH_MAX` | não se aplica | - | The most eye depth the cascades cover, the whole map seen from above at most. |
| `src/render/sunCascades.ts#SHADOW_REACH` | não se aplica | - | How far the cascades reach past the point looked at, eye depth: the street sees shadows this far (800 m). |
| `src/render/sunCascades.ts#SHRINK` | não se aplica | - | Fitted again smaller once the cascade is this many times wider than what it must hold. |
| `src/render/sunCascades.ts#SUN_CASCADES` | não se aplica | - | The sun's cascades: three lights of one direction and colour, each with its own map; the shader lights a pixel |
| `src/render/sunCascades.ts#SUN_STEP` | não se aplica | - | How far the sun the shadows are cast by may drift from the true one before it is moved (rad), at the least: 0. |
| `src/render/sunCascades.ts#ShadowPassInput` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#Strip` | não se aplica | - | A part of a cascade's square drawn alone (`drawStrips`): its bounds in the light's square about the centre (th |
| `src/render/sunCascades.ts#SunCascades` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#SunHold` | não se aplica | - | The sun held in steps of `SUN_STEP`, and the light's axes (right, up) turned with it by the least turn |
| `src/render/sunCascades.ts#SunHold.held` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#SunHold.r` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#SunHold.s` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#SunHold.take` | não se aplica | - | Takes the true sun; true when the held one moved: when the true sun left `tolerance` (rad) of it. |
| `src/render/sunCascades.ts#SunHold.u` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#UNION_MOST` | não se aplica | - | The union of a cascade's strips past this share of its map: drawn whole instead (the same raster, and no store |
| `src/render/sunCascades.ts#cascadeMap` | não se aplica | - | A cascade's map as three's PCF shadow map makes it (r186 `WebGLShadowMap`: a depth texture compared, linear),  |
| `src/render/sunCascades.ts#cascadedSunChunks` | não se aplica | - | The sun's cascades in three's shaders: the directional lights that cast shadows are the sun's cascades, all of |
| `src/render/sunCascades.ts#cascadedSunVertexChunk` | não se aplica | - | The vertex's part of the cascades (after `environment.ts` slopeScaledShadowOffset): its world position and nor |
| `src/render/sunCascades.ts#createSunCascades` | não se aplica | - | `renderer`: with it, each cascade keeps the depth of its still casters (STATIC SHADOW CACHE): what moves is dr |
| `src/render/sunCascades.ts#createSunCascades.blitShift` | não se aplica | - | Cascade `i`'s square moved by (`dx`, `dy`) whole texels of its grid (`fit`, the store standing for its town):  |
| `src/render/sunCascades.ts#createSunCascades.castersIn` | não se aplica | - | The meshes under a mover `root` drawn into a cascade of `texel`: each part that casts, holds instances, and is |
| `src/render/sunCascades.ts#createSunCascades.coverBox` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.dispose` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.drawOver` | não se aplica | - | What moves drawn into cascade `i` over what its map holds, by three's own shadow pass (depth materials and all |
| `src/render/sunCascades.ts#createSunCascades.drawStrips` | não se aplica | - | Cascade `i`'s `strips` cleared to the far depth and their still casters drawn alone - the shadow camera narrow |
| `src/render/sunCascades.ts#createSunCascades.drawnIn` | não se aplica | - | A mover is drawn into cascade `i` only if it is wider than two of its texels: under that, its shadow is a smud |
| `src/render/sunCascades.ts#createSunCascades.dropDeferred` | não se aplica | - | What was put off let go: the near cascade drawn whole, or its store gone. |
| `src/render/sunCascades.ts#createSunCascades.ensureMap` | não se aplica | - | Cascade `i`'s map as three made it, or ours: one byte of colour, its depth compared. |
| `src/render/sunCascades.ts#createSunCascades.fit` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.follow` | não se aplica | - | Fits the cascades to what the camera sees of the ground and what stands on it |
| `src/render/sunCascades.ts#createSunCascades.instancesUnder` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.maps` | não se aplica | - | Every cascade's map, for the video memory count. |
| `src/render/sunCascades.ts#createSunCascades.measureSphere` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.nearBoxes` | não se aplica | - | Of `changed` and what was put off before, those whose shadow may reach `view` (patched now); the others kept f |
| `src/render/sunCascades.ts#createSunCascades.nearBoxes.route` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.nearTexel` | não se aplica | - | The near cascade's texel, world units (0 before it is placed): what a caster under its cover must span to cast |
| `src/render/sunCascades.ts#createSunCascades.pass` | não se aplica | - | Marks the cascades that must be drawn this frame; true when any is (the renderer's `shadowMap.needsUpdate`). |
| `src/render/sunCascades.ts#createSunCascades.place` | não se aplica | - | Writes a fit into its light: the light stands at the top of the depth range, looking down the sun. |
| `src/render/sunCascades.ts#createSunCascades.rectOf` | não se aplica | - | Where a world box falls in cascade `i`'s square, a texel of margin round it, on whole texels: the strip to dra |
| `src/render/sunCascades.ts#createSunCascades.rectsOf` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.remakeMaps` | não se aplica | - | The maps made again, for the renderer's shadow map type changed (`shadowMap.type`: PCF compares its depth, Bas |
| `src/render/sunCascades.ts#createSunCascades.restore` | não se aplica | - | Puts back what a cascade's pass hid (`Scene.onAfterRender`). |
| `src/render/sunCascades.ts#createSunCascades.setCasters` | não se aplica | - | What chooses its casters by size, told each cascade's texel and half side (m) before that cascade is drawn (a  |
| `src/render/sunCascades.ts#createSunCascades.setEnabled` | não se aplica | - | `mapSize`: one size for every cascade, or a size for each: the near cascade carries the shadow detail, the far |
| `src/render/sunCascades.ts#createSunCascades.setFarEvery` | não se aplica | - | The far cascades (1 and on) drawn at most every `n` passes, what changed meanwhile kept for their turn (the la |
| `src/render/sunCascades.ts#createSunCascades.setMovers` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.sphereOf` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.stripsOfShift` | não se aplica | - | The strips a shift of (`dx`, `dy`) texels uncovers in cascade `i`: in the light's square (the camera's bounds, |
| `src/render/sunCascades.ts#createSunCascades.sunTolerance` | não se aplica | - | How far the true sun may leave cascade `i`'s held one, rad: until the shadow of the tallest caster in the view |
| `src/render/sunCascades.ts#createSunCascades.syncLight` | não se aplica | - | Copies the sun's colour and intensity to the other cascades. |
| `src/render/sunCascades.ts#createSunCascades.takeStats` | não se aplica | - | How the passes went since the last call (counts, then zeroed): each cascade's whole draws and draws over its s |
| `src/render/sunCascades.ts#createSunCascades.texelOf` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#createSunCascades.unionStrip` | não se aplica | - | cascatas de sombra próprias: DirectionalLight3D SHADOW_PARALLEL_4_SPLITS do Godot |
| `src/render/sunCascades.ts#staticStore` | não se aplica | - | Where a cascade's still casters are kept between its whole draws: a depth as its map's, copied by a blit (thre |
| `src/render/terrain.ts#DabRect` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | A dab's square, the plate's own (x, y): min x, min y, max x, max y. |
| `src/render/terrain.ts#GRASS_FIELD` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Where the grass field stands this frame (`grassField.ts`): world x, y, how far it reaches, and 1 when it is dr |
| `src/render/terrain.ts#GRID` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Corners per side of the terrain grid — one more than its cells. |
| `src/render/terrain.ts#GROUND_AFTER` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The ground's place in three's opaque order (`renderOrder`): after the layers standing on it, whose order is 0  |
| `src/render/terrain.ts#LAND_DIRT` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#LAND_GRID` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | A plate's grid as its patches read it (`terrainLod.ts`). |
| `src/render/terrain.ts#LAND_ROCK` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The layers of the land's arrays (`bake/catalogue.ts` `terrain-land`): the detail's grass and soil, the rocks'  |
| `src/render/terrain.ts#LIGHT_LAYER` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The array layer holding the corners' ecology (RGB) and the land's slope (A, `terrainLightCompute.ts`), corner  |
| `src/render/terrain.ts#MAX_FLOOD_CELLS` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Most cells one body of water may spread over into a basin: about 400 m square. |
| `src/render/terrain.ts#PAINT_LAYERS` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The painted ground's layers, as ONE texture array |
| `src/render/terrain.ts#PAINT_RES` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Texels across a plate in each painted-ground weight texture. |
| `src/render/terrain.ts#PAINT_START_RES` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The side of the painted ground's array before anything is painted on it: the land's light layer |
| `src/render/terrain.ts#PlateSurface` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | One plate's own ground: its own revisions and water, never merged over the plates shown (a product follows the |
| `src/render/terrain.ts#RIBBON_ACROSS` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Strips across a ribbon. |
| `src/render/terrain.ts#RIBBON_HALF_WIDTH` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#RIBBON_STEP` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Most units between two cross-sections of a ribbon. |
| `src/render/terrain.ts#RIVER_MIN_DEPTH` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The least water over a river's bed, units (half a metre). |
| `src/render/terrain.ts#RiverPath` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#RiverPoint` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | One point of a river's course: where, how wide its water each side, at what level. |
| `src/render/terrain.ts#SEASON_DRY` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | How far into the dry season the land is, 0 (the rains: everything green) to 1 |
| `src/render/terrain.ts#SHORE_BAND` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | How many water cells (24 units, about 10 m) of an over-large flood stay, so a river meets its own banks. |
| `src/render/terrain.ts#SLAB_DEPTH` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | How far the map's cut sides reach below the base level (SimCity 4's slab). |
| `src/render/terrain.ts#STRATA_SPAN_X` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Metres along the rim one strata texture spans before it repeats. |
| `src/render/terrain.ts#STRATA_SPAN_Y` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Metres of depth one strata texture spans before it repeats. |
| `src/render/terrain.ts#ShapeAsking` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | `shapeToRoads` asked off the page (`groundShape.ts`): the corners its pass reads for `region` |
| `src/render/terrain.ts#ShoreTextures` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The corners' textures, `side` corners a side (TER-07): the water's level alone, a float, sent when the water c |
| `src/render/terrain.ts#TERRAIN_BASE` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#TERRAIN_GRID` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The universal grid drawn by the ground itself (`SceneHandle.setGrid`): the cell (world units), the strength |
| `src/render/terrain.ts#TOPSOIL` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The darker topsoil band under the rim, in metres. |
| `src/render/terrain.ts#TerrainBakes` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The land's surfaces, baked at build time (`bake/terrainRecipes.ts`): the grass, the dirt, and the rocks as ONE |
| `src/render/terrain.ts#TerrainData` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | What a plate's land materials share: their uniforms, kept in `userData` too for the plate to change. |
| `src/render/terrain.ts#TerrainLodShader` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The ground of a plate drawn by distance (`terrainLod.ts`): its grid and the uniforms its patches read. |
| `src/render/terrain.ts#TerrainPart` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The plates the ground is made of, each with its own surface (`TerrainPart`). |
| `src/render/terrain.ts#TerrainSource` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | What the surface reads of the document: one plate's share of it, in the plate's own coordinates (`flatAtlas.ts |
| `src/render/terrain.ts#TerrainSurface` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#WATER_CELL` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Grid resolution of the unified river surface. |
| `src/render/terrain.ts#WATER_FILL` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Share of a channel's depth the water fills, measured down from its banks. |
| `src/render/terrain.ts#WATER_KEY_OFFSET` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#WATER_KEY_STRIDE` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#WATER_MARGIN` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | How far under the ground the surface stays, so its rim is never left dry. |
| `src/render/terrain.ts#WATER_RADIUS` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Radius of a river's water disc, as a fraction of the stamp that carved it. |
| `src/render/terrain.ts#WATER_SPREAD` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | How far past the brush the surface may reach to find its shore. |
| `src/render/terrain.ts#WaterKey` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#WaterMade` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#WaterPoint` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#WaterStamp` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#WaterVertex` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | One plate of the ground (`flatAtlas.ts`); `heights`, the shared arrays it keeps its corners in, read by the wo |
| `src/render/terrain.ts#createTerrainSurface.applyEcology` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.applyLight` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.askShape` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | `shapeToRoads` asked off the page (`groundShape.ts`): the corners its pass reads for `region` |
| `src/render/terrain.ts#createTerrainSurface.askShape.take` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.askShore` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.chooseDiagonals` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The cells round a box of corners (inclusive) and the diagonal `corners` gives each. |
| `src/render/terrain.ts#createTerrainSurface.clamp` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.clampCorner` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.cornerX` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Where a corner stands, the plate's own (x, y): as its mesh held it (float32), so the heights read there are th |
| `src/render/terrain.ts#createTerrainSurface.cornerY` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.cornersMoved` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The drawn corners of a box (inclusive) moved: their heights, the diagonals and normals round them and the leve |
| `src/render/terrain.ts#createTerrainSurface.coverAt` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | How much of a cover (forest, scrub, flowers, rocks) was painted at a point, 0..1. |
| `src/render/terrain.ts#createTerrainSurface.coverCorner` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The covers painted at a corner, as the shore array carries them. |
| `src/render/terrain.ts#createTerrainSurface.ecologyAll` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.ecologyOver` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.ecologyUploaded` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The ecology changed: its bytes into the corner layer, and the layer sent again. |
| `src/render/terrain.ts#createTerrainSurface.fillPaint` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The array's `layers` (null: all) from their truth: the dabs and the corner layer. |
| `src/render/terrain.ts#createTerrainSurface.fitPaint` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.flushPaint` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The box of the paint layers laid since the last frame sent alone, layer by layer, from the held copy: DO NOT s |
| `src/render/terrain.ts#createTerrainSurface.landMovedWater` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The water brought up to land moved in `dirty` (corners; 'all' or null: anywhere, or a river dab): rebuilt only |
| `src/render/terrain.ts#createTerrainSurface.layPaintInto` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | `res`: the side of the array the texels are of (the one being sent), never the side the plate has now: a small |
| `src/render/terrain.ts#createTerrainSurface.levelsLanded` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.levelsMeasuredWhole` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Nothing asked of the workers lands any more: the levels are measured whole on the page. |
| `src/render/terrain.ts#createTerrainSurface.markLand` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.markWaterDirty` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.measureMoved` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.measureOnPage` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.naturalHeightAt` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.naturalRenderedHeightAt` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The ground as DRAWN, before any road cut or filled it. |
| `src/render/terrain.ts#createTerrainSurface.paintRaster` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The paint layers a stroke's dabs are laid on: held while the brush paints this plate, let go at its end (`sett |
| `src/render/terrain.ts#createTerrainSurface.paintStep` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.paintStep.lay` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.patchMesh` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The plate's patches (`lod.geometry`) and its rim's (`lod.rimGeometry`), placed each frame by `lookFrom`. |
| `src/render/terrain.ts#createTerrainSurface.readOnPage` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.rebuildWater` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.rebuildWater.bankLevel` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.rebuildWater.landAt` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.rediagonal` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.refreshEcology` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.regionList` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.renderedHeightAt` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The height the terrain is actually DRAWN at. |
| `src/render/terrain.ts#createTerrainSurface.rewrite` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.sampleGrid` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Bilinear read of one corner array, reproducing the plane's own diagonal. |
| `src/render/terrain.ts#createTerrainSurface.sendAllLand` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.sendCorners` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The covers or the painted geology changed over `rect` (the plate's own x, y; null: anywhere): those rows of co |
| `src/render/terrain.ts#createTerrainSurface.sendGround` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.sendLand` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | A box of corners (inclusive) sent again, row by row (`Texture.addUpdateRange`): never the whole plate for a da |
| `src/render/terrain.ts#createTerrainSurface.sendLevels` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The water's levels changed: sent whole. |
| `src/render/terrain.ts#createTerrainSurface.setSun` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Where the sun is (a direction towards it, three's axes) |
| `src/render/terrain.ts#createTerrainSurface.shapeLanded` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | After a shaping pass (here or off the page): a road that cut through a valley changes where the water's shore  |
| `src/render/terrain.ts#createTerrainSurface.shapePass` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.shapePass.restore` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.shapeToRoads` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | `region`: one region or several, shaped in one pass |
| `src/render/terrain.ts#createTerrainSurface.shoreNext` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.stagePaint` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.take` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.touchesWater` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.updatePaint` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Brings the painted ground up to `doc.paintRevision`. |
| `src/render/terrain.ts#createTerrainSurface.waterLanded` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.waterReaches` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Whether land moved in `box` (corners) can change the water: it reaches the water's mesh, or a river's banks |
| `src/render/terrain.ts#createTerrainSurface.waterReaches.near` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The plates the quality tier holds in full (`quality.ts`): drawn round the view at most, and made but not drawn |
| `src/render/terrain.ts#createTerrainSurface.wetAt` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Whether a point is under a river's water. |
| `src/render/terrain.ts#createTerrainSurface.wetCorners` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#createTerrainSurface.wholePlateMoved` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | @rebuild The whole plate's diagonals and levels worked out again, its texels at their send: every corner moved |
| `src/render/terrain.ts#createTerrainSurface.woodsMadeWhole` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Nothing asked of the workers lands any more: the woods are made whole. |
| `src/render/terrain.ts#createTerrainSurface.woodsMoved` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The woods' cells a dab's natural land reaches, asked of a worker one job at a time: the old woods stand until  |
| `src/render/terrain.ts#createTerrainSurface.woodsOnPage` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#ecologyBytesInto` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | A plate's ecology into its corner layer's R, G and B (dry grass, wet ground, bare ground: what the shader read |
| `src/render/terrain.ts#ecologyScratch` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The ecology's classes as floats over 255 (`ecologyCornersInto`), one array for the page. |
| `src/render/terrain.ts#farLandMaterial` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The land's material for the far patches (`flatAtlas.ts`), made without a plate: a far plate holds no paint, no |
| `src/render/terrain.ts#farLandMaterial.dispose` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#floodBasins` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#grassMean` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#landShader` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#meanLuma` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#paintLayer` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | One layer of the painted ground's texels (`res` a side), as a view on them. |
| `src/render/terrain.ts#paintLayers` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The painted ground's array, its texels on the page only while sent (`stageTexels`). |
| `src/render/terrain.ts#paintTexelBox` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The paint texels a dab lays (`rasterPaint`, the same bounds). |
| `src/render/terrain.ts#plateGrid` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | A plate's grid of corners as the shaping and the sampling read it (`terrainShaping.ts`). |
| `src/render/terrain.ts#pushTriangle` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#pushWetTriangle` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#rasterBiome` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Lays one biome dab on the terrain's corners: within its radius the painted biome weights move towards the dab' |
| `src/render/terrain.ts#rasterPaint` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Lays one dab into the weight textures (`res` texels a side): within its radius every layer moves toward the da |
| `src/render/terrain.ts#riverRibbon` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#riverRibbon.at` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#riverRibbon.push` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#shapePasses` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#shapeVisited` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Corners visited by the current shaping pass, by pass, so regions and boxes that overlap visit each once: one f |
| `src/render/terrain.ts#shoreLayers` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#shoreLayers.nearest` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#slabGeometry` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | THE WORLD'S SLAB: the cut sides round a whole rectangle of land (the far ring's, `flatAtlas.ts`) |
| `src/render/terrain.ts#slabGeometry.vertex` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#stampAppended` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#strataTexture` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The soil layers of the map's cut sides |
| `src/render/terrain.ts#swap` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Replaces `chunk` in a shader, or throws: a three upgrade that renamed it must not leave the land drawn wrong i |
| `src/render/terrain.ts#terrainBakes` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The land's surfaces, baked at build time (`bake/terrainRecipes.ts`): the grass, the dirt, and the rocks as ONE |
| `src/render/terrain.ts#terrainData` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | A plate's land textures and uniforms, made once for all its materials: the painted ground's array and the corn |
| `src/render/terrain.ts#terrainMaterial` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The land's material: on an ordinary mesh of the ground (`lod` null: the far patches, the verges), or on a plat |
| `src/render/terrain.ts#unifiedWaterGeometry` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#unifiedWaterGeometry.levelAt` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#unionDab` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#unionTexels` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#wallMaterial` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | The cut sides' material: the strata texture, and over it what a painted cross-section shows at its top |
| `src/render/terrain.ts#waterCell` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | chão pintado, geologia e grama; a malha é do Terrain3D; os estratos da rocha (graniteSheets e outros) e a grama seca da ecologia não portados |
| `src/render/terrain.ts#waterKey` | portado em parte | project/render/shaders/terrain.gdshader, render/terrain_bridge.cpp | Exact numeric cell IDs on the playable map; a string preserves arbitrary out-of-map coordinates. |
| `src/render/terrainGrid.ts#TERRAIN_CELL` | já existia | sim/terrain/heightfield.h | World size of one terrain quad. |
| `src/render/terrainGrid.ts#TERRAIN_HALF` | já existia | sim/terrain/heightfield.h | Half the terrain plate's extent. |
| `src/render/terrainGrid.ts#TERRAIN_SEGMENTS` | já existia | sim/terrain/heightfield.h | Cells per side: 320 cells of 6.4 m (`world/bounds.ts` PLATE_SIZE), fine enough for a brush. |
| `src/render/terrainGrid.ts#TERRAIN_SIZE` | já existia | sim/terrain/heightfield.h | Side of one terrain plate, in world units: the map is laid in square plates |
| `src/render/terrainLevels.ts#LOD_LEVELS` | não se aplica | - | Levels: 0 reads every corner of the plate, each next level every other corner of the one before. |
| `src/render/terrainLevels.ts#LOD_NODE` | não se aplica | - | Cells along a node of level 0. |
| `src/render/terrainLevels.ts#LOD_PATCH` | não se aplica | - | Cells along a patch, the one grid a plate instances; a quadtree node is 2 x 2 patches, so its grid is even and |
| `src/render/terrainLevels.ts#LandLevels` | não se aplica | - | Each node's lowest and highest corner and how far its level stands off the plate's full ground, per level (`[l |
| `src/render/terrainLevels.ts#LandLevels.constructor` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#LandLevels.data` | não se aplica | - | Level 0's nodes first, then each next level's: `[lo, hi, err]` a node. |
| `src/render/terrainLevels.ts#LandLevels.dirty` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#LandLevels.first` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#LandLevels.hi` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#LandLevels.lo` | não se aplica | - | The node's lowest corner, highest corner and error. |
| `src/render/terrainLevels.ts#LandLevels.markAll` | não se aplica | - | Every node marked. |
| `src/render/terrainLevels.ts#LandLevels.markBox` | não se aplica | - | The nodes holding a box of corners or the cells round it (their diagonals follow those corners) marked to be m |
| `src/render/terrainLevels.ts#LandLevels.markCorner` | não se aplica | - | The nodes holding corner `i` marked to be measured again. |
| `src/render/terrainLevels.ts#LandLevels.measure` | não se aplica | - | The marked nodes measured again from the drawn corners and diagonals, and each level's worst error with them. |
| `src/render/terrainLevels.ts#LandLevels.n` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#LandLevels.seg` | não se aplica | - | Nodes along a side at each level. |
| `src/render/terrainLevels.ts#LandLevels.sumUp` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#LandLevels.take` | não se aplica | - | The nodes as measured elsewhere (the plate's worker, `plateMesh.ts`), every mark cleared. |
| `src/render/terrainLevels.ts#LandLevels.worst` | não se aplica | - | The plate's worst error at each level (metres), as `measure` left it. |
| `src/render/terrainLevels.ts#coarseAt` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#coarseRange` | não se aplica | - | The height range of a cell of `step` corners at a point of it (corner units), over both its diagonals: the sha |
| `src/render/terrainLevels.ts#fineAt` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#nodesAt` | não se aplica | - | Nodes along a side at each level. |
| `src/render/terrainLevels.ts#parting` | não se aplica | - | níveis por placa: Terrain3D |
| `src/render/terrainLevels.ts#parting.take` | não se aplica | - | The nodes as measured elsewhere (the plate's worker, `plateMesh.ts`), every mark cleared. |
| `src/render/terrainLight.worker.ts#serve` | não se aplica | - | This service of the job system (`job.worker.ts`): its messages answered on the scope it is given. |
| `src/render/terrainLightCompute.ts#CornerRect` | não se aplica | - | Corners, inclusive, columns x0..x1 and rows y0..y1 (rows run from +y downwards). |
| `src/render/terrainLightCompute.ts#LightGrid` | não se aplica | - | The corner grid: `n` corners a side, `cell` world units apart, centred on the origin. |
| `src/render/terrainLightCompute.ts#LightRequest` | não se aplica | - | A request: the corners whose slope to work out, and the heights of the box round them they read (rows of the b |
| `src/render/terrainLightCompute.ts#LightResult` | não se aplica | - | Its answer: the slope of the corners in `rect`, a byte a corner (degrees over 90), rows of the rect's width. |
| `src/render/terrainLightCompute.ts#SLOPE_REACH` | não se aplica | - | How far a corner's slope reads the heights round it, corners: the steepest cell round each corner (one), blend |
| `src/render/terrainLightCompute.ts#grownOn` | não se aplica | - | `r` grown by `by` corners, kept on the grid. |
| `src/render/terrainLightCompute.ts#landSlope` | não se aplica | - | Each corner's slope: half the steepest cell round it, half the mean of that over the corners round it, degrees |
| `src/render/terrainLightCompute.ts#landSlope.h` | não se aplica | - | luz assada no chão: sol do Godot |
| `src/render/terrainLightCompute.ts#slopeRequest` | não se aplica | - | The request for the corners whose heights moved in `moved` ('all': every corner): the corners whose slope they |
| `src/render/terrainLightCompute.ts#unionCorners` | não se aplica | - | luz assada no chão: sol do Godot |
| `src/render/terrainLightPool.ts#Job` | não se aplica | - | pool da luz |
| `src/render/terrainLightPool.ts#Lane` | não se aplica | - | pool da luz |
| `src/render/terrainLightPool.ts#POOL_SIZE` | não se aplica | - | Workers for every plate's slope: a sliver of the machine, never all of it. |
| `src/render/terrainLightPool.ts#broken` | não se aplica | - | pool da luz |
| `src/render/terrainLightPool.ts#lanes` | não se aplica | - | pool da luz |
| `src/render/terrainLightPool.ts#next` | não se aplica | - | pool da luz |
| `src/render/terrainLightPool.ts#start` | não se aplica | - | pool da luz |
| `src/render/terrainLightPool.ts#submitLight` | não se aplica | - | Sends a plate's slope request to its worker (queued behind the others); false when there are no workers |
| `src/render/terrainLod.ts#LEVEL_BITS,SIDE_SHIFT` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LOD_MARGIN` | não se aplica | - | How far short of its reach a level's morph ends, and how far past the last reach the next one starts: the GPU' |
| `src/render/terrainLod.ts#LOD_PIXELS` | não se aplica | - | The screen error a level may make, pixels (Strugar's LOD ranges; Ulrich's chunked LOD screen-space error). |
| `src/render/terrainLod.ts#LOD_SKIRT` | não se aplica | - | How far a plate's rim hangs down where it meets another plate, drawn at its own level (the far patches' skirt, |
| `src/render/terrainLod.ts#LandGrid` | não se aplica | - | A plate's grid as the levels read it: cells a side, their size and the plate's half side (`terrain.ts`). |
| `src/render/terrainLod.ts#LandLod` | não se aplica | - | A plate's ground as patches of one grid, chosen each frame from the camera (Strugar's `LODSelect`): one draw f |
| `src/render/terrainLod.ts#LandLod.add` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.add.write` | não se aplica | - | Written only where it differs from what the buffer holds; the first such index kept (`select`). |
| `src/render/terrainLod.ts#LandLod.boxOf` | não se aplica | - | A node's box in the world (three's axes) into `box`, and how far the eye stands from it. |
| `src/render/terrainLod.ts#LandLod.constructor` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.count` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.dispose` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.geometry` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.morph` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.patches` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.patchesFrom` | não se aplica | - | The first patch and rim this selection wrote anew (Infinity: none). |
| `src/render/terrainLod.ts#LandLod.ranges` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.reach` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.rimCount` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.rimGeometry` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.rims` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.rimsFrom` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#LandLod.select` | não se aplica | - | The patches the camera needs, nearest levels where its error would show; returns how many. |
| `src/render/terrainLod.ts#LandLod.skirt` | não se aplica | - | How far the rim hangs on the west, east, north and south (0: a side that hangs none). |
| `src/render/terrainLod.ts#LandLod.visit` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#box` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#eye` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#frustum` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#lodFragmentGlsl` | não se aplica | - | The fragment shader's part: the full ground under the drawn point, as the finest level would have drawn it (he |
| `src/render/terrainLod.ts#lodFragmentGlsl.f` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#lodVertexGlsl` | não se aplica | - | The vertex shader's part: the patch placed, its corners read from the land's texels and morphed by their dista |
| `src/render/terrainLod.ts#lodVertexGlsl.f` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#patchCells` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#patchRim` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainLod.ts#viewProjection` | não se aplica | - | LOD do chão: clipmap do Terrain3D |
| `src/render/terrainShaping.ts#CornerGrid` | não se aplica | - | A plate's grid of corners: `n` a side, corner `i` at `ix + iy * n`, and its world point. |
| `src/render/terrainShaping.ts#ShapeAt` | não se aplica | - | forma do chão por vias: Heightfield modifiers |
| `src/render/terrainShaping.ts#TerrainRegion` | não se aplica | - | A box of grid corners, inclusive: x0..x1 across, y0..y1 down. |
| `src/render/terrainShaping.ts#TerrainShaper` | não se aplica | - | What `shapeToRoads` needs to know about the road network. |
| `src/render/terrainShaping.ts#boxRegion` | não se aplica | - | forma do chão por vias: Heightfield modifiers |
| `src/render/terrainShaping.ts#sampleCorners` | não se aplica | - | The height a plate's ground is drawn at, at a point of the plate (its own coordinates, inside it) |
| `src/render/terrainShaping.ts#shapeBoth` | não se aplica | - | The roads' and the buildings' platforms' shaping as one (`renderer.ts` `shapeGround`, and the ground's worker) |
| `src/render/terrainShaping.ts#shapeCorners` | não se aplica | - | DO NOT walk `shapeBounds` against the regions: every box of the town |
| `src/render/terrainShaping.ts#shapeCorners.visit` | não se aplica | - | forma do chão por vias: Heightfield modifiers |
| `src/render/torusWindow.ts#TorusWindow.bandOf` | portado | render/atmosphere/fog_field.cpp | janela que dá a volta |
| `src/render/water.ts#DEEP_AT` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Depth, in world units, at which the tint has reached its deep-water value. |
| `src/render/water.ts#DRIFT_A` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Scroll speed of each layer, in TILES per second. |
| `src/render/water.ts#DRIFT_B` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#FLOW_CYCLE` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Seconds of one phase of the flow map (Vlachos, "Water Flow in Portal 2"). |
| `src/render/water.ts#FOAM_AT` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Depth over which the shore foam band fades out. |
| `src/render/water.ts#FOAM_ROUGHNESS` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#FRESNEL_BASE` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Reflectance straight down at the surface, before the angular term. |
| `src/render/water.ts#FRESNEL_POWER` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | The Schlick exponent, lowered from the physical 5. |
| `src/render/water.ts#LAYER_A_TILE` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | World units one tile of each layer covers. |
| `src/render/water.ts#LAYER_B_TILE` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#RIM_AT` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Depth over which the sheet fades in from nothing. |
| `src/render/water.ts#TEXTURE_SIZE_HIGH` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Side of the baked normal map at the top tier. Power of two, as GL wants. |
| `src/render/water.ts#TEXTURE_SIZE_LOW` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#WATER_CLOCK_WRAP` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Seconds after which the animation clock restarts. |
| `src/render/water.ts#WATER_DEPTH_ATTRIBUTE` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | The per-vertex water depth the builder writes and the shader reads. |
| `src/render/water.ts#WATER_DRIFTS` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Both layers' drifts, exposed so the wrap invariant can be tested. |
| `src/render/water.ts#WATER_FLOW_ATTRIBUTE` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | The per-vertex way the water runs (three's x and z, unit down a river, nought in a lake). |
| `src/render/water.ts#WATER_ROUGHNESS` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Low enough for the sun to catch, high enough not to alias into fireflies. |
| `src/render/water.ts#WAVE_TILE` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | World units across one tile of the wave layer. |
| `src/render/water.ts#WaterLook` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | How the water looks and moves (`world/weather.ts`) |
| `src/render/water.ts#WaterSurface` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#createWaterSurface` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#createWaterSurface.attach` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Makes the mesh drive its own animation. |
| `src/render/water.ts#createWaterSurface.dispose` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#createWaterSurface.setGround` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | The ground under the water as a texture of the terrain's grid heights |
| `src/render/water.ts#createWaterSurface.setLook` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | The waves, foam and current (`WaterLook`). |
| `src/render/water.ts#normalsMade` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | The ripple texture, baked once per size and filtering and shared by every water surface (one per plate) |
| `src/render/water.ts#sharedWaterNormals` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#waterClock` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Where the animation clock stands, given milliseconds of uptime. |
| `src/render/water.ts#waterNormalTexels` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | The surface's height field, in one sweep, as a tiling normal map plus a coarse noise for the foam to break its |
| `src/render/water.ts#waterNormalTexels.at` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#waterNormalTexture` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | ondas, espuma, correnteza, profundidade |
| `src/render/water.ts#waterTextureSize` | portado | project/render/shaders/water.gdshader, render/water/water_renderer.cpp | Texture side for a tier, derived from the anisotropy the tier already sets. |
| `src/render/wind.ts#NEAR_FADE` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | Foliage the camera has come into fades out, a pixel at a time |
| `src/render/wind.ts#NEAR_FADE_GLSL` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#SWAYS` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | The mark of a material whose vertices the wind moves: what it draws sways every frame, and the sun's cascades  |
| `src/render/wind.ts#Swaying` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#WIND_GLSL` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#WIND_PACE` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#WIRE_WIND_UNIFORMS_GLSL` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | The uniforms a wire in the wind reads (`wireWindGlsl`), declared once in the vertex shader. |
| `src/render/wind.ts#WindResponse` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#advanceWind` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | Advances the wind clock. Called once a frame; costs one uniform write. |
| `src/render/wind.ts#applyPartSway` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#applyStructureSway` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | A stiff structure in the wind - a signal post, its arm and heads |
| `src/render/wind.ts#applyWind` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | Makes a material's instances bend in the wind (and fade where the camera is among them); `_name` says whose, f |
| `src/render/wind.ts#install` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#installStructureSway` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#markSway` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | A material the wind moves (its shader reads `windUniforms`): every shader that sways marks its material, lit a |
| `src/render/wind.ts#projectWithWind` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | vento nas plantas e na água |
| `src/render/wind.ts#setWindWeather` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | The map's wind (`world/weather.ts`): which way it blows, towards (three's x and z), and how fast, metres a sec |
| `src/render/wind.ts#sways` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | The mark of a material whose vertices the wind moves: what it draws sways every frame, and the sun's cascades  |
| `src/render/wind.ts#windDepthMaterial` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | The shadow-pass twin of a wind material, for `mesh.customDepthMaterial`: one program for every response (`appl |
| `src/render/wind.ts#windUniforms` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | Shared by every material with wind; `renderer.ts` advances the time. |
| `src/render/wind.ts#wireWindGlsl` | portado | project/render/shaders/vegetation.gdshaderinc, globais wind_* | Wires in the wind: a span swings sideways downwind and bobs a little, most at mid-span and not at all at the i |
| `src/render/woodsBase.ts#WOODS_SLOPE_STEP` | já existia | render/trees/tree_place.cpp field() | The step the land's slope is read over, either side of a cell's centre. |
| `src/render/woodsBase.ts#WOODS_SPACING` | já existia | render/trees/tree_place.cpp field() | The woods' lattice: one candidate tree a cell this wide (`renderer.ts` natureSweep). |
| `src/render/woodsBase.ts#cornerLand` | já existia | render/trees/tree_place.cpp field() | A plate's natural land read from its corners as the terrain reads them |
| `src/render/woodsBase.ts#onPlate` | já existia | render/trees/tree_place.cpp field() | Whether a point of the plate's own (x, y) lies on it, as the terrain asks before reading its corners (`terrain |
| `src/render/woodsBase.ts#woodsBase` | já existia | render/trees/tree_place.cpp field() | THE WOODS' DENSITY OF A PLATE BUT ITS ECOSYSTEM, off the page (`flatGround.worker.ts`) |
| `src/render/woodsBase.ts#woodsBaseInto` | já existia | render/trees/tree_place.cpp field() | base da densidade da mata |
| `src/render/woodsBase.ts#woodsOutsideReads` | já existia | render/trees/tree_place.cpp field() | The points off the plate `woodsBaseInto` reads over `rect`, x then y, in the order it reads them: their land r |
| `src/render/woodsBase.ts#woodsOutsideReads.read` | já existia | render/trees/tree_place.cpp field() | base da densidade da mata |
| `src/render/woodsBase.ts#woodsSpan` | já existia | render/trees/tree_place.cpp field() | base da densidade da mata |
| `src/world/buildings/cameraSolids.ts#SolidFiling.forEachIn` | já existia | sim/buildings | sólidos da câmera e pisos dos prédios |
| `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny` | já existia | sim/buildings | sólidos da câmera e pisos dos prédios |
| `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells` | já existia | sim/buildings | sólidos da câmera e pisos dos prédios |
| `src/world/buildings/pads.ts#padShaper` | já existia | sim/buildings | sólidos da câmera e pisos dos prédios |
| `src/world/clouds.ts#CLOUD_LIMITS` | portado | sim/nature/sky.cpp | nuvens colocadas |
| `src/world/clouds.ts#CLOUD_SPAN` | portado | sim/nature/sky.cpp | The sky's period: the placed clouds repeat every this much each way, so over any part of the unbounded world t |
| `src/world/clouds.ts#MAX_PLACED_CLOUDS` | portado | sim/nature/sky.cpp | The most clouds a map keeps the sky draws , all of them the map's own: since 2026-10-07 the sky invents none t |
| `src/world/clouds.ts#PlacedCloud` | portado | sim/nature/sky.cpp | nuvens colocadas |
| `src/world/clouds.ts#clamp` | portado | sim/nature/sky.cpp | nuvens colocadas |
| `src/world/clouds.ts#cloudUnder` | portado | sim/nature/sky.cpp | The cloud the pointer is on: of the clouds whose body |
| `src/world/clouds.ts#driftedCloud` | portado | sim/nature/sky.cpp | Where a cloud is now, carried by the wind (`world/weather.ts`): its repeat nearest `about` (where the view loo |
| `src/world/clouds.ts#driftedCloud.wrap` | portado | sim/nature/sky.cpp | nuvens colocadas |
| `src/world/clouds.ts#finite` | portado | sim/nature/sky.cpp | nuvens colocadas |
| `src/world/clouds.ts#readCloud` | portado | sim/nature/sky.cpp | A placed cloud read from a saved map, kept in range, or null if it is not one. |
| `src/world/clouds.ts#scatterClouds` | portado | sim/nature/sky.cpp | Clouds spread over the map, as a procedural spawner fills a volume and the placed instances are then edited on |
| `src/world/clouds.ts#scatterClouds.vary` | portado | sim/nature/sky.cpp | nuvens colocadas |
| `src/world/clouds.ts#undriftedCloud` | portado | sim/nature/sky.cpp | Where to keep a cloud that is to be seen at `seen` now: the wind's drift taken off (`driftedCloud`'s inverse). |
| `src/world/doc.ts#RoadDoc.addCloud` | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp | estado da paisagem, desfazer e save |
| `src/world/doc.ts#RoadDoc.addElements` | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp | estado da paisagem, desfazer e save |
| `src/world/doc.ts#RoadDoc.addFogDab` | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp | estado da paisagem, desfazer e save |
| `src/world/doc.ts#RoadDoc.addGullyDab` | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp | estado da paisagem, desfazer e save |
| `src/world/doc.ts#RoadDoc.cutTrees` | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp | estado da paisagem, desfazer e save |
| `src/world/doc.ts#RoadDoc.removeElements` | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp | estado da paisagem, desfazer e save |
| `src/world/doc.ts#RoadDoc.updateCloud` | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp | estado da paisagem, desfazer e save |
| `src/world/ecology.ts#Community` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | One plant community: the densities of its strata. |
| `src/world/ecology.ts#DEFAULT_REGION` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#EcologyField` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | What grows at each corner, every value 0..1. |
| `src/world/ecology.ts#EcologyInput` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#EcologyScratch` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | Arrays a caller that reads the answer and lets it go hands every ecology it asks for (`farWoods.ts`): the answ |
| `src/world/ecology.ts#EcologyScratch.arrays` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#EcologyScratch.count` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | Mixes the first `count` communities of `mix` into `out` (the weights need not sum to one). |
| `src/world/ecology.ts#EcologyScratch.region` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#EcologyScratch.take` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | Ten fields, the water's distance and the mean's two passes, `count` corners each; the regions. |
| `src/world/ecology.ts#EcologyUpdate` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | A field brought up to date over a region (Etapa 5g) |
| `src/world/ecology.ts#FORMATIONS` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | The fitofisionomias, as densities (IBGE; the field descriptions of each biome). |
| `src/world/ecology.ts#MEAN_RADIUS` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | How far a corner's answer reaches: the local mean's radius, and a corner either side for the slope. |
| `src/world/ecology.ts#Mix` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | The communities a corner mixes and their weights, as many as `blend` is told: made once a `computeEcology` (DO |
| `src/world/ecology.ts#Mix.parts` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#Mix.weigh` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#Mix.weights` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#NatureSettings` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | A map's ecosystem: its biome and the seed its patches are laid with. Absent on maps made before it. |
| `src/world/ecology.ts#REACH` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#REGIONS` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#RegionId` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | The biome a map belongs to (`RoadDoc.nature`); a biome painted overrides it locally. |
| `src/world/ecology.ts#Site` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | Per corner, what the land is like there. |
| `src/world/ecology.ts#blend` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | Mixes the first `count` communities of `mix` into `out` (the weights need not sum to one). |
| `src/world/ecology.ts#clamp01` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#community` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#communityKeys` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | A community's fields, listed once |
| `src/world/ecology.ts#computeEcology` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | Reads the land and answers what grows at every corner; `scratch`: the answer written into its arrays (DO NOT m |
| `src/world/ecology.ts#isNatureSettings` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#isRegionId` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#localMean` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | The mean of the heights round each corner over a square of `radius` corners: a separable box blur. |
| `src/world/ecology.ts#localMeanIn` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | `localMean` over the corners x0..x1, y0..y1 only (the rest 0) |
| `src/world/ecology.ts#regionCommunity` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | One region's communities over one site. |
| `src/world/ecology.ts#smooth` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | o que cresce onde, por bioma |
| `src/world/ecology.ts#waterDistance` | portado | sim/nature/ecology.cpp, data/catalog/nature.json | What a later update reuses: the distance to water per corner and the lowest ground (`computeEcology`). |
| `src/world/elements.ts#EffectKind` | portado | sim/nature/elements.cpp, sim/commands/element_commands.cpp, project/data/ui/land_elements.json | elementos do pincel: tipos, pincel de cada tipo, espalhar e apagar |
| `src/world/elements.ts#isEffectKind` | portado | sim/nature/elements.cpp, sim/commands/element_commands.cpp, project/data/ui/land_elements.json | elementos do pincel: tipos, pincel de cada tipo, espalhar e apagar |
| `src/world/elements.ts#scatter` | portado | sim/nature/elements.cpp, sim/commands/element_commands.cpp, project/data/ui/land_elements.json | elementos do pincel: tipos, pincel de cada tipo, espalhar e apagar |
| `src/world/elevation.ts#BATTER` | outro dono | road-elevation | Run per unit of rise on a cut or fill batter — a 1:2.5 slope. |
| `src/world/elevation.ts#BESIDE_DROP` | outro dono | road-elevation | How far under the carriageway the ground beside a road is laid |
| `src/world/elevation.ts#CURVE_PEAK` | outro dono | road-elevation | Peak-to-mean slope ratio of the smoothstep used for vertical curves. |
| `src/world/elevation.ts#CUT_SHOULDER` | outro dono | road-elevation | The same batter for a tunnel's approach cutting, and much narrower. |
| `src/world/elevation.ts#CUT_WALL_REACH` | outro dono | road-elevation | How far past a road's casing a cutting held by retaining walls is dug (`RoadSegment.cutWalls`) |
| `src/world/elevation.ts#DECK_GRADE` | outro dono | road-elevation | Steepest grade a deck may take to clear a rise under it. |
| `src/world/elevation.ts#DECK_TIE` | outro dono | road-elevation | Steepest gradient with which a deck is carried up to a higher deck it meets in the air. |
| `src/world/elevation.ts#DILATE` | outro dono | road-elevation | Stations either side of one that must also be cleared (chord protection). |
| `src/world/elevation.ts#FLAT_GROUND` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#GROUND_GRADE` | outro dono | road-elevation | Steepest gradient of a road at grade (rise / run), from `roads/tuning.ts`. |
| `src/world/elevation.ts#GROUND_ONLY` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#LATERAL` | outro dono | road-elevation | Lateral samples across the casing when reading the ground under a road. |
| `src/world/elevation.ts#LIFT_OFF` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#LIFT_ON` | outro dono | road-elevation | Lift above the ground at which a structure stops shaping it: piers, not fill. |
| `src/world/elevation.ts#MAX_AUTHORED_GRADE` | outro dono | road-elevation | Maximum authored height change per unit of alignment before a ramp needs more run. |
| `src/world/elevation.ts#MAX_SOLVE_PASSES` | outro dono | road-elevation | Most rounds of the profile solve. |
| `src/world/elevation.ts#MAX_STATIONS` | outro dono | road-elevation | Hard cap on stations per segment, so a 4 km road does not allocate 1000. |
| `src/world/elevation.ts#NO_PROFILES` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#NO_SHAPE` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#NodeHeights` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ORDER_CELL` | outro dono | road-elevation | The side of the squares the roads left are taken in (`pendingOrder`). |
| `src/world/elevation.ts#PLATE_BLEND` | outro dono | road-elevation | How far past a junction plate the ground floor stops constraining a ramp. |
| `src/world/elevation.ts#PLATE_MARGIN` | outro dono | road-elevation | Extra reach past the junction trim that the junction plate has to cover. |
| `src/world/elevation.ts#PLATE_TANGENT_REACH` | outro dono | road-elevation | Decay length of the grade correction just outside a flat junction plate. |
| `src/world/elevation.ts#PLATE_TANGENT_SHARE` | outro dono | road-elevation | Cancels enough of the incoming grade to round the join without bending the whole hill. |
| `src/world/elevation.ts#Profile` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.add` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.cells` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.constructor` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.derive` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.digest` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.empty` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.meeting` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.near` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.oversized` | outro dono | road-elevation | Each road's whole solved profile as a digest, with its box (for `differences`). |
| `src/world/elevation.ts#ProfileIndex.remove` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ProfileIndex.seal` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#RAMP_CREST` | outro dono | road-elevation | Softness of the crest where a ramp meets its deck (a height: 3.2 m of run at the ramp's grade). |
| `src/world/elevation.ts#RAMP_CURVE` | outro dono | road-elevation | Length of the vertical curve at the foot of a ramp, where the grade builds up from level, in world units (8 m) |
| `src/world/elevation.ts#RAMP_GRADE` | outro dono | road-elevation | Grade of the straight part of a ramp between a raised deck and a road at grade: sixteen per cent. |
| `src/world/elevation.ts#RAMP_RELAX` | outro dono | road-elevation | The share of a ramp over which the natural ground stops being a floor. |
| `src/world/elevation.ts#RoadElevation` | outro dono | road-elevation | The solved field: its reads (`roadField.ts`) and what the ground and the cache ask of it. |
| `src/world/elevation.ts#RoadShapeCut` | outro dono | road-elevation | The roads `shapeAt` and the reads can meet at a point of `rects` |
| `src/world/elevation.ts#SHAPE_DROP` | outro dono | road-elevation | How far below the road SURFACE the ground beside it is pulled. |
| `src/world/elevation.ts#SHAPE_INNER` | outro dono | road-elevation | Extra width past the casing over which the ground is held at road level. |
| `src/world/elevation.ts#SHAPE_SHOULDER` | outro dono | road-elevation | Width of the embankment or cutting that carries the ground back to its natural height. |
| `src/world/elevation.ts#SHAPE_SHOULDER_MAX` | outro dono | road-elevation | Cap on the batter, so one deep cut cannot reshape a quarter of the map. |
| `src/world/elevation.ts#SMOOTH_REACH` | outro dono | road-elevation | How far along a road its grade line is averaged, in world units. |
| `src/world/elevation.ts#STATION` | outro dono | road-elevation | Station spacing along a profile, in world units. |
| `src/world/elevation.ts#SURFACE_READ_REACH` | outro dono | road-elevation | How far from a point ON a road any profile can still change what a query there answers |
| `src/world/elevation.ts#SettledNode` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ShapingRoad` | outro dono | road-elevation | What the ground's shaping reads of a road (`roadShapeAt`): a solve's own profile, or its copy off the page. |
| `src/world/elevation.ts#SolveMemo` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#SolveScope` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#StationHint` | outro dono | road-elevation | A road's ground read off the page (`roadStations`, the job system's `roadStations`), for the solve that reads  |
| `src/world/elevation.ts#Summary` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#TIE_REACH` | outro dono | road-elevation | How far past its junction plate a profile is tied back to the node height. |
| `src/world/elevation.ts#VERTICAL_CURVE_REACH` | outro dono | road-elevation | Reach of the vertical curves rounded into a road wherever its grade changes (see `roundGradeBreaks`), in world |
| `src/world/elevation.ts#buildRoadElevation` | outro dono | road-elevation | Solves the height of every road surface in the network: from nothing, or from a solve before it, where the net |
| `src/world/elevation.ts#buildRoadElevation.capOf` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.count` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.gradeOf` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.incident` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.keyOf` | outro dono | road-elevation | A node's inputs as a digest (no text per node of the map): where it stands, its authored height, its roads. |
| `src/world/elevation.ts#buildRoadElevation.localDigest` | outro dono | road-elevation | What a profile can give any point of a rectangle |
| `src/world/elevation.ts#buildRoadElevation.mine` | outro dono | road-elevation | A profile made this solve's own before it is written: from its grade line (`fresh`), or with the heights it ha |
| `src/world/elevation.ts#buildRoadElevation.profileOf` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.quantum` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.roadsAt` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.settledAt` | outro dono | road-elevation | A node's figures: solved here, or as it settled in the solve reused (its inputs did not change). |
| `src/world/elevation.ts#buildRoadElevation.solve` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.take` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#buildRoadElevation.wantedOf` | outro dono | road-elevation | Free height a sunken span wants, before the ramps are fitted. |
| `src/world/elevation.ts#descentEnvelope` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#dilate` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#ease` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#elevationWork` | outro dono | road-elevation | The work of the solves, counted (`tests/scale`): profiles made and nodes whose inputs were read. |
| `src/world/elevation.ts#everyRoad` | outro dono | road-elevation | @rebuild Every road of the network, by id: what a scoped solve from nothing leaves but those it takes (once, a |
| `src/world/elevation.ts#flatSolves` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#flatSolvesOf` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#flattenPlates` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#gradeEnvelope` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#lookDigest` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#memos` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#noGround` | outro dono | road-elevation | The roads of a cut (`RoadElevation.shapeCut`) read as the solve reads its own: `shapeAt` |
| `src/world/elevation.ts#pendingOrder` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#pinPlates` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#rampLength` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#rampRise` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#readStations` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#roadShapeAt` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#roadShaper` | outro dono | road-elevation | The roads of a cut (`RoadElevation.shapeCut`) read as the solve reads its own: `shapeAt` |
| `src/world/elevation.ts#roadShaper.shapeAt` | outro dono | road-elevation | How far the ground should be pulled towards the road at a point, and to what height. |
| `src/world/elevation.ts#roadStations` | outro dono | road-elevation | The ground under a road at its stations, read as the solve reads it (`readStations`): what a worker reads off  |
| `src/world/elevation.ts#roadsToRead` | outro dono | road-elevation | The roads the next solve of `net` from `previous` reads the ground under (new, moved or widened, or on ground  |
| `src/world/elevation.ts#roundGradeBreaks` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#roundPlateJoins` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#roundPlateJoins.steep` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#sameLine` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#sameProfile` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#sameXY` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#scopeIds` | outro dono | road-elevation | The roads a scope names, those of the network only. |
| `src/world/elevation.ts#shapeHit` | outro dono | road-elevation | Scratch of `roadShapeAt`'s nearest-point reads. |
| `src/world/elevation.ts#slopeLimit` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#smoothMin` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#smoothProfile` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#solveGround` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#solveGround.baseAt` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#solveGround.tieFor` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#solveRaised` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#solveSunken` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#solveVariable` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#stationIndex` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#stationsAt` | outro dono | road-elevation | greide das vias |
| `src/world/elevation.ts#withProfile` | outro dono | road-elevation | The profiles in id order with `profile` put in its place (replacing the one of its id): a new list, the old on |
| `src/world/fogPaint.ts#DEFAULT_FOG` | portado | sim/nature/nature_state.cpp | neblina pintada |
| `src/world/fogPaint.ts#DEFAULT_FOG_HEIGHT` | portado | sim/nature/nature_state.cpp | A fog dab's height and speed when a saved one carries none. |
| `src/world/fogPaint.ts#DEFAULT_FOG_SPEED` | portado | sim/nature/nature_state.cpp | neblina pintada |
| `src/world/fogPaint.ts#FOG_LIMITS` | portado | sim/nature/nature_state.cpp | neblina pintada |
| `src/world/fogPaint.ts#FogDab` | portado | sim/nature/nature_state.cpp | neblina pintada |
| `src/world/fogPaint.ts#FogSample` | portado | sim/nature/nature_state.cpp | neblina pintada |
| `src/world/fogPaint.ts#FogSettings` | portado | sim/nature/nature_state.cpp | The map's own fog setting, over every bank painted. |
| `src/world/fogPaint.ts#MAX_FOG_DABS` | portado | sim/nature/nature_state.cpp | A map keeps at most this many fog dabs; the oldest go first. |
| `src/world/fogPaint.ts#clamp` | portado | sim/nature/nature_state.cpp | neblina pintada |
| `src/world/fogPaint.ts#fogAt` | portado | sim/nature/nature_state.cpp | The fog the dabs leave at a point: each dab in turn, a smooth falloff from its centre to its rim, adding towar |
| `src/world/fogPaint.ts#num` | portado | sim/nature/nature_state.cpp | neblina pintada |
| `src/world/fogPaint.ts#readFogDab` | portado | sim/nature/nature_state.cpp | A fog dab read from a saved map, or null if it is not one. |
| `src/world/fogPaint.ts#readFogSettings` | portado | sim/nature/nature_state.cpp | The settings read from a saved map, kept in range; absent ones take the defaults. |
| `src/world/groundPatch.ts#GroundPatch` | já existia | sim/terrain/height_patch.h | The ground's corners over a rectangle: columns `i0`.., rows `j0`.. (southwards), `cols` × `rows` corners. |
| `src/world/groundPatch.ts#ON_LINE` | já existia | sim/terrain/height_patch.h | How close to a grid line a point counts as on it: the corner itself, not a hair into the next cell. |
| `src/world/groundPatch.ts#PatchMiss` | já existia | sim/terrain/height_patch.h | A point read off a patch outside it: the patch sent was too small for what was asked (`GrowthMiss`). |
| `src/world/groundPatch.ts#PatchMiss.constructor` | já existia | sim/terrain/height_patch.h | leitura do chão fora da trava |
| `src/world/groundPatch.ts#cellDiagonal` | já existia | sim/terrain/height_patch.h | The diagonal a cell is drawn with: 1 the a-c one, 0 the b-d one (a margin keeps near-flat cells on b-d). |
| `src/world/groundPatch.ts#cellHeight` | já existia | sim/terrain/height_patch.h | The height at (u, v) of a cell with corners a (u 0, v 0), b (0, 1), c (1, 1), d (1, 0), split along `diagonal` |
| `src/world/groundPatch.ts#latticeHeight` | já existia | sim/terrain/height_patch.h | The height at (x, y) from the corners of a grid of `cell` over the plane: corner (i, j) stands at (i·cell, -j· |
| `src/world/groundPatch.ts#patchGround` | já existia | sim/terrain/height_patch.h | The ground of a patch at any point within it, as the terrain draws it; `PatchMiss` off it. |
| `src/world/groundPatch.ts#sampleGroundPatch` | já existia | sim/terrain/height_patch.h | The corners round a rectangle [minX, minY, maxX, maxY], each read off `groundAt` at the corner itself. |
| `src/world/gullies.ts#DEFAULT_GULLY_AUTO` | portado | sim/nature/ground_paint.cpp | How much of the steep land carries gullies of itself, 0..1, when the map says nothing. |
| `src/world/gullies.ts#GullyDab` | portado | sim/nature/ground_paint.cpp | ravinas pintadas |
| `src/world/gullies.ts#MAX_GULLY_DABS` | portado | sim/nature/ground_paint.cpp | A map keeps at most this many gully dabs; the oldest go first. |
| `src/world/gullies.ts#readGullyDab` | portado | sim/nature/ground_paint.cpp | ravinas pintadas |
| `src/world/gullies.ts#readGullyDab.finite` | portado | sim/nature/ground_paint.cpp | ravinas pintadas |
| `src/world/junction/levels.ts#JunctionLevels.has` | outro dono | roads | bulbos e níveis de cruzamento |
| `src/world/landscape.ts#ACCESS_CELL` | outro dono | street-furniture | The side of the cells `crossingAccessTester` files the landings in. |
| `src/world/landscape.ts#ACCESS_REACH` | outro dono | street-furniture | How far from a point a crossing's landing can reach it, besides the item's own radius |
| `src/world/landscape.ts#CrossingAccess` | outro dono | street-furniture | The landing of a crossing on the footways: kept clear of everything. |
| `src/world/landscape.ts#FootwayHit` | outro dono | street-furniture | Where a point lies on the footways: the road, the station along it and the side. |
| `src/world/landscape.ts#GROW_MINUTES` | outro dono | street-furniture | City minutes a planted tree or shrub takes to reach its full size: three days. |
| `src/world/landscape.ts#ITEM_GAP` | outro dono | street-furniture | The least clear distance kept between two placed items, besides their radii. |
| `src/world/landscape.ts#LANDSCAPE_KINDS` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#LANDSCAPE_RADIUS` | outro dono | street-furniture | Plan radius of each kind, for spacing and for anyone walking round it. |
| `src/world/landscape.ts#LandscapeItem` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#LandscapeKind` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#LandscapeRefusal` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#LandscapeSnap` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#MAX_ITEM_RADIUS` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#MEDIAN_NOSE_GAP` | outro dono | street-furniture | Room left between a crossing's far edge and a median island's nose, world units. |
| `src/world/landscape.ts#MEDIAN_TREE_MIN` | outro dono | street-furniture | Narrowest median a tree is planted in: its pit and a kerb's width each side. |
| `src/world/landscape.ts#MEDIAN_UTURN_OPENING` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#MedianHit` | outro dono | street-furniture | A planted median a point stands on: trees and shrubs go there too. |
| `src/world/landscape.ts#PLANTED_SIZE` | outro dono | street-furniture | The size a plant is planted at, as a share of its full size. |
| `src/world/landscape.ts#SIGN_HAS_TEXT` | outro dono | street-furniture | Signs whose plate shows the player's text. |
| `src/world/landscape.ts#SIGN_TEXT_MAX` | outro dono | street-furniture | The longest text a sign or a street name keeps. |
| `src/world/landscape.ts#SIGN_TYPES` | outro dono | street-furniture | The signs the one sign tool makes (the player's order of 2026-10-05) |
| `src/world/landscape.ts#SignType` | outro dono | street-furniture | A sign's type (`SIGN_TYPES`). |
| `src/world/landscape.ts#SnapLocal` | outro dono | street-furniture | What a caller placing many items hands `snapLandscape`, made once: the crossings' landings filed (`crossingAcc |
| `src/world/landscape.ts#accessMemo` | outro dono | street-furniture | Every crossing's approach, including its landing on each footway. |
| `src/world/landscape.ts#apart` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#crossingAccessTester` | outro dono | street-furniture | `onCrossingAccess` for many items against the same landings |
| `src/world/landscape.ts#crossingAccesses` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#crossingAccessesAt` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#crossingAccessesNow` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#depthFor` | outro dono | street-furniture | How far out from the centreline each kind stands, inside the furnishing zone. |
| `src/world/landscape.ts#footwayAt` | outro dono | street-furniture | The footway a point stands on, or, with `reach`, the nearest one within that distance of the footway's own ban |
| `src/world/landscape.ts#isLandscapeKind` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#isSignType` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#landscapeNear` | outro dono | street-furniture | The placed item nearest a point, within `radius`. |
| `src/world/landscape.ts#landscapeNearIn` | outro dono | street-furniture | DO NOT hand `doc.landscape.values()` to `landscapeNear` for one point: every item of the map, every move. |
| `src/world/landscape.ts#medianAt` | outro dono | street-furniture | The planted median under a point (within `reach` of its band), or null. |
| `src/world/landscape.ts#medianNose` | outro dono | street-furniture | How far from `node` along the segment its median island begins: past the crossing there |
| `src/world/landscape.ts#nearCrossingAccess` | outro dono | street-furniture | DO NOT test against `crossingAccesses(net)` (every landing of the map) for one point. |
| `src/world/landscape.ts#onAnyRoad` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/landscape.ts#onCrossingAccess` | outro dono | street-furniture | Whether something of `radius` at a point of `segment`'s footway stands in a crossing's landing. |
| `src/world/landscape.ts#plantGrowth` | outro dono | street-furniture | A plant's size now, as a share of its full size: planted small, grown over `GROW_MINUTES`. |
| `src/world/landscape.ts#plantedMedian` | outro dono | street-furniture | Whether a road's median takes plants: wide enough, kerbed (not painted flush), not paved. |
| `src/world/landscape.ts#ribbonsNear` | outro dono | street-furniture | The ribbons a point within `reach` of a road's surface can be on |
| `src/world/landscape.ts#snapLandscape` | outro dono | street-furniture | Where an item of `kind` put at `at` would stand, or why it cannot. |
| `src/world/landscape.ts#snapLandscape.others` | outro dono | street-furniture | itens de rua (calçada, canteiro) |
| `src/world/network.ts#Network.crossingLandingAt` | já existia | sim/roads/road_network | rede viária |
| `src/world/roadField.ts#SpatialIndex.all` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#SpatialIndex.buckets` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#SpatialIndex.cell` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#SpatialIndex.constructor` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#SpatialIndex.joined` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#SpatialIndex.near` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#SpatialIndex.oversized` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#SpatialIndex.oversizedSet` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#cellKeyOf` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roadField.ts#packField` | outro dono | road-elevation | campo das vias no chão |
| `src/world/roads/derivedSigns.ts#signApproach` | outro dono | street-furniture | mobiliário e nomes de rua |
| `src/world/structures.ts#isSunken` | outro dono | road-elevation | estruturas |
| `src/world/terrain.ts#BASE_AMPLITUDE` | portado | sim/terrain/terrain_brush.cpp | The land the map starts with, before the player touches it. |
| `src/world/terrain.ts#EMPTY` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#MAX_TERRAIN_STAMPS` | portado | sim/terrain/terrain_brush.cpp | How many brush dabs one map may hold. |
| `src/world/terrain.ts#MIN_HARD_RIM` | portado | sim/terrain/terrain_brush.cpp | The narrowest share of its radius a hard dab's edge falls over (`TerrainStamp.hardness`). |
| `src/world/terrain.ts#MIN_HARD_WALL` | portado | sim/terrain/terrain_brush.cpp | The narrowest a hard dab's wall is: two cells of the drawn terrain grid (6.4 m). |
| `src/world/terrain.ts#RELIEF_BLEND` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#RELIEF_FLAT` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#RELIEF_HALF` | portado | sim/terrain/terrain_brush.cpp | Where a map's own relief (1, 2) stands |
| `src/world/terrain.ts#RELIEF_LEGACY` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#RELIEF_NATURAL` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#RIVER_BED_DEPTH` | portado | sim/terrain/terrain_brush.cpp | A natural river dab carves this share of the old canyon's depth. |
| `src/world/terrain.ts#RIVER_BED_FLOOR` | portado | sim/terrain/terrain_brush.cpp | The share of a natural river dab's reach, from its edge, that is bank; inside it the bed is flat. |
| `src/world/terrain.ts#RIVER_CARVE` | portado | sim/terrain/terrain_brush.cpp | The extra factor a river stamp carves with, over a raise of the same strength. |
| `src/world/terrain.ts#ROUGH_REACH_MIN` | portado | sim/terrain/terrain_brush.cpp | The smallest share of its radius a rough dab reaches (`TerrainStamp.rough`). |
| `src/world/terrain.ts#ReliefVersion` | portado | sim/terrain/terrain_brush.cpp | Which land a map starts from (`RoadDoc.terrainRelief`) |
| `src/world/terrain.ts#STAMP_CELL` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#TERRAIN_MAX_HEIGHT` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#TERRAIN_MIN_HEIGHT` | portado | sim/terrain/terrain_brush.cpp | The vertical extent of the land, in world units: how deep a pit and how high a mountain the brush may ever mak |
| `src/world/terrain.ts#TERRAIN_WATER_HEIGHT` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#TerrainIndex` | portado | sim/terrain/terrain_brush.cpp | A uniform grid over the stamps, so a height query touches only nearby ones. |
| `src/world/terrain.ts#TerrainIndex.buckets` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#TerrainIndex.constructor` | portado | sim/terrain/terrain_brush.cpp | `before`: an index of the first of these stamps on the same land (a dab appended): its cells shared, only the  |
| `src/world/terrain.ts#TerrainIndex.ground` | portado | sim/terrain/terrain_brush.cpp | The land under the stamps when it is read otherwise than from the relief at the point itself |
| `src/world/terrain.ts#TerrainIndex.near` | portado | sim/terrain/terrain_brush.cpp | The stamps that can affect a point, in authoring order. |
| `src/world/terrain.ts#TerrainIndex.relief` | portado | sim/terrain/terrain_brush.cpp | The map's own land at a point, before any stamp: its relief's field. |
| `src/world/terrain.ts#TerrainIndex.revision` | portado | sim/terrain/terrain_brush.cpp | The revision the caller built it at (`RoadDoc.terrainRevision`), kept for it; the index itself never reads it. |
| `src/world/terrain.ts#TerrainIndex.stamps` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#TerrainMode` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#TerrainProfile` | portado | sim/terrain/terrain_brush.cpp | A raise's SHAPE other than the dome of its falloff. 'dome': a granite sugarloaf |
| `src/world/terrain.ts#TerrainStamp` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#baseRelief` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#baseReliefTuned` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#hash2` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#isTerrainMode` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#isTerrainProfile` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#naturalRelief` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#naturalReliefTuned` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#readRelief` | portado | sim/terrain/terrain_brush.cpp | A saved map's relief as the document reads it, or null when the value is none. 4 was the planet's Earth |
| `src/world/terrain.ts#reliefHeight` | portado | sim/terrain/terrain_brush.cpp | The map's own land at a point, before any stamp: its relief's field. |
| `src/world/terrain.ts#sampleTerrainHeight` | portado | sim/terrain/terrain_brush.cpp | The height of the land at a point: base relief plus every stamp over it. |
| `src/world/terrain.ts#sampleTerrainHeight.settle` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#smooth01` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#stampCellKey` | portado | sim/terrain/terrain_brush.cpp | relevo: rio, dureza, borda natural |
| `src/world/terrain.ts#sugarloafProfile` | portado | sim/terrain/terrain_brush.cpp | A sugarloaf's section, 1 at its centre to 0 at its foot, over `d` = the share of the reach out from the centre |
| `src/world/terrain.ts#terrainInfluence` | portado | sim/terrain/terrain_brush.cpp | How much a stamp of strength 1 bends the ground `unit` of the way out |
| `src/world/terrain.ts#valueNoise` | portado | sim/terrain/terrain_brush.cpp | Smooth deterministic value noise, -1..1 (the land's and the ecosystem's). |
| `src/world/terrainPaint.ts#BIOME_KINDS` | portado | sim/nature/ground_paint.cpp | The BIOMES (`world/ecology.ts`): painting one makes that ground the biome's own country |
| `src/world/terrainPaint.ts#BiomeKind` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/terrainPaint.ts#COVER_KINDS` | portado | sim/nature/ground_paint.cpp | The kinds painted as a density of things standing on the ground. |
| `src/world/terrainPaint.ts#CoverKind` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/terrainPaint.ts#GEOLOGY_KINDS` | portado | sim/nature/ground_paint.cpp | The rocks the land can be made of, in the terrain shader's layer order. |
| `src/world/terrainPaint.ts#GeologyKind` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/terrainPaint.ts#MAX_PAINT_DABS` | portado | sim/nature/ground_paint.cpp | A map keeps at most this many dabs; the oldest go first. |
| `src/world/terrainPaint.ts#PAINT_KINDS` | portado | sim/nature/ground_paint.cpp | The paintable layers, in channel order: two RGBA textures, then grass, then the covers. |
| `src/world/terrainPaint.ts#PaintDab` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/terrainPaint.ts#PaintKind` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/terrainPaint.ts#isBiomeKind` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/terrainPaint.ts#isCoverKind` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/terrainPaint.ts#isGeologyKind` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/terrainPaint.ts#isPaintKind` | portado | sim/nature/ground_paint.cpp | pintura do chão |
| `src/world/trees.ts#DEFAULT_TREE_BRUSH` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#KIND_SLOTS` | portado | sim/nature/planted_trees.cpp | The models' slots (`render/natureTrees.ts` TREE_MODELS): two oaks, two cypresses, two palms. |
| `src/world/trees.ts#MAX_PLANTED_TREES` | portado | sim/nature/planted_trees.cpp | A map keeps at most this many planted trees, and this many clearings; the oldest go first. |
| `src/world/trees.ts#MAX_TREE_CLEARINGS` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#MODEL_COUNT` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#PlantedTree` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#TREE_KINDS` | portado | sim/nature/planted_trees.cpp | The tree models to choose from (`render/natureTrees.ts` TREE_MODELS, in order): each its share of the seed's r |
| `src/world/trees.ts#TreeBrush` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#TreeClearing` | portado | sim/nature/planted_trees.cpp | Where the trees were cut away: no tree of the woods grows inside. |
| `src/world/trees.ts#TreeKind` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#finite` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#oneTree` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#plantTrees` | portado | sim/nature/planted_trees.cpp | The trees one dab of the brush plants in a circle: as many as the density asks for its area |
| `src/world/trees.ts#readPlantedTree` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#readTreeClearing` | portado | sim/nature/planted_trees.cpp | árvores plantadas e clareiras |
| `src/world/trees.ts#treeSeed` | portado | sim/nature/planted_trees.cpp | A seed in the chosen kind's share of the range (any model for 'mixed'). |
| `src/world/weather.ts#DEFAULT_WEATHER` | portado | sim/nature/sky.cpp, nature_types.h | clima do mapa |
| `src/world/weather.ts#WEATHER_LIMITS` | portado | sim/nature/sky.cpp, nature_types.h | clima do mapa |
| `src/world/weather.ts#Weather` | portado | sim/nature/sky.cpp, nature_types.h | clima do mapa |
| `src/world/weather.ts#clamp` | portado | sim/nature/sky.cpp, nature_types.h | clima do mapa |
| `src/world/weather.ts#readWeather` | portado | sim/nature/sky.cpp, nature_types.h | The weather read from a saved map (or a partial change merged over one), kept in range. |
| `src/world/weather.ts#readWeather.pick` | portado | sim/nature/sky.cpp, nature_types.h | clima do mapa |
| `src/world/weather.ts#windVector` | portado | sim/nature/sky.cpp, nature_types.h | The wind on the map, world units a second along x and y. |

## Fatias de código compartilhado

| Fatia | Linhas | Estado | Dono no jogo |
|---|---|---|---|
| `src/actionsWiring.ts#createActions.explodeAt` | 10 | outro dono | demolition |
| `src/editor/bulldozer.ts#Bulldozer.boxed` | 7 | outro dono | demolition |
| `src/editor/bulldozer.ts#Bulldozer.click` | 8 | outro dono | demolition |
| `src/editor/chunkedSave.ts#KEYED` | 6 | já existia | sim/persistence/save_landscape.cpp |
| `src/editor/cityGenerator.ts#greenCity` | 5 | já existia | sim/citygen |
| `src/editor/commit.ts#DraftResult` | 3 | outro dono | road-elevation / roads |
| `src/editor/commit.ts#boreDeepCuts` | 19 | outro dono | road-elevation / roads |
| `src/editor/commit.ts#commitRoadPath` | 44 | outro dono | road-elevation / roads |
| `src/editor/commit.ts#draftStations` | 9 | outro dono | road-elevation / roads |
| `src/editor/commit.ts#lastDryRun` | 6 | outro dono | road-elevation / roads |
| `src/editor/commit.ts#layCopy` | 5 | outro dono | road-elevation / roads |
| `src/editor/commit.ts#onTerrain` | 1 | outro dono | road-elevation / roads |
| `src/editor/commit.ts#solveOver` | 8 | outro dono | road-elevation / roads |
| `src/editor/commit.ts#terrainIndexOf` | 6 | outro dono | road-elevation / roads |
| `src/editor/editRules.ts#flatSolves` | 4 | outro dono | roads |
| `src/editor/editRules.ts#flatSolves.after` | 1 | outro dono | roads |
| `src/editor/editRules.ts#flatSolves.beforeSolve` | 1 | outro dono | roads |
| `src/editor/editRules.ts#grade` | 6 | outro dono | roads |
| `src/editor/editRules.ts#tooSteep` | 9 | outro dono | roads |
| `src/editor/growthFeed.ts#GrowthSession.send` | 5 | não se aplica | - |
| `src/editor/growthReplica.ts#GrowAsk` | 3 | não se aplica | - |
| `src/editor/growthReplica.ts#GrowthReplica.handle` | 10 | não se aplica | - |
| `src/editor/history.ts#openedGenerated` | 3 | já existia | sim/commands/edit_history.cpp |
| `src/editor/persistence.ts#isSerializedDoc` | 19 | portado | sim/persistence/save_nature.cpp |
| `src/editor/poles.ts#inLanding` | 8 | outro dono | street-furniture |
| `src/editor/roadTool.ts#RoadToolHost` | 4 | outro dono | roads |
| `src/editor/roadTool.ts#landscapeAlong` | 7 | outro dono | roads |
| `src/editor/roads/streetKit.ts#carried` | 1 | outro dono | street-furniture |
| `src/editor/roads/streetKit.ts#kitOf` | 5 | outro dono | street-furniture |
| `src/editor/smallCityMap.ts#growLots` | 7 | já existia | sim/citygen |
| `src/editor/smallCityMap.ts#growLots.pavedAt` | 1 | já existia | sim/citygen |
| `src/editor/streetObjects.ts#commitUturn` | 7 | outro dono | street-furniture |
| `src/lab/catalog/layers.ts#signItems` | 7 | não se aplica | - |
| `src/lab/catalog/layers.ts#signItems.one` | 3 | não se aplica | - |
| `src/lab/catalog/layout.ts#surfacesOf` | 11 | não se aplica | - |
| `src/main.ts#<ExpressionStatement>@1674` | 1 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#<ExpressionStatement>@192` | 1 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#<ExpressionStatement>@195` | 1 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#<IfStatement>@1703` | 7 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#<IfStatement>@4036` | 9 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#BrushMode` | 1 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#LANDFORMS` | 6 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#MAP_ACTIONS` | 11 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#NEW_RELIEF` | 1 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#SLIDERS` | 8 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#bulldozer` | 3 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#cloudTool` | 14 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#currentGesture` | 5 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#dabSettings` | 2 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#drawOverlayScreen` | 102 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#drawStreetscapeHover` | 33 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#endPointer` | 8 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#endTerrainStroke` | 7 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#frame` | 4 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#gestureInProgress` | 3 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#growth` | 3 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#mapBiome` | 3 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#newMap` | 5 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#newNature` | 1 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#openCity` | 3 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#openCityNow` | 7 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#pointerDown` | 13 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#pointerMove` | 15 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#restoreBootSave` | 7 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#setMapBiome` | 5 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#setNodeHeightMetres` | 16 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#streetscapeTool` | 12 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#terrainBrush` | 11 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/main.ts#weatherSlider` | 2 | portado | project/ui/land_strips.gd, project/game/tools/* |
| `src/render/agents.ts#LAMP_GAIN` | 7 | já existia | render/agents |
| `src/render/agents.ts#createAgentMeshes.sync` | 12 | já existia | render/agents |
| `src/render/buildings/buildingMesh.ts#LAWN_LATTICE` | 1 | já existia | render/buildings |
| `src/render/buildings/buildingMesh.ts#emitLots` | 12 | já existia | render/buildings |
| `src/render/buildings/flagLayer.ts#createFlagLayer.set` | 8 | já existia | render/buildings |
| `src/render/buildings/flagLayer.ts#material` | 5 | já existia | render/buildings |
| `src/render/buildings/kit.ts#createBuildingKit` | 7 | já existia | render/buildings |
| `src/render/buildings/kit.ts#createBuildingKit.setNight` | 8 | já existia | render/buildings |
| `src/render/cellInstances.ts#createCellInstances.append` | 5 | já existia | render/rs_util.h MultiMeshDraw |
| `src/render/cellInstances.ts#createCellInstances.visit` | 11 | já existia | render/rs_util.h MultiMeshDraw |
| `src/render/clearingCells.ts#ClearingCells.cells` | 1 | portado | sim/nature/planted_trees.cpp |
| `src/render/clearingCells.ts#ClearingCells.file` | 7 | portado | sim/nature/planted_trees.cpp |
| `src/render/clearingCells.ts#ClearingCells.filed` | 1 | portado | sim/nature/planted_trees.cpp |
| `src/render/clearingCells.ts#ClearingCells.follow` | 16 | portado | sim/nature/planted_trees.cpp |
| `src/render/clearingCells.ts#ClearingCells.has` | 5 | portado | sim/nature/planted_trees.cpp |
| `src/render/clearingCells.ts#ClearingCells.over` | 5 | portado | sim/nature/planted_trees.cpp |
| `src/render/clearingCells.ts#ClearingCells.refile` | 4 | portado | sim/nature/planted_trees.cpp |
| `src/render/clearingCells.ts#ClearingCells.unfile` | 9 | portado | sim/nature/planted_trees.cpp |
| `src/render/coverSweep.ts#Candidate` | 3 | portado em parte | render/cover/cover_renderer.cpp, render/trees/tree_place.cpp |
| `src/render/coverSweep.ts#forestIn` | 13 | portado em parte | render/cover/cover_renderer.cpp, render/trees/tree_place.cpp |
| `src/render/coverSweep.ts#insidePlate` | 6 | portado em parte | render/cover/cover_renderer.cpp, render/trees/tree_place.cpp |
| `src/render/coverSweep.ts#latticeOver` | 6 | portado em parte | render/cover/cover_renderer.cpp, render/trees/tree_place.cpp |
| `src/render/coverSweep.ts#rocksIn` | 12 | portado em parte | render/cover/cover_renderer.cpp, render/trees/tree_place.cpp |
| `src/render/coverSweep.ts#scrubIn` | 14 | portado em parte | render/cover/cover_renderer.cpp, render/trees/tree_place.cpp |
| `src/render/coverSweep.ts#shoreIn` | 16 | portado em parte | render/cover/cover_renderer.cpp, render/trees/tree_place.cpp |
| `src/render/densityCars.ts#TILE_RADIUS` | 1 | outro dono | traffic |
| `src/render/densityCars.ts#createDensityCars.tileAt` | 12 | outro dono | traffic |
| `src/render/exhaust.ts#createExhaust` | 7 | outro dono | traffic |
| `src/render/flatAtlas.ts#FarResident` | 3 | não se aplica | - |
| `src/render/flatAtlas.ts#Plate` | 5 | não se aplica | - |
| `src/render/flatAtlas.ts#PlateSource` | 12 | não se aplica | - |
| `src/render/flatAtlas.ts#PlateSource.paintSources` | 1 | não se aplica | - |
| `src/render/flatAtlas.ts#PlateSource.stampSources` | 1 | não se aplica | - |
| `src/render/flatAtlas.ts#PlateSource.terrainPaint` | 1 | não se aplica | - |
| `src/render/flatAtlas.ts#PlateSource.terrainStamps` | 1 | não se aplica | - |
| `src/render/flatAtlas.ts#SIDE` | 1 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas` | 175 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.ask` | 4 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.askCorners` | 5 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.bareAt` | 1 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.buildFar` | 8 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.buildSlab` | 3 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.decode` | 5 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.drawFar` | 6 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.encode` | 4 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.ensure` | 43 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.fill` | 9 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.followFar` | 20 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.groundOn` | 11 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.keepWithinBudget` | 7 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.letGo` | 4 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.measureTop` | 3 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.one` | 3 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.plateHolds` | 9 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.plateHolds.reach` | 1 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.readContent` | 21 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent` | 19 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.refresh` | 5 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.shaperFor` | 4 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.show` | 3 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.sync` | 45 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.take` | 8 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.takePaint` | 8 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.takeStamps` | 8 | não se aplica | - |
| `src/render/flatAtlas.ts#createFlatAtlas.writeFar` | 3 | não se aplica | - |
| `src/render/flatAtlas.ts#reaches` | 2 | não se aplica | - |
| `src/render/flatAtlas.ts#writeFarNode` | 3 | não se aplica | - |
| `src/render/flatAtlas.ts#writeFarPatch` | 3 | não se aplica | - |
| `src/render/flatGround.worker.ts#Ask` | 1 | não se aplica | - |
| `src/render/flatGround.worker.ts#serve` | 13 | não se aplica | - |
| `src/render/flatGroundPool.ts#Job` | 3 | não se aplica | - |
| `src/render/gardens.ts#createGardenCells` | 7 | já existia | sim/buildings/b40/lot_yard + render/buildings/lot_props |
| `src/render/gardens.ts#createGardenCells.dispose` | 3 | já existia | sim/buildings/b40/lot_yard + render/buildings/lot_props |
| `src/render/gardens.ts#gardenKinds` | 7 | já existia | sim/buildings/b40/lot_yard + render/buildings/lot_props |
| `src/render/gardens.ts#gardenKinds.kind` | 7 | já existia | sim/buildings/b40/lot_yard + render/buildings/lot_props |
| `src/render/gardens.ts#gardenPack` | 24 | já existia | sim/buildings/b40/lot_yard + render/buildings/lot_props |
| `src/render/gardens.ts#gardenPictures` | 1 | já existia | sim/buildings/b40/lot_yard + render/buildings/lot_props |
| `src/render/gpu/twins.ts#LandLodData` | 3 | não se aplica | - |
| `src/render/gpu/twins.ts#ground` | 13 | não se aplica | - |
| `src/render/gpu/twins.ts#ground.point` | 1 | não se aplica | - |
| `src/render/gpu/twins.ts#ground.texel` | 1 | não se aplica | - |
| `src/render/groundShape.worker.ts#serve` | 14 | não se aplica | - |
| `src/render/groundShapePool.ts#GroundShapePool.digest` | 3 | não se aplica | - |
| `src/render/groundShapePool.ts#GroundShapePool.pads` | 3 | não se aplica | - |
| `src/render/groundShapePool.ts#GroundShapePool.run` | 5 | não se aplica | - |
| `src/render/groundShapePool.ts#GroundShapePool.shape` | 3 | não se aplica | - |
| `src/render/groundShapePool.ts#GroundShapePool.start` | 17 | não se aplica | - |
| `src/render/groundShapePool.ts#GroundWorkers` | 5 | não se aplica | - |
| `src/render/groundShapePool.ts#Lane` | 3 | não se aplica | - |
| `src/render/landStroke.ts#GRID` | 1 | não se aplica | - |
| `src/render/landStroke.ts#measureLevels` | 7 | não se aplica | - |
| `src/render/landStroke.ts#runLandStroke` | 7 | não se aplica | - |
| `src/render/landStroke.ts#shoreLevelsInto` | 28 | portado | render/water/shore_field.cpp |
| `src/render/landStroke.ts#woodsBlock` | 9 | não se aplica | - |
| `src/render/landStroke.ts#woodsBlock.land` | 1 | não se aplica | - |
| `src/render/materials.ts#createMaterials` | 9 | já existia | project/data/render/materials.json |
| `src/render/materials.ts#createMaterials.setDetail` | 3 | já existia | project/data/render/materials.json |
| `src/render/optimizationsApply.ts#OptimizerDeps` | 3 | outro dono | interface (gráficos) |
| `src/render/optimizationsApply.ts#createOptimizer` | 3 | outro dono | interface (gráficos) |
| `src/render/optimizationsApply.ts#createOptimizer.set` | 8 | outro dono | interface (gráficos) |
| `src/render/plateMesh.ts#buildPlateMesh` | 11 | não se aplica | - |
| `src/render/platePaint.ts#dabInRects` | 2 | não se aplica | - |
| `src/render/platePaint.ts#dabKey` | 1 | não se aplica | - |
| `src/render/platePaint.ts#plateDabs` | 21 | não se aplica | - |
| `src/render/poleCells.ts#PoleCells.constructor` | 3 | outro dono | street-furniture |
| `src/render/poleLayer.ts#buildPolePreview` | 6 | outro dono | street-furniture |
| `src/render/poleLayer.ts#buildUtilities` | 7 | outro dono | street-furniture |
| `src/render/postprocess.ts#PostChain` | 3 | portado | project/render/shaders/atmosphere.gdshader |
| `src/render/postprocess.ts#createPostChain` | 64 | portado | project/render/shaders/atmosphere.gdshader |
| `src/render/postprocess.ts#createPostChain.disposeAll` | 3 | portado | project/render/shaders/atmosphere.gdshader |
| `src/render/postprocess.ts#createPostChain.followTown` | 3 | portado | project/render/shaders/atmosphere.gdshader |
| `src/render/postprocess.ts#createPostChain.mapLost` | 1 | portado | project/render/shaders/atmosphere.gdshader |
| `src/render/postprocess.ts#createPostChain.render` | 3 | portado | project/render/shaders/atmosphere.gdshader |
| `src/render/postprocess.ts#createPostChain.setAtmosphere` | 3 | portado | project/render/shaders/atmosphere.gdshader |
| `src/render/postprocess.ts#layClouds` | 14 | portado | project/render/shaders/atmosphere.gdshader |
| `src/render/propCells.ts#PropLayerSpec` | 3 | já existia | render/props |
| `src/render/propCells.ts#createPropLayer.batches` | 27 | já existia | render/props |
| `src/render/propCells.ts#createPropLayer.cull` | 3 | já existia | render/props |
| `src/render/propCells.ts#createPropLayer.note` | 9 | já existia | render/props |
| `src/render/propCells.ts#createPropLayer.show` | 6 | já existia | render/props |
| `src/render/propCells.ts#nearSquareOf` | 4 | já existia | render/props |
| `src/render/propGeometry.ts#TREE_KIND` | 3 | já existia | render/props/prop_kit.cpp |
| `src/render/renderer.ts#NaturePlate` | 3 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#SURFACE_SETS` | 1 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#SceneHandle` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#Shaping` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer` | 704 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.applyQuality` | 4 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.blockRead` | 9 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.blocksOver` | 15 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.bringPlateLand` | 7 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.buildWorld` | 16 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.builtGroundAt` | 1 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.compareSolves` | 74 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.completeSolve` | 12 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.coverCellsOf` | 10 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.coverPlain` | 1 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.deckHeight` | 4 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.elementLayerOf` | 8 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.farCellRect` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.fileNature` | 10 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.landGroundDigest` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.landMovedSince` | 9 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.landShapes` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.levelGroundDigest` | 6 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.loadEffects` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.natureOpen` | 6 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.natureStands` | 4 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.natureSweep` | 74 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.networkExtent` | 11 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.noteSway` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.padsAskOf` | 12 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.paintedForestOf` | 9 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.plantAll` | 9 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.plantedForestOf` | 10 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.plantedIn` | 1 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.platesUnder` | 4 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.readOf` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.rebuildUtilities` | 3 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.regionRect` | 1 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.requestNetworkFar` | 4 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.shapeGround` | 47 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.startNatureTrees` | 25 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.stationsAsked` | 2 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.strikeAt` | 4 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.sweepAllCovers` | 5 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.switchedOff` | 16 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.takeEditedCells` | 20 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.terrainRegion` | 1 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders` | 9 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` | 15 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.watchRoads` | 14 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.waterMeshes` | 3 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#createSceneRenderer.worldSteps` | 24 | portado | render/render_world.cpp, project/render/* |
| `src/render/renderer.ts#isLevelLand` | 1 | portado | render/render_world.cpp, project/render/* |
| `src/render/riggedCitizens.ts#createRiggedCitizens.draw` | 6 | outro dono | people |
| `src/render/riggedCitizens.ts#createRiggedCitizens.emit` | 4 | outro dono | people |
| `src/render/roadStations.ts#Hint` | 2 | outro dono | road-elevation |
| `src/render/roadStations.ts#RoadStations.current` | 2 | outro dono | road-elevation |
| `src/render/roadStations.ts#RoadStations.waiting` | 3 | outro dono | road-elevation |
| `src/render/roadStations.worker.ts#serve` | 8 | outro dono | road-elevation |
| `src/render/roadSurfacePass.ts#GROUND_MAX_EDGE` | 1 | outro dono | road-elevation |
| `src/render/roadSurfacePass.ts#RAISED_MAX_EDGE` | 1 | outro dono | road-elevation |
| `src/render/roadSurfacePass.ts#followsGround` | 1 | outro dono | road-elevation |
| `src/render/roadSurfacePass.ts#passDesc` | 5 | outro dono | road-elevation |
| `src/render/roadSurfaceRings.ts#medianOf.back` | 4 | outro dono | road-elevation |
| `src/render/roadSurfaces.ts#buildRoadSurfaces` | 9 | outro dono | road-elevation |
| `src/render/roadSurfaces.ts#roadSurfaceSteps` | 204 | outro dono | road-elevation |
| `src/render/sharedGround.ts#GroundShare.relief` | 1 | não se aplica | - |
| `src/render/sharedGround.ts#GroundShare.setRelief` | 4 | não se aplica | - |
| `src/render/sharedGround.ts#SharedGroundData` | 3 | não se aplica | - |
| `src/render/sharedGround.ts#sharedGroundReaders.bare` | 1 | não se aplica | - |
| `src/render/sharedGround.ts#sharedGroundReaders.read` | 3 | não se aplica | - |
| `src/render/signLayer.ts#SignLayerKept` | 3 | outro dono | street-furniture |
| `src/render/signLayer.ts#SignProduct.allKeys` | 3 | outro dono | street-furniture |
| `src/render/signLayer.ts#SignProduct.dirty` | 12 | outro dono | street-furniture |
| `src/render/signLayer.ts#SignSource` | 3 | outro dono | street-furniture |
| `src/render/signLayer.ts#Stand` | 3 | outro dono | street-furniture |
| `src/render/signLayer.ts#Standing` | 3 | outro dono | street-furniture |
| `src/render/signLayer.ts#createSignLayer.putQuad` | 24 | outro dono | street-furniture |
| `src/render/signLayer.ts#isSignItem` | 1 | outro dono | street-furniture |
| `src/render/signLayer.ts#itemSigns` | 40 | outro dono | street-furniture |
| `src/render/signals.ts#createSignalHeads` | 9 | já existia | render/props/signal_lamps |
| `src/render/signs.ts#PLATE` | 12 | outro dono | street-furniture |
| `src/render/signs.ts#Sheet` | 3 | outro dono | street-furniture |
| `src/render/signs.ts#SignSheets.drop` | 20 | outro dono | street-furniture |
| `src/render/signs.ts#SignSheets.plateCell` | 17 | outro dono | street-furniture |
| `src/render/signs.ts#SignSheets.take` | 6 | outro dono | street-furniture |
| `src/render/signs.ts#onTopOfPost` | 1 | outro dono | street-furniture |
| `src/render/signs.ts#plateCanvas` | 75 | outro dono | street-furniture |
| `src/render/signs.ts#plateFaces` | 4 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#FurnitureSource` | 3 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#MEADOW_STORES` | 4 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys` | 3 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.compute` | 4 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.constructor` | 4 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty` | 9 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#ROAD_REACH` | 1 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` | 3 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#createStreetFurniture` | 22 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose` | 3 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#furniturePlace` | 59 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#furnitureStores` | 26 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#furnitureStores.plant` | 7 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#isFurniture` | 1 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#meadowInto` | 5 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#meadowInto.add` | 4 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#meadowProduct` | 10 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#placeInto` | 10 | outro dono | street-furniture |
| `src/render/streetFurnitureLayer.ts#stillGrowing` | 2 | outro dono | street-furniture |
| `src/render/structureCells.ts#StructureCells` | 4 | outro dono | road-elevation |
| `src/render/structures.ts#buildStructureDetails` | 5 | outro dono | road-elevation |
| `src/render/structures.ts#retainingWalls` | 44 | outro dono | road-elevation |
| `src/render/structures.ts#structureParts` | 187 | outro dono | road-elevation |
| `src/render/structures.ts#structureRibbons` | 3 | outro dono | road-elevation |
| `src/render/transit.ts#createTransit` | 4 | outro dono | transit |
| `src/render/transit.ts#createTransit.update` | 9 | outro dono | transit |
| `src/render/utilities.ts#placePole` | 5 | outro dono | street-furniture |
| `src/render/utilities.ts#poleGroundAt` | 10 | outro dono | street-furniture |
| `src/render/utilities.ts#polePartSpecs` | 19 | outro dono | street-furniture |
| `src/render/utilities.ts#wireMaterials` | 16 | outro dono | street-furniture |
| `src/ui/toolChoices.ts#kind` | 1 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#paint` | 1 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#paintKind` | 2 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#setPaintKind` | 4 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#setStreetscapeKind` | 4 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#setTreeBrush` | 4 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#setTreeKind` | 1 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#signChoice` | 1 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#streetscapeKind` | 2 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#treeBrush` | 1 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#treeBrushNow` | 3 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#treeKind` | 1 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/toolChoices.ts#treeKindNow` | 1 | não portado | project/game/tools/brush_defaults.gd |
| `src/ui/v2/shell.ts#PAINT_SWATCH` | 5 | portado | project/ui/land_strips.gd, project/data/ui/land_panels.json |
| `src/ui/v2/shell.ts#mountShell.renderLandscape` | 30 | portado | project/ui/land_strips.gd, project/data/ui/land_panels.json |
| `src/ui/v2/shell.ts#mountShell.textInput` | 3 | portado | project/ui/land_strips.gd, project/data/ui/land_panels.json |
| `src/world/cityGen/field.ts#TensorField.major` | 5 | já existia | sim/citygen |
| `src/world/cityGen/plan.ts#planCity.inside` | 4 | já existia | sim/citygen |
| `src/world/cityGen/plan.ts#planCity.zoneAt` | 8 | já existia | sim/citygen |
| `src/world/cityGen/squares.ts#SquarePlan` | 4 | já existia | sim/citygen |
| `src/world/cityGen/squares.ts#planSquares` | 13 | já existia | sim/citygen |
| `src/world/doc.ts#RoadDoc.addClouds` | 11 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.addLandscape` | 3 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.addLandscapeMany` | 14 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.addPaintDab` | 7 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.addTerrainStamp` | 9 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.clearTerrain` | 4 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.clouds` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.countStreetName` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.deleteLandscapeRecord` | 3 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.fogDabs` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.fogSettings` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.fromJSON` | 42 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.gullyAuto` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.gullyDabs` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.landscape` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.landscapeAt` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.landscapeIn` | 2 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.landscapeInAny` | 2 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.landscapeMap` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.layOver` | 7 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.nature` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.plantTrees` | 9 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.putLandscapeRecord` | 7 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.removeCloud` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.removeLandscape` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.replaceContents` | 36 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.replaceWith` | 7 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.restore` | 16 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.restore.itemRects` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.restoreValue` | 9 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.savedGlobals` | 11 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.serialized` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.setFogSettings` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.setGullyAuto` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.setNature` | 4 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.setWeather` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.takeFork` | 14 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.terrainPaint` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.terrainRelief` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.terrainStamps` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.treeCells` | 5 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.treeClearings` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.treeGrid` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.trees` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.treesIn` | 3 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.valueOf` | 9 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.weather` | 1 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.withRoom` | 6 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#RoadDoc.worldPart` | 11 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#SerializedDoc` | 3 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#TILE_CAPS` | 4 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#itemDifferences` | 11 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#landscapeRecord` | 8 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#listBox` | 9 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#placedCloud` | 2 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#samePaint` | 7 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#sameStamps` | 10 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#savedLandscape` | 3 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/doc.ts#stampDifferences` | 12 | portado | sim/nature/nature_state.h, sim/commands/nature_commands.cpp, sim/persistence/save_nature.cpp |
| `src/world/junction/bulb.ts#clearBulbsOf` | 7 | outro dono | roads |
| `src/world/poleLines.ts#PoleLines` | 3 | outro dono | street-furniture |
| `src/world/poleLines.ts#poleLinesOf` | 5 | outro dono | street-furniture |
| `src/world/roads/buildMode.ts#stationsAlong` | 10 | outro dono | street-furniture |
| `src/world/roads/furnitureCatalog.ts#FurnitureEntry` | 3 | outro dono | street-furniture |
| `src/world/roads/furnitureCatalog.ts#furnitureEntry` | 1 | outro dono | street-furniture |
| `src/world/roads/furnitureSets.ts#FurniturePlacement` | 3 | outro dono | street-furniture |
| `src/world/roads/furnitureSets.ts#Pattern` | 3 | outro dono | street-furniture |
| `src/world/roads/furnitureSets.ts#furnitureFor` | 47 | outro dono | street-furniture |
| `src/world/roads/furnitureSets.ts#furnitureFor.file` | 6 | outro dono | street-furniture |
| `src/world/roads/furnitureSets.ts#nearGrid` | 3 | outro dono | street-furniture |
| `src/world/roads/furnitureSets.ts#nearGrid.around` | 7 | outro dono | street-furniture |
| `src/world/roads/furnitureSets.ts#nearGrid.file` | 5 | outro dono | street-furniture |
| `src/world/roads/streetNames.ts#platesNear` | 9 | outro dono | street-furniture |
| `src/world/roads/streetNames.ts#streetNameOf` | 6 | outro dono | street-furniture |
| `src/world/solids.ts#FURNITURE` | 4 | já existia | sim/buildings |
| `src/world/solids.ts#createSolids` | 7 | já existia | sim/buildings |
| `src/world/solids.ts#solidsOfItem` | 9 | já existia | sim/buildings |
| `src/world/streetFurniture.ts#FURNITURE_OF` | 11 | outro dono | street-furniture |
| `src/world/streetFurniture.ts#furnitureOf` | 9 | outro dono | street-furniture |
| `src/world/streetFurniture.ts#standing` | 23 | outro dono | street-furniture |
| `src/world/streetFurniture.ts#streetFurniture` | 7 | outro dono | street-furniture |
