# Cobertura do rastreamento: transit

Rastreamento refeito por mim: `node scripts/port.ts extract transit src/editor/transitTools.ts src/render/transit.ts src/sim/transit/transit.ts src/ui/v2/transitLine.ts src/world/transit.ts`: 197 declarações próprias, 35 fatias, 19 arquivos. Conferido contra `src/` do 4.0 na leitura de `transitTools.ts`, `render/transit.ts`, `sim/transit/transit.ts`, `world/transit.ts`, `transitLine.ts` e dos chamadores.

Escrito depois do código, não antes como o prompt pedia: a primeira versão do render foi feita sem esta tabela e saiu errada (ver relatório).

Contagem: fatia não se aplica 2, fatia portado 9, já existia 1, não portado 6, não se aplica 27, portado 163.

## src/editor/transitTools.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `PICK` | alcance do clique numa parada ou trilho | portado | data/catalog/traffic.json transit_pick_m |
| `STOP_REACH` | quão longe da rua cabe uma parada | portado | data/catalog/traffic.json transit_stop_reach_m |
| `TransitTool` | a ferramenta do transporte | portado | game/tools/transit_tool.gd |
| `TransitTool.built` | a entrada do metrô não cai sob prédio | portado | sim/transit/transit_ground transit_built, folga de 1 m (caixa do registro, não o polígono de cada volume) |
| `TransitTool.click` | o clique: parada, terminal, estação, trilho, linha; Shift remove; duplo clique encerra | portado | game/tools/transit_tool.gd click, _pick |
| `TransitTool.constructor` | construção da classe | não se aplica | ToolBase recebe controlador e parâmetros |
| `TransitTool.cursor` | onde está o ponteiro | portado | game/tools/transit_tool.gd cursor |
| `TransitTool.data` | acesso ao plano | não se aplica | o plano está em CityState; a ferramenta consulta CityStore |
| `TransitTool.draw` | o plano sobre o mapa: linhas, paradas, pontos e linha em desenho | portado | render/transit_overlay.gd |
| `TransitTool.finishLine` | fecha a linha, avisa se é curta ou criada | portado | game/tools/transit_tool.gd finish; sim/commands/transit_commands transit_make_line |
| `TransitTool.finishTrack` | fecha o trilho, recusa o que corre sobre a rua | portado | game/tools/transit_tool.gd finish; sim/commands/transit_commands transit_lay_track |
| `TransitTool.footwayAt` | a calçada mais perto de um clique | portado | sim/commands/transit_commands footway_at |
| `TransitTool.key` | Enter, Backspace, Esc | portado | game/tools/transit_tool.gd key, cancel |
| `TransitTool.kind` | o que o clique põe | portado | game/tools/transit_tool.gd kind (params do drawers.json) |
| `TransitTool.lineStops` | paradas da linha em desenho | portado | game/tools/transit_tool.gd stops |
| `TransitTool.lines` | linhas para o painel | portado | CityCore::transit_plan |
| `TransitTool.move` | o ponteiro andou | portado | game/tools/transit_tool.gd hover |
| `TransitTool.onRoad` | um ponto está na pista | portado | sim/transit/transit_ground transit_on_road |
| `TransitTool.points` | pontos do trilho em desenho | portado | game/tools/transit_tool.gd points |
| `TransitTool.preview` | o trilho em desenho como trilho de verdade | portado | game/tools/transit_tool.gd _show_run; render/transit/transit_renderer set_preview |
| `TransitTool.rail` | trem ou metrô | portado | game/tools/transit_tool.gd mode |
| `TransitTool.removeLine` | apagar a linha | portado | ui/transit_strip.gd _drop; sim/commands/transit_commands |
| `TransitTool.setKind` | trocar o que o clique põe | portado | gaveta: cada aba refaz a ferramenta com seu kind |
| `TransitTool.setVehicles` | mais ou menos veículos | portado | ui/transit_strip.gd _change; sim/commands/transit_commands |
| `TransitTool.snapTrack` | o clique cola em ponta ou trecho de trilho | portado | sim/commands/transit_commands transit_snap_track (varre os pontos, sem índice: ver stopIndex) |
| `TransitTool.stopAt` | a parada sob o clique | portado | CityCore::transit_pick; sim/transit/transit_data stop_near |
| `TransitToolDeps` | injeção de dependências | não se aplica | CityStore e ToolController fazem esse papel |
| `TransitToolKind` | os tipos de clique | portado | data/ui/drawers.json kind |
| `current` | ferramenta global para o painel | não se aplica | ToolController.active |
| `setTransitTool` | idem | não se aplica | ToolController |
| `transitTool` | idem | não se aplica | ToolController |

