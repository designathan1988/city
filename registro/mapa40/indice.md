# Inventário funcional do Roadcraft 4.0

Gerado em 2026-11-05T05:15:57.805Z por `ferramentas/mapa40/funcionalidades.cjs` sobre `grafo.json` (16771 unidades, 64093 arestas, 1480 canais dinâmicos, 661 specs; commit do 4.0 22357c928fb9). Sementes achadas no código: 308. Unidades fora dos laboratórios: 15548; alcançadas por alguma funcionalidade: 14754 (94.9 %); só da raiz e da infraestrutura: 794 em 40 arquivo(s) (`infraestrutura.md`). Linhas próprias somadas: 32133.

| Funcionalidade | Grupo | Próprias | Linhas | Alcance | Textos | Msgs | Workers | Testes | Estado no jogo |
|---|---|---|---|---|---|---|---|---|---|
| [editor:buildingTool](especificacoes/editor_buildingTool.md) | editor | 224 | 4818 | 988 | 0 | 10 | 0 | 11 | - |
| [editor:lotTool](especificacoes/editor_lotTool.md) | editor | 94 | 1488 | 512 | 0 | 0 | 0 | 6 | - |
| [editor:roadTool](especificacoes/editor_roadTool.md) | editor | 57 | 952 | 1602 | 0 | 0 | 0 | 2 | economy: portado 2; roads: portado 50, não se aplica 20, em parte 22, não portado 22; street-furniture: portado 2; traffic: já existia 2 |
| [editor:persistence](especificacoes/editor_persistence.md) | editor | 68 | 810 | 954 | 0 | 0 | 0 | 9 | economy: portado 2; interface: portado 106, não se aplica 38; landscape: portado 4 |
| [editor:growthFeed](especificacoes/editor_growthFeed.md) | editor | 41 | 543 | 664 | 0 | 0 | 0 | 1 | landscape: não se aplica 4 |
| [editor:transitTools](especificacoes/editor_transitTools.md) | editor | 31 | 409 | 291 | 0 | 5 | 0 | 0 | people: não se aplica 2; transit: portado 50, não se aplica 12 |
| [editor:terrainBrush](especificacoes/editor_terrainBrush.md) | editor | 23 | 311 | 209 | 0 | 0 | 0 | 1 | landscape: portado 48 |
| [editor:history](especificacoes/editor_history.md) | editor | 27 | 287 | 1366 | 0 | 0 | 0 | 13 | interface: já existia 52, não se aplica 4; landscape: já existia 4 |
| [editor:toolMachine](especificacoes/editor_toolMachine.md) | editor | 27 | 270 | 28 | 0 | 0 | 0 | 4 | - |
| [editor:fromReference](especificacoes/editor_fromReference.md) | editor | 10 | 228 | 97 | 0 | 0 | 0 | 1 | - |
| [editor:poles](especificacoes/editor_poles.md) | editor | 20 | 225 | 591 | 0 | 0 | 0 | 6 | street-furniture: portado 122 |
| [editor:bulldozer](especificacoes/editor_bulldozer.md) | editor | 15 | 222 | 819 | 0 | 0 | 0 | 1 | demolition: portado 2; economy: portado 2; street-furniture: portado 2 |
| [editor:barriers](especificacoes/editor_barriers.md) | editor | 18 | 221 | 151 | 0 | 0 | 0 | 2 | street-furniture: portado 38 |
| [editor:gameKeys](especificacoes/editor_gameKeys.md) | editor | 14 | 220 | 15 | 0 | 0 | 0 | 2 | interface: portado 20 |
| [editor:streetscapeTool](especificacoes/editor_streetscapeTool.md) | editor | 13 | 204 | 554 | 0 | 0 | 0 | 2 | roads: não se aplica 26; street-furniture: portado 28 |
| [editor:nodeMover](especificacoes/editor_nodeMover.md) | editor | 14 | 183 | 1425 | 0 | 0 | 0 | 0 | roads: portado 28 |
| [editor:cloudTool](especificacoes/editor_cloudTool.md) | editor | 10 | 142 | 121 | 0 | 0 | 0 | 0 | landscape: portado 22 |
| [editor:commit](especificacoes/editor_commit.md) | editor | 4 | 131 | 1216 | 0 | 0 | 0 | 20 | economy: portado 2; roads: em parte 90; traffic: de outra funcionalidade 2 |
| [editor:roadTop](especificacoes/editor_roadTop.md) | editor | 13 | 127 | 131 | 0 | 0 | 0 | 1 | roads: não se aplica 28 |
| [editor:streetObjects](especificacoes/editor_streetObjects.md) | editor | 5 | 90 | 1183 | 0 | 0 | 0 | 4 | roads: não se aplica 10; street-furniture: não se aplica 12; traffic: de outra funcionalidade 2 |
| [editor:buildings](especificacoes/editor_buildings.md) | editor | 7 | 79 | 435 | 0 | 0 | 0 | 13 | interior: outro porte 90, já existia 24 |
| [editor:blueprintLibrary](especificacoes/editor_blueprintLibrary.md) | editor | 11 | 73 | 159 | 0 | 0 | 0 | 1 | interior: outro porte 2 |
| [editor:roundabout](especificacoes/editor_roundabout.md) | editor | 6 | 71 | 901 | 0 | 0 | 0 | 2 | roads: não portado 14 |
| [editor:blocks](especificacoes/editor_blocks.md) | editor | 3 | 36 | 1203 | 0 | 0 | 0 | 0 | traffic: de outra funcionalidade 2 |
| [editor:cookedCities](especificacoes/editor_cookedCities.md) | editor | 8 | 36 | 91 | 0 | 0 | 0 | 1 | interface: não se aplica 16 |
| [editor:mapExport](especificacoes/editor_mapExport.md) | editor | 4 | 35 | 311 | 0 | 0 | 0 | 2 | - |
| [editor:roadParking](especificacoes/editor_roadParking.md) | editor | 8 | 32 | 11 | 0 | 0 | 0 | 0 | roads: portado 16; traffic: já existia 2 |
| [editor:growthReplica](especificacoes/editor_growthReplica.md) | editor | 9 | 23 | 676 | 0 | 0 | 0 | 1 | landscape: não se aplica 6 |
| [editor:preparedCity](especificacoes/editor_preparedCity.md) | editor | 5 | 22 | 258 | 0 | 0 | 0 | 15 | - |
| [editor:snap](especificacoes/editor_snap.md) | editor | 4 | 21 | 180 | 0 | 0 | 0 | 6 | roads: em parte 2, portado 36, não se aplica 4, não portado 10, já existia 2 |
| [editor:storeTurns](especificacoes/editor_storeTurns.md) | editor | 3 | 20 | 7 | 0 | 1 | 0 | 0 | - |
| [editor:editRules](especificacoes/editor_editRules.md) | editor | 1 | 15 | 1048 | 0 | 0 | 0 | 4 | economy: portado 2; roads: em parte 62 |
| [editor:smallCityMap](especificacoes/editor_smallCityMap.md) | editor | 3 | 9 | 2117 | 0 | 0 | 0 | 10 | landscape: já existia 6; people: não se aplica 2 |
| [editor:saveStore](especificacoes/editor_saveStore.md) | editor | 1 | 4 | 17 | 0 | 1 | 0 | 2 | interface: não se aplica 44 |
| [editor:buildingFloors](especificacoes/editor_buildingFloors.md) | editor | 0 | 0 | 189 | 0 | 0 | 0 | 1 | - |
| [editor:buildingPlans](especificacoes/editor_buildingPlans.md) | editor | 0 | 0 | 111 | 0 | 0 | 0 | 5 | - |
| [editor:buildingProfile](especificacoes/editor_buildingProfile.md) | editor | 0 | 0 | 83 | 0 | 0 | 0 | 1 | - |
| [editor:buildingRoofs](especificacoes/editor_buildingRoofs.md) | editor | 0 | 0 | 63 | 0 | 0 | 0 | 3 | - |
| [editor:buildingSnap](especificacoes/editor_buildingSnap.md) | editor | 0 | 0 | 207 | 0 | 0 | 0 | 3 | - |
| [editor:chunkedSave](especificacoes/editor_chunkedSave.md) | editor | 0 | 0 | 459 | 0 | 0 | 0 | 2 | interface: não se aplica 104; landscape: já existia 2; street-furniture: portado 1 |
| [editor:cityGenerator](especificacoes/editor_cityGenerator.md) | editor | 0 | 0 | 1175 | 0 | 0 | 0 | 28 | landscape: já existia 2; street-furniture: já existia 1 |
| [editor:editStream](especificacoes/editor_editStream.md) | editor | 0 | 0 | 14 | 0 | 0 | 0 | 0 | - |
| [editor:poleRoutes](especificacoes/editor_poleRoutes.md) | editor | 0 | 0 | 195 | 0 | 0 | 0 | 0 | street-furniture: portado 13 |
| [editor:regionFrames](especificacoes/editor_regionFrames.md) | editor | 0 | 0 | 151 | 0 | 0 | 0 | 1 | interface: não se aplica 9 |
| [editor:repair](especificacoes/editor_repair.md) | editor | 0 | 0 | 693 | 0 | 0 | 0 | 0 | interface: já existia 3 |
| [editor:roadPath](especificacoes/editor_roadPath.md) | editor | 0 | 0 | 20 | 0 | 0 | 0 | 3 | roads: portado 9 |
| [editor:roads-economy](especificacoes/editor_roads-economy.md) | editor | 0 | 0 | 149 | 0 | 0 | 0 | 1 | economy: portado 1; roads: não portado 3 |
| [editor:roads-powerLine](especificacoes/editor_roads-powerLine.md) | editor | 0 | 0 | 578 | 0 | 0 | 0 | 3 | people: não se aplica 1; roads: não se aplica 12; street-furniture: parcial 1 |
| [editor:roads-profile](especificacoes/editor_roads-profile.md) | editor | 0 | 0 | 170 | 0 | 0 | 0 | 2 | roads: não portado 2; traffic: de outra funcionalidade 1 |
| [editor:roads-streetKit](especificacoes/editor_roads-streetKit.md) | editor | 0 | 0 | 965 | 0 | 0 | 0 | 1 | demolition: já existia 1; roads: não se aplica 9; street-furniture: portado 1 |
| [editor:saveBinary](especificacoes/editor_saveBinary.md) | editor | 0 | 0 | 23 | 0 | 0 | 0 | 1 | interface: já existia 23 |
| [editor:workerParts](especificacoes/editor_workerParts.md) | editor | 0 | 0 | 20 | 0 | 1 | 0 | 1 | interface: não se aplica 8 |
| [ui:v2](especificacoes/ui_v2.md) | interface | 34 | 2201 | 517 | 205 | 0 | 0 | 1 | interface: portado 174, outra sessão 66, não se aplica 12; economy: já existia 2; landscape: portado 8; street-furniture: portado 2; transit: portado 4 |
| [ui:overlay](especificacoes/ui_overlay.md) | interface | 33 | 516 | 201 | 0 | 0 | 0 | 2 | - |
| [ui:inspector](especificacoes/ui_inspector.md) | interface | 32 | 451 | 509 | 57 | 0 | 0 | 0 | interface: portado 50, outra sessão 28; roads: não se aplica 12; traffic: portado 2 |
| [ui:healthWatch](especificacoes/ui_healthWatch.md) | interface | 22 | 299 | 43 | 12 | 0 | 0 | 7 | interface: portado 36, não se aplica 2 |
| [ui:builder](especificacoes/ui_builder.md) | interface | 5 | 282 | 99 | 1 | 0 | 0 | 2 | interface: outra sessão 8; interior: portado 6, outro porte 10 |
| [ui:healthPanel](especificacoes/ui_healthPanel.md) | interface | 10 | 264 | 31 | 10 | 0 | 0 | 3 | interface: portado 43 |
| [ui:effectsPanel](especificacoes/ui_effectsPanel.md) | interface | 7 | 170 | 17 | 6 | 0 | 0 | 0 | interface: portado 24 |
| [ui:profilerHud](especificacoes/ui_profilerHud.md) | interface | 25 | 161 | 33 | 0 | 0 | 0 | 1 | interface: portado 22 |
| [ui:graphicsPanel](especificacoes/ui_graphicsPanel.md) | interface | 7 | 103 | 17 | 18 | 0 | 0 | 1 | interface: portado 22 |
| [ui:roadSwatch](especificacoes/ui_roadSwatch.md) | interface | 11 | 85 | 33 | 0 | 0 | 0 | 0 | roads: não portado 24 |
| [ui:thunder](especificacoes/ui_thunder.md) | interface | 6 | 66 | 6 | 0 | 0 | 0 | 0 | interface: portado 12 |
| [ui:toolChoices](especificacoes/ui_toolChoices.md) | interface | 19 | 65 | 98 | 0 | 0 | 0 | 0 | interface: outra sessão 136, portado 24; interior: outro porte 30; landscape: não portado 28; street-furniture: portado 2 |
| [ui:i18n](especificacoes/ui_i18n.md) | interface | 4 | 26 | 24 | 0 | 0 | 0 | 11 | interface: portado 44, outra sessão 2; build: não se aplica 2 |
| [ui:about](especificacoes/ui_about.md) | interface | 2 | 18 | 20 | 0 | 0 | 0 | 0 | interface: portado 6 |
| [ui:roads](especificacoes/ui_roads.md) | interface | 4 | 13 | 449 | 116 | 5 | 0 | 7 | economy: já existia 1, portado 2; interface: outra sessão 153; interior: outro porte 21, não se aplica 1; roads: em parte 11, não portado 196; street-furniture: |
| [ui:chrome](especificacoes/ui_chrome.md) | interface | 3 | 9 | 3 | 0 | 0 | 0 | 0 | interface: já existia 6 |
| [ui:labels](especificacoes/ui_labels.md) | interface | 3 | 9 | 31 | 0 | 0 | 0 | 0 | interface: portado 12; interior: outro porte 4 |
| [ui:buildStamp](especificacoes/ui_buildStamp.md) | interface | 0 | 0 | 16 | 1 | 0 | 0 | 0 | interface: portado 5, não se aplica 3 |
| [ui:materialSwatch](especificacoes/ui_materialSwatch.md) | interface | 0 | 0 | 8 | 0 | 0 | 0 | 0 | interface: outra sessão 7 |
| [ui:planSwatch](especificacoes/ui_planSwatch.md) | interface | 0 | 0 | 4 | 0 | 0 | 0 | 0 | interface: outra sessão 3 |
| [ui:roadSectionEditor](especificacoes/ui_roadSectionEditor.md) | interface | 0 | 0 | 38 | 7 | 0 | 0 | 1 | interface: outra sessão 3; roads: não portado 5 |
| [people:wardrobe](especificacoes/people_wardrobe.md) | people | 1 | 171 | 17 | 0 | 0 | 0 | 0 | people: já existia 22 |
| [people:body](especificacoes/people_body.md) | people | 1 | 3 | 65 | 0 | 0 | 0 | 8 | people: já existia 111; traffic: de outra funcionalidade 2 |
| [people:hair](especificacoes/people_hair.md) | people | 0 | 0 | 23 | 0 | 0 | 0 | 2 | people: já existia 43; traffic: de outra funcionalidade 1 |
| [people:phenotype](especificacoes/people_phenotype.md) | people | 0 | 0 | 5 | 0 | 0 | 0 | 0 | people: já existia 5 |
| [people:roster](especificacoes/people_roster.md) | people | 0 | 0 | 52 | 0 | 0 | 0 | 0 | people: já existia 18 |
| [people:spec](especificacoes/people_spec.md) | people | 0 | 0 | 54 | 0 | 0 | 0 | 5 | people: já existia 32 |
| [src:conflict](especificacoes/src_conflict.md) | raiz | 15 | 213 | 109 | 0 | 6 | 0 | 0 | people: não se aplica 2; traffic: não se aplica 2 |
| [src:growth](especificacoes/src_growth.md) | raiz | 15 | 139 | 680 | 0 | 1 | 0 | 0 | - |
| [src:bay](especificacoes/src_bay.md) | raiz | 9 | 71 | 101 | 0 | 1 | 0 | 0 | traffic: não se aplica 2 |
| [render:renderer](especificacoes/render_renderer.md) | render | 43 | 4216 | 7354 | 0 | 0 | 0 | 4 | demolition: portado 2; landscape: portado 108; people: já existia 2; street-furniture: portado 2; traffic: já existia 2; transit: portado 2; interface: portado  |
| [render:bake](especificacoes/render_bake.md) | render | 34 | 814 | 136 | 0 | 1 | 0 | 3 | landscape: portado 58; roads: não se aplica 40; people: não se aplica 26; traffic: não se aplica 1 |
| [render:buildings](especificacoes/render_buildings.md) | render | 52 | 765 | 2476 | 0 | 16 | 0 | 57 | demolition: não portado (pendência) 3, não se aplica 3; interior: portado 14, outro porte 135, não se aplica 8; landscape: já existia 12; street-furniture: não  |
| [render:pole](especificacoes/render_pole.md) | render | 48 | 378 | 668 | 0 | 0 | 0 | 4 | street-furniture: portado 230, já existia 64 |
| [render:transit](especificacoes/render_transit.md) | render | 25 | 209 | 260 | 0 | 0 | 0 | 2 | transit: não se aplica 20, portado 98 |
| [render:inventory](especificacoes/render_inventory.md) | render | 10 | 159 | 10 | 0 | 0 | 0 | 0 | - |
| [render:barriers](especificacoes/render_barriers.md) | render | 20 | 152 | 202 | 0 | 0 | 0 | 0 | street-furniture: parcial 64 |
| [render:road](especificacoes/render_road.md) | render | 17 | 132 | 1209 | 0 | 1 | 0 | 13 | roads: não se aplica 405, não portado 3; people: já existia 5; traffic: portado 2 |
| [render:sign](especificacoes/render_sign.md) | render | 19 | 128 | 587 | 0 | 0 | 0 | 1 | people: já existia 2; street-furniture: portado 96 |
| [render:quality](especificacoes/render_quality.md) | render | 27 | 118 | 40 | 0 | 0 | 0 | 7 | interface: portado 62, não se aplica 10 |
| [render:surface](especificacoes/render_surface.md) | render | 16 | 111 | 38 | 0 | 0 | 0 | 5 | roads: não se aplica 62; build: não se aplica 2 |
| [render:garden](especificacoes/render_garden.md) | render | 3 | 110 | 131 | 0 | 0 | 0 | 2 | - |
| [render:profiler](especificacoes/render_profiler.md) | render | 14 | 100 | 22 | 0 | 0 | 0 | 1 | - |
| [render:street](especificacoes/render_street.md) | render | 12 | 63 | 714 | 0 | 9 | 0 | 2 | people: já existia 2; roads: não se aplica 102; street-furniture: portado 94, já existia 10 |
| [render:people](especificacoes/render_people.md) | render | 7 | 57 | 653 | 0 | 1 | 0 | 17 | people: já existia 567, portado 169, não se aplica 18; traffic: de outra funcionalidade 17 |
| [render:uploads](especificacoes/render_uploads.md) | render | 9 | 57 | 83 | 0 | 0 | 0 | 9 | build: a portar 4; interior: outro porte 4 |
| [render:optimizations](especificacoes/render_optimizations.md) | render | 7 | 45 | 58 | 0 | 0 | 0 | 0 | interface: não se aplica 19, portado 63 |
| [render:ragdoll](especificacoes/render_ragdoll.md) | render | 5 | 36 | 166 | 0 | 0 | 0 | 4 | demolition: já existia 2, não portado (pendência) 1, não se aplica 2; people: não se aplica 1; traffic: de outra funcionalidade 1 |
| [render:gardens](especificacoes/render_gardens.md) | render | 2 | 30 | 348 | 0 | 4 | 0 | 8 | landscape: já existia 14; street-furniture: não se aplica 2 |
| [render:mesh](especificacoes/render_mesh.md) | render | 5 | 24 | 263 | 0 | 0 | 0 | 18 | landscape: portado 7, não se aplica 2; roads: não se aplica 150; build: a portar 1; traffic: não se aplica 1 |
| [render:far](especificacoes/render_far.md) | render | 3 | 20 | 239 | 0 | 0 | 0 | 5 | landscape: portado 98, não se aplica 7 |
| [render:ground](especificacoes/render_ground.md) | render | 3 | 19 | 578 | 0 | 1 | 0 | 8 | build: não se aplica 6; landscape: já existia 32, portado 28, não se aplica 44; people: já existia 1; traffic: já existia 1 |
| [render:network](especificacoes/render_network.md) | render | 4 | 19 | 195 | 0 | 1 | 0 | 1 | roads: não se aplica 96 |
| [render:optimization](especificacoes/render_optimization.md) | render | 1 | 18 | 4 | 0 | 0 | 0 | 0 | - |
| [render:terrain](especificacoes/render_terrain.md) | render | 2 | 17 | 553 | 0 | 1 | 0 | 16 | landscape: portado em parte 154, já existia 5, não se aplica 118 |
| [render:vehicle](especificacoes/render_vehicle.md) | render | 2 | 17 | 144 | 0 | 0 | 0 | 7 | traffic: já existia 4, portado 1, não portado 1; people: já existia 2 |
| [render:cell](especificacoes/render_cell.md) | render | 3 | 15 | 111 | 0 | 0 | 0 | 12 | landscape: já existia 6; street-furniture: já existia 12 |
| [render:cover](especificacoes/render_cover.md) | render | 3 | 15 | 78 | 0 | 0 | 0 | 1 | landscape: portado em parte 22 |
| [render:agents](especificacoes/render_agents.md) | render | 4 | 13 | 1624 | 0 | 0 | 0 | 8 | demolition: portado 2; landscape: já existia 6; people: não se aplica 2; street-furniture: não se aplica 2; traffic: portado 2 |
| [render:gpu](especificacoes/render_gpu.md) | render | 2 | 9 | 80 | 0 | 0 | 0 | 7 | landscape: não se aplica 5; build: a portar 2; interior: outro porte 2 |
| [render:instance](especificacoes/render_instance.md) | render | 1 | 8 | 100 | 0 | 0 | 0 | 4 | landscape: já existia 4; street-furniture: já existia 3 |
| [render:plate](especificacoes/render_plate.md) | render | 1 | 8 | 49 | 0 | 0 | 0 | 4 | landscape: não se aplica 22; roads: não portado 4 |
| [render:rain](especificacoes/render_rain.md) | render | 1 | 8 | 12 | 0 | 0 | 0 | 1 | landscape: portado 26 |
| [render:sun](especificacoes/render_sun.md) | render | 3 | 8 | 43 | 0 | 0 | 0 | 3 | interface: outra sessão 4; landscape: portado em parte 2, não se aplica 148 |
| [render:derived](especificacoes/render_derived.md) | render | 2 | 6 | 16 | 0 | 1 | 0 | 1 | build: não se aplica 2; interior: não se aplica 1 |
| [render:signs](especificacoes/render_signs.md) | render | 2 | 6 | 165 | 0 | 0 | 0 | 3 | street-furniture: portado 112, já existia 6 |
| [render:citizen](especificacoes/render_citizen.md) | render | 1 | 5 | 346 | 0 | 0 | 0 | 14 | people: não se aplica 287; traffic: de outra funcionalidade 6 |
| [render:candidate](especificacoes/render_candidate.md) | render | 1 | 3 | 19 | 0 | 0 | 0 | 2 | - |
| [render:effect](especificacoes/render_effect.md) | render | 1 | 3 | 17 | 0 | 0 | 0 | 0 | interface: portado 24, não se aplica 14 |
| [render:flat](especificacoes/render_flat.md) | render | 1 | 3 | 953 | 0 | 1 | 0 | 6 | landscape: não se aplica 84; street-furniture: não se aplica 2 |
| [render:lot](especificacoes/render_lot.md) | render | 2 | 2 | 128 | 0 | 0 | 0 | 2 | - |
| [render:scenery](especificacoes/render_scenery.md) | render | 1 | 1 | 164 | 0 | 0 | 0 | 11 | build: não se aplica 2; landscape: já existia 58; street-furniture: parcial 2 |
| [render:water](especificacoes/render_water.md) | render | 1 | 1 | 33 | 0 | 0 | 0 | 1 | landscape: portado 68 |
| [render:blast](especificacoes/render_blast.md) | render | 0 | 0 | 28 | 0 | 0 | 0 | 1 | demolition: não portado (pendência) 1; traffic: de outra funcionalidade 1 |
| [render:blob](especificacoes/render_blob.md) | render | 0 | 0 | 8 | 0 | 0 | 0 | 0 | people: portado 1; traffic: de outra funcionalidade 1 |
| [render:building](especificacoes/render_building.md) | render | 0 | 0 | 95 | 0 | 0 | 0 | 0 | - |
| [render:canyon](especificacoes/render_canyon.md) | render | 0 | 0 | 19 | 0 | 0 | 0 | 1 | interface: outra sessão 18, portado 1 |
| [render:car](especificacoes/render_car.md) | render | 0 | 0 | 73 | 0 | 0 | 0 | 4 | people: já existia 1; traffic: já existia 1 |
| [render:casualties](especificacoes/render_casualties.md) | render | 0 | 0 | 12 | 0 | 0 | 0 | 1 | demolition: portado 1 |
| [render:change](especificacoes/render_change.md) | render | 0 | 0 | 13 | 0 | 0 | 0 | 0 | - |
| [render:clearing](especificacoes/render_clearing.md) | render | 0 | 0 | 18 | 0 | 0 | 0 | 2 | landscape: portado 9 |
| [render:cloud](especificacoes/render_cloud.md) | render | 0 | 0 | 7 | 0 | 0 | 0 | 1 | landscape: não se aplica 8 |
| [render:density](especificacoes/render_density.md) | render | 0 | 0 | 241 | 0 | 0 | 0 | 3 | traffic: não se aplica 1 |
| [render:depth](especificacoes/render_depth.md) | render | 0 | 0 | 3 | 0 | 0 | 0 | 0 | - |
| [render:destruction](especificacoes/render_destruction.md) | render | 0 | 0 | 333 | 0 | 1 | 0 | 0 | demolition: portado 1; traffic: de outra funcionalidade 1 |
| [render:ecology](especificacoes/render_ecology.md) | render | 0 | 0 | 142 | 0 | 1 | 0 | 1 | landscape: não se aplica 19, já existia 16 |
| [render:elements](especificacoes/render_elements.md) | render | 0 | 0 | 124 | 0 | 0 | 0 | 1 | demolition: não se aplica 1; landscape: portado 29; traffic: de outra funcionalidade 1 |
| [render:environment](especificacoes/render_environment.md) | render | 0 | 0 | 57 | 0 | 0 | 0 | 0 | interface: outra sessão 2; landscape: portado 34 |
| [render:exhaust](especificacoes/render_exhaust.md) | render | 0 | 0 | 10 | 0 | 0 | 0 | 4 | demolition: portado 1; traffic: de outra funcionalidade 1 |
| [render:fog](especificacoes/render_fog.md) | render | 0 | 0 | 28 | 0 | 0 | 0 | 1 | landscape: portado 18 |
| [render:frustum](especificacoes/render_frustum.md) | render | 0 | 0 | 6 | 0 | 0 | 0 | 2 | - |
| [render:gore](especificacoes/render_gore.md) | render | 0 | 0 | 15 | 0 | 0 | 0 | 1 | demolition: não portado (pendência) 1 |
| [render:grass](especificacoes/render_grass.md) | render | 0 | 0 | 34 | 0 | 0 | 0 | 1 | landscape: portado em parte 13; roads: não portado 12; street-furniture: portado 13 |
| [render:impostor](especificacoes/render_impostor.md) | render | 0 | 0 | 19 | 0 | 0 | 0 | 0 | landscape: portado 15 |
| [render:inspector](especificacoes/render_inspector.md) | render | 0 | 0 | 3 | 0 | 0 | 0 | 0 | - |
| [render:interior](especificacoes/render_interior.md) | render | 0 | 0 | 7 | 0 | 0 | 0 | 1 | traffic: já existia 1 |
| [render:iso](especificacoes/render_iso.md) | render | 0 | 0 | 50 | 0 | 0 | 0 | 9 | interface: portado 36 |
| [render:land](especificacoes/render_land.md) | render | 0 | 0 | 140 | 0 | 0 | 0 | 1 | landscape: em parte 1, não se aplica 10, portado 1 |
| [render:light](especificacoes/render_light.md) | render | 0 | 0 | 7 | 0 | 0 | 0 | 0 | landscape: já existia 8; street-furniture: já existia 7, portado 1; traffic: já existia 1 |
| [render:lightning](especificacoes/render_lightning.md) | render | 0 | 0 | 5 | 0 | 0 | 0 | 0 | landscape: portado 13 |
| [render:low](especificacoes/render_low.md) | render | 0 | 0 | 76 | 0 | 0 | 0 | 3 | landscape: já existia 70 |
| [render:markings](especificacoes/render_markings.md) | render | 0 | 0 | 36 | 0 | 0 | 0 | 1 | roads: não portado 2 |
| [render:materials](especificacoes/render_materials.md) | render | 0 | 0 | 52 | 0 | 0 | 0 | 10 | landscape: já existia 3 |
| [render:memory](especificacoes/render_memory.md) | render | 0 | 0 | 9 | 0 | 0 | 0 | 3 | - |
| [render:nature](especificacoes/render_nature.md) | render | 0 | 0 | 148 | 0 | 0 | 0 | 4 | landscape: já existia 4, portado 33 |
| [render:occlusion](especificacoes/render_occlusion.md) | render | 0 | 0 | 35 | 0 | 0 | 0 | 1 | interface: portado 6, não se aplica 25 |
| [render:occupants](especificacoes/render_occupants.md) | render | 0 | 0 | 32 | 0 | 0 | 0 | 1 | people: já existia 1; traffic: já existia 1 |
| [render:page](especificacoes/render_page.md) | render | 0 | 0 | 5 | 0 | 0 | 0 | 0 | people: já existia 1; traffic: não se aplica 1 |
| [render:paint](especificacoes/render_paint.md) | render | 0 | 0 | 324 | 0 | 0 | 0 | 0 | roads: não portado 10; traffic: portado 1 |
| [render:part](especificacoes/render_part.md) | render | 0 | 0 | 16 | 0 | 0 | 0 | 1 | people: já existia 1 |
| [render:pinned](especificacoes/render_pinned.md) | render | 0 | 0 | 1 | 0 | 0 | 0 | 0 | - |
| [render:plant](especificacoes/render_plant.md) | render | 0 | 0 | 185 | 0 | 0 | 0 | 3 | landscape: já existia 18, portado 40; street-furniture: já existia 1 |
| [render:play](especificacoes/render_play.md) | render | 0 | 0 | 1543 | 0 | 0 | 0 | 1 | demolition: não se aplica 1; people: já existia 1; street-furniture: não se aplica 1; traffic: de outra funcionalidade 1 |
| [render:postprocess](especificacoes/render_postprocess.md) | render | 0 | 0 | 70 | 0 | 0 | 0 | 2 | landscape: portado 9 |
| [render:prop](especificacoes/render_prop.md) | render | 0 | 0 | 286 | 0 | 0 | 0 | 3 | landscape: já existia 29; roads: não portado 1; street-furniture: já existia 44, portado 27, não se aplica 8 |
| [render:reveal](especificacoes/render_reveal.md) | render | 0 | 0 | 11 | 0 | 0 | 0 | 1 | interface: portado 11 |
| [render:rider](especificacoes/render_rider.md) | render | 0 | 0 | 58 | 0 | 0 | 0 | 2 | people: pendente 63 |
| [render:rigged](especificacoes/render_rigged.md) | render | 0 | 0 | 709 | 0 | 0 | 0 | 6 | people: não se aplica 71; traffic: de outra funcionalidade 1 |
| [render:roads](especificacoes/render_roads.md) | render | 0 | 0 | 3 | 0 | 0 | 0 | 1 | roads: não se aplica 3 |
| [render:room](especificacoes/render_room.md) | render | 0 | 0 | 244 | 0 | 0 | 0 | 0 | - |
| [render:screen](especificacoes/render_screen.md) | render | 0 | 0 | 2 | 0 | 0 | 0 | 1 | - |
| [render:shader](especificacoes/render_shader.md) | render | 0 | 0 | 39 | 0 | 0 | 0 | 1 | - |
| [render:shadow](especificacoes/render_shadow.md) | render | 0 | 0 | 6 | 0 | 0 | 0 | 1 | landscape: portado em parte 1 |
| [render:shared](especificacoes/render_shared.md) | render | 0 | 0 | 51 | 0 | 0 | 0 | 4 | build: não se aplica 1; landscape: não se aplica 6 |
| [render:signals](especificacoes/render_signals.md) | render | 0 | 0 | 348 | 0 | 0 | 0 | 1 | landscape: já existia 2; street-furniture: já existia 63; traffic: já existia 1 |
| [render:staged](especificacoes/render_staged.md) | render | 0 | 0 | 13 | 0 | 0 | 0 | 1 | landscape: não se aplica 3 |
| [render:still](especificacoes/render_still.md) | render | 0 | 0 | 4 | 0 | 0 | 0 | 1 | landscape: não se aplica 7 |
| [render:structure](especificacoes/render_structure.md) | render | 0 | 0 | 244 | 0 | 0 | 0 | 0 | road-elevation: a portar 11 |
| [render:structures](especificacoes/render_structures.md) | render | 0 | 0 | 219 | 0 | 0 | 0 | 2 | economy: não se aplica 1; road-elevation: a portar 66 |
| [render:target](especificacoes/render_target.md) | render | 0 | 0 | 4 | 0 | 0 | 0 | 2 | - |
| [render:torus](especificacoes/render_torus.md) | render | 0 | 0 | 11 | 0 | 0 | 0 | 0 | landscape: portado 2 |
| [render:utilities](especificacoes/render_utilities.md) | render | 0 | 0 | 183 | 0 | 0 | 0 | 2 | people: já existia 1; street-furniture: portado 50 |
| [render:wear](especificacoes/render_wear.md) | render | 0 | 0 | 22 | 0 | 0 | 0 | 2 | - |
| [render:wind](especificacoes/render_wind.md) | render | 0 | 0 | 25 | 0 | 0 | 0 | 5 | landscape: portado 22; street-furniture: parcial 5 |
| [render:woods](especificacoes/render_woods.md) | render | 0 | 0 | 14 | 0 | 0 | 0 | 3 | landscape: já existia 10 |
| [sim:meso](especificacoes/sim_meso.md) | sim | 279 | 2273 | 933 | 0 | 0 | 0 | 8 | people: já existia 60; traffic: já existia 8, não se aplica 4 |
| [sim:agents](especificacoes/sim_agents.md) | sim | 52 | 376 | 2833 | 0 | 0 | 0 | 35 | traffic: portado 15, não se aplica 4, já existia 5, de outra funcionalidade 1; people: não se aplica 41, portado 1, já existia 398; transit: portado 3, não se a |
| [sim:ambient](especificacoes/sim_ambient.md) | sim | 5 | 21 | 1727 | 0 | 0 | 0 | 7 | demolition: não se aplica 2; people: não se aplica 346, portado 114; traffic: portado 3, não se aplica 3, de outra funcionalidade 2 |
| [sim:routing](especificacoes/sim_routing.md) | sim | 2 | 19 | 234 | 0 | 0 | 0 | 13 | traffic: já existia 8 |
| [sim:world](especificacoes/sim_world.md) | sim | 5 | 19 | 1521 | 0 | 0 | 0 | 75 | demolition: já existia 2; people: já existia 12; traffic: já existia 2; transit: já existia 2 |
| [sim:pose](especificacoes/sim_pose.md) | sim | 2 | 11 | 99 | 0 | 0 | 0 | 25 | demolition: já existia 3; people: já existia 4; traffic: portado 2; roads: não se aplica 2 |
| [sim:intersections](especificacoes/sim_intersections.md) | sim | 3 | 9 | 839 | 0 | 0 | 0 | 10 | people: já existia 2; traffic: já existia 6; transit: portado 1 |
| [sim:vehicles](especificacoes/sim_vehicles.md) | sim | 2 | 5 | 901 | 0 | 0 | 0 | 43 | traffic: portado 6, já existia 9; people: já existia 2, não se aplica 7; transit: portado 1 |
| [sim:transit](especificacoes/sim_transit.md) | sim | 1 | 4 | 693 | 0 | 0 | 0 | 1 | demolition: não se aplica 2; people: já existia 8; traffic: de outra funcionalidade 2; transit: portado 106, não se aplica 14 |
| [sim:city](especificacoes/sim_city.md) | sim | 1 | 3 | 1621 | 0 | 0 | 0 | 0 | people: já existia 2; traffic: já existia 2; transit: portado 2 |
| [sim:clock](especificacoes/sim_clock.md) | sim | 1 | 3 | 17 | 0 | 0 | 0 | 1 | - |
| [sim:population](especificacoes/sim_population.md) | sim | 1 | 3 | 21 | 0 | 0 | 0 | 1 | traffic: já existia 2 |
| [sim:vehicle](especificacoes/sim_vehicle.md) | sim | 1 | 3 | 86 | 0 | 0 | 0 | 1 | traffic: já existia 2 |
| [sim:params](especificacoes/sim_params.md) | sim | 2 | 2 | 38 | 0 | 0 | 0 | 47 | traffic: já existia 2 |
| [sim:host](especificacoes/sim_host.md) | sim | 1 | 1 | 4694 | 0 | 22 | 0 | 16 | traffic: não se aplica 6; demolition: não se aplica 3; people: já existia 3; interface: outra sessão 1, portado 1; roads: não se aplica 1; transit: portado 1 |
| [sim:pipeline](especificacoes/sim_pipeline.md) | sim | 1 | 1 | 3010 | 0 | 0 | 0 | 57 | people: já existia 2; traffic: já existia 2 |
| [sim:audit](especificacoes/sim_audit.md) | sim | 0 | 0 | 3 | 0 | 0 | 0 | 1 | - |
| [sim:crossings](especificacoes/sim_crossings.md) | sim | 0 | 0 | 363 | 0 | 0 | 0 | 1 | demolition: não se aplica 2; people: já existia 2; traffic: já existia 2 |
| [sim:drive](especificacoes/sim_drive.md) | sim | 0 | 0 | 149 | 0 | 0 | 0 | 4 | traffic: já existia 3 |
| [sim:dt](especificacoes/sim_dt.md) | sim | 0 | 0 | 1 | 0 | 0 | 0 | 0 | - |
| [sim:invariants](especificacoes/sim_invariants.md) | sim | 0 | 0 | 386 | 0 | 0 | 0 | 1 | people: já existia 1; traffic: já existia 1 |
| [sim:peds](especificacoes/sim_peds.md) | sim | 0 | 0 | 337 | 0 | 0 | 0 | 2 | demolition: já existia 2; people: já existia 2; traffic: já existia 2; street-furniture: já existia 1 |
| [sim:people](especificacoes/sim_people.md) | sim | 0 | 0 | 33 | 0 | 0 | 0 | 2 | demolition: portado 1; people: não se aplica 1, já existia 17; traffic: de outra funcionalidade 2, portado 1 |
| [sim:roads](especificacoes/sim_roads.md) | sim | 0 | 0 | 352 | 0 | 0 | 0 | 2 | traffic: não portado 2, portado 1 |
| [sim:signals](especificacoes/sim_signals.md) | sim | 0 | 0 | 85 | 0 | 0 | 0 | 9 | traffic: portado 1, já existia 3 |
| [sim:topology](especificacoes/sim_topology.md) | sim | 0 | 0 | 1448 | 0 | 0 | 0 | 1 | traffic: não se aplica 1 |
| [view:camera](especificacoes/view_camera.md) | view | 81 | 712 | 119 | 0 | 0 | 0 | 6 | interface: portado 198, não se aplica 6; roads: não se aplica 2 |
| [view:viewport](especificacoes/view_viewport.md) | view | 1 | 53 | 16 | 0 | 0 | 0 | 0 | build: não se aplica 2; interface: portado 28; interior: não se aplica 2 |
| [worker:job](especificacoes/worker_job.md) | worker | 21 | 163 | 139 | 0 | 5 | 1 | 4 | build: não se aplica 2; interior: não se aplica 2; landscape: já existia 10; roads: não se aplica 4 |
| [worker:conflictTable](especificacoes/worker_conflictTable.md) | worker | 1 | 152 | 387 | 0 | 1 | 0 | 0 | people: não se aplica 2; traffic: não se aplica 2 |
| [worker:citizenBake](especificacoes/worker_citizenBake.md) | worker | 10 | 111 | 353 | 0 | 8 | 0 | 0 | people: não se aplica 28 |
| [worker:roadTile](especificacoes/worker_roadTile.md) | worker | 8 | 92 | 307 | 0 | 1 | 0 | 0 | - |
| [worker:derivedWriter](especificacoes/worker_derivedWriter.md) | worker | 4 | 52 | 12 | 0 | 1 | 0 | 1 | build: não se aplica 2; interior: não se aplica 1 |
| [worker:ecology](especificacoes/worker_ecology.md) | worker | 1 | 30 | 128 | 0 | 1 | 0 | 0 | landscape: já existia 4 |
| [worker:networkBake](especificacoes/worker_networkBake.md) | worker | 4 | 28 | 1246 | 0 | 2 | 0 | 1 | - |
| [worker:garmentPadding](especificacoes/worker_garmentPadding.md) | worker | 1 | 28 | 5 | 0 | 1 | 0 | 0 | people: já existia 6 |
| [worker:flatGround](especificacoes/worker_flatGround.md) | worker | 1 | 23 | 52 | 0 | 1 | 0 | 0 | landscape: não se aplica 6 |
| [worker:cityGen](especificacoes/worker_cityGen.md) | worker | 3 | 22 | 2117 | 0 | 2 | 0 | 0 | - |
| [worker:bays](especificacoes/worker_bays.md) | worker | 1 | 20 | 177 | 0 | 1 | 0 | 0 | traffic: não se aplica 2 |
| [worker:terrainLight](especificacoes/worker_terrainLight.md) | worker | 1 | 20 | 8 | 0 | 1 | 0 | 0 | landscape: não se aplica 4 |
| [worker:groundShape](especificacoes/worker_groundShape.md) | worker | 1 | 17 | 419 | 0 | 1 | 0 | 0 | landscape: não se aplica 4 |
| [worker:cell](especificacoes/worker_cell.md) | worker | 1 | 16 | 1232 | 0 | 1 | 0 | 0 | - |
| [worker:fracture](especificacoes/worker_fracture.md) | worker | 1 | 15 | 27 | 0 | 1 | 0 | 0 | demolition: não portado (pendência) 2 |
| [worker:networkFarRoads](especificacoes/worker_networkFarRoads.md) | worker | 1 | 14 | 32 | 0 | 1 | 0 | 0 | - |
| [worker:save](especificacoes/worker_save.md) | worker | 2 | 13 | 21 | 0 | 1 | 0 | 0 | - |
| [worker:growth](especificacoes/worker_growth.md) | worker | 1 | 13 | 663 | 0 | 2 | 0 | 0 | - |
| [worker:landStroke](especificacoes/worker_landStroke.md) | worker | 1 | 13 | 53 | 0 | 1 | 0 | 1 | - |
| [worker:roadStations](especificacoes/worker_roadStations.md) | worker | 1 | 13 | 74 | 0 | 1 | 0 | 0 | - |
| [worker:sim](especificacoes/worker_sim.md) | worker | 4 | 9 | 4697 | 0 | 7 | 1 | 14 | demolition: não se aplica 1; interface: outra sessão 1, portado 1; people: já existia 1; roads: não se aplica 1; traffic: não se aplica 1 |
| [world:doc](especificacoes/world_doc.md) | world | 63 | 651 | 1184 | 0 | 0 | 0 | 277 | build: a portar 8; economy: já existia 2; interface: já existia 37; landscape: portado 134; people: já existia 2; roads: não portado 34; street-furniture: porta |
| [world:buildings](especificacoes/world_buildings.md) | world | 84 | 441 | 1225 | 0 | 0 | 0 | 140 | interior: outro porte 37, já existia 3; interface: portado 14, outra sessão 6; landscape: já existia 12; traffic: já existia 3, de outra funcionalidade 1; marco |
| [world:cityGen](especificacoes/world_cityGen.md) | world | 38 | 186 | 199 | 0 | 0 | 0 | 34 | landscape: já existia 10; roads: não portado 165 |
| [world:zone](especificacoes/world_zone.md) | world | 12 | 178 | 204 | 0 | 0 | 0 | 5 | street-furniture: não se aplica 2 |
| [world:junction](especificacoes/world_junction.md) | world | 35 | 118 | 721 | 0 | 0 | 0 | 8 | roads: não portado 273, não se aplica 98; people: já existia 2; street-furniture: não se aplica 2; road-elevation: a portar 98; traffic: já existia 3 |
| [world:growth](especificacoes/world_growth.md) | world | 8 | 83 | 700 | 0 | 5 | 0 | 38 | - |
| [world:road](especificacoes/world_road.md) | world | 7 | 61 | 191 | 0 | 0 | 0 | 55 | road-elevation: a portar 20; roads: não portado 168, em parte 70; traffic: já existia 2 |
| [world:turn](especificacoes/world_turn.md) | world | 5 | 47 | 271 | 0 | 0 | 0 | 5 | traffic: já existia 3, não se aplica 2; roads: não se aplica 44 |
| [world:lots](especificacoes/world_lots.md) | world | 3 | 32 | 561 | 0 | 0 | 0 | 55 | people: já existia 2 |
| [world:surfaces](especificacoes/world_surfaces.md) | world | 4 | 32 | 101 | 0 | 0 | 0 | 8 | roads: não portado 24; street-furniture: não se aplica 4 |
| [world:terrain](especificacoes/world_terrain.md) | world | 2 | 31 | 64 | 0 | 0 | 0 | 24 | interface: outra sessão 4; landscape: portado 109 |
| [world:roads](especificacoes/world_roads.md) | world | 3 | 26 | 533 | 0 | 0 | 0 | 24 | economy: não se aplica 2; roads: não se aplica 33, portado 12, não portado 86, em parte 34; interface: outra sessão 25; interior: outro porte 6; traffic: já exi |
| [world:tiles](especificacoes/world_tiles.md) | world | 3 | 22 | 20 | 0 | 0 | 0 | 15 | - |
| [world:utilities](especificacoes/world_utilities.md) | world | 2 | 22 | 41 | 0 | 0 | 0 | 4 | street-furniture: portado 68 |
| [world:walking](especificacoes/world_walking.md) | world | 1 | 20 | 115 | 0 | 0 | 0 | 2 | people: já existia 2 |
| [world:clouds](especificacoes/world_clouds.md) | world | 1 | 19 | 14 | 0 | 0 | 0 | 2 | landscape: portado 28 |
| [world:walkable](especificacoes/world_walkable.md) | world | 4 | 16 | 183 | 0 | 0 | 0 | 1 | people: já existia 2, não se aplica 36; roads: não se aplica 57 |
| [world:lanelets](especificacoes/world_lanelets.md) | world | 2 | 14 | 529 | 0 | 0 | 0 | 39 | roads: não portado 140; street-furniture: não se aplica 4; traffic: portado 2 |
| [world:markings](especificacoes/world_markings.md) | world | 2 | 13 | 347 | 0 | 0 | 0 | 8 | roads: em parte 60 |
| [world:changes](especificacoes/world_changes.md) | world | 2 | 12 | 57 | 0 | 0 | 0 | 31 | build: a portar 2; roads: não portado 2 |
| [world:derived](especificacoes/world_derived.md) | world | 4 | 12 | 60 | 0 | 0 | 0 | 4 | traffic: já existia 2 |
| [world:parking](especificacoes/world_parking.md) | world | 1 | 11 | 409 | 0 | 0 | 0 | 1 | roads: portado 13, não portado 48; traffic: portado 3 |
| [world:bounds](especificacoes/world_bounds.md) | world | 3 | 10 | 27 | 0 | 0 | 0 | 14 | build: a portar 2 |
| [world:ramp](especificacoes/world_ramp.md) | world | 10 | 10 | 26 | 0 | 0 | 0 | 0 | roads: não se aplica 42 |
| [world:street](especificacoes/world_street.md) | world | 1 | 10 | 342 | 0 | 0 | 0 | 2 | roads: não se aplica 16; street-furniture: portado 18 |
| [world:ribbons](especificacoes/world_ribbons.md) | world | 5 | 9 | 184 | 0 | 0 | 0 | 0 | road-elevation: a portar 2; roads: não portado 42 |
| [world:view](especificacoes/world_view.md) | world | 3 | 9 | 83 | 0 | 0 | 0 | 0 | roads: não portado 4; street-furniture: já existia 2, não se aplica 8 |
| [world:pole](especificacoes/world_pole.md) | world | 1 | 7 | 381 | 0 | 0 | 0 | 4 | street-furniture: portado 108, não se aplica 8; build: outro porte 4; people: já existia 2 |
| [world:cells](especificacoes/world_cells.md) | world | 2 | 6 | 50 | 0 | 0 | 0 | 5 | roads: não portado 2 |
| [world:conflict](especificacoes/world_conflict.md) | world | 1 | 4 | 225 | 0 | 0 | 0 | 16 | traffic: já existia 3; people: já existia 1 |
| [world:geometry](especificacoes/world_geometry.md) | world | 1 | 3 | 97 | 0 | 0 | 0 | 4 | roads: não portado 30 |
| [world:ground](especificacoes/world_ground.md) | world | 1 | 3 | 9 | 0 | 0 | 0 | 5 | build: a portar 10; landscape: já existia 20 |
| [world:ids](especificacoes/world_ids.md) | world | 1 | 3 | 18 | 0 | 0 | 0 | 56 | - |
| [world:lot](especificacoes/world_lot.md) | world | 1 | 3 | 153 | 0 | 0 | 0 | 2 | - |
| [world:network](especificacoes/world_network.md) | world | 1 | 3 | 621 | 0 | 0 | 0 | 227 | landscape: já existia 4; people: já existia 2; roads: não portado 192; street-furniture: não se aplica 6; traffic: já existia 1 |
| [world:walkways](especificacoes/world_walkways.md) | world | 1 | 3 | 493 | 0 | 0 | 0 | 5 | demolition: já existia 2; people: já existia 4; traffic: já existia 2 |
| [world:ecology](especificacoes/world_ecology.md) | world | 1 | 1 | 40 | 0 | 0 | 0 | 4 | landscape: portado 68 |
| [world:approach](especificacoes/world_approach.md) | world | 0 | 0 | 26 | 0 | 0 | 0 | 3 | roads: não portado 17 |
| [world:barriers](especificacoes/world_barriers.md) | world | 0 | 0 | 6 | 0 | 0 | 0 | 0 | street-furniture: portado 7 |
| [world:building](especificacoes/world_building.md) | world | 0 | 0 | 60 | 0 | 0 | 0 | 0 | - |
| [world:carriageway](especificacoes/world_carriageway.md) | world | 0 | 0 | 144 | 0 | 0 | 0 | 1 | roads: não portado 3 |
| [world:curb](especificacoes/world_curb.md) | world | 0 | 0 | 299 | 0 | 0 | 0 | 3 | people: já existia 1; roads: não portado 23 |
| [world:economy](especificacoes/world_economy.md) | world | 0 | 0 | 129 | 0 | 0 | 0 | 3 | economy: portado 1; roads: parcial 1, já existia 10 |
| [world:elements](especificacoes/world_elements.md) | world | 0 | 0 | 13 | 0 | 0 | 0 | 2 | landscape: portado 4 |
| [world:elevation](especificacoes/world_elevation.md) | world | 0 | 0 | 359 | 0 | 0 | 0 | 38 | road-elevation: a portar 116; roads: não se aplica 116 |
| [world:fog](especificacoes/world_fog.md) | world | 0 | 0 | 16 | 0 | 0 | 0 | 3 | landscape: portado 14 |
| [world:footway](especificacoes/world_footway.md) | world | 0 | 0 | 107 | 0 | 0 | 0 | 1 | people: já existia 1; roads: não portado 5; transit: não se aplica 1, portado 1 |
| [world:grid](especificacoes/world_grid.md) | world | 0 | 0 | 8 | 0 | 0 | 0 | 0 | roads: não portado 5 |
| [world:gullies](especificacoes/world_gullies.md) | world | 0 | 0 | 4 | 0 | 0 | 0 | 2 | landscape: portado 6; roads: não se aplica 5 |
| [world:heading](especificacoes/world_heading.md) | world | 0 | 0 | 6 | 0 | 0 | 0 | 0 | traffic: já existia 1 |
| [world:landscape](especificacoes/world_landscape.md) | world | 0 | 0 | 351 | 0 | 0 | 0 | 12 | people: já existia 1; roads: não se aplica 14; street-furniture: portado 1, não se aplica 16 |
| [world:leg](especificacoes/world_leg.md) | world | 0 | 0 | 74 | 0 | 0 | 0 | 0 | roads: portado 3 |
| [world:nearest](especificacoes/world_nearest.md) | world | 0 | 0 | 73 | 0 | 0 | 0 | 0 | roads: não portado 3 |
| [world:open](especificacoes/world_open.md) | world | 0 | 0 | 147 | 0 | 0 | 0 | 0 | - |
| [world:paving](especificacoes/world_paving.md) | world | 0 | 0 | 122 | 0 | 0 | 0 | 2 | people: já existia 1; roads: não portado 7 |
| [world:pedestrian](especificacoes/world_pedestrian.md) | world | 0 | 0 | 17 | 0 | 0 | 0 | 0 | people: já existia 1 |
| [world:point](especificacoes/world_point.md) | world | 0 | 0 | 24 | 0 | 0 | 0 | 0 | - |
| [world:raster](especificacoes/world_raster.md) | world | 0 | 0 | 7 | 0 | 0 | 0 | 1 | street-furniture: não se aplica 2 |
| [world:rebuild](especificacoes/world_rebuild.md) | world | 0 | 0 | 2 | 0 | 0 | 0 | 4 | - |
| [world:rescale](especificacoes/world_rescale.md) | world | 0 | 0 | 95 | 0 | 0 | 0 | 1 | - |
| [world:section](especificacoes/world_section.md) | world | 0 | 0 | 48 | 0 | 0 | 0 | 6 | people: já existia 1; roads: não portado 17; street-furniture: já existia 2 |
| [world:signal](especificacoes/world_signal.md) | world | 0 | 0 | 276 | 0 | 0 | 0 | 2 | demolition: já existia 1; street-furniture: já existia 7 |
| [world:solids](especificacoes/world_solids.md) | world | 0 | 0 | 142 | 0 | 0 | 0 | 1 | demolition: já existia 1; landscape: já existia 4; street-furniture: portado 1 |
| [world:structures](especificacoes/world_structures.md) | world | 0 | 0 | 24 | 0 | 0 | 0 | 11 | road-elevation: a portar 20; roads: não portado 1 |
| [world:surface](especificacoes/world_surface.md) | world | 0 | 0 | 118 | 0 | 0 | 0 | 0 | roads: não portado 4; traffic: já existia 1 |
| [world:swept](especificacoes/world_swept.md) | world | 0 | 0 | 8 | 0 | 0 | 0 | 0 | traffic: já existia 1 |
| [world:tile](especificacoes/world_tile.md) | world | 0 | 0 | 65 | 0 | 0 | 0 | 0 | - |
| [world:transit](especificacoes/world_transit.md) | world | 0 | 0 | 58 | 0 | 0 | 0 | 5 | transit: portado 32, não se aplica 3, não portado 6, já existia 1 |
| [world:trees](especificacoes/world_trees.md) | world | 0 | 0 | 16 | 0 | 0 | 0 | 4 | landscape: portado 17 |
| [world:waiting](especificacoes/world_waiting.md) | world | 0 | 0 | 2 | 0 | 0 | 0 | 1 | - |
| [world:weather](especificacoes/world_weather.md) | world | 0 | 0 | 6 | 0 | 0 | 0 | 1 | landscape: portado 8 |
| [world:zones](especificacoes/world_zones.md) | world | 0 | 0 | 11 | 0 | 0 | 0 | 5 | - |