## src/render/transit.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `<EmptyStatement>` | nada | não se aplica | ponto e vírgula solto |
| `Box` | uma caixa | portado | render/transit/transit_parts TransitBox |
| `CAR` | tamanho do vagão | portado | data/render/transit.json car_m |
| `GAUGE` | bitola | portado | data/render/transit.json track.gauge_m |
| `MAX_CARS` | teto do buffer de vagões | não se aplica | o MultiMesh é preenchido pelo que há no raio |
| `METRO_DEPTH` | profundidade do metrô | portado | data/render/transit.json metro.rails_below_m |
| `PIECE` | trilho em peças de 4 m, cada uma sobre o que tem embaixo | portado | data/render/transit.json track.piece_m |
| `PREVIEW_LAYERS` | camadas da prévia | portado | a prévia usa as mesmas partes do trilho; sem a opacidade 0,9 do lastro e 0,95 dos dormentes do 4.0 |
| `SLEEPER_STEP` | passo dos dormentes | portado | data/render/transit.json track.sleeper_step_m |
| `STOP_READS` | até onde uma parada lê o chão | portado | render/transit/transit_renderer build, raio de meio chunk |
| `TRACK_READS` | até onde um trilho lê o chão | portado | render/transit/transit_renderer build |
| `TUNNEL` | o túnel do metrô | portado | data/render/transit.json metro |
| `TrackPreview` | a prévia | portado | render/transit/transit_renderer set_preview |
| `TransitMeshes` | o que o render expõe | portado | render/transit/transit_renderer |
| `TransitParts` | caixas por tipo | portado | render/transit/transit_parts TransitMeshes |
| `TransitPlace` | o desenho de um item | portado | render/transit/transit_renderer Item |
| `TransitProduct` | cada trilho e parada refeito só quando muda | portado | render/transit/transit_renderer see_plan, touch, build |
| `TransitProduct.allKeys` | todos os itens | portado | render/transit/transit_renderer see_plan |
| `TransitProduct.byId` | detalhe do diário do 4.0 | não se aplica | ChangeLog |
| `TransitProduct.changed` | guarda a área do item | portado | render/transit/transit_renderer build reach |
| `TransitProduct.compute` | monta as caixas | portado | render/transit/transit_renderer build |
| `TransitProduct.compute.runOf` | caixas em superfícies | portado | render/transit/transit_renderer draw |
| `TransitProduct.constructor` | construção | não se aplica |  |
| `TransitProduct.dirty` | o que uma mudança alcança | portado | render/transit/transit_renderer touch, touch_terrain |
| `TransitProduct.dirty.meets` | teste de área | portado | render/transit/transit_renderer touch (por chunk) |
| `TransitProduct.equals` | nada a refazer se é igual | portado | render/transit/transit_renderer assinatura do item |
| `TransitProduct.filing` | itens por área | portado | render/transit/transit_renderer caixa de cada item |
| `TransitProduct.kinds` | mudanças que importam | portado | RenderWorld::sync: vias e terreno; prédios não, pois a entrada só os lê ao ser posta |
| `TransitProduct.name` | nome no diário | não se aplica |  |
| `TransitSource` | fontes do render | não se aplica | CityState |
| `TransitStop` | apelido de tipo | não se aplica |  |
| `TransitTrack` | apelido de tipo | não se aplica |  |
| `VISIBLE` | tipos de caixa visíveis | portado | data/render/transit.json parts |
| `XRAY` | tipos do corte | portado | data/render/transit.json parts, chave xray |
| `buildTrackPreview` | aquece os shaders da prévia | não se aplica | o Godot compila os materiais no carregamento |
| `createTrackPreview` | a prévia | portado | render/transit/transit_renderer set_preview |
| `createTrackPreview.dispose` | solta a prévia | portado | render/transit/transit_renderer drop |
| `createTrackPreview.set` | desenha a prévia | portado | render/transit/transit_renderer set_preview (malha nova a cada movimento do ponteiro, só enquanto desenha) |
| `createTransit` | o render do transporte | portado | render/transit/transit_renderer |
| `createTransit.cull` | só o que a câmera vê | portado | o RenderingServer corta por instância; trilho em trechos de run_m |
| `createTransit.dispose` | solta tudo | portado | render/transit/transit_renderer reset |
| `createTransit.material` | materiais | portado | materials.json transit, transit_glass, transit_xray |
| `createTransit.place` | põe um vagão | portado | render/transit/transit_renderer update |
| `createTransit.setXray` | liga o corte do metrô | portado | render/transit/transit_renderer set_xray |
| `createTransit.shown` | lojas visíveis | portado | render/transit/transit_renderer draw |
| `createTransit.sync` | traz o desenho ao plano | portado | render/transit/transit_renderer sync, com orçamento items_per_frame |
| `createTransit.through` | lojas do corte, em ordem | portado | materials.json transit_xray, layers: render_priority |
| `createTransit.update` | os vagões onde a simulação os tem | portado | render/transit/transit_renderer update |
| `emptyParts` | estrutura vazia | não se aplica |  |
| `entranceSpot` | a entrada do metrô fora da pista e de prédio | portado | render/transit/transit_parts entrance_spot |
| `pointsBox` | caixa de um trilho | portado | render/transit/transit_renderer build reach |
| `sharedMaterial` | um material por combinação | portado | materials.json; rugosidade e metal por vértice (UV) |
| `sharedMaterials` | idem | portado | idem |
| `stopKey` | chave do item | portado | render/transit/transit_renderer stop_key |
| `stopParts` | estação com prédio, passagem e placas; entrada e corte do metrô; abrigo e terminal | portado | render/transit/transit_parts transit_stop_parts; data/render/transit.json station, stop, terminal, entrance, below |
| `stopParts.colourAt` | a placa na cor da linha | portado | render/transit/transit_parts line_colour |
| `trackBoxes` | lastro, dormentes, trilhos, túnel; rente à rua na passagem | portado | render/transit/transit_parts track_piece |
| `trackKey` | chave do item | portado | render/transit/transit_renderer track_key |
| `trackParts` | trilho nas partes do seu modo | portado | render/transit/transit_parts transit_track_parts |

## src/sim/agents/laneGrid.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `laneChangesSince` | o que mudou nas faixas | portado | RoadNetwork::version: toda edição de via reencontra as faixas de todas as paradas; não por área |

## src/sim/transit/transit.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `AHEAD_TIME` | segundos de trilho à frente que o pedestre evita | portado | data/catalog/traffic.json train_clear_time_s |
| `APPROACH` | ônibus entra atrás da parada | portado | data/catalog/traffic.json bus_approach_m |
| `AT_STOP,STOPPED` | ônibus chegou | portado | data/catalog/traffic.json bus_at_stop_m, bus_stopped_ms |
| `AWAY_MARGIN` | margem para sair da via | portado | a janela micro do tráfego decide; bus_away_margin_m fica nos dados sem leitura própria aqui, ver relatório |
| `BUS_DWELL,TERMINAL_DWELL,TRAIN_DWELL` | tempos de parada | portado | data/catalog/traffic.json bus_dwell_s, bus_terminal_s, train_dwell_s |
| `BUS_PACE` | ritmo entre paradas longe da câmera | portado | data/catalog/traffic.json bus_pace |
| `BUS_SPACING` | intervalo entre ônibus | portado | data/catalog/traffic.json bus_spacing_s |
| `Bus` | um ônibus | portado | sim/transit/transit_sim Bus |
| `CROSSING_STOP` | onde se espera antes do trilho | portado | data/catalog/traffic.json crossing_stop_m |
| `CROSSING_WARN` | passagem fecha a esta distância | portado | data/catalog/traffic.json crossing_warn_m |
| `CROSS_SLACK` | folga da busca de faixas | portado | sim/transit/rail_run for_segments_near |
| `DETOUR` | rua é mais longa que a reta | portado | data/catalog/traffic.json bus_detour |
| `KEEP_CLEAR` | passagem perto demais da linha de parada | portado | data/catalog/traffic.json crossing_keep_clear_m |
| `LevelCrossing` | uma passagem de nível | portado | sim/transit/transit_sim LevelCrossing |
| `NONE` | lista vazia constante | não se aplica |  |
| `NO_CROSSINGS` | lista vazia constante | não se aplica |  |
| `RailLine` | uma linha de trilho | portado | sim/transit/transit_sim RailLine |
| `TRAIN_AHEAD` | trilho à frente que o pedestre evita | portado | data/catalog/traffic.json train_clear_ahead_m |
| `TRAIN_CARS,METRO_CARS,CAR_LENGTH` | vagões | portado | data/catalog/traffic.json train_cars, metro_cars, train_car_m |
| `TRAIN_GAP` | distância entre trens | portado | data/catalog/traffic.json train_gap_m |
| `TRAIN_TOP,METRO_TOP,TRAIN_ACCEL,TRAIN_BRAKE` | velocidades e frenagem | portado | data/catalog/traffic.json |
| `Train` | um trem | portado | sim/transit/transit_sim Train |
| `TrainView` | trem publicado | portado | PoseFrame::trains |
| `TransitSim` | o transporte rodando | portado | sim/transit/transit_sim TransitRun, transit::step |
| `TransitSim.busLinesAt` | linhas por parada, para refazer só elas | não se aplica | rebuild passa por todas as linhas, mantendo os ônibus; planos são pequenos |
| `TransitSim.busStops` | paradas por célula | não se aplica | idem |
| `TransitSim.buses` | os ônibus | portado | TransitRun::buses |
| `TransitSim.closedOn` | passagens fechadas numa faixa | portado | sim/transit/rail_run crossing_gap |
| `TransitSim.crossingNear` | há passagem perto | não se aplica | sem chamador no 4.0 |
| `TransitSim.crossingNearEnd` | passagem perto da linha de parada | portado | sim/transit/rail_run crossing_near_end; sim/transit/transit_sim hold_ahead |
| `TransitSim.crossingsByLane` | passagens por faixa | portado | TransitRun::crossings_by_lane |
| `TransitSim.followLanes` | edição de via refaz o que ela alcança | portado | sim/transit/transit_sim rebuild, inteiro a cada edição de via, não por área |
| `TransitSim.followLanes.near` | teste de área | não se aplica | idem |
| `TransitSim.lanesSeen` | faixas vistas | portado | TransitRun::roads_seen |
| `TransitSim.lineOf` | linha pelo id | portado | TransitData::line |
| `TransitSim.lines` | linhas | portado | TransitData |
| `TransitSim.passStopAway` | longe da câmera o ônibus segue pela tabela | portado | sim/transit/transit_sim pass_stop_away |
| `TransitSim.placeBuses` | ônibus da linha ficam onde estão; novos no terminal | portado | sim/transit/transit_sim place_buses |
| `TransitSim.placeRails` | voltas, trens e passagens | portado | sim/transit/rail_run place_rails, make_loop |
| `TransitSim.pointAt` | ponto e rumo na volta | portado | sim/transit/rail_run point_at |
| `TransitSim.rails` | linhas de trilho | portado | TransitRun::rails |
| `TransitSim.rebuild` | refaz do plano sem tirar ônibus da rua | portado | sim/transit/transit_sim rebuild |
| `TransitSim.sendBus` | manda à próxima parada alcançável | portado | sim/transit/transit_sim send_bus |
| `TransitSim.spawnBus` | põe ônibus na faixa, na cor da linha | portado | sim/transit/transit_sim spawn_bus |
| `TransitSim.step` | um tick | portado | sim/transit/transit_sim step |
| `TransitSim.stepBus` | um tick do ônibus | portado | sim/transit/transit_sim step_bus |
| `TransitSim.stepTrain` | um tick do trem | portado | sim/transit/rail_run step_train |
| `TransitSim.stopLanes` | faixa de cada parada | portado | TransitRun::stop_lanes |
| `TransitSim.stopPoint` | ponto da parada na faixa | portado | sim/transit/transit_sim lane_beside |
| `TransitSim.stops` | paradas por id | portado | TransitData::stop |
| `TransitSim.trainZones` | chão do trem que o pedestre evita | portado | sim/transit/rail_run train_zones, walker_held; traffic/pedestrians.cpp walk |
| `TransitSim.trainZones.far` | trem longe dos pedestres fica de fora | portado | sim/transit/rail_run train_zones |
| `TransitSim.trainZones.zone` | zona por trem | portado | sim/transit/transit_sim TrainZone |
| `TransitSim.trains` | vagões publicados | portado | sim/transit/rail_run publish |
| `TransitSim.transitFor` | revisão vista | portado | TransitRun::plan_seen |
| `crossAt` | dois segmentos se cruzam | portado | sim/transit/rail_run cross_at |
| `levelCrossings` | onde a volta cruza as faixas | portado | sim/transit/rail_run crossings_of |
| `sameLane` | mesma faixa | não se aplica | send_bus de novo em todos os ônibus |
| `samePoints` | mesma volta | portado | sim/transit/rail_run place_rails: tamanho, comprimento e nº de trens; não compara ponto a ponto |
| `travelTime` | tempo entre paradas | portado | sim/transit/transit_sim travel_time |

## src/sim/world.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `SimWorld.inFocusReach` | o que está perto da câmera | já existia | traffic::in_window |

## src/ui/v2/transitLine.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `transitLineRow` | a linha no painel: cor, nome, modo e paradas | portado | ui/transit_strip.gd |

## src/world/doc.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `RoadDoc.transitRevision` | revisão do plano | portado | CityState::transit_revision |

## src/world/footwayPath.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `FootwayNet` | interface | não se aplica | RoadNetwork |
| `nearestFootway` | calçada mais perto, pulando o que está sob prédio de metro em metro | portado | sim/commands/transit_commands footway_at |

## src/world/transit.ts

| Declaração | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `ALONG_ROAD` | trilho não corre sobre a rua | portado | data/catalog/traffic.json transit_along_road_m |
| `HEX_COLOUR` | valida cor em texto | não se aplica | o save guarda número |
| `JOIN` | pontas se juntam | portado | data/catalog/traffic.json transit_join_m |
| `LINE_COLOURS` | cores das linhas | portado | data/catalog/traffic.json line_colours |
| `RailEdge` | trecho do grafo | portado | sim/transit/transit_data |
| `RailGraph` | grafo de trilhos | portado | sim/transit/transit_data |
| `RailTrack` | um trilho | portado | sim/transit/transit_data |
| `STOP_CELL` | índice de paradas por célula | não portado | busca linear por clique; só pesa em planos muito grandes, não medido |
| `TRACK_CELL` | idem, trilhos | não portado | idem |
| `TrackStretch` | idem | não portado | idem |
| `TransitData` | o plano | portado | sim/transit/transit_data |
| `TransitLine` | uma linha | portado | sim/transit/transit_data |
| `TransitMode` | ônibus, trem, metrô | portado | sim/transit/transit_data |
| `TransitStop` | uma parada | portado | sim/transit/transit_data |
| `addLine` | criar linha, cor e nome | portado | sim/transit/transit_data transit_add_line; sim/commands/transit_commands |
| `addStop` | pôr parada | portado | sim/transit/transit_data |
| `addTrack` | pôr trilho | portado | sim/transit/transit_data |
| `emptyTransit` | plano vazio | portado | sim/transit/transit_data |
| `finite` | valida número do JSON | não se aplica | save binário |
| `hasTransit` | há algo a mover | não se aplica | simShowsMotion do 4.0; aqui a simulação sempre corre |
| `isMode` | modo válido | portado | sim/transit/transit_data transit_normalize |
| `longestOnRoad` | maior trecho sobre a rua | portado | sim/transit/transit_data longest_on_road |
| `nearestTrack` | trilho mais perto | portado | sim/transit/transit_data nearest_track, linear |
| `normalizeTransit` | o plano lido sem o que está mal formado | portado | sim/transit/transit_data transit_normalize |
| `railGraph` | grafo | portado | sim/transit/transit_data rail_graph |
| `railGraph.cellOf` | células do grafo | portado | sim/transit/transit_data cell_key |
| `railGraph.link` | liga nós | portado | sim/transit/transit_data rail_graph |
| `railGraph.node` | nó por proximidade | portado | sim/transit/transit_data rail_graph |
| `railPath` | caminho no grafo (Dijkstra) | portado | sim/transit/transit_data rail_path |
| `railPath.onEdge` | ponto no trecho | portado | sim/transit/transit_data on_graph |
| `removeLine` | apagar linha | portado | sim/transit/transit_data |
| `removeStop` | apagar parada e linhas que ficam curtas | portado | sim/transit/transit_data |
| `removeTrack` | apagar trilho e suas estações | portado | sim/transit/transit_data |
| `setLineVehicles` | veículos da linha | portado | sim/transit/transit_data |
| `setTerminal` | terminal | portado | sim/transit/transit_data |
| `stopById` | parada pelo id | portado | sim/transit/transit_data |
| `stopIndex` | idem | não portado | idem |
| `stopNear` | parada mais perto | portado | sim/transit/transit_data stop_near, linear |
| `stretchesNear` | idem | não portado | idem |
| `trackCells` | idem | não portado | idem |
| `trackPointNear` | ponta de trilho perto | portado | sim/commands/transit_commands transit_snap_track, linear |

## Arquivos só com fatias de código compartilhado

| Arquivo | Para o jogador | Estado | Dono ou motivo |
|---|---|---|---|
| `src/lab/catalog/layers.ts` | catálogo do laboratório do 4.0 | não se aplica | não há laboratório aqui |
| `src/main.ts` | liga a ferramenta: clique, tecla, duplo clique, desenho, painel | portado | ToolController, game/tools/transit_tool.gd |
| `src/render/renderer.ts` | liga o render e o corte | portado | RenderWorld; aquecimento de shader e camadas do laboratório não se aplicam |
| `src/sim/agents/trafficBuildings.ts` | manda o plano ao worker | não se aplica | simulação e plano estão no mesmo CityState |
| `src/sim/agents/walk.ts` | pedestre evita o trem | portado | traffic/pedestrians.cpp walk: para antes do chão do trem; quem já está nele segue |
| `src/sim/city/city.ts` | o transporte no passo da cidade | portado | traffic_system.cpp |
| `src/sim/host/simHost.ts` | publica os trens | portado | traffic_system.cpp publish; PoseFrame::trains |
| `src/sim/intersections/admission.ts` | pedido de vez a partir de antes do trilho | portado | look_ahead_m 60 m já cobre os 29 m da passagem; sim/transit/transit_sim hold_ahead usa v.pass |
| `src/sim/vehicles/obstacles.ts` | passagem fechada é obstáculo, até 3 faixas à frente | portado | sim/transit/transit_sim hold_ahead |
| `src/ui/v2/shell.ts` | painel de linhas | portado | ui/transit_strip.gd |
| `src/world/doc.ts` | salvar, desfazer, abrir | portado | sim/persistence, sim/commands/journal_ops j_transit |

## Recursos

Textos `transit.*` e `hint.transit.*`: portados em `data/i18n/strings_c.csv`, com `transit.stops`, `hint.transit.removed`, `hint.transit.lineMade` e as marcas B, T e M. Estilo `v2-transit-line`: portado em `ui/transit_strip.gd` com tokens. Ícone `demolish`: já existia.
