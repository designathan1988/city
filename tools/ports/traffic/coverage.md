# Cobertura do rastreamento: traffic

Uma linha por arquivo e por declaração própria de `converter/out/features/traffic` (boundary.json `own`). A coluna "o que faz" da declaração é o comentário dela no 4.0; a situação é a marca do porte. Gerado por `evidence/cobertura_gerar.py` a partir de `evidence/cobertura_mapa.py`.

Situações: de outra funcionalidade: 358, já existia: 1543, não portado: 44, não se aplica: 351, portado: 1026. Total de declarações: 3322.

| Arquivo | Declaração | O que faz para o jogador | Situação | Dono no jogo / motivo |
|---|---|---|---|---|
| `src/actionsWiring.ts` | **arquivo** | tiro e explosão em veículos | de outra funcionalidade | pistola/bomba: funcionalidade `destruction` |
| `src/bayPool.ts` | **arquivo** | vagas dos lotes calculadas em workers | não se aplica | sem workers: lotes por orçamento no tick (sim/traffic/parking_sources.cpp) |
| | `BayPool.check` |  | não se aplica | sem workers: lotes por orçamento no tick (sim/traffic/parking_sources.cpp) |
| `src/bays.worker.ts` | **arquivo** | worker das vagas | não se aplica | idem bayPool |
| `src/conflictPool.ts` | **arquivo** | tabela de conflitos em workers | não se aplica | o jogo calcula conflitos por nó em sim/traffic/lane_graph.cpp |
| | `ConflictPool.measure` |  | não se aplica | o jogo calcula conflitos por nó em sim/traffic/lane_graph.cpp |
| `src/conflictTable.worker.ts` | **arquivo** | worker da tabela de conflitos | não se aplica | idem conflictPool |
| `src/core/polygonBands.ts` | **arquivo** | faixas de polígono de lotes curvos | não se aplica | lotes retangulares no jogo; grade em sim/buildings/lot_access.cpp |
| | `PolygonBands` | A ring's edges sorted into horizontal bands: Haines' "bins" for the crossings test | não se aplica | lotes retangulares no jogo; grade em sim/buildings/lot_access.cpp |
| | `bandAt` | The band a height falls in. | não se aplica | lotes retangulares no jogo; grade em sim/buildings/lot_access.cpp |
| | `insideBands` | `pointInPolygon` (`polygon.ts`) over the edges of the point's band only | não se aplica | lotes retangulares no jogo; grade em sim/buildings/lot_access.cpp |
| | `polygonBands` |  | não se aplica | lotes retangulares no jogo; grade em sim/buildings/lot_access.cpp |
| `src/core/runPool.ts` | **arquivo** | colunas de números do worker | não se aplica | vetores do C++ |
| | `RunPool` | RUNS OF NUMBERS BY KEY IN ONE TYPED ARRAY: each key's run (its start and length in columns at the key) in a shared `Float64Array`; | não se aplica | vetores do C++ |
| | `RunPool.clear` | Every run let go; the room kept. | não se aplica | vetores do C++ |
| | `RunPool.data` | The numbers of every run; DO NOT keep it across a `put`: packing makes another. | não se aplica | vetores do C++ |
| | `RunPool.drop` | A key's run let go. | não se aplica | vetores do C++ |
| | `RunPool.length` | How many numbers a key's run holds (0: none). | não se aplica | vetores do C++ |
| | `RunPool.lengths` |  | não se aplica | vetores do C++ |
| | `RunPool.live` |  | não se aplica | vetores do C++ |
| | `RunPool.pack` | The live runs packed together, room made for `more` numbers: in an array of the same size while that leaves a quarter of it free,  | não se aplica | vetores do C++ |
| | `RunPool.put` | Room for `n` numbers at a key, its old run let go: where to write them in `data` (read `data` after the call). | não se aplica | vetores do C++ |
| | `RunPool.reserve` | Room for the keys below `keys` (a map's buildings, known before they are put). | não se aplica | vetores do C++ |
| | `RunPool.start` | Where a key's run starts in `data`. | não se aplica | vetores do C++ |
| | `RunPool.starts` |  | não se aplica | vetores do C++ |
| | `RunPool.used` |  | não se aplica | vetores do C++ |
| `src/core/scalar.ts` | **arquivo** | contas escalares | já existia | core/math.h |
| | `hypot2` |  | já existia | core/math.h |
| `src/core/simplifyLine.ts` | **arquivo** | tira pontos sem forma da pintura | não se aplica | o pintor do jogo pinta retângulos por trecho (render/roads/road_markings.cpp) |
| | `simplifyLine` |  | não se aplica | o pintor do jogo pinta retângulos por trecho (render/roads/road_markings.cpp) |
| `src/core/weightedSlots.ts` | **arquivo** | sorteia vaga ou carro por pesos de classe | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots` |  | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.add` |  | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.cap` | Slots the trees hold room for: a power of two, doubled as slots are handed out. | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.classes` | Each slot's class, -1 empty. | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.clear` | Every slot taken back: the next `push` is slot 0 (the room is kept). | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.constructor` | `kinds`: how many classes, under 128. | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.count` | The slots of a class. | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.counts` |  | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.findWeighted` | The first slot whose weights summed from the first slot on pass `target` (≥ 0), a slot's weight its class's; -1 when the whole sum | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.grow` | Room doubled: each tree made again from the classes in one pass (a parent takes its child's sum). | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.length` | Slots handed out (`push`) since the last `clear`. | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.nth` | The `k`-th slot not empty, from 0, in order; -1 when there are not that many. | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.occupied` |  | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.push` | A new slot after every other, of class `cls` (-1: empty). | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.set` | A slot's class changed (-1: empty). | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.total` | The slots not empty. | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.trees` | Per class, a Fenwick tree of the slots of that class (1-based). | portado | sim/traffic/lot_traffic.cpp |
| | `WeightedSlots.used` |  | portado | sim/traffic/lot_traffic.cpp |
| `src/editor/blocks.ts` | **arquivo** | grade de quarteirões com estacionamento | de outra funcionalidade | editor de vias: `roads` |
| `src/editor/commit.ts` | **arquivo** | gravar via com estacionamento | de outra funcionalidade | editor de vias: `roads` |
| `src/editor/roadParking.ts` | **arquivo** | estacionamento da via nova | já existia | perfil escolhido no catálogo de vias (project/ui/road_catalog.gd) |
| `src/editor/roadTool.ts` | **arquivo** | ferramenta de via passa o estacionamento | já existia | perfil em road_tool.gd |
| `src/editor/roads/profile.ts` | **arquivo** | aplicar perfil | de outra funcionalidade | editor de perfil: `roads` |
| `src/editor/streetObjects.ts` | **arquivo** | retorno na via | de outra funcionalidade | objetos de rua: `street-furniture` |
| `src/lab/inspector/entity.ts` | **arquivo** | laboratório de inspeção | não se aplica | ferramenta do 4.0; o jogo tem traffic_probe |
| `src/main.ts` | **arquivo** | laço do navegador | não se aplica | Simulation e RenderWorld |
| `src/people/body/morph.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `Morpher.coefficients` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `bodyHeight` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/people/hair/procedural.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `generateHairStrands` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/agents.ts` | **arquivo** | desenho dos veículos, duas rodas, ocupantes, gente na guia | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `<ExpressionStatement>` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `<ExpressionStatement>@75` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `<ExpressionStatement>@76` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `AgentMeshes` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `AgentRenderOptions` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `BLACK_ROOF` | A contrasting black roof panel (`VehicleLook.blackRoof`). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `BOX_BODY` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `BRAKELAMP` | A tail lamp with the brakes on. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `BRAKE_DECEL` | Deceleration, in units per second per second, at which the brake lights come on. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `BodyPlan` | Per-class geometry, derived once at module load. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CAR_LENGTH` | A car's length, for the smallest scale anything is drawn at (2 px). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CELL_HALF_DIAGONAL` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CHROME` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CLIP_FADE` | Seconds a change of a person's clip cross-fades over: about half a step at a walk | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CRANK_RATIO` | Crank turns per wheel turn of a bicycle. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CULL_GRADE` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CULL_GROWTH` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CULL_REUSE` | A culled vehicle's deck is reused while it has moved under this; the steepest a road climbs (`ROAD_TUNING.grade.refuse`), and the  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CarParts` | One set of instanced meshes per car class, built from its own model (`vehicleModels.ts`) | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `CellRecord` | What a cell of parked cars drew (`sync`): its version then, whether every car of it drawn stood as kept, those kept, each whole ca | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `DESTINATION` | A bus's destination blind: amber on black, read as lit amber. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `ElevationAt` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `FALLBACK_PLAN` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `FAR_BODY_ZOOM` | Below this zoom a vehicle is its far proxy (`VehicleModel.far`) | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `FIELD_SLACK` | Relative slack of the screen's scale worked out from its field (`depthField`) against the asked one: a cell at a level's edge is w | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `FLEET_CELL` | The side of the client's cells of parked cars (`fleetGrid.ts`), and half their diagonal. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `GRAVITY` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `HEADLAMP` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `HEADLAMP_OFF` | A parked car's lamps, switched off: clear and red lenses, unlit. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `HELMET_COLOURS` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `HUB` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `INDICATOR` | The amber of an indicator, on the side the vehicle is moving towards. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `KeptHolder` | Who holds kept instances (`VehicleDraw`): each one's place in a part, moved when another kept one leaves. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `LAMP_GAIN` | How much brighter each lit lamp burns after dark (`lightLevels.ts`), by its colour | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `LEAN_RATE` | How fast a lean follows the bend, per second. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `Ladder` | A part at each level (`vehicleLevels.ts`): `parts[k]` within `LEVEL_ERRORS[k - 1]`, `route[k]` the part drawn at level k (-1: none | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `LampState` | Lamp state for the vehicle currently being drawn. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `LeanState` | A two-wheeler's lean as last drawn (`VehicleDraw`): known for the view it was drawn for. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `MAX_LEAN` | Steepest lean drawn, radians (about 35 degrees). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `MAX_PEDS` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `MAX_VEHICLES` | DO NOT stop drawing at this number again: past 1,200 parked cars, the rest vanished from the town. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `NEAR_DETAIL_ZOOM` | Zoom at which the closest band switches on. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `OCCUPANT_ZOOM` | From this zoom the cabin and the people in it are drawn (the tier may ask for more, `QualitySettings.occupantZoom`). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `OFF_ROAD_GROUND` | What a car off the road stands on (`offRoadDeck`): the terrain and the ground as cut and filled, the roads' heights | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `PART_CAPACITY` | DO NOT size a part by the fleet's ceiling: 8 bodies x ~25 parts x 1,200 x 76 B was 15-22 MB of CPU and GPU; every part starts here | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `PED_IN_BOX` | Whether the vehicles' ground box holds every walker the view can show (`pedestrianVisible`: a sphere of 8 m 3 m over the feet). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `PLANS` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `PLATE` | A number plate is retro-reflective, so unlit white is the honest material. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `POSE` | A vehicle's pose and its frame on the road, written in place for each vehicle drawn | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `Part` | DO NOT write every vehicle's every instance each frame again: a parked car's stay where they were written (`kept`), only the frame | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `RUBBER` | Tints for parts that are never painted the vehicle's own colour. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `SEAT_HEAD_RISE` | Height of a seated head above the middle of its torso. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `SHIRT_COLOURS` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `SIDES` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `SKIN_TONES` | Skin tones, light to dark. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `SUSPENSION_TAU` | Time constant of the pitch and roll ease, seconds. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `TAILLAMP` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `TAILLAMP_OFF` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `TRIM` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `Tint` | A tint as the instance colours hold it (the working colour space), and a lamp's glow by it (`LAMP_GAIN`): worked out once a colour | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `TwoWheelerParts` | A two-wheeler's parts, each at the level its size on the screen allows (`vehicleLevels.ts`), as a car's. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `VEHICLE_HYSTERESIS` | Each vehicle's own level, by the screen's scale where it stands | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `VEHICLE_NEAR_CAP` | At most this many vehicles whole at once: past it, the smallest on the screen are drawn a level down. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `VEHICLE_REACH` | The widest reach a vehicle is tested with (`vehicleVisible`'s radius): the view's ground box is grown by it. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `VehicleDraw` | DO NOT look a vehicle up in a map for each thing drawn (its ride, its look, its parked copy, its body by a joined string, its pain | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `VehicleLook` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `WANT` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `agentHash` | A cheap integer avalanche hash. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `bodyPlan` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes` | `groundAt`: what a walker off the streets stands on (a yard, a terrace, the paving, the land); `roughGroundAt` | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.animProbe` | What is posing a procedural person now (the weapons lab): the baked clip played | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.bodyKey` | Which body a vehicle is drawn with: its class and, for a car, the style its id picks. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.carcass` | The burnt-out shell of this vehicle's own body | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.carcass.put` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.carcass.wave` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.carriesChild` | Whether anybody aboard is a child: a car with a child in it is never on a work trip. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.census` | Every figure drawn last frame and the body it was cast as (`citizenCasting.ts`). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.deckHolds` | DO NOT key it by position alone: a parked car kept the height it was first asked at and floated over (kept for the view it was ask | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.depthField` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.dispose` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.drawCar` | Hatchback, sedan, SUV and van, from the class's model (`vehicleModels.ts`) | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.drawOf` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.drawTwoWheeler` | A motorcycle or a bicycle from its model | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.driverBody` | The body a vehicle's driver or rider is drawn with, for their body when they are thrown out (`Occupant.index`). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.dropKept` | A parked car's kept instances let go: the last kept one of each part takes each one's place, and this frame's last the place it le | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.elevationOnCurrent` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.forceClip` | Plays `clip` on a person whatever they are doing (the lab's clip check); null gives them back. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.forgetCast` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.forgetGone` | The people of a vehicle gone from the world let go by the casting, told by the client as it goes (DO NOT walk every vehicle cast a | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.frameAt` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.glazing` | A body's glazing at every level, each part with its instances' window drops (`vehicleWindows.ts`): the coarser levels' own geometr | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.growPart` | DO NOT go back to dropping what passes the capacity: the town's parked cars past the fleet's ceiling were not drawn. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.hexOf` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.keepPeople` | Every body of the vehicles' people loaded now and kept loaded (`riggedCitizens.keepAll`): asked once the page has a scene to draw. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.ladder` | `base` and a part per coarser level, empty until drawn (they grow, `growPart`), routed to `base` until the levels are made. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.levelsFor` | The levels of `source`, made once; `copies` of it on a vehicle (`partLevels`). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.listed` | In the frame's list of parts drawn (`live`). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.meshProbe` | How far each procedural person's drawn skeleton is from a body (the weapons lab's mesh probe) | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.moveInstance` | One instance moved within a part: its matrix, its tint, a lamp's glow. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.panelLadder` | A body and its panel joined at every level: each level the body's and the panel's own (`vehicleLevels.ts`), null without a panel. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.panelLadder.at` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.panelPart` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.place` | Writes one instance in the current vehicle's frame: the frame times the part's place, its turn (Euler y then z, `YXZ`) and its siz | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.placeBlob` | The soft disc under a vehicle (`blobShadows.ts` blobInto). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.placeCarOccupants` | The people in a car, each in a seat of the model | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.placeKerbPerson` | Somebody getting out at the kerb, or in. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.placeKerbPerson.walkTo` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.placeRiderFigure` | The person astride: a baked IK pose | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.placeSteeringWheel` | A steering wheel about its column: yaw with the vehicle, tilt, then the turn. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.placeWheels` | Road wheels, plus their hubs at the closest band. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.placeWhole` | A part that does not move on the vehicle (its body, glass, trim): the vehicle's own frame, copied. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.procBuildNext` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.procDraw` | A person drawn this frame: their pose picked (`procPose`), and a change of clip made a cross-fade. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.procFinish` | Gone from the street: hidden; gone a second, its person freed for the next walker. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.procOf` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.procPose` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.procSpareFor` | A spare body of that age, taken out of the pool; null when there is none. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.procSuits` | Whether a body suits a walker's age (the simulation's child, adult or elder: their pace, their company) | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.renderPalettes` | The people's skeletons (procedural and cooked), worked out on the GPU: once a frame, after `sync`, before the scene is drawn. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.riderPlay` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.seatIdentity` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.seatWorld` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.seatWorldInto` | A seat (X forward, Z to the right, `up` above the road) to world, through the vehicle's transform | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.seeKept` | A kept car drawn this frame, counted once while it is kept. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.send` | An attribute's instances [from, to) sent at the next draw (the ranges of three's `WebGLAttributes.updateBuffer`). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.setBleed` | Called for each walker drawn bleeding (a limb lost), to drip blood where they go. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.setNight` | Lamps burn brighter than white after dark, so headlights and tail lights glow. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.slotOf` | The next instance of the part: this frame's, or, while a parked car is written to be kept, one more of the kept ones (the frame's  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.standingDeck` | The ground under a vehicle: its lot or the land off the road, its segment's deck on it; asked again only where it moved or the dia | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.sync` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.sync.drawVehicle` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.sync.replays` | The cap's answers for a kept cell, asked again in this frame's order: the same answers, or the cell is walked (the counts put back | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.sync.standsIn` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.sync.wholeOrNot` | Whole or not by the cap (`VEHICLE_NEAR_CAP`, the biggest on the screen, last frame's floor): 2 or 1. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.tintAt` | The tint (none: left white) and a lamp's glow of instance `at`. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.tintOf` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.toVehicle` | A world point in the current vehicle's frame: X forward, Z to the right. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.toWorld` | A point in the current frame (forward, up, left) to world (x, y) and height. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.touch` | Instances [from, to) of a part changed: sent at the end of the frame, with whatever the GPU has not taken yet. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.vehicleCompany` | The company a vehicle's people are cast in (`citizenCasting.codesFor`). | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.wholeAt` | A part that does not move on the vehicle (`drawCar`), at the level `detail` its size allows: the vehicle's own frame. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.wholeGlass` | A body's shut glazing at the level `detail` allows (`vehicleWindows.ts`): the vehicle's own frame, and how far its left and right  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.wholePanels` | A body and its panel (`vehiclePanels.ts`) at the level `detail` allows: the vehicle's own frame, the body's tint and the panel's. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `createAgentMeshes.write` | DO NOT write `setMatrixAt` on a part anywhere else: a parked car's kept instances are only what passed through `slotOf`. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `doorOpening` | Open fraction of one door, 0 shut to 1 fully open, from the simulation. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `from` | Reads a palette entry, with the indexing narrowed away. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `gaitClip` | The gaits, whose phase is the ground covered (one stride a cycle) and runs on from one into another. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `instanced` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `leanOf` | How far a two-wheeler leans into the bend it is on | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `levelsKey` | Every level a vehicle's parts are drawn at, as one number: a parked car's copied writes hold while it stays the same. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `partOf` | A part over `mesh`: nothing drawn, every upload clearing what was waiting to go. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `pick` | A slice of a hash as an index into `n` items. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `planOf` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `riderMoving` | How much a two-wheeler is under way, 0 stopped (a foot down) to 1 riding. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `rimSize` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `rung` | The part of `ladder` for `level`; null when nothing of it is left to draw there. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `smooth01` |  | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `standingRange` | The screen scales (CSS pixels a unit) over which a standing vehicle drawn at `own` from `pixels` keeps its own level and its parts | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `tyreSize` | The largest scale a road wheel's unit tyre and rim are drawn at (`placeWheels`): their levels are chosen by it. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `vehicleLook` | Everything about how one vehicle's occupants and windows look, from its id. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `watchUploads` | The GPU took the part's instances (three calls it once a matrix buffer is sent): nothing waits now. | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| | `windowPane` | How a door's window is drawn with a share `drop` of it lowered (0 shut, 1 fully down), written into `out`: what the glazing's prog | portado | render/agents/agent_renderer.cpp; sem janela aberta, pisca e pessoas de corpo inteiro nos bancos (pendências no CLAUDE.md) |
| `src/render/bakePool.ts` | **arquivo** | bake em workers | não se aplica | sem workers: render/agents/people_asset.cpp lê people.bin |
| | `bakeBodyInWorker` |  | não se aplica | sem workers: render/agents/people_asset.cpp lê people.bin |
| | `bakeDeferredInWorker` |  | não se aplica | sem workers: render/agents/people_asset.cpp lê people.bin |
| | `bakeRig` |  | não se aplica | sem workers: render/agents/people_asset.cpp lê people.bin |
| | `classRigInWorker` |  | não se aplica | sem workers: render/agents/people_asset.cpp lê people.bin |
| | `personFaceInWorker` |  | não se aplica | sem workers: render/agents/people_asset.cpp lê people.bin |
| | `wearInWorker` |  | não se aplica | sem workers: render/agents/people_asset.cpp lê people.bin |
| `src/render/blast.ts` | **arquivo** | explosão | de outra funcionalidade | funcionalidade `destruction` |
| `src/render/blobShadows.ts` | **arquivo** | sombras simples das pessoas | de outra funcionalidade | funcionalidade `people` |
| | `BlobShadows` |  | de outra funcionalidade | funcionalidade `people` |
| | `LIFT` |  | de outra funcionalidade | funcionalidade `people` |
| | `SEGMENTS` |  | de outra funcionalidade | funcionalidade `people` |
| | `blobInto` |  | de outra funcionalidade | funcionalidade `people` |
| | `blobMesh` |  | de outra funcionalidade | funcionalidade `people` |
| | `createBlobShadows` |  | de outra funcionalidade | funcionalidade `people` |
| | `createBlobShadows.add` | One under `matrix` (its origin on the ground): `radius` across and `length` along, in the matrix's own units. | de outra funcionalidade | funcionalidade `people` |
| | `createBlobShadows.begin` | Starts a frame's list. | de outra funcionalidade | funcionalidade `people` |
| | `createBlobShadows.finish` | Ends the list: only what was written goes to the GPU. | de outra funcionalidade | funcionalidade `people` |
| | `createBlobShadows.grow` | `capacity` is where it starts, not a ceiling: DO NOT drop what passes it | de outra funcionalidade | funcionalidade `people` |
| `src/render/carBody.ts` | **arquivo** | carroceria do carro | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `BLACKOUT` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `CAB_HEEL_FORWARD` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `CAB_HIP_OVER_FLOOR` | A van's cab seat: the hip over the floor, and the heels ahead of it (`riderPoses` CAB poses). | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `CARD` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `CARPET` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `CAR_PACKAGE` | Where a person sits, relative to the seat's HIP POINT, metres | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `CHROME` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `COVER` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `CarShape` | The surface description a body is built from, kept for the anchors and the tests. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `CarStyle` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `DARK_TRIM` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `DASH` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `Face` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `GRILLE` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `HEADREST` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `LINER` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `M` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `MIRROR_GLASS` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `P2` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `Pt` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `Region` | What a face of the body is. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `SEAM` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `SEAT_FABRIC` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `SEAT_SIDE` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `STYLES` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `Style` | A style's shape, metres from the middle of the car (x, forward positive) and from the road (y). | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `UPRIGHT_CUSHION_REACH` | How far an upright seat's cushion (a van's or a truck's cab, a bus's) reaches ahead of the hip point, metres | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `WHEEL_RIM` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `WHITE` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `axlesOf` | Axle positions, world units (the renderer's wheels are placed there). | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `bar` | A bar between two points, `thick` square in section. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `box` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.archTop` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.capGeometry` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.crownOf` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.fromFaces` | Builds a geometry from quads, with the surface's smooth normals and per-face colours. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.fromFaces.put` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.inArchX` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.inDoor` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.isGlass` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.isRear` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.isRoof` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.isScreen` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.nearB` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.nearEdge` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.noseAt` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.onRoof` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.ringPoint` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.rowOf` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.seatAt` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.sectionAt` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.sectionAt.bulge` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.sectionAt.gy` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.sectionAt.push` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.sectionAt.side` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.sectionAt.sideG` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.tailAt` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `buildCarModel.vid` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `carShape` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `carShape.beltLine` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `carShape.bottom` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `carShape.halfWidth` | Half the width at the waist, at x. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `carShape.sideAt` | Half the width of the glasshouse at height y over x. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `carStyleOf` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `carStylesFor` | The styles a class may be drawn as. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `clamp01` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `extrude` | A side profile extruded across, bevelled, centred on z. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `headrest` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `headrestAt` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `lerp` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `merge` | Merges parts into one indexed geometry with position, normal and colour (white where none): corners equal in all three shared, so  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `monotone` | Monotone cubic interpolation (Fritsch-Carlson) through points sorted by x: smooth, and never overshooting | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `roundedSlab` | A rectangle rounded in plan, `height` tall, bottom at y. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `seatIndex` | The seat a door serves and a seat's number: the driver's side first, front to back, then the kerb side. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `seatPoints` | A seat's hip point for a style: x along, z across (right positive), y up. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `seatShell` | A seat's side profile extruded across: cushion under the hip point, backrest behind it. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `steeringWheel` | A steering wheel: rim, three spokes and a boss, axis along X, centred on the origin. | já existia | render/agents/car_body.cpp, car_shape.cpp |
| | `tint` |  | já existia | render/agents/car_body.cpp, car_shape.cpp |
| `src/render/citizenBake.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CitizenClipKey` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/citizenCasting.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `Casting` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.beginFrame` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.candidates` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.candidates.of` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.census` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.codes` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.constructor` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.drawn` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.dressesMembers` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.drop` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.forgetCompany` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.join` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.members` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.note` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.pickCitizenModel` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.recode` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.recordCensus` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CastingRegistry.wearers` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `codesFor` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `codesFor.rotate` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `codesFor.withTradition` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `hashOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/citizenGait.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `add` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `approach` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `clipSpeed` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createGait` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `drawnVelocity` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `fractionAtTravel` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `fractionAtYaw` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `gaitHeading` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `gaitPlays` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `moving` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `moving.share` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `naturalPace` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `pace` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `play` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `put` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `runAt` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `smoothstep` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `standing` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `standingKey` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `startEntry` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `stepGait` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `stopEntry` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `travelAt` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `walkStyle` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `walksOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `wrap` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/citizenPalette.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `blendPackedFrames` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `copyPackedFrame` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/citizenPlays.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `animationOf.refill` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `animationOf.rowOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `appendClip` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `cpuPose` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `gpuPlays` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/densityCars.ts` | **arquivo** | carros de densidade dos ladrilhos distantes | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `CarTile` | The cars of one tile: a mesh culled by the tile's sphere, its links (slots), and whether one of them changed since it was written. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `DensityCars` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `FAR_LANE` | A far road's lane, for its cars' places across it (its profile is the store's, not on the page). | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `LINK_POINTS` | Points of a road's centreline in the texture, evenly spaced along it (a bend between two of them is a straight chord). | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `MOST_CARS` | Most far cars drawn: past it the roads of a tile written last are not written. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `RECENTRE` | How far the view's centre moves from the window's before the window is moved. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `RESEND_MS` | Seconds of the wall a window message may go unanswered (the worker not up yet) before it is sent again. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `ROADS_PER_ROW` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `SIGNATURE` | A link's numbers its cars are written from (`SIGNATURE` floats a slot): their count, queue, speed, lanes, road length and flags. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `TEX_WIDTH` | Texels a row of the centreline texture: a whole number of roads a row. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `TILE_RADIUS` | A tile's sphere: its half diagonal and the land's heights, with a car's room over the highest deck. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `TILE_START` | A tile's mesh starts with room for this many cars; it doubles with them. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `TIME_WRAP` | The shader's clock wraps here (seconds): a float keeps a car's place to a few centimetres. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `VERTEX_PARS` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `VERTEX_PLACE` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `VERTEX_TRANSFORM` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `carGeometry` | The car: a body and a cabin, 4.3 m by 1.8 m, its length along x, its width along z, standing on y = 0. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars` | `heightAt`: the road's deck at a point, or the ground's for a road not on the page (`renderer.ts` deckHeight). | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.dispose` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.dropTile` | A tile with no road left: its cars out of the count, its mesh off the group, its geometry's buffers released (three keeps them unt | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.fileSlot` | The slot's link filed in the tile of its middle (`ML.mx`, `ML.my`); the tiles it left and joined written again. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.noteChanges` | The tiles of the slots whose numbers the meso step changed (their count, queue, speed, lanes, line or flags), marked to be written | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.readLines` | The slots of the deltas whose link or line the worker changed (`ML.shape`): their link noted and their line written. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.roomFor` | Room in the texture for slot `slot`: rows doubled, what was written kept. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.syncLines` | DO NOT write every line again for an edit or a tile come or gone: only the slots of the segments the network's delta names (a road | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.tellWindow` | The window told the worker (again when the view moved far enough, or nobody answered); whole while no delta came (every link sent) | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.tileAt` | The tile at (cx, cy), made on first use. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.tileGeometry` | A geometry for a tile with room for `n` cars (the old one let go: three keeps a geometry's buffers until it is disposed), its sphe | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.track` | Room to note `n` slots' links. | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.update` | Each frame: the window told the worker, the roads' lines the network or the meso changed written, the cars of a new meso step writ | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.watch` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.writeLine` | One slot's line into the texture, its range marked for upload: the page's road by its centreline and deck, a far one straight thro | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.writeTile` | One tile's cars of the meso step: each road's count, its queue packed at its end, the rest moving round the free stretch; only its | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `createDensityCars.writeTile.put` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `farLateral` | A far road's car across it, to the right of its travel (`laneOffset` without the road's profile: a two-way road's lanes right of i | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `letGoTile` | A tile's geometry let go: the car's shape it shares with every tile (its index, position and normal) taken off first, so three fre | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| | `makeTexture` |  | não se aplica | mapa não ladrilhado; o meso não desenha longe |
| `src/render/destruction.ts` | **arquivo** | destroços | de outra funcionalidade | funcionalidade `destruction` |
| `src/render/elements.ts` | **arquivo** | camada de efeitos | de outra funcionalidade | funcionalidade `destruction` |
| `src/render/exhaust.ts` | **arquivo** | fumaça do escapamento | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `EXHAUST_LIFE` | Seconds an exhaust puff lives. | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `Exhaust` |  | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `POOL` | Puffs alive at once, map-wide. | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `PuffKind` | 0 exhaust, 1 dust (brown-grey, low and wide), 2 dark smoke (rises), 3 concrete dust (pale, billowing), 4 blood spray | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `createExhaust` |  | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `createExhaust.burst` | `count` puffs of `kind` at once, scattered over `spread` round (x, y, z) | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `createExhaust.dispose` |  | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `createExhaust.emit` | A vehicle at (x, y) on a deck at height `z`, heading `angle`, `length` long, moving at `speed` (units/s) | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `createExhaust.spawn` |  | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| | `createExhaust.tick` | Advances the clock the shader ages puffs by. | de outra funcionalidade | sistema: Porte 7; emissão por classe: Porte 5 (project/data/render/agents.json exhaust) |
| `src/render/groundShear.ts` | **arquivo** | inclinação no chão | já existia | publish_poses (frente e trás) |
| | `Gradient` | FEET ON A SLOPE, WITHOUT IK. | já existia | publish_poses (frente e trás) |
| | `MAX_SHEAR_GRADIENT` | Steepest gradient a body is sheared to; beyond it the ground is a step, not a slope. | já existia | publish_poses (frente e trás) |
| | `groundGradient` | The ground's gradient at a point, from two forward differences off the height already known there. | já existia | publish_poses (frente e trás) |
| | `groundGradient.clampGradient` |  | já existia | publish_poses (frente e trás) |
| | `shearMatrix` | Premultiplies `matrix` (three's axes: world y is -Z) by the shear about the body's centre (X0, Z0): Y += gx (X - X0) | já existia | publish_poses (frente e trás) |
| `src/render/interiorSolids.ts` | **arquivo** | sólidos do interior | já existia | render/agents/agent_cabin.cpp |
| | `CabinParts` | The cabin's parts as they are added, each named; merged by the caller in the same order. | já existia | render/agents/agent_cabin.cpp |
| | `CabinParts.add` | Adds parts of one kind, belonging to `seats` (a seat, several, or none). | já existia | render/agents/agent_cabin.cpp |
| | `CabinParts.geometries` |  | já existia | render/agents/agent_cabin.cpp |
| | `CabinParts.named` |  | já existia | render/agents/agent_cabin.cpp |
| | `CabinParts.solids` | The solids as index ranges of the merge of `geometries` and then `after` (a closed interior's door cards): one a piece. | já existia | render/agents/agent_cabin.cpp |
| | `CabinParts.solids.take` |  | já existia | render/agents/agent_cabin.cpp |
| | `InteriorSolid` | One closed piece of the cabin (a box, an extruded profile, a bar), named | já existia | render/agents/agent_cabin.cpp |
| | `InteriorSolidKind` | THE SOLIDS OF A VEHICLE'S INTERIOR, BY NAME. | já existia | render/agents/agent_cabin.cpp |
| `src/render/lightLevels.ts` | **arquivo** | níveis de luz | já existia | render/agents e project/render/daylight.gd |
| | `nightGain` |  | já existia | render/agents e project/render/daylight.gd |
| `src/render/mesh/simplify.ts` | **arquivo** | simplificar malha em tempo de carga | não se aplica | os dois níveis são feitos em render/agents |
| | `simplified` | A coarser copy of a mesh for when it is a few pixels big | não se aplica | os dois níveis são feitos em render/agents |
| `src/render/occupants.ts` | **arquivo** | pessoas nos bancos | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `DOOR_SWING` | How far a door swings open, radians. The renderer swings the leaves by it. | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `KerbFigure` | One frame of somebody moving between a seat and the footway. | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `KerbPlaces` | Everything the choreography needs from the body model, in its frame (X forward, Z right). | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `LEAF_CLEARANCE` | Clearance kept from the edge of an open door leaf by a body walking past it, metres. | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `OccupantClip` | Anything the rigged citizens can play (`riggedCitizens.CitizenClipKey`, without the import cycle). | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `Play` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `REAR_CAR_CLIPS` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `SEAT_CLIPS` | The still poses a seat is sat in: the base, and the two glances blended into it. | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `STAND_UP_REACH` | How far the feet are ahead of the pelvis at the start of the captured stand-up (`standUp`), metres, over the roster | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `clipsFor` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `createKerbFigure` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `distanceToSegment` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `kerbFigure` | Somebody getting out (`drop`) or in (`pick`), at a moment of the transfer | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `kerbRoute` | Where the person stands up, and the way they walk from there to the footway. | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `leafTip` | The open leaf's free end, vehicle frame, for a door swung by `open` (0 to 1). | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `lerp` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `mix` | A cheap integer hash, for the rhythm of one person's glances. | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `occupantPlays` | A seated person's pose at `time` (seconds of their own), written into `out` | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `onePlay` | DO NOT push a new object a call. | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `pointSegment` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `seatBaseClip` | The pose somebody sits still in, in this seat. | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `segmentsCross` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `segmentsCross.o` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| | `smooth` |  | já existia | render/agents/agent_cabin.cpp e agents.gdshader |
| `src/render/pagePorts.ts` | **arquivo** | portas da página | não se aplica | sem workers |
| | `provideLanes` |  | não se aplica | sem workers |
| `src/render/paintQuads.ts` | **arquivo** | cores da pintura de estacionamento | portado | data/render/streets.json markings |
| `src/render/people/classRigMade.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `canShare` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `rigFromMade` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `sharedCopy` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/cookPack.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `unpackRecord` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/crowdAnimation.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `buildClassAnimation.refill` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `buildClassAnimation.rowOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createPalettePass.grow` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createPalettePass.render` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createPalettePass.rest` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createPalettePass.setAtlas` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createPalettePass.setOverride` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createPalettePass.setPlays` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/crowdBodies.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `canShare` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createClassBodies` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createClassBodies.bake` | Body `b`'s every vertex move, baked from its coefficients here, sent up at the next flush (a restore). | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createClassBodies.flush` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createClassBodies.make` | A body made from coefficients `c`: its vertices baked (in a worker where one can), every joint's move. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createClassBodies.pick` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createClassBodies.restore` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/crowdLod.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CardSelection` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `FarPart` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `boneZone` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `cardCentres` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `cardSelections` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `cardSelections.find` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `farMesh` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `filterTriangles` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `headZone` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `itemShownAt` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `levelFor` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `positionsOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `simplifiedIndex` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `simplifiedIndexAway` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `sortSkinWeights` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/faceExpression.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `applyFace` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/gpuRows.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `Step` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `copyPass` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createGpuRows` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createGpuRows.copy` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createGpuRows.flush` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createGpuRows.grow` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createGpuRows.reset` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createGpuRows.write` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `drawRows` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `makeTarget` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `wholeRows` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/hairTexture.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `CACHE` | The strand atlas a procedural hair card samples (`people/hair/procedural.ts`), as DATA, not colour | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `W,H` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `hairStrandTexture` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `strandsInto` | The strands of `kind`, the same at every call: coverage, how far along, and a seed a strand (RGB). | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `strandsInto.rnd` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/heldProps.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createHeldProps.begin` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createHeldProps.place` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createHeldProps.placeMatrix` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createHeldProps.put` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/paletteTargets.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `growSlab` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/proceduralBake.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `classFromRecord` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `hairCards` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `packFromRecord` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/proceduralCook.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `clipOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `cookedNames` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `loadProcedural` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/proceduralCrowd.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `LodPlan` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `PersonState` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `Posture` | A wounded body's posture over its clip (Euphoria's reach-for-wound and a body guarding a wound) | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `PostureRig` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `ProceduralCrowd` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `ProceduralStats` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `StoredPlan` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `Wear` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `bandOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.add` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.applyJolt` | The upper body turned about the lower spine by `angle`, in a palette row at `at`. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.applyPosture` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.bareSkin` | The skin for a person: never one of the made-up skins | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.buildClass` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.buildClass.headOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.buildClass.reloadHead` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.buildClass.visit` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.buildFar` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.byDistance` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.charSlots` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.classFor` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.classOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.classRigFor` | A class's rig and how its items are fitted: made on the people's worker (its arrays shared, nothing of what it sent kept), or here | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.classRigFor.fit` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.clipDuration` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.closeLimb` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.coverRow` | A garment's cover row in its class, made on first use. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.faceShare` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.growPiece` | DO NOT dispose the geometry for its old per-person attributes | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.growRows` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.instanced` | An instanced attribute of `size` floats an entry, for a piece of `capacity` people. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.item` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.item.at` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.lodPlan` | How an item is drawn at each level (`LodPlan`), worked out once for every class. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.makeItem` | An item fitted to a class, drawn at every level it has: its vertices (`source`) once, an index and a mesh a level | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.makeItem.indexAt` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.makeLodPlan` | The simplifying itself by the worker (`simplifiedIndexAway`), every level at once. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.makeLodPlan.attr` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.makePiece` | A piece: an instanced mesh over `geometry` (an index over its item's vertices), with its own entries a person. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.moveTo` | Drawn at `level` from now on (-1: not drawn). | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.overridden` | Whether a person's palette is worked out here this frame rather than by the GPU pass. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.place` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.poseRow` | A person's palette row worked out here: held by a ragdoll | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.posed` | Where a joint (bind point `bind`) is in the posed row `at`, bone `i`. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.postureRig` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.postureRig.arm` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.postureRig.bindOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.postureRig.find` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.postureRig.name` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.postureRig.subtree` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.reachTo` | One arm's hand sent towards `target` by `weight` (0..1), a two-bone solve | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.refit` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.refresh` | Their entries written again (wounded, burnt). | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.renderPalettes` | The skeletons worked out on the GPU (`crowdAnimation.ts`): once a frame, after `update`, before the scene is drawn. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.restore` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.setFace` | Row `row`'s own face: the regional sliders on the class body, posed as it is (zero: none), sent up at the next flush. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.skinItem` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.strandMap` | A strand ribbon's texture: solid across (its edges fade in the shader), root to tip in G. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.strandPiece` | A grown style's strands on a class body: a piece like the cards, filled each frame with the nearest people. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.strandsUpdate` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.stride` | Ground one walk cycle covers at the person's scale, metres (of `clip`, their own by default). | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.takeRow` | A row of a class for a person (a spare one first when `spare`). | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.textureOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.turnAbout` | Every bone of `set` turned by `rot` about point `p`, in the row `at`. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.unplace` | A person's slot in a piece given up: the last slot moved into it. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.update` | `strands`: how many of the nearest people get their hair's strands (the tier's, `quality.ts` hairStrandPeople). | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.upload` | A piece's frame to the GPU: its count, and its entries when they changed - only the written part (the people's places are their cl | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.warmClasses` | Every class made ahead, one at a time: its rig on the people's worker (`classRigInWorker`), the page only putting it together | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.wornItem` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.woundSpots` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.woundedRow` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.writeEntry` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.writeFace` | Row `row`'s face as worked out in the worker (null: none, zero moves), sent up at the next flush. | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.zoneColours` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.zoneColours.layer` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createProceduralCrowd.zoneCover` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `farColour` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `headFrame` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `kindOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `meanColour` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `personRows` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `plainTexture` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `proceduralLook` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `proceduralLook.mouth` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `roleOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `rowTexture` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `textureSampler` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `textureSampler.lin` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `touchRows` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `vertexZones` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `woundSpotsOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `woundUnit` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `zoneAreas` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/people/skinAppearance.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `itemTexture` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `skinTextures` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/playEffects.ts` | **arquivo** | efeitos de jogo | de outra funcionalidade | funcionalidade `destruction` |
| `src/render/ragdoll.ts` | **arquivo** | corpos atropelados | de outra funcionalidade | funcionalidade `destruction` |
| `src/render/renderer.ts` | **arquivo** | liga os agentes à cena | já existia | render/render_world.cpp |
| `src/render/riggedCitizens.ts` | **arquivo** | corpo e animação das pessoas | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `ClipIdentity` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `citizenBandAt` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `citizenLevelAt` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `citizenLevelFor` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `citizenLevelOfFigure` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `companyOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.addToAtlas` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.begin` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.bodyFor` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.capturedPose` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.census` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.clipPose` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.contextOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.dispose` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.draw` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.drawClip` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.drawPalette` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.emit` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.finish` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.forgetCompany` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.grow` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.helmetFits` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.indexFor` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.keepAll` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.levelOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.loadedIndices` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.maim` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.place` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.pose` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.pump` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.renderPalettes` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.request` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.skeletonOf` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `createRiggedCitizens.want` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `growLevel` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `seatedChat` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| | `setFacialExpression` |  | de outra funcionalidade | funcionalidade `people` (o rastreamento os alcança pela ocupação dos veículos) |
| `src/render/roadSurfaceInputs.ts` | **arquivo** | superfície e pintura com estacionamento | portado | render/roads/road_markings.cpp |
| `src/render/signals.ts` | **arquivo** | cabeças dos semáforos | já existia | render/props/signal_lamps.cpp |
| `src/render/vehicleFrame.ts` | **arquivo** | referencial do veículo | já existia | sim/traffic/traffic_system.cpp publish_poses |
| | `MAX_FRAME_PITCH` | Steepest gradient and camber a frame will take from its samples. | já existia | sim/traffic/traffic_system.cpp publish_poses |
| | `MAX_FRAME_ROLL` |  | já existia | sim/traffic/traffic_system.cpp publish_poses |
| | `RoadFrame` | A vehicle's one frame on the road: height, pitch and roll, from the road surface under its wheels. | já existia | sim/traffic/traffic_system.cpp publish_poses |
| | `RoadFrameOut` | A frame written in place (`roadFrameInto`). | já existia | sim/traffic/traffic_system.cpp publish_poses |
| | `roadFrame` | The frame through the road under the axles and the sides. | já existia | sim/traffic/traffic_system.cpp publish_poses |
| | `roadFrameInto` | `roadFrame` into `out`: the renderer asks it for every vehicle near enough, every frame. | já existia | sim/traffic/traffic_system.cpp publish_poses |
| `src/render/vehicleLevels.ts` | **arquivo** | níveis de detalhe | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| | `ERROR_PIXELS` | A level is drawn only while its error on the screen is under this: the swap is not seen (meshoptimizer README, `ErrorAbsolute`). | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| | `LEVELS` | Levels of a part: 0 is the model's own geometry, `i + 1` the one within `LEVEL_ERRORS[i]`. | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| | `SHED` |  | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| | `attributeOf` | An attribute from the numbers sent back. | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| | `copyOf` | A copy of an attribute's numbers, to be moved to the worker (the part keeps its own). | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| | `levelFor` | The coarsest level whose error stays under `ERROR_PIXELS` for a part drawn at `pixelsPerUnit` (pixels a unit of its own geometry). | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| | `partLevels` |  | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| | `trianglesOf` |  | já existia | render/agents/agent_renderer.cpp (dois níveis) |
| `src/render/vehicleModels.ts` | **arquivo** | modelos: ônibus, caminhão, moto e bicicleta | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `BLACKOUT` | Black-out on paint: B-pillars, window surrounds, a bus's pillars. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `BUS_FLOOR` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `BUS_SEAT` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `BUS_SEAT_RECLINE` | How far a bus seat's back leans, radians back from upright: as far as the shoulders of a seated passenger | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `BUS_SHELL` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `CAB_EXTENTS` | The same for a bus or truck driver's upright seat. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `CARD` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `CARPET` | Cabin colours, sRGB. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `CHAIR_EXTENTS` | The same for a bus seat (`chairSit` and its variations). | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `CHROME_TRIM` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `DARK_TRIM` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `DASH` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `DoorModel` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `DoorSide` | Side of the vehicle a door is on: -1 left (the driver's), +1 right (the kerb's). | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `GRILLE` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `HEADREST` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `Lamp` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `M` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `P2` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `PANEL_GREY` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `POLE_YELLOW` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `REAR_SEATED_EXTENTS` | Rear car passengers fold their legs into the shorter footwell. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `RUBBER_TRIM` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SEAM` | Shut lines and seams on paint. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SEATED_EXTENTS` | How far a seated person reaches from their pelvis, metres at full size: the worst of the roster's bodies | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SEAT_FABRIC` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SEAT_SIDE` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SHUT` | Gap left at each end of a truck cab's door leaf, so it clears its opening as it swings. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SeatModel` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SeatPose` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SeatedExtents` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `SteeringModel` | Where a steering wheel sits and how it is inclined, in the vehicle frame. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `TwoWheelerModel` | A motorcycle or a bicycle, in the same frame as a car, with the parts that move kept apart | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `VehicleModel` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `WHEEL_RIM` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `WHITE` | Vertex colours: they multiply the instance colour, so these are shades. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `archOutline` | A mudguard's outline over a wheel: a half ring. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `axleStations` | Axle positions along the body, front first, world units. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `bar` | A bar between two points in 3D, `thick` square in section. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `bicycleModel` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `box` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildBusModel` | A low-floor city bus. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildBusModel.archTop` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildBusModel.blocked` | A row on the floor where a seated body does not fit, measured against the bodies (`tests/render/interiorSolids.spec.ts`) | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildBusModel.inDoorway` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildBusModel.overWheel` | The rear wheel a row of seats (shell back to the sitter's feet) stands over, if any. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildBusModel.sideRoomOf` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildBusModel.wallOutline` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildTruckModel` | A rigid truck: a cab over the front axle with a raked windscreen, a roof deflector | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `buildTwoWheelerModel` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `busSeat` | A bus seat: a moulded shell on a pedestal, a grab handle on top. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `clean` | Drops consecutive duplicates and a closing point equal to the first. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `clipX` | Points of a polyline monotone in x, clipped to [xa, xb]. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `clipX.push` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `extentsOf` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `extrude` | Extrudes a side profile across the body, centred on Z (or on `z`). | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `farWheels` | Plain dark wheels for the far LOD. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `gripInHead` | A grip's centre in the steering frame (origin on the steering head, X forward, Z right). | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `handlebar` | Rubber grips and a swept bar from a clamp on the steering head out to them. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `headrest` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `headrestAt` | Where a headrest goes for a seat reclined by `recline`. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `lerp` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `loft` | A surface lofted along a profile: at each station | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `merge` | Merges parts into one indexed geometry with position, normal and colour: corners equal in all three shared, so a hard edge stays s | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `motorcycleModel` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `polygon` | A flat polygon mapped into 3D, triangulated in its own 2D coordinates, and wound so its face points along `outward`. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `rimGeometry` | A five-spoke rim, axis along Z, unit diameter, one unit wide. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `roundedSlab` | A rectangle rounded in plan (X-Z), `height` tall, bottom at `y`. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `seatFitScale` | The largest size a person may be drawn at in this seat and stay entirely inside the cabin | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `seatFitScale.metres` |  | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `seatShell` | A seat's side profile, extruded across its width: cushion, backrest and a rounded front edge, reclined by `recline` | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `spokedRimGeometry` | A bicycle's wheel inside its tyre: a thin rim, a hub and spokes; unit diameter. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `spreadOrder` | The order the simulation's passenger seats take the bus's seats in | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `steeringWheel` | A steering wheel: rim, three spokes and a boss, axis along X, centred on the origin. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `strut` | A bar from (x0, y0) to (x1, y1) at `z`, `wide` along the body and `deep` across it. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `tint` | Sets a constant vertex colour on a geometry, in place, and returns it. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `twoSided` | A polygon given twice, facing both ways, a sliver apart: a pane seen from inside and out. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `tyreGeometry` | A tyre with a rounded shoulder, axis along Z, unit diameter. | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| | `weldPart` | A part with only position, normal and colour, indexed: a loose triangle list welded where position, normal and colour agree (three | portado | render/agents/heavy_meshes.cpp (bicicleta nova) |
| `src/render/vehiclePanels.ts` | **arquivo** | portas como painéis móveis | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| | `JoinedPanels` | The vertices and index of a body and panel joined: shared by every geometry drawn from them (`panelGeometry`). | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| | `NAMES` | The attributes a body and its panel carry (`vehicleModels.ts`, `vehicleLevels.ts`): both must have the same ones to be joined. | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| | `createPanelJoiner` | Joins bodies and panels, each pair once: the levels of a part share their vertices (`vehicleLevels.ts` shared), so do the joined o | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| | `createPanelJoiner.verticesOf` | The body's vertices, then the panel's, end to end, and `panel` marking the panel's. | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| | `indicesInto` | A geometry's triangles as indices (its own, or 0..n-1 for one drawn unindexed), offset by `base`, into `out` at `at`. | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| | `joinAttribute` | The parts' attribute `name` end to end, and the panel mark; undefined when neither has it, null when they cannot be joined. | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| | `panelGeometry` | A geometry of its own over joined vertices (they are shared), with this mesh's panel colours (an instanced attribute is a mesh's o | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| | `panelPaint` | The body paint, its panel's vertices coloured by the instance's `panelColor` instead of `instanceColor`: the same material in ever | não portado | portas não abrem: o carro para, o tempo da porta corre (sim/traffic/kerb_stops.cpp), a folha não gira; pendência no CLAUDE.md |
| `src/render/vehicleWindows.ts` | **arquivo** | vidros | já existia | render/agents/agent_parts.cpp |
| | `WINDOW_DROP` | The per-instance attribute: the left side's drop, the right side's (0 shut, 0.45 part-way, 1 fully down). | já existia | render/agents/agent_parts.cpp |
| | `marked` | Every vertex of `g` marked as one pane: its side and its sill. | já existia | render/agents/agent_parts.cpp |
| | `sideMark` | The pane mark of a door's side: what the drop of `drawCar`'s `windowsDown` bit for that side moves. | já existia | render/agents/agent_parts.cpp |
| | `windowGlass` | The shut glazing of a body: its fixed glass, then each door's pane where the door shuts, marked. | já existia | render/agents/agent_parts.cpp |
| | `windowGlassMaterial` | The glass `glass` is, its panes lowered by each instance's `windowDrop`: a program of its own (the other glass has no drop to read | já existia | render/agents/agent_parts.cpp |
| `src/sim/agents/bay.ts` | **arquivo** | a vaga e a faixa que a alcança | portado | sim/traffic/parking_state.h |
| | `Bay` |  | portado | sim/traffic/parking_state.h |
| | `BayLane` |  | portado | sim/traffic/parking_state.h |
| | `bayWork` | The bays worked out here, counted (`tests/sim/tileBays.spec.ts`): lots worked out or asked of the workers, streets laid out. | portado | sim/traffic/parking_state.h |
| `src/sim/agents/buildingOrder.ts` | **arquivo** | ordem dos prédios no diário do worker | não se aplica | BuildingStore e ChangeLog |
| | `BuildingDelta` | What changed among the buildings since a reader last looked. | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder` | The buildings that pass `keep`, in the store's order, kept from the diary. | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.constructor` | `keep`: asked of a building's facts, never its traffic record (`TrafficBuildings.facts`: the worker works no record out for it). | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.doc` | The document followed: another one is read whole. | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.epoch` | Moves each time the order is read whole: ranks of before and after are not comparable. | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.file` |  | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.filedAt` | Each kept building and the rank it was filed in the list at. | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.has` |  | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.ids` | The ids the diary names, each once. | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.kinds` |  | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.list` |  | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.merge` | The named buildings taken out of the list in one walk, those kept now filed back by one merge of the two orders. | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.order` | The store's order, this reader's own (it follows the diary with it). | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.rank` | A building's place in the store's order as of the last `update` (to sort other buildings by it). | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.reread` | @rebuild The order read whole: at the first look, or when the diary cannot say. DO NOT decode every record for its id: the ids alo | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.resort` | @rebuild The kept buildings in the store's order read again whole: its ranks were (`StoreOrder.reread`). | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.seenSerial` | What it has read up to (`JournalReader`): its diary keeps the buildings' entries after it until it reads them. | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.serial` |  | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.slot` | Where a rank stands, or would stand, in the list (the list is in the order of the ranks it was filed at). | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.unfile` |  | não se aplica | BuildingStore e ChangeLog |
| | `BuildingsInOrder.update` | Up to date with the store. | não se aplica | BuildingStore e ChangeLog |
| | `MERGE_PAST` | Past this many named in one update, the list is walked once and merged (`merge`), not spliced a building at a time. | não se aplica | BuildingStore e ChangeLog |
| | `NOTHING` |  | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder` | The store's order as ranks, kept from the diary. | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.apply` | One entry of the diary applied to the ranks; false when its effect on the order is not known. | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.constructor` | `keep`: asked of a building's facts, never its traffic record (`TrafficBuildings.facts`: the worker works no record out for it). | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.doc` | The document followed: another one is read whole. | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.epoch` | Moves each time the order is read whole: ranks of before and after are not comparable. | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.follow` | Brought up to the diary's latest entry. | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.has` |  | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.next` |  | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.place` |  | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.rank` | A building's place in the store's order as of the last `update` (to sort other buildings by it). | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.ranks` |  | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.reread` | @rebuild The order read whole: at the first look, or when the diary cannot say. DO NOT decode every record for its id: the ids alo | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.serial` |  | não se aplica | BuildingStore e ChangeLog |
| | `StoreOrder.slots` | Each building's rank in a column at its id's slot (`core/idSlots.ts`): DO NOT keep a `Map` entry a building of the city. | não se aplica | BuildingStore e ChangeLog |
| `src/sim/agents/hybridAStar.ts` | **arquivo** | manobra do caminhão: busca por poses | portado | sim/traffic/way_search.cpp |
| | `AxlePose` | A pose of the rear axle: where it is and where the body points. | portado | sim/traffic/way_search.cpp |
| | `Clearance` | Distance from each cell's centre to the nearest solid cell's centre (0 in one), world units. | portado | sim/traffic/way_search.cpp |
| | `DISC_STEP` | Most spacing of the axis discs. | portado | sim/traffic/way_search.cpp |
| | `Discs` | What a body is tested with: discs along its axis as wide as the body and a little | portado | sim/traffic/way_search.cpp |
| | `HEADINGS` |  | portado | sim/traffic/way_search.cpp |
| | `H_RS_REACH` | The Reeds-Shepp length guides the search this near the goal. | portado | sim/traffic/way_search.cpp |
| | `H_WEIGHT` | The heuristic's weight: a little over one finds a way sooner, a little longer than the shortest. | portado | sim/traffic/way_search.cpp |
| | `Holonomic` | The rear axle's shortest distance to a goal from every cell (`holonomic`). | portado | sim/traffic/way_search.cpp |
| | `MAX_EXPANDED` | Most poses expanded before giving up. | portado | sim/traffic/way_search.cpp |
| | `Node` |  | portado | sim/traffic/way_search.cpp |
| | `PlanOptions` |  | portado | sim/traffic/way_search.cpp |
| | `REVERSE_COST` | Costs, per unit of length driven: backing, and a change of direction (a stop and a start), and the wheel. | portado | sim/traffic/way_search.cpp |
| | `SHOT_EVERY` |  | portado | sim/traffic/way_search.cpp |
| | `SHOT_NEAR` |  | portado | sim/traffic/way_search.cpp |
| | `SHOT_REACH` | A Reeds-Shepp shot to the goal is tried from poses this near it (every pose within the inner reach, one in `SHOT_EVERY` beyond). | portado | sim/traffic/way_search.cpp |
| | `SHOT_STEP` | Spacing of the poses a shot is tested and driven at. | portado | sim/traffic/way_search.cpp |
| | `STEER_CHANGE_COST` |  | portado | sim/traffic/way_search.cpp |
| | `STEER_COST` |  | portado | sim/traffic/way_search.cpp |
| | `STEP` | One step's arc: a little longer than a cell's diagonal, so a step always leaves its cell. | portado | sim/traffic/way_search.cpp |
| | `SUBSTEPS` | Poses tested along a step. | portado | sim/traffic/way_search.cpp |
| | `SWITCH_COST` |  | portado | sim/traffic/way_search.cpp |
| | `Solid` | What is solid, on a grid: 1 for a cell a body may not touch. | portado | sim/traffic/way_search.cpp |
| | `Steered` | A vehicle as the planner sees it. World units. | portado | sim/traffic/way_search.cpp |
| | `WaySearch` | The search, run a slice at a time: the simulation gives it a few hundred poses a tick | portado | sim/traffic/way_search.cpp |
| | `WaySearch.byKey` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.constructor` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.discs` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.expanded` | Poses expanded so far. | portado | sim/traffic/way_search.cpp |
| | `WaySearch.h` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.hAt` | The cost still to go, at least: the paper's two heuristics' larger - round the obstacles for the axle alone, and | portado | sim/traffic/way_search.cpp |
| | `WaySearch.kappas` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.keyOf` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.nodes` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.nx` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.open` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.push` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.reach` | How near the goal's position and heading a pose must come. | portado | sim/traffic/way_search.cpp |
| | `WaySearch.run` | Expands up to `budget` more poses; the state says whether it is over. | portado | sim/traffic/way_search.cpp |
| | `WaySearch.shot` | The Reeds-Shepp way from a pose to the goal as poses, if its body touches nothing (and it arrives the way asked). | portado | sim/traffic/way_search.cpp |
| | `WaySearch.state` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.turn` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.turnCos` | Cosine and sine of each curvature's turn over a forward substep. | portado | sim/traffic/way_search.cpp |
| | `WaySearch.turnSin` |  | portado | sim/traffic/way_search.cpp |
| | `WaySearch.way` |  | portado | sim/traffic/way_search.cpp |
| | `Waypoint` | A pose on the way, and whether it is reached backing. | portado | sim/traffic/way_search.cpp |
| | `XY_CELL` | The search's own cells: this much across, and this many headings round. | portado | sim/traffic/way_search.cpp |
| | `bodyClear` | Whether the body at the rear-axle pose `(x, y, angle)` touches nothing solid (nor leaves the grid). | portado | sim/traffic/way_search.cpp |
| | `bodyClearCS` | `bodyClear` with the heading as its cosine and sine (the search turns them without trigonometry). | portado | sim/traffic/way_search.cpp |
| | `bodyClearExact` | Whether the body's rectangle at the pose covers no solid cell | portado | sim/traffic/way_search.cpp |
| | `clearAt` |  | portado | sim/traffic/way_search.cpp |
| | `clearanceOf` | The clearance field of a grid of solid cells: an exact Euclidean distance transform. | portado | sim/traffic/way_search.cpp |
| | `clearanceSteps` | `clearanceOf` in slices: it yields after about `slice` cells of work, so a yard's field | portado | sim/traffic/way_search.cpp |
| | `discsOf` |  | portado | sim/traffic/way_search.cpp |
| | `fillRing` | `fillRings` of one ring. | portado | sim/traffic/way_search.cpp |
| | `fillRings` | Sets every cell of the grid whose centre lies inside the polygon | portado | sim/traffic/way_search.cpp |
| | `holonomic` | Shortest distance from every cell to the goal for the rear axle alone | portado | sim/traffic/way_search.cpp |
| | `holonomic.free` |  | portado | sim/traffic/way_search.cpp |
| | `searchStats` | Poses expanded and searches made, all told (for probes and measures). | portado | sim/traffic/way_search.cpp |
| | `trace` | The poses from the start to node `id`. | portado | sim/traffic/way_search.cpp |
| | `transform1d` | One row of the squared distance transform | portado | sim/traffic/way_search.cpp |
| | `wrap` |  | portado | sim/traffic/way_search.cpp |
| `src/sim/agents/kerbBays.ts` | **arquivo** | vagas ao longo da guia, por rua | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `Box` |  | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `Entrance` |  | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `EntranceBook` | THE BUILDINGS' ENTRANCES AS NUMBERS IN COLUMNS (`core/idSlots.ts`), NOT THEIR RECORDS' ARRAYS: each building's run of entrances (e | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.box` | The box of a building's entrances kept; null for none. | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.cells` | The buildings whose entrances' box reaches each cell (`KERB_CELL`). | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.clear` |  | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.each` | Each of a building's entrances kept, ends (x, y) to (x, y). | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.file` | Files (or unfiles) a building by the cells a box reaches. | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.put` | A building's entrances now (none: let go), filed by the cells their box reaches. | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.runs` | Each building's run of entrances at its slot, four numbers each (`RunPool`: a run let go stays until the dead outweigh the live). | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.same` | Whether a building's entrances kept are these (none kept: `list` is empty). | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `EntranceBook.slots` |  | não se aplica | portarias lidas do corpo do lote (b40::record_body), sem colunas de worker |
| | `FRONTAGE_REACH` | Past this beyond a street's half width, a lot's entrance keeps no bay off its kerb (`parkingLayout.ts` `frontagesOn`). | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `KERB_AHEAD` | Ahead of a kerb bay, the place on the lane a car stops at to back in, and leaves for. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `KERB_CELL` | Cell of the kerbs' and entrances' grids, world units. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `KERB_MIN_AHEAD` | The least a kerb bay's joining point may lie ahead of it, u. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `KerbBook` | The bays along the kerbs of the streets that park (`RoadSegment.parking`), kept by street. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `KerbReads` | What a street's kerb bays read besides the network and the entrances, and what else is asked for them (`parking.ts`). | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `KerbWorld` | DO NOT import `SimWorld`: the world reaches this file, and the type import closed an import cycle | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `NO_LOT` | No lot: the bay stands on the street. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `entranceBox` |  | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `entrancesFacing` | The entrances that may face a street (`frontagesOn`'s box test), in the store's order. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `fileStreet` | A street that parks filed: its reach (its line's box, grown) and the cells it covers. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `followKerbs` | DO NOT lay out every street of the town when the roads move: every road drawn laid out the town's kerbs again. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `kerbBaysOf` | One street's kerb bays: laid out alone (`parkingLayout` over a network of this one street), against the entrances in front of it. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `kerbCell` |  | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `kerbKey` | DO NOT leave out anything `kerbBaysOf` reads: what one street's kerb bays are worked out from, digested (its lay out's own reads,  | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `kerbStreetsIn` | The streets of the book whose reach meets the rectangle, found by their cells (not every street of the map), in id order. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `newKerbBook` |  | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `readEntrances` | @rebuild Every building's entrances, read whole: the first look, or when the diary cannot say what changed. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `readStreets` | @rebuild The streets that park, read whole: the first look, a map opened, or roads further behind than the logs remember. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `sameBays` | Whether two lists of a street's bays are the same bays, field by field. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| | `unfileStreet` | A street taken out of the cells it was filed in. | portado | sim/roads/parking_layout.cpp e sim/traffic/parking_sources.cpp |
| `src/sim/agents/laneGrid.ts` | **arquivo** | faixas por lugar | já existia | SpatialHash do RoadNetwork |
| | `Filed` |  | já existia | SpatialHash do RoadNetwork |
| | `LANE_CELL` | Cell of the lanes' grid, world units. | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid` | DO NOT file every lane of the map again when the graph moves | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.cells` |  | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.changes` | The boxes of the lanes each follow changed, before and after (null: every lane). | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.digest` | The lane's digest (`digest`), worked out when first asked; NaN until then. | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.file` |  | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.fileAll` | @rebuild Every lane filed again. | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.filed` |  | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.follow` | Brought up to the lane graph's changes since it was last. | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.gather` | The lanes filed in the cells a box covers, each once, into `out` in no set order: DO NOT make a set and a list of them for each qu | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.inBox` | The lanes whose bounds meet a box, in id order: what a loop over every lane with the same bounds test keeps. | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.lanesIn` | The lanes filed in the cells a box covers, in id order. | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.near` | The lanes whose bounds come within `reach` of a point (their cells), in id order (the tie-break) | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.nearInto` | `near`'s lanes into `out`, in NO set order: for a reader whose answer does not depend on it (`SimWorld.vehiclesNear` sorts its veh | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.seen` |  | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.unfile` |  | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.visit` | The query that last met it (`LaneGrid.visit`): a lane in several of a query's cells is taken once. | já existia | SpatialHash do RoadNetwork |
| | `LaneGrid.work` | Lanes filed or taken out since made: the engine's ledger (`main.ts`). | já existia | SpatialHash do RoadNetwork |
| | `LaneGridWorld` | DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle | já existia | SpatialHash do RoadNetwork |
| | `boxOf` |  | já existia | SpatialHash do RoadNetwork |
| | `byId` |  | já existia | SpatialHash do RoadNetwork |
| | `cellKey` |  | já existia | SpatialHash do RoadNetwork |
| | `cellsMeet` | Whether the grid finds a lane of these bounds for a box (`LaneGrid.near`, `lanesIn`) | já existia | SpatialHash do RoadNetwork |
| | `digestOf` |  | já existia | SpatialHash do RoadNetwork |
| | `laneChangesSince` | The boxes of the lanes the lane graph changed since `seen` | já existia | SpatialHash do RoadNetwork |
| | `laneGrid` | The world's lanes' grid (`SimWorld.laneGrid`), brought up to the lane graph's changes. | já existia | SpatialHash do RoadNetwork |
| | `lanesInBox` | The lanes whose bounds meet a box, in id order (`LaneGrid.inBox`). | já existia | SpatialHash do RoadNetwork |
| | `lanesNear` | The lanes whose bounds come within `reach` of a point, in id order (`LaneGrid.near`). | já existia | SpatialHash do RoadNetwork |
| | `lanesNearInto` | `lanesNear` into `out`, in no set order (`LaneGrid.nearInto`). | já existia | SpatialHash do RoadNetwork |
| `src/sim/agents/lorryDocks.ts` | **arquivo** | pátio de um galpão como o caminhão o dirige | portado | sim/traffic/yard.cpp |
| | `AsphaltPiece` | The carriageways' polygons, each with its box in the world: a yard takes in only those near it. | portado | sim/traffic/yard.cpp |
| | `CELL` | Cell of a yard's grid. | portado | sim/traffic/yard.cpp |
| | `DOCK_GAP` | Room left between a docked lorry's tail and the building, and a body length's stretch of lane before a gate it stops at. | portado | sim/traffic/yard.cpp |
| | `GATE_LANES_READ` | How far from a yard's grid the lanes its gates may be joined to are read (`parking.ts` gateLane's reach, and a margin). | portado | sim/traffic/yard.cpp |
| | `JOIN_PAST` |  | portado | sim/traffic/yard.cpp |
| | `LORRY_MARGIN` | Kept between the lorry and anything solid while it is planned. | portado | sim/traffic/yard.cpp |
| | `LORRY_RADIUS` | The rear axle's least turning radius (SU-30, above). | portado | sim/traffic/yard.cpp |
| | `LorryWay` | A way a lorry drives, in the world: rear-axle poses with the direction each is reached in. | portado | sim/traffic/yard.cpp |
| | `MOUTH_FLARE` | The paved mouth in front of a gate a lorry may cross the footway by: the gate's width and this much more either side. | portado | sim/traffic/yard.cpp |
| | `NEAR_KINDS` | What a yard reads near it besides the roads: a change of these there is looked at (`knownYard`), elsewhere never. | portado | sim/traffic/yard.cpp |
| | `OVER_A_LORRY` | Parts this high or higher stand over a lorry (3.5 m tall). | portado | sim/traffic/yard.cpp |
| | `PLAN_BUDGET` | Poses expanded a tick, across all the yards being planned: under a millisecond (a dock's ways, some 50 000 poses, in a few seconds | portado | sim/traffic/yard.cpp |
| | `PRE_DOCK` | Farthest from the dock its last straight reverse starts (nearer leaves the lorry more of the yard to turn its nose in: 40 docks of | portado | sim/traffic/yard.cpp |
| | `Plan` |  | portado | sim/traffic/yard.cpp |
| | `REACH` | The goal's tolerance: within this of the pose, a short leg settles the rest. | portado | sim/traffic/yard.cpp |
| | `REAR_SHARE` | Where the rear axles are, as a share of the length behind the body's centre (the drawn model's axles, `render/vehicleModels.ts` ax | portado | sim/traffic/yard.cpp |
| | `SETTLE_STEP` | Spacing of the poses on a settling leg. | portado | sim/traffic/yard.cpp |
| | `SOLID` | What a lorry cannot drive through: what a car cannot (`lotNav.ts`), less the docks themselves. | portado | sim/traffic/yard.cpp |
| | `STOP_BEFORE` | The lorry stops this far before the gate on its lane and joins the lane this far past it (a lorry swings wider than a car). | portado | sim/traffic/yard.cpp |
| | `STREET_REACH` | How far beyond the lot the street is taken in, and how far from a gate along it. | portado | sim/traffic/yard.cpp |
| | `STREET_WINDOW` |  | portado | sim/traffic/yard.cpp |
| | `TURN` |  | portado | sim/traffic/yard.cpp |
| | `WorldYards` | The yards of a world, made when first asked and again when their building or the roads change | portado | sim/traffic/yard.cpp |
| | `YARDS` |  | portado | sim/traffic/yard.cpp |
| | `YARD_SLICE` | Cells of work a slice of a yard's making: some 2 ms. | portado | sim/traffic/yard.cpp |
| | `Yard` |  | portado | sim/traffic/yard.cpp |
| | `Yard.body` |  | portado | sim/traffic/yard.cpp |
| | `Yard.bodyInLot` | Whether the whole body at a pose stands in the lot. | portado | sim/traffic/yard.cpp |
| | `Yard.constructor` | Made by `buildYard`, whose grid and clearance it takes; here its gates and docks are worked out. | portado | sim/traffic/yard.cpp |
| | `Yard.docks` |  | portado | sim/traffic/yard.cpp |
| | `Yard.field` |  | portado | sim/traffic/yard.cpp |
| | `Yard.gates` |  | portado | sim/traffic/yard.cpp |
| | `Yard.grids` | The grid and its clearance, while a dock is still to be planned: DO NOT keep them for the yard's life (a works' grid is some 100 0 | portado | sim/traffic/yard.cpp |
| | `Yard.inputs` | What it was made from, near it (`yardInputs`): it stands while these do, whatever changed elsewhere. | portado | sim/traffic/yard.cpp |
| | `Yard.maps` | The distance maps of the searches on this yard's field, by goal (a dock's ways in from every gate share one): let go with the grid | portado | sim/traffic/yard.cpp |
| | `Yard.plan` | Plans on, up to `budget` poses expanded; how many it spent. Every dock planned, the grids are let go (the ways keep only poses). | portado | sim/traffic/yard.cpp |
| | `Yard.planDock` |  | portado | sim/traffic/yard.cpp |
| | `Yard.planned` | Whether a dock's planning is over (`gate` then says through which gate, -1 for none). | portado | sim/traffic/yard.cpp |
| | `Yard.reach` | The world box its grid covers: the lots and buildings it reads are those the diary places there. | portado | sim/traffic/yard.cpp |
| | `Yard.ring` | The lot's ring, local. | portado | sim/traffic/yard.cpp |
| | `Yard.seen` | The diary's serial its inputs were last read at (`knownYard`). | portado | sim/traffic/yard.cpp |
| | `Yard.settles` | Whether the short leg from where a search stopped to its exact goal is clear, cell by cell. | portado | sim/traffic/yard.cpp |
| | `Yard.solid` | The yard's grid, while it is kept (`plan` lets it go once every dock is planned). | portado | sim/traffic/yard.cpp |
| | `Yard.step` |  | portado | sim/traffic/yard.cpp |
| | `Yard.toWorld` |  | portado | sim/traffic/yard.cpp |
| | `Yard.ways` | The way in to a planned dock and out of it, in the world; null when it has none. | portado | sim/traffic/yard.cpp |
| | `YardDock` |  | portado | sim/traffic/yard.cpp |
| | `YardGate` |  | portado | sim/traffic/yard.cpp |
| | `YardGrid` | A yard's grid as `buildYard` fills it, before its gates and docks. | portado | sim/traffic/yard.cpp |
| | `asphaltNear` | DO NOT merge the whole map's carriageways for a yard (`levelPolygons`) | portado | sim/traffic/yard.cpp |
| | `buildYard` | Makes a yard a slice at a time: what is solid to a lorry filled in its grid - from what is near the yard only | portado | sim/traffic/yard.cpp |
| | `elementRing` |  | portado | sim/traffic/yard.cpp |
| | `forgetYard` | A building's yard forgotten: its building gone, or no longer one with a yard (`lorryTraffic.ts` YardList). | portado | sim/traffic/yard.cpp |
| | `forgetYardsBut` | @rebuild The yards of the buildings `keep` turns down forgotten: when the town's yards were read whole. | portado | sim/traffic/yard.cpp |
| | `gateDistance` | How far a gate's inside is from a dock's open end. | portado | sim/traffic/yard.cpp |
| | `keepYard` | Keeps a yard made (`buildYard`) for its building. | portado | sim/traffic/yard.cpp |
| | `knownYard` | DO NOT throw a yard away on `yardKey` alone, nor on a lot changed anywhere: its inputs are read again only when the roads moved or | portado | sim/traffic/yard.cpp |
| | `last` |  | portado | sim/traffic/yard.cpp |
| | `localDir` |  | portado | sim/traffic/yard.cpp |
| | `lorryOf` | The lorry as the planner sees it. | portado | sim/traffic/yard.cpp |
| | `lotsNear` | DO NOT file the town's lots again for a yard: the lot store's own index by place, kept by its writers (`RoadDoc.lotsIn`). | portado | sim/traffic/yard.cpp |
| | `nearestGate` |  | portado | sim/traffic/yard.cpp |
| | `ownLot` | A building's own lot among those near it: the last in the lots' order, as one filing of the whole map kept it. | portado | sim/traffic/yard.cpp |
| | `settle` | A short leg from a pose to another, driven forward or backing | portado | sim/traffic/yard.cpp |
| | `straight` | A straight leg (both poses on one line, one heading). | portado | sim/traffic/yard.cpp |
| | `straightClear` |  | portado | sim/traffic/yard.cpp |
| | `yardBox` | A yard's grid (local frame: its corner, cells across and down, its box) and the grid's box in the world. | portado | sim/traffic/yard.cpp |
| | `yardInputs` | What a yard is made from, near it, as one number: the lanes its gates may be joined to | portado | sim/traffic/yard.cpp |
| | `yardKey` | The roads as they stand: a yard known good for them needs no look (`knownYard`). | portado | sim/traffic/yard.cpp |
| | `yardOf` | The yard of a building as it stands now, made to the end at once if it must be (probes and specs). | portado | sim/traffic/yard.cpp |
| | `yardsOf` |  | portado | sim/traffic/yard.cpp |
| `src/sim/agents/lorryTraffic.ts` | **arquivo** | caminhões às docas | portado | sim/traffic/lorry_traffic.cpp |
| | `ASK_WAY` | Seconds it waits for somebody on its way before going on (they step out of a vehicle's way, `walk.ts`). | portado | sim/traffic/lorry_traffic.cpp |
| | `CALL_REACH` | Farthest a passing lorry is called from, and how long it has to reach the gate. | portado | sim/traffic/lorry_traffic.cpp |
| | `CALL_TIMEOUT` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `CLOSURE_MARGIN` | Kept clear before and after the stretch a lorry sweeps. | portado | sim/traffic/lorry_traffic.cpp |
| | `JOIN_SPEED` | Speed a lorry joins its lane at. | portado | sim/traffic/lorry_traffic.cpp |
| | `LANE_HALF` | Body points within this of a lane's centre close it (half a wide lane and a hand). | portado | sim/traffic/lorry_traffic.cpp |
| | `LOOK` | Seconds between two looks for a lorry to send. | portado | sim/traffic/lorry_traffic.cpp |
| | `Lorry` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.busy` | Whether a lorry is in, or about to swing into, a lot's yard (its cars wait meanwhile). | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.call` | A yard with no lorry, a dock of it drawn; planned, it is sent the nearest passing lorry. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.clock` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.closeIfClear` | Closes the stretches the street part sweeps if nobody is on them or too near to stop; whether it did. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.coming` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.docked` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.drive` | Along its way: arriving onto the dock, or leaving it for the lane. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.drop` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.join` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.left` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.lorries` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.make` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.making` | The yard being made, a slice a call, with what it is made from (`yardInputs`) as of the roads it was last checked on (`key`). | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.mayGo` | Whether nobody stands where the lorry goes over the next few metres (its body as discs) | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.moving` | Lorries driving off the road now. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.offRoad` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.places` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.plan` | A slice of planning for the dock in hand (a slice of making its yard first). | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.planning` | The dock being planned now. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.readYards` | Only which buildings have docks: a yard is worked out | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.release` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.reset` | A different map (`LotTraffic.reset`, from `AmbientWorld.reset`) | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.sendNow` | Sends a lorry to a dock now, standing on its lane at the stop before the gate (for probes and specs) | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.sent` | Counters for probes. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.step` | Stepped by the lots' traffic (`lotTraffic.ts`) each tick. `carsBusy`: whether a lot's own cars are moving now. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.unloading` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.view` | For probes: each lorry's phase, building, dock and where its body is. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.waiting` | Stopped before the gate, until the street it sweeps is clear and the lot's cars are still. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.world` | The world last stepped: a reset opens the lanes its lorries closed there. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.yardFor` | A building's yard, made a slice a call (`buildYard`): undefined while it is being made. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.yardList` | The yards of the town and their docks, kept from the diary. | portado | sim/traffic/lorry_traffic.cpp |
| | `LorryTraffic.yards` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `MAX_LORRIES` | Most lorries at the docks at once in the whole town. | portado | sim/traffic/lorry_traffic.cpp |
| | `Phase` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `UNLOAD_MAX` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `UNLOAD_MIN` | Seconds a lorry stands at the dock while it is unloaded. | portado | sim/traffic/lorry_traffic.cpp |
| | `YardList` | The town's yards and their docks, in the store's order (the lorries' draw follows it), kept from the diary | portado | sim/traffic/lorry_traffic.cpp |
| | `YardList.docks` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `YardList.gone` | The buildings the last `read` took out (gone, or with no yard now); null: it read the town whole. | portado | sim/traffic/lorry_traffic.cpp |
| | `YardList.has` | Whether a building has a yard, as of the last `read`. | portado | sim/traffic/lorry_traffic.cpp |
| | `YardList.kept` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `YardList.list` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `YardList.read` |  | portado | sim/traffic/lorry_traffic.cpp |
| | `closures` | The stretches of lane a street part sweeps | portado | sim/traffic/lorry_traffic.cpp |
| | `hasYard` | A building with a yard a lorry is sent to: a car-gated property with loading docks. | portado | sim/traffic/lorry_traffic.cpp |
| | `setFree` |  | portado | sim/traffic/lorry_traffic.cpp |
| `src/sim/agents/lotBays.ts` | **arquivo** | vagas do lote e saída pela portaria | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `BayLaneLine` | What a lot's bays read of a lane: a `Lanelet`'s own fields, on the page or made again in a worker. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `BayReads` | Everything a lot's bays read of the world. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `BayShare` | Lots to work out, as plain data for a worker (`postMessage` copies it, MDN "structured clone") | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `CAR_GATE` | The narrowest gate a car is driven through (a person's is 1.2 m); `parkingLayout.ts` `CAR_GATE_WIDTH`. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `EDGE_INSET` | How far inside the street edge the last turn is made. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `EDGE_MARGIN` | Clear of the corners of a lot where a car leaves it for the street. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `EDGE_REACH` | Farthest a lot's street edge may be from the lane it opens onto. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `EXIT_STEP` | Spacing of the points tried along a lot's sides. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `FIT_EPS` | A body touching the stall's end is in it. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `GATE_LANE_END` | Kept clear of a lane's ends for a gate's stop and join points. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `JOIN_PAST_GATE` | Where a car leaving through a gate joins its lane, past the gate; where one arriving stops, before it. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `LANE_END` | Kept clear of the ends of a lane where a car joins or leaves it. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `LANE_REACH` | How far from a point the lane beside it is looked for (`laneBeside`), u. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `LORRY_DOCK` | A stall wider than this is a lorry's dock, not a car's bay. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `LotAsk` | One lot to work out away from the page, as the page reads it: the record, the lanes and walls within its reach. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `LotBlock` | What a lot's bays read of a block of its building (`bayRows`, `stallRoom`, `gatedProperty`). | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `LotExit` | A way out of a lot: a point inside its street edge, and the lane it joins, clear of every building. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `LotRecord` | What a lot's bays read of its building's record: its frame, its blocks' boxes and its parts. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `OVER_A_BODY` | Parts this high off the ground or more stand over a parked body (a canopy, a sign). | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `PART` | Numbers a part of a lot is sent as (`shareOf`): x, y, w, d, facing, z, angle, whether it has an angle. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `STALL` | Width of one stall, as the lot planner lays them (2.5 x 5 m). | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `STALL_BAND` | Half the widest body that parks in a stall (a van, 2 m) and a hand: the band a stall's clear length is read in. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `STALL_BLOCKS` | Every part of a lot that a parked body may not stand in: what a car cannot drive through | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `STOP_BEFORE_GATE` |  | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `SWEEP` | Half the width of the swept path a car needs between the lot and its lane. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `WallRing` | A building's solid footprint, world frame, with its bounds. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `bayRows` | A lot's stalls, each with the way out of its lot and the lane it is reached from (ids from 0). | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `baysOfShare` | Every lot of a share worked out (`bayRows`) on what was sent, in order: each lane's line made once | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `carGates` | A building's car gates: the `gate` elements a car fits through. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `clearOfWalls` | Whether a car can be driven straight from `a` to `b` without its body touching a building. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `clip` | A convex ring cut to one side of a line of the stall's frame (Sutherland-Hodgman): `axis` 0 keeps u >= `at` | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `clip.val` |  | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `exitsOf` | The ways out of a lot onto a street | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `gateExits` | The ways out of a gated lot: one per car gate, its inner point a few metres inside, its outer point just outside | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `gateLane` | The lane in front of a gate, on the kerb side | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `gatedProperty` | The property of a lot closed by a boundary with car gates: the box of all its open blocks | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `laneFor` | The nearest drivable lane with room to join, its point beside `(x, y)`, on the kerb side. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `personGates` | A building's people's gates: the `gate` elements narrower than a car's (`carGates`). | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `recordOf` | A lot's record from its share (`shareOf`): the parts as they were, an angle absent kept absent. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `shareOf` | Lots put as a share (`BayShare`), each lane and wall once. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `stallFits` | Whether a body this long stands wholly inside the stall, parked centred in it. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `stallRoom` | Half the clear length of a stall (local frame of its building): from its centre `(lx, ly)` along `(dx, dy)` | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `stallRoom.along` |  | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| | `wallsNear` | The walls whose bounds come within a few metres of a lot. | portado | sim/buildings/lot_access.cpp e sim/traffic/parking_sources.cpp |
| `src/sim/agents/lotDoors.ts` | **arquivo** | portas dos lotes para pedestres | de outra funcionalidade | funcionalidade `people` |
| | `DOORS` |  | de outra funcionalidade | funcionalidade `people` |
| | `DOOR_OFF` | Where the door's walker stands: this far in front of the wall. | de outra funcionalidade | funcionalidade `people` |
| | `DOOR_REACH` | Farthest a door may be behind its gate. | de outra funcionalidade | funcionalidade `people` |
| | `DoorWay` | A lot's way on foot, world units. | de outra funcionalidade | funcionalidade `people` |
| | `GATE_IN` | Inside the gate, the point the way runs square through it from; outside, its foot on the footway. | de outra funcionalidade | funcionalidade `people` |
| | `GATE_OUT` |  | de outra funcionalidade | funcionalidade `people` |
| | `MARGIN` | Room round the gate and the door the grid covers. | de outra funcionalidade | funcionalidade `people` |
| | `doorWay` | The building's way from its door out through its people's gate; null when it has none that is walkable. | de outra funcionalidade | funcionalidade `people` |
| | `hitRings` | Distance along the ray from (x, y) in (dx, dy) to the nearest side of the rings; null when it meets none. | de outra funcionalidade | funcionalidade `people` |
| | `personGates` | A gate narrower than a car's is a person's (`parking.ts` CAR_GATE). | de outra funcionalidade | funcionalidade `people` |
| | `streetWay` | A shop or a bar built up to the footway, with no people's gate: its front door opens onto the street | de outra funcionalidade | funcionalidade `people` |
| | `workOut` |  | de outra funcionalidade | funcionalidade `people` |
| `src/sim/agents/lotNav.ts` | **arquivo** | grade de navegação do lote | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `DEFAULT_CELL` | Cell size of a car's grid. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `DEFAULT_CLEARANCE` | Kept between a car's centreline and anything solid: half a car's width and a little. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `LotGrid` |  | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `LotGridOptions` | The grid's make for one kind of agent: its cell, the clearance its centre keeps, what is solid to it. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `LotPart` | What a lot's grid reads of a part of the lot (`buildLotGrid`): all a worker is sent of it (`lotBays.ts` `shareOf`). | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `LotRect` |  | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `OVERHEAD` | Parts this high off the ground or more stand over a walker's head (a canopy's wall, a sign). | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `PEOPLE_GRID` | A person's grid: 20 cm cells, so a gate's opening | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `PEOPLE_SOLID` | What a person cannot walk through on a lot: a car's list less the stairs and ramps, and the flower beds. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `RingCover` | DO NOT build arrays per cell here | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `RingCover.constructor` |  | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `RingCover.px` |  | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `RingCover.py` |  | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `RingCover.sides` | Per side with an axis: ax, ay, the ring's least and greatest projection on it. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `RingCover.squareAxes` | The square's four axes last met, and the ring's extent on each | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `RingCover.touches` | Whether the ring overlaps the cell square from (`cx`, `cy`) `size` across | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `SOLID` | What a car cannot drive through on a lot. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `apart` | Whether a cell square's projection on an axis and a ring's (`b0`..`b1`) at most touch. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `buildLotGrid` | The lot's grid: `walls` are the buildings' footprints already in this building's local frame; `exits` the points | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `centre` |  | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `elementCorners` | The corners of an element's box in the local frame, turned by its angle. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `fillRing` | DO NOT test every cell's centre against every ring again | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `freeCellNear` | The free cell nearest a local point within a few cells, or -1. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `growByClearance` | DO NOT spread the disc from every solid cell again | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `sees` | Whether the straight line between two local points crosses only free cells. | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| | `wayOut` | The way from a local point (in front of a stall) down the aisles to the nearest exit | já existia | sim/buildings/lot_access.cpp (Dijkstra na grade) |
| `src/sim/agents/lotTraffic.ts` | **arquivo** | carros que entram e saem dos lotes | portado | sim/traffic/lot_traffic.cpp |
| | `ASK_WAY` | Seconds a car off the road waits for people on its way before going on (they walk out of a car's ground, `walk.ts`). | portado | sim/traffic/lot_traffic.cpp |
| | `CALL_REACH` | Farthest a passing car is called from to a bay, and how long it has to get there. | portado | sim/traffic/lot_traffic.cpp |
| | `CALL_TIMEOUT` |  | portado | sim/traffic/lot_traffic.cpp |
| | `CLASSES` |  | portado | sim/traffic/lot_traffic.cpp |
| | `COMING_SECONDS` |  | portado | sim/traffic/lot_traffic.cpp |
| | `DEPART_PER_LOOK` | Most cars sent out of the lots in one look. | portado | sim/traffic/lot_traffic.cpp |
| | `GAP_AHEAD` |  | portado | sim/traffic/lot_traffic.cpp |
| | `GAP_BEHIND` |  | portado | sim/traffic/lot_traffic.cpp |
| | `GONE` |  | portado | sim/traffic/lot_traffic.cpp |
| | `GatedBay` | A gated bay as the lots keep it: its place, its slot in the bays' order, its building's class. | portado | sim/traffic/lot_traffic.cpp |
| | `JOIN_SPEED` | Speed a car joins the lane at. | portado | sim/traffic/lot_traffic.cpp |
| | `KINDS` | The kinds of place a bay's building is (`ambient/demand.ts` kindOf), each a class of the draws; the last class, a building gone (w | portado | sim/traffic/lot_traffic.cpp |
| | `LANE_EDGE` | How close to the lane a car leaving pulls up before looking for a gap; the room the traffic must leave it. | portado | sim/traffic/lot_traffic.cpp |
| | `LOOK` | Seconds between two looks for a car to send in. | portado | sim/traffic/lot_traffic.cpp |
| | `LotCar` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.active` | DO NOT step every car of the lots a tick: those parked and not sent out stand still and are left out until `depart` sends them. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.ambient` | The scenery the bays ask who stands in them; it tells each change of a bay of its (`AmbientWorld.sceneryWatch`). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.bays` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.baysOf` | The gated bays by building: those whose class a building's change reaches (`followBuildings`). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.baysRead` | The bays' diary as last read (`bayChangesSince`), and the serial seen a look before it was read. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.baysSeen` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.bodies` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.buildingsSeen` | The buildings' diary as last read (`followBuildings`). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.bySlot` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.bySource` | The gated bays by their source (`L<building>`, `K<segment>`), and by place (`spotKey`) in the order they were filed: kept from the | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.call` | DO NOT draw among the bays near the view (`SimWorld.focus`); DO NOT list the free bays to draw one: their running weights by class | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.called` | Counters for probes: cars sent in, parked, driven out. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.carBySlot` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.cars` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.carsAt` | The cars of the lots by the place of their bay, how many of their bodies stand at each place, the places the lorries stand at duri | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.carsMoving` | Whether a lot's cars are driving in or out of it now. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.clock` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.coming` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.comingCount` | The cars on their way, and those driving into or out of a bay, in all and by building: kept by each change of phase (`enter`, `tra | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.count` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.depart` | DO NOT build a pool of every ripe car and scenery's car a look: drawn by their running weights, the lots' cars in their order firs | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.drop` | A car off the road taken out of the lots' list and of the drawn ones. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.dropBay` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.dropEveryBay` | @rebuild Every gated bay let go: the bays read whole again (a map loaded, the diary unable to say which changed). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.enter` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.fileBay` | A gated bay filed at its place; a second bay at a place held takes it and keeps its slot, as the list of bays kept the place's tur | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.followBuildings` | The bays and ripe cars of the buildings the diary says changed: their class read again (a draw weighs a building's kind). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.free` | DO NOT walk every gated bay a look with a text key each: each bay's slot in the bays' order, of its building's class while it is f | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.gap` | Room in the traffic for a car joining the lane at its joining point. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.holdLorries` | The places the lorries' bodies stand at held through a look's calls (their bays not free), or let go. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.hour` | The hour of the city's day, for the lots' tables (`ambient/demand.ts`). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.leaveBuilding` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.leaving` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.leftBy` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.liftBody` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.lorries` | The lorries to the works' docks, stepped with the cars (`lorryTraffic.ts`). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.lorriesAt` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.mayGo` | Whether nobody stands where the car will go over the rest of its way (its body as three discs every metre) | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.move` | A car's body moved off the road, counted where it stands now. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.moving` | Cars driving off the road, into a bay or out of one: off the lanes | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.movingCount` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.movingIn` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.nextOrder` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.owed` | Trips owed to the lots (`TRIP_SECONDS`), fractions carried from look to look. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.parked` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.parkedIn` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.parking` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.placeBody` | A car's body counted where it stands now, the bay there no longer free. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.rank` | A ripe car's class: its bay's building's kind. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.rankBay` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.rankEvery` | @rebuild Every bay's and ripe car's class read again: the buildings' diary unable to say which changed. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.readBays` | DO NOT read the town's list (`collectBays`) on a change of `buildings.revision` | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.refresh` | A bay's place looked at again (`refreshBay`). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.refreshBay` | Free while no car of the lots holds it, no body of theirs or lorry stands on it and no scenery's car is in it; the scenery's while | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.release` | A car sent to a bay that will not reach it: on to a road end like the rest. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.reset` | A different map (`AmbientWorld.reset`): the cars of the old one's lots are forgotten. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.reslotBays` | The bays' slots handed out again in their order, those let go dropped (more of them than of bays now). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.reslotCars` | The cars' slots handed out again in the lots' order, those of cars gone dropped (more of them than of cars now). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.ripen` | The parked cars whose least stay is over drawn among (`waiting`) from now on, until sent out or gone. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.scenery` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.sceneryMoved` |  | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.stays` | The parked cars' ids by when their least stay is over (`ripen`). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.step` | Stage 2 (`sim/city/city.ts`): with the scenery on, cars go in and out of the lots. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.time` | Seconds the lots have run: what a parked car's least stay is counted against (`LotCar.until`). | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.track` | A car taken into the lots' list, in its phase: its bay's place held, and its body's. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.untrack` | A car taken out of the lots' list: its places let go. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.view` | The cars of the lots, for probes: id, phase, the bay's building, where the body stands. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.waiting` | The cars of the lots in their order, each of its class while it is ripe: those `depart` draws among. | portado | sim/traffic/lot_traffic.cpp |
| | `LotTraffic.weights` | The draws' weights by class, filled for each draw. | portado | sim/traffic/lot_traffic.cpp |
| | `MAX_COMING` | Fewest cars on their way in at once, and the seconds a car called takes to reach its lot, about (more traffic, more on their way). | portado | sim/traffic/lot_traffic.cpp |
| | `PARKING_KINDS` | The kinds of car that park in lots. | portado | sim/traffic/lot_traffic.cpp |
| | `Phase` |  | portado | sim/traffic/lot_traffic.cpp |
| | `STAY_MAX` |  | portado | sim/traffic/lot_traffic.cpp |
| | `STAY_MIN` | Seconds a car stays parked: the stop's `duration`. | portado | sim/traffic/lot_traffic.cpp |
| | `TRIP_SECONDS` | Mean seconds a car drives between two lots: one trip in this many ends in a lot each second. | portado | sim/traffic/lot_traffic.cpp |
| | `kindClass` | A building's class in the draws: its kind of place (`KINDS`), or `GONE`. | portado | sim/traffic/lot_traffic.cpp |
| | `offRoad` | The car as a drawn body off the road, standing at `(x, y)` facing `angle`. | portado | sim/traffic/lot_traffic.cpp |
| | `setFree` |  | portado | sim/traffic/lot_traffic.cpp |
| | `weightOf` | A list's whole weight: each class's count by its weight. | portado | sim/traffic/lot_traffic.cpp |
| `src/sim/agents/manoeuvre.ts` | **arquivo** | caminho fora da via: entrar e sair da vaga | portado | sim/traffic/manoeuvre.cpp |
| | `ACROSS_INTO_LANE` | How far into the lane from its kerb side a car crossing the footway first aims, u. | portado | sim/traffic/manoeuvre.cpp |
| | `BACKWARD` |  | portado | sim/traffic/manoeuvre.cpp |
| | `CREEP` | Slowest it creeps while moving (so a leg always ends). | portado | sim/traffic/manoeuvre.cpp |
| | `FORWARD` | Top speed in a car park, forward and backing out. | portado | sim/traffic/manoeuvre.cpp |
| | `Leg` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.advance` | Moves on by `ds`, never past the end. | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.constructor` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.done` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.leg` | Index of the leg the sample belongs to. | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.legEnds` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.length` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.poseAt` | Body centre and heading at distance `s` (now, by default). | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.s` | Distance along the whole path. | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.samples` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Manoeuvre.speed` | Speed for the current place: up to speed and down to rest within each leg. | portado | sim/traffic/manoeuvre.cpp |
| | `PathSample` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Point` |  | portado | sim/traffic/manoeuvre.cpp |
| | `RAMP` | Distance over which a car gets up to speed and comes to rest, per leg. | portado | sim/traffic/manoeuvre.cpp |
| | `SAMPLES` | Points per leg: a leg is a few metres to a few tens of metres long. | portado | sim/traffic/manoeuvre.cpp |
| | `SHUNT_BACKWARD` |  | portado | sim/traffic/manoeuvre.cpp |
| | `SHUNT_FORWARD` | Top speeds of a lorry shunting in a yard, forward and backing, and the distance it gets up to speed and stops in. | portado | sim/traffic/manoeuvre.cpp |
| | `SHUNT_RAMP` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt` | A lorry's way through a yard (`lorryDocks.ts`) | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.advance` | Moves on by `ds`, never past the end. | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.at` | Distance along the way at each pose, and where each run in one direction ends. | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.constructor` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.distanceAt` | Distance along the way at a pose. | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.done` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.indexAt` | Index of the pose at or before `s`. | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.length` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.poseAt` | Body centre and heading at distance `s` (now, by default). | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.runEnds` |  | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.s` | Distance along the whole path. | portado | sim/traffic/manoeuvre.cpp |
| | `Shunt.speed` | Speed for the current place: up to speed and down to rest within each leg. | portado | sim/traffic/manoeuvre.cpp |
| | `TANGENT` | Hermite tangent length as a share of a leg's chord. | portado | sim/traffic/manoeuvre.cpp |
| | `acrossFootway` | Where a car leaving a lot by its gate `edge` is once across the footway | portado | sim/traffic/manoeuvre.cpp |
| | `arrival` | From the lane into a bay, nose first: from where the car stopped | portado | sim/traffic/manoeuvre.cpp |
| | `departure` | Out of a bay to the lane: backing out of the stall into the aisle, swinging the nose towards the way out | portado | sim/traffic/manoeuvre.cpp |
| | `through` | Forward legs through `points`, leaving the first along `t0` and arriving at the last along `t1` | portado | sim/traffic/manoeuvre.cpp |
| | `through.dirAt` |  | portado | sim/traffic/manoeuvre.cpp |
| | `unit` | Unit vector, or `fallback` for a zero one. | portado | sim/traffic/manoeuvre.cpp |
| `src/sim/agents/parking.ts` | **arquivo** | o livro das vagas da cidade | portado | sim/traffic/parking_sources.cpp |
| | `Asked` | A lot asked of the workers, with what it is worked out from. | portado | sim/traffic/parking_sources.cpp |
| | `BOOKS` |  | portado | sim/traffic/parking_sources.cpp |
| | `BakedBays` | A tile's bays as baked where it was written (`sim/tileBays.ts` TileBays): each lot's and street's, with what they were worked out  | não se aplica | bake de ladrilho |
| | `BakedSource` | One lot's or street's bays as a bake keeps them (`bakeBays`): the record's digest (0 for a street) and the sources'. | não se aplica | bake de ladrilho |
| | `BayBook` | The bays of a world as last worked out, and what they were worked out from. | portado | sim/traffic/parking_sources.cpp |
| | `BayWorkers` | What works lots' bays out off the page (`bayPool.ts`): each lot asked, in order, as `bayRows` would on the page. | não se aplica | sem workers |
| | `BayWorld` | DO NOT import `SimWorld`: what the bays read of a world (the simulation's, or a tile's own roads and buildings where it is baked,  | portado | sim/traffic/parking_sources.cpp |
| | `LANE_BESIDE_REACH` | How far from a point the lane beside it is looked for (`laneBeside`), u. | portado | sim/traffic/parking_sources.cpp |
| | `LOT_REACHED` | How far round a change a lot's box grown by `LOT_READS` (`TrafficBuilding.reach`) may reach | portado | sim/traffic/parking_sources.cpp |
| | `LotBays` | A lot's bays as last worked out, with what they were worked out from: its record | portado | sim/traffic/parking_sources.cpp |
| | `LotSources` | What a lot's bays are worked out from: its record (`lotKey`), and the lanes and walls within reach, digested. | portado | sim/traffic/parking_sources.cpp |
| | `OF_THE_LOT` | What of a record its bays are worked out from (`lotBays.ts` LotRecord): its frame, its blocks and its parts. | portado | sim/traffic/parking_sources.cpp |
| | `RECORD_READS` | What a building's bays are worked out from, digested, once per traffic record | portado | sim/traffic/parking_sources.cpp |
| | `Wall` |  | portado | sim/traffic/parking_sources.cpp |
| | `Walls` | DO NOT go back to a list of every building's walls: one change of one building made it again. | portado | sim/traffic/parking_sources.cpp |
| | `Walls.constructor` |  | portado | sim/traffic/parking_sources.cpp |
| | `Walls.inBox` | The walls whose bounds meet a box, in the store's order. | portado | sim/traffic/parking_sources.cpp |
| | `Walls.read` | Each building's walls read in this look (a look's lots share their neighbours). | portado | sim/traffic/parking_sources.cpp |
| | `bakeBays` | @rebuild Every lot's and street's bays of a world worked out here and now (no workers), each with what it was worked out from: wha | não se aplica | bake de ladrilho |
| | `bayChangesSince` | DO NOT read the town's list (`collectBays`) at every change: every bay of the town, for each building grown. `reader` has read up  | portado | sim/traffic/parking_sources.cpp |
| | `baySources` | @rebuild Every source of bays, in the town's order (the lots in the store's, then the streets by id): read whole. | portado | sim/traffic/parking_sources.cpp |
| | `baySourcesIn` | The sources of bays whose lot or street meets the rectangle (the micro window's square), in the town's order as `baySources`: foun | portado | sim/traffic/parking_sources.cpp |
| | `baysNow` | The bays brought up to date, from the diary: the lots a building change reaches worked out again, the others kept | portado | sim/traffic/parking_sources.cpp |
| | `baysOfSource` | A source's bays now (`bayChangesSince`), as fresh copies (ids within the source); none when it has none. | portado | sim/traffic/parking_sources.cpp |
| | `bookOf` | A world's book, made empty the first time. | portado | sim/traffic/parking_sources.cpp |
| | `bothWays` | Whether a street runs both ways (a gate onto a one-way street may face either kerb, `lotBays.ts` gateLane). | portado | sim/traffic/parking_sources.cpp |
| | `buildingWalls` | A building's solid footprints, with their bounds and digests: read by id alone (`TrafficBuildings.footprints`), no traffic record  | portado | sim/traffic/parking_sources.cpp |
| | `byText` |  | portado | sim/traffic/parking_sources.cpp |
| | `collectBays` | Every stall of every parking element, with the way out of its lot and the lane it is reached from | portado | sim/traffic/parking_sources.cpp |
| | `dropBaked` |  | não se aplica | bake de ladrilho |
| | `gateExits` | The ways out of a gated lot (`lotBays.ts` `gateExits`), read on the page. | portado | sim/traffic/parking_sources.cpp |
| | `hasParking` |  | portado | sim/traffic/parking_sources.cpp |
| | `kerbReads` | What a street's kerb bays read of the world, and its tile's bake (`kerbBays.ts` KerbReads). | portado | sim/traffic/parking_sources.cpp |
| | `kerbReads.baked` |  | portado | sim/traffic/parking_sources.cpp |
| | `kerbReads.laneAt` |  | portado | sim/traffic/parking_sources.cpp |
| | `kerbReads.linksKey` |  | portado | sim/traffic/parking_sources.cpp |
| | `kerbReads.rank` |  | portado | sim/traffic/parking_sources.cpp |
| | `kerbReads.waits` |  | portado | sim/traffic/parking_sources.cpp |
| | `land` | The workers' answers, each kept and logged if its lot still waits for those very sources. | não se aplica | sem workers |
| | `laneBeside` | The lane beside a point of a street (a bus stop): the nearest on the kerb side of that road, its point there. | portado | sim/traffic/parking_sources.cpp |
| | `lookAtLot` | A lot looked at: kept as it is (or as it is being worked out) when its record and its reach are untouched | portado | sim/traffic/parking_sources.cpp |
| | `lotKey` | DO NOT key it by its JSON text: a string the size of the record held for every building with parking. | portado | sim/traffic/parking_sources.cpp |
| | `lotsReached` | The lots whose reach (`LOT_READS`) meets one of the rectangles, and the lots named; found by place. | portado | sim/traffic/parking_sources.cpp |
| | `meets` |  | portado | sim/traffic/parking_sources.cpp |
| | `notOfTheLot` |  | portado | sim/traffic/parking_sources.cpp |
| | `noteBays` | A source's bays changed: logged, and the town's list to be put together again. | portado | sim/traffic/parking_sources.cpp |
| | `pageReads` | The world as a lot's bays read it (`lotBays.ts` `BayReads`), on the page. | portado | sim/traffic/parking_sources.cpp |
| | `pageReads.bothWays` | Whether a street runs both ways (a gate onto a one-way street may face either kerb, `lotBays.ts` gateLane). | portado | sim/traffic/parking_sources.cpp |
| | `pageReads.ghost` |  | portado | sim/traffic/parking_sources.cpp |
| | `pageReads.lanesNear` |  | portado | sim/traffic/parking_sources.cpp |
| | `pageReads.wallsIn` |  | portado | sim/traffic/parking_sources.cpp |
| | `recordReads` |  | portado | sim/traffic/parking_sources.cpp |
| | `release` | The streets held for a tile's bays looked at again at the next follow (those whose tile still waits held again). | portado | sim/traffic/parking_sources.cpp |
| | `streetTile` | The tile a street belongs to (`OWNER`: by the middle of its ends); NaN when it cannot be said. | portado | sim/traffic/parking_sources.cpp |
| | `streetsInOrder` | The streets that park, in id order: the town's list puts their kerbs after the lots. | portado | sim/traffic/parking_sources.cpp |
| | `tileBaysGone` | A tile let go: its baked bays dropped, its streets no longer wait. | não se aplica | bake de ladrilho: mapa não ladrilhado |
| | `tileBaysRead` | A tile's read landed: its baked bays (null: none stored, of another format, or not read) offered while that read is the tile's las | não se aplica | bake de ladrilho: mapa não ladrilhado |
| | `tileBaysReset` | Another map put in: every tile's baked bays and reads of the old one dropped. | não se aplica | bake de ladrilho: mapa não ladrilhado |
| | `tileBaysWanted` | A tile's baked bays are being read (`simHost.ts` tileBuildings): its own streets wait for them (`KerbReads.waits`), as a lot waits | não se aplica | bake de ladrilho: mapa não ladrilhado |
| | `townBays` | @rebuild The town's list of bays, put together again only after a change (`collectBays`). | portado | sim/traffic/parking_sources.cpp |
| | `trimLog` | The log kept from the least serial its readers read up to; a lagging reader's part longer than every source read once (it reads ev | portado | sim/traffic/parking_sources.cpp |
| | `useBayWorkers` | The workers a world's lots are worked out by (the game's, `main.ts`). | não se aplica | sem workers |
| | `wallsOf` | The town's walls, by place. | portado | sim/traffic/parking_sources.cpp |
| `src/sim/agents/reedsShepp.ts` | **arquivo** | curvas de Reeds-Shepp que fecham a manobra | portado | sim/traffic/reeds_shepp.cpp |
| | `CCC` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `CCCC` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `CCSC` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `CCSCC` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `CSC` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `LpRmL` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `LpRmSLmRp` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `LpRmSmLm` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `LpRmSmRm` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `LpRumLumRp` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `LpRupLumRm` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `LpSpLp` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `LpSpRp` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `P` | `polar`'s answer, written in place. | portado | sim/traffic/reeds_shepp.cpp |
| | `PI` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `Piece` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `RsPath` | A Reeds-Shepp way: its pieces and their signed lengths (radians or radii; negative is backing), on a unit radius. | portado | sim/traffic/reeds_shepp.cpp |
| | `T` | The pieces' lengths, written by the formulas. | portado | sim/traffic/reeds_shepp.cpp |
| | `TO` | `tauOmega`'s answer, written in place. | portado | sim/traffic/reeds_shepp.cpp |
| | `TWO_PI` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `TYPES` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `ZERO` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `bestL` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `bestType,bestN,bestTotal` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `mod2pi` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `polar` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `reedsShepp` | The shortest Reeds-Shepp way from pose `a` to pose `b` | portado | sim/traffic/reeds_shepp.cpp |
| | `reedsSheppLength` | The length (world units) of the shortest Reeds-Shepp way between two poses; Infinity if none. | portado | sim/traffic/reeds_shepp.cpp |
| | `sampleReedsShepp` | Poses along a Reeds-Shepp way from pose `a`, one every `step` | portado | sim/traffic/reeds_shepp.cpp |
| | `sampleReedsShepp.push` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `search` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `take` |  | portado | sim/traffic/reeds_shepp.cpp |
| | `tauOmega` |  | portado | sim/traffic/reeds_shepp.cpp |
| `src/sim/agents/trafficBuildings.ts` | **arquivo** | prédios como o worker os vê | não se aplica | b40::record_body lê o prédio direto |
| | `BATCH_DETAIL` | A batch of the buildings' changes in the diary (`TrafficBuildingTable.apply`): its detail is this, then one letter an id - gone, k | não se aplica | b40::record_body lê o prédio direto |
| | `BATCH_GONE` |  | não se aplica | b40::record_body lê o prédio direto |
| | `BATCH_KEPT` |  | não se aplica | b40::record_body lê o prédio direto |
| | `BATCH_NEW` |  | não se aplica | b40::record_body lê o prédio direto |
| | `BuildingChange` | A building of the page changed, as the worker is told it: its traffic record now and where its record is read (`RecordRef`), null  | não se aplica | b40::record_body lê o prédio direto |
| | `BuildingSent` | A building of the page changed, as it is posted to the worker: where its record is read (`refOf`: its tile's shared table and row, | não se aplica | b40::record_body lê o prédio direto |
| | `BuildingSource` | What a reader of the buildings by the diary reads: the document (its diary) and the buildings (`SimWorld`). | não se aplica | b40::record_body lê o prédio direto |
| | `CellList` | A building's cells (`CELL`) of the table's index, each a list of slots. | não se aplica | b40::record_body lê o prédio direto |
| | `HELD_RECORDS` | Traffic records held at once by the worker's table (`TrafficBuildingTable`), the last asked (a clock's second chance): ~6 KB each. | não se aplica | b40::record_body lê o prédio direto |
| | `LOT_READS` | How far a lot's bays read lanes and walls (`LANE_REACH`, the exits' reach), with a margin (`parking.ts`). | não se aplica | b40::record_body lê o prédio direto |
| | `TRAFFIC_KINDS` | The kinds of place a building is (`ambient/demand.ts` kindOf), in the order of the draws' classes and of `LotPlaces`' trees. | não se aplica | b40::record_body lê o prédio direto |
| | `TRAIT_PARKING` | A building with a `parking` element: a lot whose stalls are bays (`parking.ts`). | não se aplica | b40::record_body lê o prédio direto |
| | `TRAIT_YARD` | A gated lot with docks: a lorry's yard (`lorryTraffic.ts`). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBlock` | What traffic reads of a block: its box in the building's frame, open (a lot) or built, its floor (`lotBays.ts` LotBlock). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBox` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuilding` | A building as the simulation reads it: its frame, use, blocks and parts, and what the page works out of the whole record | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.alive` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.all` | @rebuild Every traffic record in the store's order: DO NOT walk it in the worker, where each is worked out again (`ids` and `facts | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.append` | An id put last in the store's order (the gone packed out first when they outweigh the rest). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.apply` | The page's changes, written in the diary as ONE entry (DO NOT write one a building: a tile's ~1,500 pushed every reader past what  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.boxes` | `bounds` then `reach`, four numbers each, eight a slot. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.bufferAt` | Where its record is read: a table of `buffers` and its row. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.bufferIndex` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.bufferUses` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.buffers` | The tables the records are read from, how many slots read each, and a reader each (made when first asked). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.cells` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.clock` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.constructor` | `holding`: the traffic records held at once (`HELD_RECORDS`; fewer in a spec, to make it work them out again). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.eachCell` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.eachEntrances` | @rebuild Every record walked once (`TrafficBuildings.eachEntrances`). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.eachFacts` | @rebuild Every building's facts, entrances or footprints in the store's order: DO NOT ask them id by id of every building (the pag | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.eachFootprints` | @rebuild Every record walked once (`TrafficBuildings.eachFootprints`). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.entranceRuns` | Entrances (x, y, x, y each) and footprints (rings: their count, then each one's points' count and its x, y). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.entrances` | `entrancesOf(b)`: its car gates and the edges of its parking, where a kerb keeps no bay. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.facts` | What the walks over every building read of one, no traffic record worked out (`TrafficFacts`). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.factsAt` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.file` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.fileRect` | The rectangle a slot is filed over: its box and its anchor (`BuildingGrid`). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.filed` | Whether the slot is filed in the cells (its box and anchor are numbers). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.footprintRuns` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.footprints` | `solidFootprints(b)`: the world rings of its built ground blocks, what nobody drives or walks through. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.freeBuffers` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.frontages` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.get` | A building's traffic record (the worker works it out again from its row when it is not held: DO NOT ask it of every building). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.gone` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.grow` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.hand` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.held` | The traffic records held (a clock of `HELD_RECORDS`) and those that may still live elsewhere. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.heldAt` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.hold` | A traffic record held at its slot (one asked longest ago let go for it). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.idAt` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.ids` | Every building's id, in the order of `all()`, none read. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.inOrder` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.keepRef` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.kinds` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.load` | @rebuild Another map: every record read from its row, the diary told the whole set changed. DO NOT hold a record read here: each o | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.mark` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.marks` | Each query's mark on the slots it met (`meeting`). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.meeting` | The slots whose filed rectangle meets `rect` (touching counts), each once. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.near` | The buildings whose box may reach the rectangle, in the store's order. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.nearAny` | The buildings `meets` keeps for one of `rects`, asked of those within `pad` of it, each once, in the store's order: their ids. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.nearIds` | The ids of `near`, none read. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.order` | The ids in the store's order, NaN where one was taken out (packed when they outweigh the rest). | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.orderAt` | Its place in `order`: its rank in the store's order. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.orderGone` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.orderLength` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.packOrder` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.place` | A building's columns written from its traffic record, its row noted, filed by place: its slot. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.putEntrances` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.putFootprints` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.readers` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.record` | A slot's traffic record: held, still alive elsewhere, or worked out again from its row. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.rectAt` | A slot's rectangle in the diary, as `BuildingStore` writes it: its box, or its anchor when it has none. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.release` | A slot's table let go of; the table itself when no slot reads it now. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.remove` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.rows` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.size` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.slots` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.traits` | `TRAIT_PARKING`, `TRAIT_YARD`. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.unfile` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.unhold` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.used` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.xs` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildingTable.ys` |  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficBuildings` | The buildings as the simulation asks for them: by id, all in the store's order, by place in the store's order. | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficFacts` | What the walks over every building read of each (the order of the lots, the scenery's indexes, the meso's demand): numbers kept in | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficMap` | What the worker's copy of the map is made of: the roads and the transit, the lots and their keys, the buildings as the document's  | não se aplica | b40::record_body lê o prédio direto |
| | `TrafficPart` | What traffic reads of a part of a lot (`lotNav.ts` LotPart), and its id (a dock's element). | não se aplica | b40::record_body lê o prédio direto |
| | `boxOf` |  | não se aplica | b40::record_body lê o prédio direto |
| | `buildingChangesOf` | The buildings the diary named, as the worker is sent them: each record's place (`refOf`, not kept on the page), never its traffic  | não se aplica | b40::record_body lê o prédio direto |
| | `cellKey` |  | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings` | A document's own buildings as traffic reads them (a world on the page or in a spec): each record's traffic record. | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.all` | @rebuild Every traffic record in the store's order: DO NOT walk it in the worker, where each is worked out again (`ids` and `facts | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.eachEntrances` | @rebuild Every record walked once (`TrafficBuildings.eachEntrances`). | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.eachFacts` | @rebuild Every building's facts, entrances or footprints in the store's order: DO NOT ask them id by id of every building (the pag | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.eachFootprints` | @rebuild Every record walked once (`TrafficBuildings.eachFootprints`). | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.entrances` | `entrancesOf(b)`: its car gates and the edges of its parking, where a kerb keeps no bay. | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.facts` | What the walks over every building read of one, no traffic record worked out (`TrafficFacts`). | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.factsOfRecord` |  | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.footprints` | `solidFootprints(b)`: the world rings of its built ground blocks, what nobody drives or walks through. | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.get` | A building's traffic record (the worker works it out again from its row when it is not held: DO NOT ask it of every building). | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.ids` | Every building's id, in the order of `all()`, none read. | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.near` | The buildings whose box may reach the rectangle, in the store's order. | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.nearAny` | The buildings `meets` keeps for one of `rects`, asked of those within `pad` of it, each once, in the store's order: their ids. | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.nearIds` | The ids of `near`, none read. | não se aplica | b40::record_body lê o prédio direto |
| | `docBuildings.trafficBuilding` | DO NOT keep it per record on the page (`identityMemo`): a copy of every building of the map held for as long as the map, ~6 KB eac | não se aplica | b40::record_body lê o prédio direto |
| | `factsOf` | A traffic record's facts (`TrafficFacts`), worked out of it. | não se aplica | b40::record_body lê o prédio direto |
| | `frontageBound` | The most frontage a building's way on foot can have (`lotDoors.ts`): its blocks' width along its widest people's gate, or along it | não se aplica | b40::record_body lê o prédio direto |
| | `partOf` |  | não se aplica | b40::record_body lê o prédio direto |
| | `project` |  | não se aplica | b40::record_body lê o prédio direto |
| | `takeTrafficMap` | The worker's copy made from a map sent whole (`trafficMapOf`), each building read from its row and worked out here. | não se aplica | b40::record_body lê o prédio direto |
| | `trafficBuilding` | DO NOT keep it per record on the page (`identityMemo`): a copy of every building of the map held for as long as the map, ~6 KB eac | não se aplica | b40::record_body lê o prédio direto |
| | `trafficChangesOf` | In the worker: the buildings sent (`buildingChangesOf`) read from their rows and worked out as traffic reads them. | não se aplica | b40::record_body lê o prédio direto |
| | `trafficMapOf` | The page's map as the worker is sent it whole (a map opened, generated, new): no terrain, paint or scenery, no building's record - | não se aplica | b40::record_body lê o prédio direto |
| | `traitsOf` |  | não se aplica | b40::record_body lê o prédio direto |
| `src/sim/agents/walk.ts` | **arquivo** | caminhada dos pedestres | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ACCEL` | Acceleration and braking of a walker, u/s². | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ACROSS_PROJECTION` | `projectAcross`'s: read at once by its one caller in the walkers' step. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `AHEAD` | How far ahead on the route the body walks towards, u. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `AIM_FRAME` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `AIM_HOLD` | Seconds a stripe just taken is kept, unless it shuts. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `AIM_PROJECTION` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ALIGHT_SNAP` | How far from the door the walkway somebody gets out onto may be (`alight`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ALIGHT_TRIES` | Ways tried for somebody getting out of a car (`alight`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ASK_EVERY` | How often a waiting walker asks whether they may cross, seconds. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ASK_FROM` | A walker this far from the waiting place starts asking whether they may cross. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `BENDS` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `BEND_WINDOW` | Arc over which a line's bend is measured, u. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `BODY` | A body's radius: two people closer than twice this, side by side, touch. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `BODY_SHOCK` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `BRAKE` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `CELL` | As wide as a walker looks ahead (`LOOK`) | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `CRAWL_PACE` | A crawl's speed, u/s: the captured crawl's own (its feet and knees do not slide). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `CROSSING_COST` | Extra cost of a crossing on a route, u: a zebra is walked to only when worth it. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `CarZone` | Ground a walker keeps off: a car's body (and the way it holds), or a train's. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `DOOR_CLEAR` | Somebody this near the spot a walker steps out onto keeps them inside a moment, u. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `FLINCH` | A first wound's reaction (`flinch`): struck, hunched over the wound standing, then on, wounded (seconds). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `FOLD_SHARE` | Share of a bend's radius a walker may be inside it on a counter-bend: short of the fold. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `GAP_MARGIN` | Margin of clear road a walker wants beyond their own time across, s; after `IMPATIENT` waiting, none. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `GAP_NEAR` | A vehicle this near the crossing keeps a walker on the kerb whatever it is doing, u. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `GATE_JOIN` | Farthest a gate's foot may be from the walkway it is joined to. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `GRANT_LIFE` | Seconds a leave to cross holds for one standing still who has not yet stepped onto the carriageway. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `HAIL_REACH` | How far from a lane's centre somebody on the footway may be hailed from (`hailable`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `HEADWAY` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `HELD_SWITCH` | Held still this long, a quarter of that will do. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `IMPATIENT` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `INAPPROPRIATE` | Penalty of a stripe a walker must not take (SUMO's INAPPROPRIATE_PENALTY, as room ahead). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `JAM_AFTER` | Held still this long, a walker is let through others slowly (SUMO's jamtime: 10 s on a crossing). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `JAM_AFTER_CROSSING` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `JAM_AFTER_NARROW` | Where only one person fits abreast - a lot's path and its gate - SUMO's `pedestrian.striping.jamtime.narrow`, 1 s | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `JAM_NO_PROGRESS` | Face to face and getting no further along the way for this long, s | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `JAM_SHARE` | Speed of a jammed walker, as a share of their own. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `KEEP` | The gap a walker keeps to the body ahead, and the time headway behind it. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `KERB_BACK` | Where a walker stands to wait: this far back from the kerb. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `KNOCKED_MOST` | Knocked down by a light wound: the longest they can be down before they are up (`getUp` sets the real end). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `LATERAL_PENALTY` | Room given up for each unit across from where the body is: SUMO's LATERAL_PENALTY, a metre a stripe. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `LATERAL_SHARE` | Most a walker moves across towards its stripe, as a share of its forward speed (SUMO's LATERAL_SPEED_FACTOR). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `LOOK` | How far ahead others are looked for. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `LOT_AHEAD` | How far ahead, at most, inside a lot and through its gate (`target`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `MOVING` | Faster than this, somebody is walking (coming the other way, say), u/s. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `NEAREST` | Where `projectInto` reads the way's nearest point: made once. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `NO_POINTS` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ONCOMING_CONFLICT` | Penalty of the leftmost stripe while somebody comes the other way in it (SUMO's ONCOMING_CONFLICT_PENALTY, -1000). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `OTHERS` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `Other` | A body near the walker, in its own frame (`addOther`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `PANIC_RUN` | Running for their lives from a blow: a sprint. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `PROGRESS` | Getting this much further along the way is progress (`JAM_NO_PROGRESS`), u. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `PROJECT_FRAME` | The step frames of the hot callers, each its own (`project` runs inside `stepWalkers`' use of its own). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedEntry` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedSource` | The parked cars as their keeper tells their changes (`ambient/ambient.ts` AmbientWorld). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex` | DO NOT file every parked car again when one of them changes | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.byCar` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.cells` | The zones by cell, each in every cell its box reaches with a walker's look ahead round it. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.follow` | Brought up to the cars standing, from what their keeper says changed | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.keep` | A car filed where it stands, if it is not, or again if it moved. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.list` | What was seen, in the list's order. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.pass` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.place` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.serial` | The serial of the source last followed (`follow`); -1: never. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.sync` | Brings the cells up to `cars`, the cars standing now (`AmbientWorld.parked`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ParkedZoneIndex.unfile` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `QUEUE_GAP` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `QUEUE_REACH` | Behind somebody waiting for the zebra it is going to, this near its waiting place, a walker is in the queue | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `REACH` | Farthest a door or a car may be from the walkways, by default. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `RESERVE_MAX` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `RESERVE_ONCOMING` | Share of a corner's stripes, and most width, left to those coming the other way (SUMO's defaults). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ROOMS` | Each step's room, worked out once (it depends on the step alone) | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ROUTE_SCRATCH` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `RUN` | How much faster than their walk somebody runs away. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `RouteScratch` | DO NOT go back to `new Float64Array(n).fill(Infinity)` per search | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `SHOCK_LIFE` | A shocking event's life (seconds), and the reach of a body lying in the street. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `SHOULDERS` | Two people this far apart side by side pass each other, u. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `SHUFFLE` | Slower than this a walker faces the way its walk goes and steps aside sideways | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `SHUFFLE_COS` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `SIDESTEP` | Fastest a body steps sideways, u/s. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `SPAN` | The walker's span across its step, widened at a kerb or on a zebra: its own copy of the step's room. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `STAGGER_DISTANCE` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `STAGGER_FROM` | It starts after the hit itself (the captured jerk, `render/agents.ts`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `STAGGER_TIME` | Struck by the round: a stagger of a couple of steps back, the way it went | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `STATES` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `STEP_PROJECTION` | The projections of the walkers' step and of `target`: made once (each read before the next is made). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `STRIPE` | Width of one stripe, world units: SUMO's default, a person's room abreast. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `SWITCH_GAIN` | A new stripe must be this much better to be taken: no dithering between two. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `State` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `Step` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `StepFrame` | A place on a step and its way ahead (`frameInto`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `TARGET` | `target`'s point: made once, read by the walkers' step before it is asked again. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `THERE` | The end of a walk is reached this near it. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `TRAFFIC_POSE` | Each vehicle's pose as `gapOpen` files it, written in place. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `TRAIN_GROUND` | The ground of a train (its cars, the track just ahead): whoever is on it walks off it. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `TURN_RATE` | Fastest a body turns walking, and standing (on the spot), rad/s. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `TURN_STANDING` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WAIT_HALF` | Half the length of kerb people wait along at a zebra, u | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WALKER_SHAPE` | Every field a walker may be given later, there from its birth and in one order: added as things happened to each | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WALK_COMPONENTS` | Each node's component in the walk graph as it is (`WalkGraph.state`): labelled again when the graph changed and a route is asked. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WALK_FRAME` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WAY_CELL` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WAY_SAMPLE` | The way's frame `frameInto` samples into: made once. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WOUNDED_PACE` | A wounded walker's pace walking on, as a share of their own: slow, bent over the wound, never their own walk again. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WOUNDED_RUSH` | Getting away hurt, against their own pace: a run bent over the wound, as GTA's shot peds flee clutching it | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkTrace` | A test bench's trace of what happens to chosen walkers (the weapons lab, `traceWalkers`) | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `Walker` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid` | The walkers out on the street by cell of `CELL`, as Matthias Müller's dense spatial hash files particles | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.build` | Files everybody out on the street (not inside, not done) where they are now. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.cx` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.cy` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.entries` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.list` | What was seen, in the list's order. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.size` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.slot` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.someIn` | Whether `fn` is true of anybody filed in the cells [gx0, gx1] x [gy0, gy1]; stops at the first. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.someNear` | Whether `fn` is true of anybody filed within `reach` cells of (x, y); stops at the first. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerGrid.start` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `WalkerSeen` | Somebody walking now, as the player and the police see them. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ZEBRAS` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ZONES` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ZONE_CELL` | Cells for the cars' zones (`zonesNear`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `Zebra` | A crossing on a walker's way: a zebra as the vehicles and the signals know it (`edge`), or a crossing with no zebra | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `actName` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `addOther` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `addPlayerWalker` | A body for the player to move, out at a point (stepping out of a door or a car). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `bendRadii` | The tightest radius of a walkway's line turning left and turning right (along it from `a` to `b`) | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `bendRadii.heading` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `cell` | A grid cell as one number: a string key | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `cellKey` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `clamp` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `createAgentWalkEngine` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `createAgentWalkEngine.walk` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `crossingOf` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `ensureGraph` | DO NOT index every walkway again when the network changes: every road drawn filed the town's walkways again. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `fileWay` | A walkway filed in (or out of) the cells its usable width reaches. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `fileZones` | Zones by cell: each in every cell its box reaches with a walker's look ahead round it. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `finish` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `frame` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `frameInto` | `frame` into `out`: the walkers' step asks it several times a tick for every walker | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `freeAhead` | Free distance ahead in a stripe centred `c` across the step. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `frighten` | Somebody who saw a fright at (x, y), `d` off: running away from it, some tripping, crouching or fainting. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `gapOpen` | Whether a walker may step onto a crossing with no zebra | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `hypot` | Length of (x, y). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `insideDiscs` | Whether (x, y) is within `margin` of any of the first `count` discs. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `inspectAgentWalkers` | A read-only look at the walkers, for tests and diagnosis. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `joinFootway` | A lot's way (door ... gate, foot) joined to the footway | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `joinParty` | `p` filed with its party (`State.parties`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `joinsFootway` | Whether a lot's way out ending at `p` reaches a footway a walker can join there (`joinFootway`, within `GATE_JOIN`) | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `kerbInset` | How far a footway's walking line is from its kerb (where a crossing's end meets the road). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `lastOf` | The end of a walker's route: where they are going. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `legsLostOf` | How many legs a walker has lost (a hobble on one, no walking on none). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `lotSteps` | The straight stretches through a lot along `points` (a door to a gate's foot, or back). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `mourn` | Those walking with somebody shot (`startWalk` with): down beside them, crying, a few seconds, then away | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `movePlayerWalker` | The player's body moved: where it is now, the way it faces, how fast it goes. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `moved` | Whether a car stands otherwise than when it was filed (came, went, or moved). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `nearMoving` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `nearNow` | The walkers by cell for the cars' questions, filed at the first one after the walkers stepped (`State.near`) | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `nearParked` | The cars' zones by cell this tick (`stepWalkers`), and the ones in a walker's cell (`zonesNearInto`). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `nearestWay` | The walkway nearest a point within `reach` (crossings excluded: nobody starts on a zebra). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `newStepFrame` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `onLastStretch` | Whether somebody out on the street is on the last stretch of their walk, where `walkOn` takes them on. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `onZebra` | Whether a walker is out on a zebra's carriageway, kerb to kerb. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `othersN` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `parkedZone` | A car's body as a zone: three discs along its length, half its width round. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `plan` | The steps from a point to a point: to the walkways, along them, and off to the end. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `project` | A body projected on a step: how far along it, and how far left of it. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `projectAcross` | `project` of a body onto a step. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `projectInto` | `project` into `out` (every walker, several times a tick). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `prune` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `publish` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `queuedForZebra` | Whether the nearest body ahead in its own stripe (`d` across), near, stands waiting for `z`. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `releaseWalker` | Gives a body back from the player's hand, walking on to `to` (their home, say); null: off the street. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `removeWalker` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `replan` | A walker's route made again from where they are to `to`. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `reroute` | A walker's route found again to `to`, keeping what runs through a lot (`Step.lot`) | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `reserveCost` | What a stripe centred `c` costs for lying in the stripes kept for those coming the other way. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `reservedFor` | The width at the left of a step, in the way walked, left to those coming the other way: corners only. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `room` | The lateral room of a step in its own walking frame, [low, high], left positive, for a body's centre. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `roomOf` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `route` | DO NOT search without it: the scenery's people come in at the map's edge for the far side | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `route.distOf` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `route.estimate` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `route.push` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `routeScratch` | The graph's search arrays, on a new generation, the heap empty. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `rushName` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `seeWalker` | Another walker as `seer` sees it: one of its others if ahead within `LOOK`. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `seen` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `seer` | The walker looking round (`seeWalker`), and its frame. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `seerFrame` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `sendRunning` | Somebody sent running towards a point (a police officer after the player), their own route on the walkways. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `shock` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `staggerSpeed` | The stagger's speed `t` seconds into a flinch: fast at first, slowing to a stop. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `startWalk` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `startle` | Everybody within `radius` of a fright | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `stateOf` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `stepLength` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `stepWalkers` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `stepWalkers.enter` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `takeWalker` | Puts somebody walking under the player's hand: they stop following their route. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `target` | The point the body walks towards: a little ahead on its route, in its stripe. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `trace` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `traceSink` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `traceTick` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `traceWalkers` | Traces these walkers' changes into `sink` (an empty list stops it). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `traced` |  | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkComponents` | @rebuild The graph's nodes labelled by their component (the ways joining them), once per state of the graph. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkOn` | On their last stretch, a walk goes on to somewhere else instead of ending | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkThrough` | A walk that begins by coming out through a lot | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkWork` | The work of the walkers' derived products, counted (counts, not times: the same on every machine), for the scale specs | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkerAct` | Somebody stops a while to do something (talk, fall down), facing a point. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkerAlive` | Whether a walker is still walking (out on the street, or waiting inside to step out). | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkerOf` | One walker, by id, or null when nobody of that id is out. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkerState` | One walker as a test bench reads them (the weapons lab) | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkersNear` | The walkers within `radius` of a point, out on the street. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `walkersWithin` | DO NOT walk `State.walkers` for a place: every walker of the map, for each blow and each look. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `wayCost` | The cost of walking a way: its length, and more for a zebra. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `woundOf` | How a shot walker is hurt, from their health and what struck them: none, light (most of their health left) or grave | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `woundPace` | Which of those a wound sets. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `zebraAhead` | The zebra a walker is coming up to and has not been let onto, and the distance along its route to where it waits for it | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| | `zonesNearInto` | The zones in `p`'s cell, the standing cars' first, into `ZONES`; how many. | já existia | sim/traffic/pedestrians.cpp, ped_crowd.cpp; dentro dos lotes: `people` |
| `src/sim/ambient/ambient.ts` | **arquivo** | carros parados de cenário e povoamento | portado | sim/traffic/parking_sources.cpp |
| | `AmbientWorld.addParked` |  | portado | sim/traffic/parking_sources.cpp |
| | `AmbientWorld.release` |  | portado | sim/traffic/parking_sources.cpp |
| | `AmbientWorld.sceneryCarAt` |  | portado | sim/traffic/parking_sources.cpp |
| `src/sim/ambient/ambientIndex.ts` | **arquivo** | índices do ambiente | não se aplica | SpatialHash |
| | `BuildingMix` | How many buildings of each kind (`demand.ts` kindOf) stand where. DO NOT hold a building an object: its place, kind, cell and plac | não se aplica | SpatialHash |
| | `BuildingMix.all` |  | não se aplica | SpatialHash |
| | `BuildingMix.around` | The share of each kind among the buildings within `r * 1.2` of (x, y); r = Infinity, the whole map. | não se aplica | SpatialHash |
| | `BuildingMix.cellOf` | Its cell's key; NaN: its place is not a number. | não se aplica | SpatialHash |
| | `BuildingMix.cells` |  | não se aplica | SpatialHash |
| | `BuildingMix.constructor` |  | não se aplica | SpatialHash |
| | `BuildingMix.file` | A building's walls filed (none for a building not in the store), by its footprints alone (`TrafficBuildings.footprints`). | não se aplica | SpatialHash |
| | `BuildingMix.fileAll` | @rebuild The whole store read again: a load, a clear, or a diary that forgot. | não se aplica | SpatialHash |
| | `BuildingMix.inCircle` |  | não se aplica | SpatialHash |
| | `BuildingMix.inCircle.visit` |  | não se aplica | SpatialHash |
| | `BuildingMix.kinds` |  | não se aplica | SpatialHash |
| | `BuildingMix.memberAt` |  | não se aplica | SpatialHash |
| | `BuildingMix.nowhere` | Buildings whose place is not a number: within every circle, as `hypot(NaN) > r` is false. | não se aplica | SpatialHash |
| | `BuildingMix.serial` | DO NOT follow the diary from here (`JournalCursor`): a reader that stops reading (no view to look through) keeps every entry after | não se aplica | SpatialHash |
| | `BuildingMix.slots` |  | não se aplica | SpatialHash |
| | `BuildingMix.sync` | Takes in what the diary says changed since the last call. | não se aplica | SpatialHash |
| | `BuildingMix.unfile` |  | não se aplica | SpatialHash |
| | `BuildingMix.xs` |  | não se aplica | SpatialHash |
| | `BuildingMix.ys` |  | não se aplica | SpatialHash |
| | `Counts` |  | não se aplica | SpatialHash |
| | `Kind` |  | não se aplica | SpatialHash |
| | `MIX_CELL` | The side of a mix cell. | não se aplica | SpatialHash |
| | `MIX_KINDS` |  | não se aplica | SpatialHash |
| | `MIX_SLACK` | Rounding room in the test of a whole cell inside or outside the circle: the cells near the edge are counted building by building. | não se aplica | SpatialHash |
| | `SPOT_ROW` | The rows of `spotKey`: a place's step across times this, plus its step along (under half of it either way). | não se aplica | SpatialHash |
| | `SightWalls` |  | não se aplica | SpatialHash |
| | `SightWalls.blocked` | Whether a side of some wall properly crosses the segment a-b. | não se aplica | SpatialHash |
| | `SightWalls.boxes` | Each wall's box (x0, y0, x1, y1), the last query that read it, the next wall of its building (-1: none); its ring's x, y in `rings | não se aplica | SpatialHash |
| | `SightWalls.buildings` | Each building's first wall, its walls linked by `next`. | não se aplica | SpatialHash |
| | `SightWalls.cells` |  | não se aplica | SpatialHash |
| | `SightWalls.constructor` |  | não se aplica | SpatialHash |
| | `SightWalls.file` | A building's walls filed (none for a building not in the store), by its footprints alone (`TrafficBuildings.footprints`). | não se aplica | SpatialHash |
| | `SightWalls.fileAll` | @rebuild The whole store read again: a load, a clear, or a diary that forgot. | não se aplica | SpatialHash |
| | `SightWalls.first` |  | não se aplica | SpatialHash |
| | `SightWalls.freeWalls` |  | não se aplica | SpatialHash |
| | `SightWalls.marks` |  | não se aplica | SpatialHash |
| | `SightWalls.newWall` |  | não se aplica | SpatialHash |
| | `SightWalls.next` |  | não se aplica | SpatialHash |
| | `SightWalls.query` |  | não se aplica | SpatialHash |
| | `SightWalls.rings` |  | não se aplica | SpatialHash |
| | `SightWalls.serial` | DO NOT follow the diary from here (`JournalCursor`): a reader that stops reading (no view to look through) keeps every entry after | não se aplica | SpatialHash |
| | `SightWalls.sync` | Takes in what the diary says changed since the last call. | não se aplica | SpatialHash |
| | `SightWalls.unfile` |  | não se aplica | SpatialHash |
| | `SightWalls.wallCount` |  | não se aplica | SpatialHash |
| | `StreetLength` | Street length within `r` of (x, y), world units | não se aplica | SpatialHash |
| | `StreetLength.add` | The pieces of `ribbon` whose middle is within `r` of (x, y), added to `total` in order. | não se aplica | SpatialHash |
| | `StreetLength.around` | The share of each kind among the buildings within `r * 1.2` of (x, y); r = Infinity, the whole map. | não se aplica | SpatialHash |
| | `StreetLength.pieces` | Each ribbon's straight pieces as the walk reads them: middle x, middle y, length. Ribbons are never changed in place. | não se aplica | SpatialHash |
| | `StreetLength.whole` | The street length of the whole map (`world/networkLength.ts`). | não se aplica | SpatialHash |
| | `StreetLength.wholeMap` | DO NOT sum every ribbon of the map when the network moves: every road drawn walked the town's roads. | não se aplica | SpatialHash |
| | `WALL_CELL` | The side of a wall cell: a few buildings each, a sight line of 300 m reads some twenty. | não se aplica | SpatialHash |
| | `WALL_SLACK` | Walls are filed over their box grown by this | não se aplica | SpatialHash |
| | `ambientIndexWork` | Work done by the indexes, for the tests: buildings filed, cells read, rings tested, whole reads | não se aplica | SpatialHash |
| | `atRoll` | The item a roll falls on, items laid end to end: the first `k` with `roll < runningSum[k]` | não se aplica | SpatialHash |
| | `buildingsChangedSince` | The buildings the diary names as changed after `serial`; null when they must all be read again | não se aplica | SpatialHash |
| | `cellKey` | A cell's key from its column and row (both within ±32 767), as `world/cells.ts`. | não se aplica | SpatialHash |
| | `crosses` | Whether segment a-b crosses segment c-d (proper crossing). | não se aplica | SpatialHash |
| | `noCounts` |  | não se aplica | SpatialHash |
| | `spotKey` | DO NOT key a bay by text: its place as one number, in steps of 0.1 m, one to a place within 3,355 km of the origin (6,710 km acros | não se aplica | SpatialHash |
| `src/sim/ambient/demand.ts` | **arquivo** | tipo do lugar para a demanda | portado | sim/traffic/lot_traffic.cpp |
| | `CARS` |  | portado | sim/traffic/lot_traffic.cpp |
| | `ERRAND_STAY` |  | portado | sim/traffic/lot_traffic.cpp |
| | `Mix` | What kind of place the view is on: shares of each, from the buildings round it. | portado | sim/traffic/lot_traffic.cpp |
| | `PEOPLE` | People per 100 m of street at full bustle, and cars | portado | sim/traffic/lot_traffic.cpp |
| | `PURCHASING` |  | portado | sim/traffic/lot_traffic.cpp |
| | `WORKING` | Percent of people (15 and over) working, and purchasing goods and services, at each hour of the day from midnight | portado | sim/traffic/lot_traffic.cpp |
| | `WORK_STAY` | How long a stay lasts, hours: a working day, an errand. | portado | sim/traffic/lot_traffic.cpp |
| | `atHour` | A table of the hours at `hour` (fractional, wrapping round midnight). | portado | sim/traffic/lot_traffic.cpp |
| | `busy` | How busy each kind of place is at an hour (0..1): bumps at the hours people go out (GTA's day of the popcycle). | portado | sim/traffic/lot_traffic.cpp |
| | `busy.bump` |  | portado | sim/traffic/lot_traffic.cpp |
| | `flows` | People going in at and coming out of a kind of place at an hour (shares of everybody, an hour): from how many are there | portado | sim/traffic/lot_traffic.cpp |
| | `kindOf` | What kind of place a building is, from its function or its zone (a record, or its traffic record). | portado | sim/traffic/lot_traffic.cpp |
| | `outShare` | Share of the walks at a kind of place, at an hour, that come out of it (the rest go in). | portado | sim/traffic/lot_traffic.cpp |
| `src/sim/ambient/lotPlaces.ts` | **arquivo** | lugares a pé | de outra funcionalidade | funcionalidade `people` |
| | `FEET_PAD` | How far past a building's box (`storedBounds`: its blocks and its lot's parts) the foot of its way can be | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick` | Whole-number weights in a Fenwick tree | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick.cap` |  | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick.find` | The first item whose running sum passes `v` (0 <= v < total); -1 when none does. | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick.get` |  | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick.grow` | @rebuild Twice the room, the tree laid again from the weights (each item's room doubles at most once per doubling). | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick.raw` |  | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick.set` |  | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick.total` | The sum of every weight. | de outra funcionalidade | funcionalidade `people` |
| | `Fenwick.tree` |  | de outra funcionalidade | funcionalidade `people` |
| | `KIND_INDEX` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.bound` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.building` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.byBuilding` | Each building's place, in the order they were filed. | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.cell` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.cellAt` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.cells` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.constructor` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.feet` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.file` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.fileAll` | @rebuild The whole store read again: a load, a clear, or a diary that forgot. By the buildings' facts alone (`TrafficFacts`): no t | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.freePlaces` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.freeSlots` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.frontages` | The frontage of the lots that may be walked to, by kind (`PLACE_KINDS` order), world units. | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.kind` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.newPlace` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.pick` | A lot drawn by `coef[kind] x frontage`, or, given `not` | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.pickAny` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.pickFrom` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.placeCount` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.ready` | Whether any lot may be walked to (one whose way is not yet known counts). | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.serial` | The diary read by serial, not followed (a reader that stops reading would keep every entry after it). | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.slot` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.slots` | Each kind's tree's slots' places (-1: none). | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.spread` | The farthest a member's feet reach from the middle its cell is chosen by (grown, never shrunk). | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.store` | The store last read: a way is worked out from its building's traffic record, asked when drawn. | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.streetish` | The places whose way is a street door's (good or footless): looked at again when the roads near change. | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.sync` | Takes in the buildings and the roads the diary says changed since the last call. | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.trees` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.unfile` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.unknown` | Back to not known: drawn by its bound until worked out again. | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.wayOf` | A building's way as a place (worked out now if it was not yet); undefined for a building not filed. | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.ways` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.weigh` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.weight` |  | de outra funcionalidade | funcionalidade `people` |
| | `LotPlaces.workOut` | The lot's way worked out (`lotDoors.ts`) from its building's traffic record, its weight from now on its frontage (0: nobody walks  | de outra funcionalidade | funcionalidade `people` |
| | `MAX_PROPOSALS` | Proposals a draw makes before it gives up (`LotPlaces.pick`). | de outra funcionalidade | funcionalidade `people` |
| | `PLACE_CELL` | The side of a place cell, for the walk from one lot to another. | de outra funcionalidade | funcionalidade `people` |
| | `PLACE_KINDS` | The kinds of place, in the order of the trees and of the coefficients a draw is given. | de outra funcionalidade | funcionalidade `people` |
| | `PlaceCell` |  | de outra funcionalidade | funcionalidade `people` |
| | `PlaceDeps` | What the places are worked out with: the lot's way (`lotDoors.ts` doorWay); its bound is the store's (`TrafficFacts.frontage`). | de outra funcionalidade | funcionalidade `people` |
| | `PlaceWay` | A lot's way on foot from its door to the footway (`agents/lotDoors.ts` DoorWay). | de outra funcionalidade | funcionalidade `people` |
| | `ROAD_REACH` | How far round a road edit a street door's foot is looked at again | de outra funcionalidade | funcionalidade `people` |
| | `WEIGHT_SCALE` | Weights are frontages in millionths of a unit, whole: a frontage of 6 m or more moves by less than one part in six million. | de outra funcionalidade | funcionalidade `people` |
| | `weightOf` |  | de outra funcionalidade | funcionalidade `people` |
| `src/sim/ambient/playerId.ts` | **arquivo** | ids do jogador | não se aplica | sem multijogador |
| | `PLAYER_ID` | The id the person walked by the player has among the walkers (`sim/ambient/play.ts`). | não se aplica | sem multijogador |
| `src/sim/city/city.ts` | **arquivo** | passo da cidade | já existia | Simulation::tick |
| `src/sim/crossings/permission.ts` | **arquivo** | permissão nas travessias | já existia | sim/traffic/ped_crossing.cpp |
| | `PED_COURTESY` | Seconds a car stands at a zebra giving way to people arriving at its kerb before it is its turn. | já existia | sim/traffic/ped_crossing.cpp |
| | `RESERVATIONS` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `Reservation` | DO NOT key it by a `node:segment` text again | já existia | sim/traffic/ped_crossing.cpp |
| | `alreadyReserved` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `crossingReservedByVehicle` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `heldOnlyByPedestrians` | Whether the only thing keeping a car at its line is people at or on a zebra | já existia | sim/traffic/ped_crossing.cpp |
| | `indexReservations` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `mayEnterCrossing` | Whether a pedestrian may step off the kerb. | já existia | sim/traffic/ped_crossing.cpp |
| | `pedGapAccepted` | Gap acceptance against approaching traffic on the lanes being crossed. | já existia | sim/traffic/ped_crossing.cpp |
| | `reservedIds` | The connectors one vehicle reserves, this tick: cleared and refilled, never reallocated. | já existia | sim/traffic/ped_crossing.cpp |
| | `turnOwedToVehicle` | Whether a car at its line, at green | já existia | sim/traffic/ped_crossing.cpp |
| | `vehicleBodyIntersectsCrossing` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `vehicleBodyIntersectsCrossing.clip` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `vehicleBodyIntersectsCrossing.toBody` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `vehicleBodyOnCrossing` | An approach vehicle's body can reach a zebra even without an admission token. | já existia | sim/traffic/ped_crossing.cpp |
| `src/sim/crossings/state.ts` | **arquivo** | estado das travessias | já existia | sim/traffic/ped_crossing.cpp |
| | `emptyCrossingState` |  | já existia | sim/traffic/ped_crossing.cpp |
| `src/sim/drive/operational.ts` | **arquivo** | nível operacional | já existia | sim/traffic/vehicle_motion.cpp |
| | `COOLNESS` | Share of the heuristic in the blend (Treiber & Kesting use 0.99). | já existia | sim/traffic/vehicle_motion.cpp |
| | `CRAWL` | Below this a driver lifts off at once. | já existia | sim/traffic/vehicle_motion.cpp |
| | `DELTA` | IDM acceleration exponent. | já existia | sim/traffic/vehicle_motion.cpp |
| | `JERK_DOWN` |  | já existia | sim/traffic/vehicle_motion.cpp |
| | `JERK_UP` | Fastest a driver's acceleration rises (easing on), and falls (braking harder), u/s³. | já existia | sim/traffic/vehicle_motion.cpp |
| | `accAccel` | Adaptive Cruise Control acceleration against one obstacle. | já existia | sim/traffic/vehicle_motion.cpp |
| | `cah` | The Constant-Acceleration Heuristic: braking needed if the obstacle keeps its acceleration. | já existia | sim/traffic/vehicle_motion.cpp |
| | `iidm` | Improved IDM against one obstacle, and the free-road term. | já existia | sim/traffic/vehicle_motion.cpp |
| | `nextSpeed` | The next speed and the acceleration that produced it. | já existia | sim/traffic/vehicle_motion.cpp |
| `src/sim/drive/physicalMotion.ts` | **arquivo** | movimento físico | já existia | sim/traffic/vehicle_motion.cpp |
| | `PHYSICAL_GAP` | The standstill gap that physically stops a body: a hand's breadth, not the driver's preferred one. | já existia | sim/traffic/vehicle_motion.cpp |
| | `physicalSpeed` | Separate the driver's preferred standing gap from physical stopping room. | já existia | sim/traffic/vehicle_motion.cpp |
| `src/sim/drive/tactical.ts` | **arquivo** | nível tático | já existia | sim/traffic/vehicle_motion.cpp |
| | `CHANGE_COST` | Seconds a lane change costs the trip, per lane crossed. Also the hysteresis. | já existia | sim/traffic/vehicle_motion.cpp |
| | `ROOM_PER_LANE` | Road a change needs, per lane crossed. | já existia | sim/traffic/vehicle_motion.cpp |
| | `TripPlan` |  | já existia | sim/traffic/vehicle_motion.cpp |
| | `connectorCost` | Seconds to drive on through a connector and the link it leads to, with its congestion. | já existia | sim/traffic/vehicle_motion.cpp |
| | `planTrip` | DO NOT go back to Dijkstra's order (cost alone) | já existia | sim/traffic/vehicle_motion.cpp |
| | `planTrip.toGo` |  | já existia | sim/traffic/vehicle_motion.cpp |
| | `relax` | DO NOT make anything for a state before knowing it is kept: most are not (the allocation profile, 2026-10-17: `planTrip` 317 MB in | já existia | sim/traffic/vehicle_motion.cpp |
| `src/sim/host/fleetGrid.ts` | **arquivo** | grade da frota do worker | não se aplica | lane_vehicles e micro_list |
| | `FleetGrid.cellsNear` |  | não se aplica | lane_vehicles e micro_list |
| `src/sim/host/protocol.ts` | **arquivo** | mensagens worker-página | não se aplica | simulação na mesma memória; PoseBuffer |
| `src/sim/host/simAnswers.ts` | **arquivo** | respostas do worker | não se aplica | API CityCore |
| `src/sim/host/simClient.ts` | **arquivo** | lado da página do worker | não se aplica | PoseBuffer |
| | `SimClient.ask` |  | não se aplica | PoseBuffer |
| | `SimClient.noDeltas` |  | não se aplica | PoseBuffer |
| | `SimClient.parkedCellsIn` |  | não se aplica | PoseBuffer |
| | `SimClient.takeMeso` |  | não se aplica | PoseBuffer |
| `src/sim/host/simHost.ts` | **arquivo** | lado do worker | não se aplica | Simulation |
| `src/sim/intersections/admission.ts` | **arquivo** | quem entra no cruzamento | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass` | The Banker's tests' view of one admission pass (`stepAdmission` makes one and drops it at the end). | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass.byResource` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass.changed` | A grant or a reservation changed what this vehicle holds or may claim. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass.constructor` | `holders`: this pass's set, which grants add to (`granted`). | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass.file` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass.filed` | The holders filed under each resource of their maximum claims. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass.holdingOf` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass.holdings` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerPass.refile` | Vehicles whose holding changed since they were filed: filed again at the next test. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `BankerProcess` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `CLEAR_MARGIN` | Allowance over the estimated time for a front to reach a zebra it turns across. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `CONNECTOR_RESOURCES` | Conflict points plus a connector token for zero-point movements. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `CROSSINGS_AT` | DO NOT spell a crossing's id (`node:segment`) per request per tick: a junction's, in the order of its legs, made once per node rec | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `ConnectorReservation` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `Holding` | What one vehicle holds and may yet claim, for the Banker's test. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `KEEP_DECLARED` | A head on red whose declared chain stands: reserve only, keep `reservedConnectors`. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `MOVEMENT_TOKENS` | DO NOT keep them for the session: every connector id ever met stayed in it, across every edit and every map opened. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `NO_LEGS` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `RANK` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `Request` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `ResourceKey` | A resource of the Banker's test as a number: a conflict point is its own index | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `RowClass` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `Verdict` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `ZIP_PATIENCE` | Seconds a head waits for the other lane to take its turn before going itself. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `ZIP_READY` | How close to its line a rolling head of the other lane must be to take its turn, world units (standing heads always may). | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `actualAllocation` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `admissionProbe` | Each Banker's verdict with the applicant and the movements asked for: for the specs only (`tests/sim/admissionGroup.spec.ts`). | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `admit` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `admittedBy` | This pass's vehicles by the connector they were admitted to. Null outside `stepAdmission`. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `aheadOnMovement` | The latest time, with `CLEAR_MARGIN`, at which a vehicle already admitted onto this movement | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `aheadOnMovement.consider` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `arrivalDirection` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `bankerSafeAfterGrant` | DO NOT go back to iterating every holder (`holders`, `w.vehicles`) here. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `bankerSafeInGroup` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `byPriority` | Deterministic global order: arrival, then right of way, distance and id. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `clearTime` | Seconds for the FRONT to reach the point where the body has left the last conflict zone of this movement. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `comesFrom` | Whether `other` arrives from `side` of `conn`'s driver: by the directions the two lanes come in on. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `compactReservationChain` | Movement chain that must be declared before entering `first`. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `connectorResources` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `continues` | Whether a movement carries its road on through the node | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `convoyCanEnter` | A follower may share a same-path claim only at the stop line | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `crossingBusy` | True when a pedestrian is on, or about to step onto, the stretch of a zebra this movement drives over (`CrossingSpans`). | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `crossingLegs` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `crossingReachedFirst` | Whether somebody already on a zebra this movement crosses will reach the stretch it drives over before its body has | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `evaluate` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `exitSwingBlocked` | Whether a long body turning into a lane would sweep a vehicle beside it. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `granted` | A vehicle granted something in this pass: it holds now, and is admitted to `conn`. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `hasAcceptableGap` | Gap acceptance against the movements this one yields to. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `holderState` | Where a claim holder's body is along the movement it holds. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `holders` | This pass's vehicles that hold something (a non-empty allocation), and those granted something since it began. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `holdsAnything` | Whether a vehicle holds anything at all, without building its sets. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `mainRoadRank` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `mainRoads` | Rank of the road a junction is on, per topology build. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `mergeFollows` | Whether this vehicle may join a merge behind a holder on the other movement. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `mergeRemaining` | Distance from a vehicle's front to the start of the lane its movement runs into; negative once the front is past it. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `movementIsActive` | True when the other movement currently has permission to run. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `movementReservedByOther` | Whether a connector with no conflict points is already spoken for. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `movementToken` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `nextConnector` | The connector this vehicle intends to take at the end of its lane. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `outranksForExit` | Whether a vehicle with a better right of way is waiting for the same lane out. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `pedestrianHasPriority` | Whether somebody waiting at a kerb has the right to cross in front of this movement before it is admitted | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `pending` | This pass's requests, by node. Null outside `stepAdmission`. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `protectedNow` | True when this movement is green and protected in the current stage. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `queuedBodyInZone` |  | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `revokeStaleGrants` | Takes back a grant whose signal went red before the vehicle used it. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `rightOfWay` | Right of way for one movement. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `sharedConvoyResource` | A conflict resource may have several holders only while no two of them can touch there | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `stepAdmission` | Grants or denies entry to every junction, once per tick. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `stopShortOfIntrusion` | Distance to stop at so the body stays outside a zone that reaches back over the stop line while another movement holds | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `stoppedForWalker` | True when the vehicle stands for somebody on foot between its nose and its stop line | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `throughRank` | Authored priority, retaining the exact legacy class ordering on old roads. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `travelTime` | Seconds to cover `d` from `v0`, accelerating at `a` up to `top`. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `yieldSide` | The side a movement gives way to at a junction whose rule is a side | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| | `zipperHolds` | Whether this vehicle leaves the next place in a merge to the other lane. | já existia | sim/traffic/junction_admission.cpp, junction_control.cpp |
| `src/sim/intersections/claims.ts` | **arquivo** | reservas do cruzamento | já existia | sim/traffic/junction_admission.cpp |
| | `Claim` |  | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable` | Occupancy of the swept conflict zones inside junctions. | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.available` | True when every listed zone is free, already held by this vehicle, held by a vehicle on this exact connector | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.byPoint` | Several vehicles may convoy through the same connector, never a conflict. | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.byVehicle` |  | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.clear` |  | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.dropMissing` | Drops claims whose conflict point no longer exists after an edit. | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.dropVehicleAtPoint` |  | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.grantAll` | Grants the whole set atomically. Callers must check `available` first. | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.holder` | Holder of a conflict point, if any. | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.holders` |  | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.holdersAt` | Every current holder, used where a single connector may carry a convoy. | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.holds` |  | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.points` |  | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.releaseAll` |  | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.releaseConnector` | Releases every point held for one connector, preserving other movements. | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.releasePassed` | Releases zones the vehicle's body has already driven out of. | já existia | sim/traffic/junction_admission.cpp |
| | `ClaimTable.size` |  | já existia | sim/traffic/junction_admission.cpp |
| | `HolderState` | Where a claim holder's body is, along the movement it holds. | já existia | sim/traffic/junction_admission.cpp |
| | `LocateHolder` | Resolves a claim to its holder's live position; null when it cannot be placed. | já existia | sim/traffic/junction_admission.cpp |
| | `clearedAt` | Body-centre position past which `cls` on `connector` is clear of the zone. | já existia | sim/traffic/junction_admission.cpp |
| | `zoneShareable` | Whether two movements may hold one conflict zone together right now. | já existia | sim/traffic/junction_admission.cpp |
| `src/sim/intersections/crossingSpans.ts` | **arquivo** | trechos de travessia | já existia | sim/traffic/ped_crossing.cpp |
| | `CLEAR_PAST` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpan` | Arc interval of a crossing, measured from its `from` kerb. | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans` | The part of each zebra a movement actually drives over. | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.approachStop` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.approachStops` | Earliest safe front stop on an incoming lane near another leg's zebra. | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.build` | @rebuild Every junction: a world's first topology, or a lane graph built whole. | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.measureJunction` | One junction's spans: each movement over each zebra, and each approach's stop before another leg's. | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.measureJunction.samplesOf` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.measured` | Every span of the previous build, with the two paths it was measured on. | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.ofJunction` | What each junction measured: its connectors, its approach lanes and the keys of `measured` it made. | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.span` | Undefined: never measured (treat conservatively). Null: never crossed. | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.spans` | By connector, then by crossing: the two ids as they are, never a key string joined from them on every ask | já existia | sim/traffic/ped_crossing.cpp |
| | `CrossingSpans.update` | DO NOT measure every movement of the map for an edit: every road drawn measured the town's zebras again. | já existia | sim/traffic/ped_crossing.cpp |
| | `Measured` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `OUT_OF_BOX` | How far outside its own box a rounded point of a line may fall, u (`measure`). | já existia | sim/traffic/ped_crossing.cpp |
| | `PED_BODY` | A person's radius, a minimum pace assumed for someone about to move, and the look-ahead. | já existia | sim/traffic/ped_crossing.cpp |
| | `PED_CROSSING_STOP_BUFFER` | Reserve the near half of a zebra before stopping a car for a pedestrian. | já existia | sim/traffic/ped_crossing.cpp |
| | `PED_MIN_PACE` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `PED_REACH_TIME` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `PERSON_CLEAR` | A walker's clearance from a vehicle body (`PedestrianClearance`), plus margin. | já existia | sim/traffic/ped_crossing.cpp |
| | `REACH` | Clearance kept between a vehicle body and a pedestrian | já existia | sim/traffic/ped_crossing.cpp |
| | `SAMPLE` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `key` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `measure` | The stretch of a crossing (its `samples`, `sampleZebra`) that a movement along `path` drives over. | já existia | sim/traffic/ped_crossing.cpp |
| | `measure.gap` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `pedestrianAffectsSpan` | Whether somebody on a crossing occupies or will soon reach this movement's part of it. | já existia | sim/traffic/ped_crossing.cpp |
| | `pedestrianInSpan` | Whether somebody on this crossing is in, or about to enter, the span. | já existia | sim/traffic/ped_crossing.cpp |
| | `reservationCoversCrossing` | Whether this vehicle's current reservation still protects a zebra span. | já existia | sim/traffic/ped_crossing.cpp |
| | `sameFloats` |  | já existia | sim/traffic/ped_crossing.cpp |
| | `sampleZebra` | A crossing `length` long sampled every `SAMPLE`: s, x, y a sample. | já existia | sim/traffic/ped_crossing.cpp |
| `src/sim/intersections/cycles.ts` | **arquivo** | ciclos de espera (antitravamento) | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex` | DO NOT search every link of the map again for an edit: every road drawn did the town's searches again. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.byLanelet` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.follow` | Brought up to the lane graph's changes since it was last. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.forget` | A link's search taken away: its reads, and its share of the loop it found. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.laneReaders` | The links whose search read each lanelet, and each junction. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.loops` | Each loop found, by its links, and how many searches found it. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.nextId` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.nodeReaders` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.ofStart` | Each link searched: the loop it found (its key), and what the search read. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.restart` | @rebuild Everything forgotten, and every link to search. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.search` | One link's shortest way back to itself, and what it read on the way. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.search.length` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.search.next` | Links reachable from a link through one connector that is not a U-turn. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `CycleIndex.seen` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `MAX_CYCLE_STORAGE` | Loops with more storage than this are a street network, not a ring. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `METER_SHARE` | Share of a loop's storage past which nobody more is let on. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `OpenSet` | DO NOT go back to sorting the open list for every pop. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `OpenSet.items` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `OpenSet.pop` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `OpenSet.push` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `OpenSet.size` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `RoadCycle` | A small closed loop of road: its lanelets and the queue it can hold. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `before` | Cheaper first, then the lower id. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `cycleFull` | DO NOT go back to walking `w.vehicles` here: that was the whole fleet for every request onto a ring, every tick. | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `cyclesOf` | The small loops a lanelet belongs to, kept by what each edit changed (`CycleIndex`). | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `file` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `indexOf` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `indexes` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| | `unfile` |  | já existia | sim/traffic/junction_admission.cpp jam_release |
| `src/sim/intersections/spillback.ts` | **arquivo** | saída cheia | já existia | sim/traffic/junction_admission.cpp has_exit |
| | `endsAtSignal` | Whether a lane ends at a signalised junction. | já existia | sim/traffic/junction_admission.cpp has_exit |
| | `hasDownstreamStorage` | `strict`: a movement that gives way | já existia | sim/traffic/junction_admission.cpp has_exit |
| | `restingRear` | Where the rear of the last vehicle on a lane will come to rest. | já existia | sim/traffic/junction_admission.cpp has_exit |
| | `standingReach` | How much further a vehicle will go before something standing still holds it | já existia | sim/traffic/junction_admission.cpp has_exit |
| `src/sim/invariants.ts` | **arquivo** | auditoria do tráfego | já existia | traffic_probe e soak |
| `src/sim/meso/demand.ts` | **arquivo** | demanda do modelo distante | já existia | sim/traffic/demand.cpp |
| | `DEMAND_CELL` | The world's cell (CLAUDE.md, one grid). | já existia | sim/traffic/demand.cpp |
| | `DemandCells` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.atHour` | The hour of the day the next questions are asked at. | já existia | sim/traffic/demand.cpp |
| | `DemandCells.byBuilding` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.carsPer100m` | Cars per 100 m of street the cell's buildings put on it at the hour, scaled by how built it is (none in an empty cell). | já existia | sim/traffic/demand.cpp |
| | `DemandCells.cellAt` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.cellOf` | The dense index of the cell a point is in (made on first ask). | já existia | sim/traffic/demand.cpp |
| | `DemandCells.cells` | Cells given an index so far: every index below it is valid. | já existia | sim/traffic/demand.cpp |
| | `DemandCells.constructor` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.counts` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.file` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.index` | Each cell key's dense index; indexes are never given back (a cell is a 256 m square: a city has a few thousand). | já existia | sim/traffic/demand.cpp |
| | `DemandCells.kindAt` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.now` | The cells' busyness of the hour (`busy`), by kind, set by `atHour`. | já existia | sim/traffic/demand.cpp |
| | `DemandCells.serial` | The diary read by serial, not followed (a reader that stops reading would keep every entry after it). | já existia | sim/traffic/demand.cpp |
| | `DemandCells.sync` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.unfile` |  | já existia | sim/traffic/demand.cpp |
| | `DemandCells.weight` | How built the cell is, 0 to 1: the share of trips its streets end. | já existia | sim/traffic/demand.cpp |
| | `FULL_CELL` | Buildings in a cell at which its streets carry the ambient's full density (`ambient/demand.ts` CARS); fewer, in proportion. | já existia | sim/traffic/demand.cpp |
| | `KINDS` |  | já existia | sim/traffic/demand.cpp |
| | `STRIDE` | Numbers a cell keeps: its buildings by kind (home, shop, work, civic), then in all. | já existia | sim/traffic/demand.cpp |
| `src/sim/meso/memory.ts` | **arquivo** | memória por ladrilho | não se aplica | mapa não ladrilhado |
| | `MESO_INSIDE` |  | não se aplica | mapa não ladrilhado |
| | `MESO_ONE_WAY` |  | não se aplica | mapa não ladrilhado |
| | `MESO_PRESENT` | `ML.flags`: the link is in the model; it is inside the micro window (its cars are the micro simulation's); its road is one-way. | não se aplica | mapa não ladrilhado |
| | `MESO_RECORD` |  | não se aplica | mapa não ladrilhado |
| | `MICRO_MIN_RADIUS` | The least the micro window shrinks to round a close view (`Meso.follow`): the street and the next blocks stay micro. | não se aplica | mapa não ladrilhado |
| | `MICRO_RADIUS` | The micro window's radius: the ~1.5 km window of the brief (CLAUDE.md), the micro simulation inside, the meso outside. | não se aplica | mapa não ladrilhado |
| | `ML` |  | não se aplica | mapa não ladrilhado |
| | `MesoDelta` |  | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder` | The worker's delta under way: records appended as the publish runs (it may be spread over ticks), taken whole by the frame. | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.at` | Each changed link's slot, its link id (`tileLinks.ts` linkId), and its record (`MESO_RECORD` numbers, fields `ML`). | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.end` | A publish ended: the step's head. | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.ended` | Whether a publish ended since the last take (the head is new). | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.head` |  | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.ids` | Each slot's link (`tileLinks.ts` linkId: segment * 2 + direction), as of its `shape`. | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.n` |  | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.push` | Room for one more link: its slot and id noted; the offset of its record in `recordsNow` to write it at. | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.records` |  | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.recordsNow` | The records' array the offsets of `push` point into (it grows: ask again after each push). | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.size` | Links appended since the last take. | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.startWhole` | Everything appended dropped: the next take is whole (the page clears its copy). | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.take` | The links appended since the last take, in new buffers to hand over; null when nothing was appended and no publish ended. | não se aplica | mapa não ladrilhado |
| | `MesoDeltaBuilder.whole` | The page's copy is cleared first (a new map, the page's first window): the links in it are every link there is. | não se aplica | mapa não ladrilhado |
| | `MesoMemory` | The page's copy of every link the worker sent (`MesoDelta`), by slot: what the far cars are written from (`render/densityCars.ts`) | não se aplica | mapa não ladrilhado |
| | `MesoMemory.apply` | A delta taken in, the slots it changed appended to `changed` (a slot may come twice); a whole one first takes every link out. | não se aplica | mapa não ladrilhado |
| | `MesoMemory.capacity` | Slots it has room for. | não se aplica | mapa não ladrilhado |
| | `MesoMemory.ids` | Each slot's link (`tileLinks.ts` linkId: segment * 2 + direction), as of its `shape`. | não se aplica | mapa não ladrilhado |
| | `MesoMemory.links` |  | não se aplica | mapa não ladrilhado |
| | `MesoMemory.reserve` | Room for slots below `n`, doubling, what was there kept. | não se aplica | mapa não ladrilhado |
| | `MesoMemory.serial` |  | não se aplica | mapa não ladrilhado |
| | `MesoMemory.slots` | One past the highest slot the worker holds. | não se aplica | mapa não ladrilhado |
| | `MesoMemory.time` | The simulation's time of the step and the micro window it stood at. | não se aplica | mapa não ladrilhado |
| | `MesoMemory.windowR` |  | não se aplica | mapa não ladrilhado |
| | `MesoMemory.windowX` |  | não se aplica | mapa não ladrilhado |
| | `MesoMemory.windowY` |  | não se aplica | mapa não ladrilhado |
| | `deltaTransfer` | The buffers of a delta, to hand over with the message that carries it. | não se aplica | mapa não ladrilhado |
| | `followView` | DO NOT keep the micro window 750 m wide round a street view (`Meso.step`): 1.35 the view (as `AmbientWorld`'s cars), between `MICR | não se aplica | mapa não ladrilhado |
| `src/sim/meso/meso.ts` | **arquivo** | modelo distante | já existia | sim/traffic/meso.cpp, window.cpp |
| | `ADMIT_PER_TICK` | Most cars made in the window a tick, as the edge entries admit them (`vehicles/spawn.ts` ADMIT_PER_TICK). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `BULB` | A node's word (`nodes`): a turning circle, then the roads meeting at it counted in steps of `ROAD`. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `ENTRY_CLEARANCE` | How far ahead of a link's start a car handed in at its upstream end is made. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `HANDOFF_TRIES` | Handoff lanes tried for a new car before it goes without a destination (its lookahead drives it, the window's edge takes it back). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `IDLE,RATES,MODEL,PUBLISH` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `MESO_DT` | Seconds of the city between two steps of the meso model. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `MESO_WORK_PER_TICK` | Links (and nodes) of meso work a tick: rates, the model's four phases and the publish together, about six units a link a step (mea | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.addRoads` | A road more or fewer at `node`; with none left, the node's flags go with it. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.admit` | The cars handed to the window made, a few a tick (as the edge entries admit them), up to the fleet the machine carries and the tic | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.begin` | A step begun: the window's cars counted (bounded by the window), the demand of the hour read once a cell. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.budget` | Links of work a tick (`MESO_WORK_PER_TICK`; a test may make it whole). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.cap` | Room for slots below it in every table by slot. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.cellCars` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.cellOf` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.cellWeight` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.cells` | The slots by the 256 m cell of their middle (the window's), by the node they leave and the node they reach. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.cellsRead` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.clock` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.countMicro` | The micro simulation's vehicles on each link inside the window (all of them: buses and the lots' cars take room too). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.counted` | Slots given a count of the micro vehicles last step. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.cursor` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.demand` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.dirty` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.dropSlot` | A link gone from the world: what was on it leaves the model, its slot free. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.edgeAt` | A node where the world ends: one road, not a turning circle (`RoadDoc.mapEdge`). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.endFrom` | Each slot's road: the nodes it joins and its segment, when `placed`. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.endSeg` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.endTo` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.enter` | A link comes into the window: its cars are to be made along it. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.fill` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.flag` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.flagged` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.flow` | `flowInto`'s numbers. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.freeSlots` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.graphSeen` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.handoff` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.handoffAt` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.handoffDestination` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.handoffLanes` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.handoffNodes` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.inAt` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.inWindow` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.index` | Each link its dense slot, the slots freed taken again (a link's id is a double). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.inside` | The slots inside the window: the handoff lanes are found from them (`handoffLanes`), never from the map's. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.insideWindow` | Whether a point is inside the micro window as it stands (its links' cars are the micro simulation's). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.isLive` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.job` | The step's work under way (`IDLE`, `RATES`, `MODEL`, `PUBLISH`) and where it stands; whether a step is owed. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.laneLists` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.laneSlot` | Each link lane's slot, and each live slot's lanes. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.late` | Steps the clock owed while one was under way, let go (the far traffic ran slower than the clock), since the start. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.leave` | A link leaves the window: the micro simulation's cars on it (those the meso made) taken off and counted on it again. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.lines` | Each slot's line (start, middle, end: x, y), whether its road is one-way, and whether the page has not had its line. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.linkAt` | Each slot's link (NaN: none). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.linkOfSlot` | The link a slot holds (NaN: none). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.micro` | The micro window: where it stands, and where it was asked to go (`want`). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.microWindow` | The micro window as it stands (null before it stands anywhere). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.mid` | Each slot's middle (x, y) and its demand cell (`DemandCells.cellOf`, dense; -1: none). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.model` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.most` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.moveWindow` | DO NOT look at every link of the map: only those filed where the window was or will be. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.nodes` | Each node's word (`BULB`, `ROAD`s), while a road meets it or it is flagged. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.nodesChanged` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.oneWay` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.out` | The page's delta under way, what each slot was last sent as, and whether the next publish is whole. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.outAt` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.owed` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.owned` | The vehicles the meso handed to the micro simulation: taken back when they drive out of the window. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.pendingSlots` | Slots with vehicles to make in the window, and how many of each to spread along it (handed with the window) rather than at its sta | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.placeEnds` | A slot's road filed at its nodes (the joins, the turns). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.placeLine` | A slot's place: its middle filed for the window, its demand cell, its line (start, middle, end) for the page. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.placed` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.publishIn` | The links in [`from`, `to`) whose drawn numbers changed since they were last sent, appended to the delta (`MesoDelta`). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.ratesIn` | Each link's trips in [`from`, `to`): from off the world at an entry (`vehicles/spawn.ts` ENTRY_RATE a lane), and its street's own  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.relink` | One slot made again from its lanes; with none left, it goes. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.requestWhole` | The page's copy of the links is new (its first window, or it lost deltas): the next publish sends every link. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.reserve` | Room for slots below `n` in every table by slot, doubling. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.reset` | Everything of the model forgotten (another map, the simulation started over): the window kept (entered again over the new map's li | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.returns` | A link's turns: the links its lanes' movements lead onto (weighted by how many); a U-turn only when nothing else. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.roadsAt` | The roads meeting at `node` (distinct segments). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.scale` | The demand's scale, as the simulation last set it (`step`). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.seedNew` | A new link starts as a street of its land use stands at this hour (in free flow): the map is full from its first step. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.segmentAt` | Whether a placed slot of `segment` leaves or reaches `node`. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.sent` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.setSpec` | A live slot's numbers from its lanes (`lanelets.ts`); its exit over the roads met at its end. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.settle` | The links whose ends' roads changed: their exits and signals, and their turns, worked out again. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.shapeSerial` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotCars` | Each slot's cell demand and trips from off the world, written by `ratesIn` for `MesoModel.setRates`: typed arrays by slot and by d | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotEdge` | Each slot's start at the map's edge, and the node map's change it was found at (`nodesChanged`, bumped by every change of `nodes`  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotEdgeAt` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotEntry` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotFor` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotOfLane` | The slot of a link lane, if it has one. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotOfLink` | The slot of a link (`tileLinks.ts` linkId), if it is in the model. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotTop` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotWeight` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.slotsIn` | The slots whose middle is in the rectangle, by the cells it covers, added to `out`. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.step` | Stage 2 (`pipeline.ts`): the links brought up to the lane graph, cars made in the window, and a share of the step's work. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.sync` | DO NOT read every lane of the map for an edit: only those the topology's log names. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.takeBack` | The meso's cars that drove out of the window, taken off the micro simulation onto the link they reached. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.takeDelta` | The links changed since the last call, for the page (null: nothing new). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.touchEnds` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.touchedNodes` | Nodes whose links came or went since the last `settle`: their links' exits, signals and turns worked out again. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.unplaceEnds` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.want` | The micro window asked to stand round (x, y), moved at the next step; `r` the most it grows to (`follow`). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.wanted` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.wholeDue` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.windowMoves` | Window moves and links in or out of it (`handoffLanes` found again after one). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `Meso.work` | Up to `budget` links of the step's work: the rates, then the model's phases, then the publish. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `ROAD` |  | já existia | sim/traffic/meso.cpp, window.cpp |
| | `SENT` | Numbers kept a slot of what the page was last sent (`sent`): its rounded count and queue, speed, length, lanes, flags. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `SHAPE_WRAP` | The serial of a slot's line wraps here: a Float32 holds it whole (`memory.ts` ML.shape). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `SPEED_STEP` | A link's speed is sent again when it moved by more than this share (the far cars' pace: a few percent is not seen). | já existia | sim/traffic/meso.cpp, window.cpp |
| | `TRIP_LENGTH` | A trip's mean length, for the share of a link's traffic that ends its trip on it and how often its street's lots send one out. | já existia | sim/traffic/meso.cpp, window.cpp |
| | `cellOfPoint` | The world's 256 m cell holding a point, as `CellFiling` keys it: the window finds its links by it. | já existia | sim/traffic/meso.cpp, window.cpp |
| `src/sim/meso/model.ts` | **arquivo** | filas do meso | já existia | sim/traffic/meso.cpp |
| | `EPS` |  | já existia | sim/traffic/meso.cpp |
| | `GREEN_SHARE` | Share of the hour a lane into a signal has green, on average. | já existia | sim/traffic/meso.cpp |
| | `JAM_SPACING` | Room a car takes standing in a queue, bumper to bumper, per lane (jam density 1 / 7.5 m). | já existia | sim/traffic/meso.cpp |
| | `MesoLinkSpec` | One directed road: what the model needs of it. | já existia | sim/traffic/meso.cpp |
| | `MesoModel` | The model over links by slot (a slot is a directed road, `meso.ts`), all in typed arrays: no object, list or Map entry a link. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.beginStep` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.bytes` | Bytes the model holds: its tables by slot, the rings' and turns' arenas, the nodes' lists. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.cap` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.capOf` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.capacityAt` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.constructor` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.count` | Vehicles on the link: on the road (outside the window), or the micro simulation's and those waiting to be made there (inside). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.cum` | The cumulative count `back` steps ago in the ring at `at` of `h` (fractional: between the two steps either side). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.cursor` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.downAt` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.downLen` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.dropLink` | A link gone: what was on it and waiting for it leaves the model (counted as arrived); its blocks given back. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.dropRings` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.dropTurns` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.enterWindow` | The link comes into the micro window: its vehicles are handed over (`pending`), its own counts set aside. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.exitDemand` | A node's round of the node model: the demand on each exit (cleared after each round). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.exitOf` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.finish` | The step under way finished now (a link about to change). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.flowInto` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.freeSpeedAt` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.genOf` | Vehicles a second wanting to start on the link (from off the map, from its street's lots): set by the owner each tick. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.generated` | Since the start: vehicles generated, lost at a full origin, arrived (sunk or off the map), handed in by the micro simulation and o | já existia | sim/traffic/meso.cpp |
| | `MesoModel.handed` | The links of the micro window handed a whole vehicle or more by the last step (`pending`): the micro simulation is to make them (` | já existia | sim/traffic/meso.cpp |
| | `MesoModel.handedIn` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.handedOut` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.has` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.inLinks` | The links into `node`, in the order they came. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.inflow` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.inflowOf` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.insideOf` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.into` | The links into each node, in the order they came. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.isInside` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.lanesAt` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.lanesOf` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.leaveWindow` | The link leaves the micro window with `micro` vehicles of the micro simulation on it (taken off there): they and the pending are t | já existia | sim/traffic/meso.cpp |
| | `MesoModel.left` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.lengthAt` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.lengthOf` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.linkFlows` | The sending and receiving flows of the links in [`from`, `to`), and their origins fed. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.live` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.lost` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.microExit` | A vehicle of the micro simulation drove out of the window onto `slot`: it enters the link now. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.microOf` | Vehicles the micro simulation has on a link inside its window (`setMicro`). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.moveCounts` | The cumulative counts of the links in [`from`, `to`) moved on by the step's flows; the window's links handed what came in. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.node` | One node (its group of links in): the approaches' sending flows to their turns, by equal-priority movements, up to each exit's roo | já existia | sim/traffic/meso.cpp |
| | `MesoModel.origin` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.originsIn` | Starting from the street (a driveway, the map's edge) gives way to the through traffic: what room is left, for the links in [`from | já existia | sim/traffic/meso.cpp |
| | `MesoModel.outflow` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.outflowOf` | Last step's flow out of the link's end and onto its start, vehicles. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.pending` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.pendingOf` | Vehicles handed to the micro simulation inside its window, not on the road yet. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.phase` | Where the step under way stands (`stepSome`): -1 none; 0 the links' sending and receiving flows; 1 the nodes; 2 the origins; 3 the | já existia | sim/traffic/meso.cpp |
| | `MesoModel.present` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.queued` | Vehicles standing in the link's queue: those that have had the time to reach its end and not left. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.reserve` | Room for slots below `n`, doubling. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.ringAt` | A ring of `h`'s cell for `back` steps ago (-1: the next step). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.rings` | Cumulative counts at each end, back in time a step a cell (rings): entries upstream, exits downstream; each a block of `rings` (-1 | já existia | sim/traffic/meso.cpp |
| | `MesoModel.room` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.seed` | `n` vehicles put on a link outside the window, spread along it in free flow (counted as generated). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.sending` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.setGeneration` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.setLink` | A link made or changed: its vehicles kept (spread along it again when its length or speed changed). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.setMicro` | The micro simulation's vehicles on a link inside the window, for its supply. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.setRates` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.setSinkShare` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.setTurns` | The links `slot` feeds, each with its weight (normalised here); none: everything reaching its end leaves the network. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.sinkOf` | Share of what reaches the end of a link that ends its trip there (parks, turns into a lot): set by the owner each tick. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.size` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.slots` | One past the highest slot ever set. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.speed` | The link's mean speed: its length over the free run plus the wait in its queue at the discharge rate. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.spread` | The link's `n` vehicles laid along it as in free flow: entered evenly over the last free run, none left yet. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.step` | One whole step of `dt`: origins fed, sending and receiving flows, each node's transfers, the cumulative counts moved on. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.stepSome` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.stepping` | Whether a step is under way (begun by `stepSome`, not finished). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.sunk` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.takePending` | `k` of the link's pending vehicles made by the micro simulation. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.tally` | What a step's nodes sink, summed in place (`step`). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.tick` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.toAt` | The node the link runs into (NaN: no link). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.toOf` | Node ids: a tile's pass 2^31 (`tileIdBase`), so doubles. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.turnAt` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.turnLen` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.turns` | Each link's turns: a block of `turns`, the links it feeds then the share going to each (summing to 1); none when `turnLen` is 0. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.turnsAt` | The links `slot` feeds (`setTurns`). | já existia | sim/traffic/meso.cpp |
| | `MesoModel.upAt` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.upLen` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.vFreeOf` |  | já existia | sim/traffic/meso.cpp |
| | `MesoModel.vehicles` | Every vehicle the model holds: on links outside the window, waiting at origins, and handed to the window not made yet. | já existia | sim/traffic/meso.cpp |
| | `MesoModel.wanted` |  | já existia | sim/traffic/meso.cpp |
| | `NONE` |  | já existia | sim/traffic/meso.cpp |
| | `ORIGIN_QUEUE` | Cars queued at an origin (an entry of the map, a street's driveways) per lane before more are lost, as `vehicles/spawn.ts` ENTRY_Q | já existia | sim/traffic/meso.cpp |
| | `WAVE_SPEED` | How fast a queue's tail moves back (the backward wave), 18 km/h: the usual 15 to 20 km/h of measured urban queues. | já existia | sim/traffic/meso.cpp |
| | `laneCapacity` | A lane's capacity, vehicles a second: the triangular diagram's peak, v w k_jam / (v + w) (about 1,900 an hour at 50 km/h). | já existia | sim/traffic/meso.cpp |
| | `linkCapacity` | A link's capacity, vehicles a second: its lanes', times the green's share into a signal or a stop line. | já existia | sim/traffic/meso.cpp |
| `src/sim/meso/slots.ts` | **arquivo** | espaços do meso | já existia | sim/traffic/meso.cpp |
| | `BlockArena` | Blocks of doubles in one Float64Array, those given back taken again by their size class. DO NOT keep `data` across an `alloc`: it  | já existia | sim/traffic/meso.cpp |
| | `BlockArena.alloc` | The start of a block of `n` (its numbers as left by its last holder). | já existia | sim/traffic/meso.cpp |
| | `BlockArena.bytes` | Bytes it holds (taken and free). | já existia | sim/traffic/meso.cpp |
| | `BlockArena.data` |  | já existia | sim/traffic/meso.cpp |
| | `BlockArena.free` |  | já existia | sim/traffic/meso.cpp |
| | `BlockArena.release` | The block at `at` of `n` given back. | já existia | sim/traffic/meso.cpp |
| | `BlockArena.top` |  | já existia | sim/traffic/meso.cpp |
| | `SlotIndex` | Number keys (a link's id passes 2^31) to slots, open addressing with linear probing (no boxed key, no entry object). | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.delete` |  | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.get` | The slot of `key`, -1 when it has none. | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.keys` |  | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.rehash` |  | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.set` |  | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.size` |  | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.state` | 0 empty, 1 held, 2 a key taken out (kept for the probes past it). | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.used` |  | já existia | sim/traffic/meso.cpp |
| | `SlotIndex.vals` |  | já existia | sim/traffic/meso.cpp |
| | `Threads` | Slots in groups by a number key (a node, a tile, a cell): each slot in one group at most, a group's slots in the order they joined | já existia | sim/traffic/meso.cpp |
| | `Threads.add` | `slot` put last in `key`'s group (out of the one it was in). | já existia | sim/traffic/meso.cpp |
| | `Threads.after` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.allGroups` | Every group, in the order they were made. | já existia | sim/traffic/meso.cpp |
| | `Threads.bytes` | Bytes it holds (taken and free). | já existia | sim/traffic/meso.cpp |
| | `Threads.count` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.eachGroup` | `visit` called with every group, in the order they were made (`Map.prototype.forEach`): DO NOT walk them with an iterator a step,  | já existia | sim/traffic/meso.cpp |
| | `Threads.gCount` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.gFree` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.gHead` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.gKey` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.gTail` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.gTop` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.group` | The group of `key` (-1: none). | já existia | sim/traffic/meso.cpp |
| | `Threads.groupLimit` | One past the highest group index ever made: a resumable walk over the groups goes from 0 to it, skipping the empty (`count` 0). | já existia | sim/traffic/meso.cpp |
| | `Threads.groupOf` | Each slot's group, -1 for none. | já existia | sim/traffic/meso.cpp |
| | `Threads.groups` | Each key's group, in the order the groups were made (as a Map's). | já existia | sim/traffic/meso.cpp |
| | `Threads.head` | A group's first slot (-1: none), the slot after `slot` in its group (-1: the last), how many it holds. | já existia | sim/traffic/meso.cpp |
| | `Threads.keyOf` | The key of `slot`'s group (NaN: none). | já existia | sim/traffic/meso.cpp |
| | `Threads.list` | The slots of `key`'s group, as a new list. | já existia | sim/traffic/meso.cpp |
| | `Threads.next` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.prev` |  | já existia | sim/traffic/meso.cpp |
| | `Threads.remove` | `slot` out of its group; a group left empty goes. | já existia | sim/traffic/meso.cpp |
| | `Threads.reserve` | Room for slots below `n`. | já existia | sim/traffic/meso.cpp |
| | `grown` | A typed array of `n` with `a`'s values first, the rest `fill`. | já existia | sim/traffic/meso.cpp |
| | `hash` | A double's hash from its two 32-bit halves (an integer key up to 2^53; murmur3's finaliser). | já existia | sim/traffic/meso.cpp |
| | `roomFor` | A block's room for `n` numbers: exact up to 16, then 8 classes an octave (at most 1/8 left unused). | já existia | sim/traffic/meso.cpp |
| `src/sim/meso/tileLinks.ts` | **arquivo** | ligações entre ladrilhos | não se aplica | mapa não ladrilhado |
| | `linkDirection` |  | não se aplica | mapa não ladrilhado |
| | `linkId` | A link's id: its segment and its direction (0 from the segment's `a` to its `b`). Exact while segment ids stay under 2^52. | não se aplica | mapa não ladrilhado |
| | `linkSegment` |  | não se aplica | mapa não ladrilhado |
| | `reverseLink` | The same road the other way. DO NOT use `^ 1`: ids may pass 2^31. | não se aplica | mapa não ladrilhado |
| `src/sim/params.ts` | **arquivo** | parâmetros | já existia | data/catalog/traffic.json |
| | `CONVOY_ROLLING` | Speed above which a vehicle counts as rolling rather than queueing. | já existia | data/catalog/traffic.json |
| | `CRITICAL_GAP` | Critical gaps for yielding movements, in seconds. | já existia | data/catalog/traffic.json |
| | `CRITICAL_GAP_FLOOR` |  | já existia | data/catalog/traffic.json |
| | `DRIVER_NOISE` | Free-flow speed sampling. | já existia | data/catalog/traffic.json |
| | `FLEET_CEILING` |  | já existia | data/catalog/traffic.json |
| | `IMPATIENCE_MAX` |  | já existia | data/catalog/traffic.json |
| | `IMPATIENCE_RATE` | Waiting drivers accept smaller gaps, but never below the floor. | já existia | data/catalog/traffic.json |
| | `JAM_GAP` | Bumper-to-bumper spacing at a standstill. | já existia | data/catalog/traffic.json |
| | `LANE_CHANGE_FAST_SPEED` | Speed at which the shallow heading is reached, world units a second. | já existia | data/catalog/traffic.json |
| | `LANE_CHANGE_HEADING_FAST` |  | já existia | data/catalog/traffic.json |
| | `LANE_CHANGE_HEADING_SLOW` | A lane change is a PATH, not a slide. | já existia | data/catalog/traffic.json |
| | `LANE_CHANGE_MIN_BODIES` | Shortest curve, in body lengths of the vehicle driving it. | já existia | data/catalog/traffic.json |
| | `LANE_CHANGE_MIN_TIME` | Never quicker than this at any speed, seconds. | já existia | data/catalog/traffic.json |
| | `LATERAL_ACCEL_FALL` | Reduction of the accepted lateral acceleration per unit of speed, per second. | já existia | data/catalog/traffic.json |
| | `L_MIN` | Shortest link that can hold the largest vehicle plus its jam gap. | já existia | data/catalog/traffic.json |
| | `MAX_FRAME` | Wall-clock time is clamped to this before being accumulated. | já existia | data/catalog/traffic.json |
| | `MAX_LATERAL_ACCEL` | Speed cap through curvature, expressed as the lateral acceleration a driver accepts, which FALLS with speed. | já existia | data/catalog/traffic.json |
| | `MAX_SUBSTEPS` | Cap on catch-up steps per frame, so a stall cannot spiral. | já existia | data/catalog/traffic.json |
| | `MAX_VEHICLE_LENGTH` | Longest vehicle in the fleet, used to size link capacity. | já existia | data/catalog/traffic.json |
| | `MIN_LATERAL_ACCEL` |  | já existia | data/catalog/traffic.json |
| | `NARROW_SCREEN_SHARE` | Share of the ceiling allowed on a narrow screen. | já existia | data/catalog/traffic.json |
| | `NARROW_SCREEN_WIDTH` | Viewport width, CSS pixels, under which a screen counts as narrow. | já existia | data/catalog/traffic.json |
| | `PED` | Pedestrian parameters. | já existia | data/catalog/traffic.json |
| | `PED_CEILING` |  | já existia | data/catalog/traffic.json |
| | `PROFILE_PEAK` | Peak slope of the quintic profile, as a multiple of its mean slope. | já existia | data/catalog/traffic.json |
| | `REQUEST_MIN_DISTANCE` |  | já existia | data/catalog/traffic.json |
| | `REQUEST_TIME` | A vehicle requests entry from this far back — generous by design. | já existia | data/catalog/traffic.json |
| | `SIGNAL` | Signal timing, in seconds. | já existia | data/catalog/traffic.json |
| | `STUCK_SECONDS` | A claim holder immobile for this long is reported (diagnostic only). | já existia | data/catalog/traffic.json |
| | `TRAFFIC_DENSITY` | Vehicle population target, per unit of total road length. | já existia | data/catalog/traffic.json |
| | `WAIT_CEILING` | Seconds a driver may wait for a gap before taking the next one offered. | já existia | data/catalog/traffic.json |
| | `laneChangeLength` | Road length over which a lane change of offset `start` is driven at `speed`. | já existia | data/catalog/traffic.json |
| | `laneChangeOffset` | Offset still to cover after driving `travelled` of a `length` change (quintic). | já existia | data/catalog/traffic.json |
| | `laneChangeSlope` | Sideways offset gained per unit of road driven: the tangent of the body's heading. | já existia | data/catalog/traffic.json |
| `src/sim/peds/sidewalk.ts` | **arquivo** | grafo de calçadas | já existia | sim/traffic/ped_routes.cpp |
| | `SidewalkGraph.crossingEdge` |  | já existia | sim/traffic/ped_routes.cpp |
| | `SidewalkGraph.madeAt` |  | já existia | sim/traffic/ped_routes.cpp |
| | `SidewalkGraph.walksBeside` |  | já existia | sim/traffic/ped_routes.cpp |
| `src/sim/peds/signalPostIndex.ts` | **arquivo** | postes de semáforo | já existia | sim/roads/street_furniture_posts.cpp |
| | `SignalPostIndex.file` |  | já existia | sim/roads/street_furniture_posts.cpp |
| | `SignalPostIndex.follow` |  | já existia | sim/roads/street_furniture_posts.cpp |
| | `SignalPostIndex.found` |  | já existia | sim/roads/street_furniture_posts.cpp |
| | `SignalPostIndex.lift` |  | já existia | sim/roads/street_furniture_posts.cpp |
| | `SignalPostIndex.near` |  | já existia | sim/roads/street_furniture_posts.cpp |
| | `SignalPostIndex.placeAll` |  | já existia | sim/roads/street_furniture_posts.cpp |
| | `SignalPostIndex.placeJunction` |  | já existia | sim/roads/street_furniture_posts.cpp |
| | `SignalPostWorld` |  | já existia | sim/roads/street_furniture_posts.cpp |
| | `cellOf` |  | já existia | sim/roads/street_furniture_posts.cpp |
| `src/sim/people/casualties.ts` | **arquivo** | feridos | de outra funcionalidade | funcionalidade `destruction` |
| | `recordCasualty` |  | de outra funcionalidade | funcionalidade `destruction` |
| | `recordWound` |  | de outra funcionalidade | funcionalidade `destruction` |
| `src/sim/people/engine.ts` | **arquivo** | ponte veículo-pedestre: embarcar, descer, chamar | portado | sim/traffic/kerb_stops.cpp |
| `src/sim/people/view.ts` | **arquivo** | vista das pessoas | de outra funcionalidade | funcionalidade `people` |
| | `gaitSpeed` |  | de outra funcionalidade | funcionalidade `people` |
| `src/sim/pipeline.ts` | **arquivo** | ordem do passo | já existia | sim/traffic/traffic_system.cpp step |
| `src/sim/populationGovernor.ts` | **arquivo** | governador da frota | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `HIGH_MS` | A step costing more than this (ms, averaged) thins the town. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `HOLD_S` | Seconds after a change before the next, while its effect shows. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `LOW_MS` | And less than this lets it fill again. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `MIN_SHARE` | The thinnest the town is made. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor` |  | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.changedAt` |  | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.constructor` | `max`: the device's own share, the most the town is ever given. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.feed` | `steps` steps ran this frame at `msPerStep` each, at wall time `seconds`, with `vehicles` and `people` in the town. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.mean` |  | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.samples` |  | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.setMax` | The device's share changed (the screen narrowed or widened). | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.share` | The share in use. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.shareNow` |  | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.start` | Where the share starts, before any step is measured (`cpuPopulationShare`); later calls change nothing. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.started` |  | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `PopulationGovernor.stepMs` | The averaged cost of a step, ms (0 before any). | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `SMOOTHING` | The measure's memory: an exponential mean over about this many steps. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `STEP` | Each change multiplies or divides the share by this. | já existia | sim/traffic/junction_admission.cpp step_governor |
| | `cpuPopulationShare` | The most of the town a machine of `threads` threads is given to start with (the governor fills it from there when its steps are ch | já existia | sim/traffic/junction_admission.cpp step_governor |
| `src/sim/pose.ts` | **arquivo** | pose do veículo, deslocada na ciclofaixa | portado | sim/traffic/traffic_system.cpp |
| | `pedPoseInto` |  | portado | sim/traffic/traffic_system.cpp |
| | `vehiclePoseNowInto` |  | portado | sim/traffic/traffic_system.cpp |
| `src/sim/roads/controlAdvisor.ts` | **arquivo** | controle automático do cruzamento pelo fluxo medido | não portado | idem flowStats |
| | `AutoControl` | THE CONTROL A JUNCTION LEFT ON "AUTOMATIC" GETS: chosen from the measured trend of its flows | não portado | idem flowStats |
| | `ControlAdvisor` |  | não portado | idem flowStats |
| | `ControlAdvisor.EVERY` | Seconds between evaluations. | não portado | idem flowStats |
| | `ControlAdvisor.changedAt` |  | não portado | idem flowStats |
| | `ControlAdvisor.choice` | The control chosen for `node`, or undefined before the advisor has spoken (the game's geometric default holds). | não portado | idem flowStats |
| | `ControlAdvisor.choices` |  | não portado | idem flowStats |
| | `ControlAdvisor.drop` | These junctions' choices forgotten (gone with an edit). | não portado | idem flowStats |
| | `ControlAdvisor.evaluate` | One junction's evaluation; returns true when its choice changed. | não portado | idem flowStats |
| | `ControlAdvisor.forget` |  | não portado | idem flowStats |
| | `ControlAdvisor.since` |  | não portado | idem flowStats |
| | `ControlAdvisor.tick` | Whether it is time to evaluate again. | não portado | idem flowStats |
| | `F` |  | não portado | idem flowStats |
| | `QUIET_FLOW` | Vehicles an hour into a junction below which `warrantedControl` is 'priority' whatever it has now (the least major flow of a step  | não portado | idem flowStats |
| | `rank` |  | não portado | idem flowStats |
| | `warrantedControl` | The control a junction's flows warrant, given what it has now (hysteresis). | não portado | idem flowStats |
| | `warrantedControl.k` |  | não portado | idem flowStats |
| `src/sim/roads/flowStats.ts` | **arquivo** | fluxo medido por aproximação de cada cruzamento | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats` |  | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.carry` |  | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.counted` |  | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.counts` |  | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.drop` | These junctions' flows forgotten (gone with an edit). | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.held` | Junctions whose rates stand still (out of the micro window: nothing is counted there), by the second they were held at. | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.hold` | A junction's rates held as they are (`resume` lets them age again): DO NOT let them fall while nobody counts there, a signal came  | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.keepOnly` | Forgets what a removed junction measured. | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.measured` | Seconds of traffic measured since the town was laid out (a trend needs `trendWindow`). | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.rate` | Vehicles an hour entering `node` from `seg`, as a trend. | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.rates` |  | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.record` |  | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.resume` | A held junction's rates go on from where they were held, the seconds out not aged. | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.second` | Seconds aged so far: the clock the rates are kept against. | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.step` |  | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `FlowStats.takeCounted` | The junctions counted at since the last call, added to `into` (the advisor's, `SimWorld.stepControlAdvisor`). | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `KEEP` | What a second with nothing counted leaves of a rate. | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `Rate` | A leg's rate as it stood at the end of second `at`. | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `TAU` | DO NOT age every leg ever seen every second | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| | `agedTo` | A rate brought up to the end of `second`: a factor `KEEP` for each second since it was kept, none counted. | não portado | sem controle automático por fluxo: o semáforo vem da geometria do nó (control_of); pendência no CLAUDE.md |
| `src/sim/roads/greenWave.ts` | **arquivo** | onda verde na via principal | portado | sim/traffic/green_wave.cpp |
| | `CorridorStop` |  | portado | sim/traffic/green_wave.cpp |
| | `corridorFrom` | The junctions along the main road through `start`, in the direction of its higher leg, up to a dozen | portado | sim/traffic/green_wave.cpp |
| | `corridorFrom.otherLeg` |  | portado | sim/traffic/green_wave.cpp |
| | `corridorFrom.walk` |  | portado | sim/traffic/green_wave.cpp |
| | `greenWave` | A GREEN WAVE along the main road through a junction | portado | sim/traffic/green_wave.cpp |
| `src/sim/routing/destination.ts` | **arquivo** | destino das viagens | já existia | sim/traffic/demand.cpp |
| | `chooseVehicleDestination` | Chooses one reachable trip endpoint, once, when a vehicle enters the city. | já existia | sim/traffic/demand.cpp |
| | `reachableExits` | DO NOT search the graph from each entry | já existia | sim/traffic/demand.cpp |
| | `routeToDestination` | Least-time lanelet route to a fixed endpoint, with congestion priced at planning time. | já existia | sim/traffic/demand.cpp |
| | `routeToDestination.toGo` |  | já existia | sim/traffic/demand.cpp |
| `src/sim/routing/laneReach.ts` | **arquivo** | alcance entre faixas | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach` | DO NOT search every lane of the map again for an edit of the roads | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.ahead` | Each component asked from, with the components it leads to (itself included): forgotten at any change of the graph. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.buildAll` | @rebuild Every link lane: a body class asked for the first time, or a lane graph built whole. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.comp` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.cond` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.condIn` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.condOut` | The condensation: ways out between two components, counted, each way. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.constructor` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.countIn` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.edge` | A way out added (+1) or taken (-1): the ways in, and the condensation between two components. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.exitsFrom` | The road ends reachable from a link lane (its own included when it is one). | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.exitsMemo` | Each component's road ends, sorted, summed when asked (`exitsOf`). | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.exitsOf` | A component's road ends: those of its lanes, and those of the components it leads to (summed first). | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.follow` | Brought up to the lane graph's changes. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.follow.isLink` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.into` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.label` | A new component of these lanes; its id. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.members` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.next` | A link's ways out this body takes: the movements it fits, and the lane changes beside it. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.nextComp` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.out` | Each link's ways out (with repeats, as `next` lists them), and the ways in to each, counted. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.reaches` | Whether `u` reaches `v` now: a search from each end at once, the smaller side giving out first when it does not. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.reachesFrom` | Whether a link lane can lead to another for this body; null when one of them is no link lane known here. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.retag` | Lanes given new components by `relabel`, the condensation's counts of their ways taken out before and put back after. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.tarjan` | Tarjan's components of `lanes` (iterative), over the ways out that stay within `inSet`. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.walkCond` | The components reached from `start` over `ways` (itself included), within `within` when given. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.walkCondMany` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `BodyReach.work` | Lanes looked at since made (the scale tests' count). | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `LaneReaches` | Each body class's reach, kept with the world (`SimWorld.laneReaches`). | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `LaneReaches.bodies` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `LaneReaches.exitsFrom` | The road ends reachable from a link lane (its own included when it is one). | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `LaneReaches.follow` | Brought up to the lane graph's changes. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `LaneReaches.mayReach` | Whether a link lane can lead to another for a body (false: no route, DO NOT search the graph for one); true when it may, or when i | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `LaneReaches.seen` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `LaneReaches.work` | Lanes looked at since made (the scale tests' count). | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `ReachWorld` | DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `boundaryExit` | A lane that leaves the map: a link into a road end at the edge, with no way on. | já existia | sim/traffic/lane_list.cpp lane_reaches |
| | `bump` |  | já existia | sim/traffic/lane_list.cpp lane_reaches |
| `src/sim/routing/lowerBound.ts` | **arquivo** | limite inferior do A* | já existia | sim/traffic/astar.h |
| | `FASTEST_LIMIT` | DO NOT work out the fastest lane of the graph here | já existia | sim/traffic/astar.h |
| | `LaneLookup` | DO NOT import `SimWorld`: the world reaches this file, and the type import closed an import cycle | já existia | sim/traffic/astar.h |
| | `goalEnd` | The end of the goal lane, for `laneLowerBound`; null when there is no goal lane. | já existia | sim/traffic/astar.h |
| | `laneLowerBound` | The bound to the goal ending at `end` from the end of lane `id`, seconds (`tripLowerBound` works each out once a search). | já existia | sim/traffic/astar.h |
| | `tripLowerBound` | The bound to `goal` from the end of a lane, seconds, each lane's worked out once a search. | já existia | sim/traffic/astar.h |
| `src/sim/routing/routeIndex.ts` | **arquivo** | índice de rotas | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex` | The vehicles whose route goes over each lane or movement, filed by the writers of `Vehicle.route` (`SimWorld.noteRoute`); generic, | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.add` |  | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.after` |  | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.before` |  | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.byLane` |  | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.clear` |  | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.drop` |  | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.file` | A vehicle filed under its route now, out of the lanes of the route it had: DO NOT take it out of every lane and put it back for a  | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.filed` | The ids each vehicle is filed under: its route as last noted. | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.routing` | The vehicles whose route goes over `id`, in no set order. | já existia | sim/traffic/route_arena.cpp |
| | `RouteIndex.unfile` |  | já existia | sim/traffic/route_arena.cpp |
| | `sameTail` | Whether `tail` is `list` without its first. | já existia | sim/traffic/route_arena.cpp |
| `src/sim/routing/router.ts` | **arquivo** | rotas | já existia | sim/traffic/routing.cpp |
| | `HORIZON` | Horizon for a vehicle with no reachable boundary destination. | já existia | sim/traffic/routing.cpp |
| | `LOOKAHEAD` | Bounded network lookahead used when comparing alternate turns. | já existia | sim/traffic/routing.cpp |
| | `TrapIndex` | DO NOT go back to the reverse edges of every lane of the map built for each topology | já existia | sim/traffic/routing.cpp |
| | `TrapIndex.bodies` | Per body class asked for: the seeds, and the traps peeled from them (null: to peel again). | já existia | sim/traffic/routing.cpp |
| | `TrapIndex.follow` | The seeds brought up to the lane graph's changes since it was last. | já existia | sim/traffic/routing.cpp |
| | `TrapIndex.seen` |  | já existia | sim/traffic/routing.cpp |
| | `TrapIndex.traps` |  | já existia | sim/traffic/routing.cpp |
| | `chooseExit` |  | já existia | sim/traffic/routing.cpp |
| | `cutAtBreak` | A route cut where it stops standing on the graph (a lane gone, a movement that no longer joins them); whether it was cut. | já existia | sim/traffic/routing.cpp |
| | `extend` | Grows a route forward until it reaches the horizon or a dead end. | já existia | sim/traffic/routing.cpp |
| | `fittingExits` | The movements out of a lane this body fits. | já existia | sim/traffic/routing.cpp |
| | `isLegalTransition` |  | já existia | sim/traffic/routing.cpp |
| | `isSeed` | A lane with exits none of which this body fits. | já existia | sim/traffic/routing.cpp |
| | `peel` | The traps: the seeds, and every lane whose movements this body fits all lead into traps, peeled back from them. | já existia | sim/traffic/routing.cpp |
| | `planFrom` | Route planning over the lanelet graph. | já existia | sim/traffic/routing.cpp |
| | `reconsiderRoute` | Lets a driver who has been held up look for another way round. | já existia | sim/traffic/routing.cpp |
| | `repairRoute` | Repairs a route whose lanelets no longer exist, after a live edit. | já existia | sim/traffic/routing.cpp |
| | `routeCost` | A small bounded search keeps the fallback moving without inventing a goal it cannot reach. | já existia | sim/traffic/routing.cpp |
| | `seedsOf` | @rebuild Every lane with exits none of which this body fits. | já existia | sim/traffic/routing.cpp |
| | `trapIndexes` |  | já existia | sim/traffic/routing.cpp |
| | `trapLanes` |  | já existia | sim/traffic/routing.cpp |
| `src/sim/routing/searchScratch.ts` | **arquivo** | memória da busca | já existia | sim/traffic/astar.h |
| | `SEARCHES_PER_TICK` | Trip searches a tick may start for requests that can wait (a car to make, a driver reconsidering): the rest wait for the next tick | já existia | sim/traffic/astar.h |
| | `SEARCH_EXPANSIONS` |  | já existia | sim/traffic/astar.h |
| | `SearchBudget` | The searches started this tick, and how many gave up at the cap or waited, since the world began. | já existia | sim/traffic/astar.h |
| | `SearchScratch` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.begin` | A new search: every cost back to unknown, the heap empty; `liveIds` bounds the slots kept for lanes long gone. | já existia | sim/traffic/astar.h |
| | `SearchScratch.best` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.bestCost` | Least cost found to each state, this search. | já existia | sim/traffic/astar.h |
| | `SearchScratch.bound` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.boundOf` | The bound from `slot`, or undefined when this search has not worked it out yet. | já existia | sim/traffic/astar.h |
| | `SearchScratch.boundStamp` | The bound to the goal from each slot, this search (`tripLowerBound`). | já existia | sim/traffic/astar.h |
| | `SearchScratch.budget` | The searches of the world this tick, and since it began (`searchBudget`). | já existia | sim/traffic/astar.h |
| | `SearchScratch.changed` | Whether the state was reached by a lane change, or may not make one: what the string search's node held as `changed`. | já existia | sim/traffic/astar.h |
| | `SearchScratch.compare` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.compareKeys` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.generation` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.grow` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.grow.more` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.heapCost` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.heapF` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.heapState` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.ids` | The id of each slot. | já existia | sim/traffic/astar.h |
| | `SearchScratch.markPlainNoChange` | Marks the one state whose key is its lane's id although it may not change lanes. | já existia | sim/traffic/astar.h |
| | `SearchScratch.move` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.parent` | The state each was reached from, and the movement's slot (-1: a lane change). | já existia | sim/traffic/astar.h |
| | `SearchScratch.plainNoChange` | The state whose key is its lane but that may not change lanes (the start of a search that may not begin with one). | já existia | sim/traffic/astar.h |
| | `SearchScratch.pop` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.poppedCost` | The first state; its cost is `poppedCost`. Call only with `size > 0`. | já existia | sim/traffic/astar.h |
| | `SearchScratch.push` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.setBest` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.setBound` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.size` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.slot` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.slots` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.stamp` |  | já existia | sim/traffic/astar.h |
| | `SearchScratch.via` |  | já existia | sim/traffic/astar.h |
| | `mayRequestSearch` | Whether a request that can wait may search now; a request refused is counted as deferred and asks again next tick. | já existia | sim/traffic/astar.h |
| | `noteCapped` | A search that hit `SEARCH_EXPANSIONS`. | já existia | sim/traffic/astar.h |
| | `noteSearch` | A search started this tick (every search counts, those that cannot wait too); the states it may expand. | já existia | sim/traffic/astar.h |
| | `scratches` |  | já existia | sim/traffic/astar.h |
| | `searchBudget` | The world's search budget (kept with its search memory), brought to `tick` (a new tick starts with none used). | já existia | sim/traffic/astar.h |
| | `searchScratch` | The world's search memory. | já existia | sim/traffic/astar.h |
| `src/sim/signals/fsm.ts` | **arquivo** | máquina do semáforo: tempo fixo, defasagem; a prioridade ao ônibus é do transit | portado | sim/traffic/signal_timing.cpp |
| | `BUS_EXTENSION` | How long a green is held past its maximum for a bus about to reach the line (green extension), seconds | portado | sim/traffic/signal_timing.cpp |
| | `DEMAND_PRIORITY` | Queued vehicles that double a stage's claim to run next. | portado | sim/traffic/signal_timing.cpp |
| | `PED_HOLD_LIMIT` | Longest a green is held past its maximum for people still on its crossings, seconds. | portado | sim/traffic/signal_timing.cpp |
| | `PED_WAIT_LIMIT` | Kerb wait after which a crossing's stage is served next, seconds. | portado | sim/traffic/signal_timing.cpp |
| | `SignalClock` | The signals' passes (stage 1, `pipeline.ts`), one a tick: the clock every controller's time is read against, asleep or not. | portado | sim/traffic/signal_timing.cpp |
| | `SignalController` | Per-junction signal state. | portado | sim/traffic/signal_timing.cpp |
| | `SignalDeps` |  | portado | sim/traffic/signal_timing.cpp |
| | `SubPhase` |  | portado | sim/traffic/signal_timing.cpp |
| | `TIMED` | What makes `elapsed` read the clock: nothing to do a tick for a controller, asleep or not. | portado | sim/traffic/signal_timing.cpp |
| | `betterCandidate` | Oldest wait wins; circular order breaks exact ties deterministically. | portado | sim/traffic/signal_timing.cpp |
| | `circularDistance` |  | portado | sim/traffic/signal_timing.cpp |
| | `createController` |  | portado | sim/traffic/signal_timing.cpp |
| | `currentGreenGroups` |  | portado | sim/traffic/signal_timing.cpp |
| | `fixedCycle` | A fixed-time plan's cycle with the player's greens, seconds (V5). | portado | sim/traffic/signal_timing.cpp |
| | `isStrict` |  | portado | sim/traffic/signal_timing.cpp |
| | `pickNextStage` | Chooses the next stage without skipping any vehicle or pedestrian service. | portado | sim/traffic/signal_timing.cpp |
| | `rebuildController` | Rebuilds a controller's plan after the network changed, carrying state over. | portado | sim/traffic/signal_timing.cpp |
| | `safePlan` |  | portado | sim/traffic/signal_timing.cpp |
| | `seekFixed` | Puts a fixed-time signal where its cycle stands at `seconds` of the common clock plus its offset | portado | sim/traffic/signal_timing.cpp |
| | `seekPhase` | Where in its own cycle a plan sits `seconds` after the start of stage 0. | portado | sim/traffic/signal_timing.cpp |
| | `signalWork` | Greens that rested with nobody near (`SignalDeps.quiet`): a count for the tests and the budgets. | portado | sim/traffic/signal_timing.cpp |
| | `stageAge` |  | portado | sim/traffic/signal_timing.cpp |
| | `stageHasDemand` |  | portado | sim/traffic/signal_timing.cpp |
| | `stageScore` |  | portado | sim/traffic/signal_timing.cpp |
| | `stepController` | One tick of a controller, after its clock's pass (`SignalClock`): the time it read has moved on a step. | portado | sim/traffic/signal_timing.cpp |
| | `timedController` | A controller from its fields, its `elapsed` kept by its clock (`base` seconds at pass `since`). | portado | sim/traffic/signal_timing.cpp |
| `src/sim/signals/permission.ts` | **arquivo** | quem pode passar | já existia | sim/traffic/light_rule.cpp |
| | `RIGHT_ON_RED_ALLOWED` | Whether a right turn may be taken on red after a full stop. | já existia | sim/traffic/light_rule.cpp |
| | `mustStopAtSignal` |  | já existia | sim/traffic/light_rule.cpp |
| | `signalHolds` | `mustStopAtSignal` for one vehicle and connector, with the amber decision held once it is made. | já existia | sim/traffic/light_rule.cpp |
| `src/sim/signals/plan.ts` | **arquivo** | estágios do semáforo | já existia | sim/traffic/junction_control.cpp make_stages |
| | `AMBER_DECELERATION` | Comfortable deceleration of the yellow interval, m/s² (ITE: 10 ft/s²). | já existia | sim/traffic/junction_control.cpp make_stages |
| | `AMBER_MAX` | Longest yellow change interval, s (MUTCD: 3 to 6 s). | já existia | sim/traffic/junction_control.cpp make_stages |
| | `AMBER_REACTION` | Perception-reaction time of the yellow interval, s (ITE). | já existia | sim/traffic/junction_control.cpp make_stages |
| | `APPROACH_OVER_LIMIT` | The 85th-percentile approach speed over the limit, m/s, where it is not measured (ITE RP-040B: limit + 7 mph). | já existia | sim/traffic/junction_control.cpp make_stages |
| | `CrossingId` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `EXCLUSIVE_MAX_FACTOR` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `GroupId` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `OPPOSED_TOLERANCE` | Groups that may take green together, as one entry per stage. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `PlanValidationError` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `SignalPlan` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `Stage` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `StageInput` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `TURN_RANK` | Who gives way to whom between two green movements that cross. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `annotate` | Marks every stage's protected movements from the real conflict matrix and adds an exclusive stage for any approach whose | já existia | sim/traffic/junction_control.cpp make_stages |
| | `annotate.protectedIn` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `buildSignalPlan` | Builds a fixed-time plan for one junction and validates it before returning. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `buildStages` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `conflictFreeStages` | Groups that may take green together WITHOUT any two of their movements crossing, merging or sweeping into one another. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `conflictFreeStages.compatible` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `crossingSegment` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `crossingsCompatibleWith` | A crossing over leg L may show WALK during a stage if and only if no connector green in that stage DEPARTS from L. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `makeCrossingId` | Crossing ids are `${nodeId}:${segmentId}`. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `mustYieldToPeds` | True when this connector must yield to pedestrians already in the crossing over its destination leg. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `normaliseAngle` | Wraps an angle into (-PI, PI]. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `pairOpposingGroups` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `roundRobinPlan` | Fallback plan used when validation fails in a production build. | já existia | sim/traffic/junction_control.cpp make_stages |
| | `stage` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `validate` |  | já existia | sim/traffic/junction_control.cpp make_stages |
| | `withAmbers` | Each stage's yellow from the fastest approach it releases, by the ITE kinematic formula Y = t + V / 2a (level ground) | já existia | sim/traffic/junction_control.cpp make_stages |
| `src/sim/signals/query.ts` | **arquivo** | consultas do semáforo | já existia | sim/traffic/signal_timing.cpp |
| | `SignalState` |  | já existia | sim/traffic/signal_timing.cpp |
| | `WalkState` |  | já existia | sim/traffic/signal_timing.cpp |
| | `anyGreen` | True when at least one group at this junction currently has green. | já existia | sim/traffic/signal_timing.cpp |
| | `pedestrianSignalState` | Walk indicator for one crossing. | já existia | sim/traffic/signal_timing.cpp |
| | `remainingProtectedTime` | Seconds for which the crossing stays protected from the traffic that would otherwise use its leg. | já existia | sim/traffic/signal_timing.cpp |
| | `signalStateFor` | Signal state for one approach group. | já existia | sim/traffic/signal_timing.cpp |
| `src/sim/topologyCatchUp.ts` | **arquivo** | recuperar topologia do worker | não se aplica | ChangeLog |
| `src/sim/transit/transit.ts` | **arquivo** | ônibus de linha | de outra funcionalidade | funcionalidade `transit`, a seguir nesta sessão |
| | `TransitSim.closedOn` |  | de outra funcionalidade | funcionalidade `transit`, a seguir nesta sessão |
| | `TransitSim.crossingNearEnd` |  | de outra funcionalidade | funcionalidade `transit`, a seguir nesta sessão |
| | `TransitSim.trainZones` |  | de outra funcionalidade | funcionalidade `transit`, a seguir nesta sessão |
| | `TransitSim.trainZones.far` |  | de outra funcionalidade | funcionalidade `transit`, a seguir nesta sessão |
| | `TransitSim.trainZones.zone` |  | de outra funcionalidade | funcionalidade `transit`, a seguir nesta sessão |
| `src/sim/vehicleSignals.ts` | **arquivo** | sinais dos veículos: freio | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `AFTER` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `BEFORE` | The two tangents `pathCurvature` reads: made once (every vehicle drawn, every frame). | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `BLINK_DUTY` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `BLINK_HZ` | Flash rate of an indicator, Hz, and the share of each flash that is lit. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `HERE` | What `indicatorSide` reads of the two lanes, made once. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `HIT` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `IndicatorSide` | +1 indicating left, -1 right, 0 none — in the vehicle's own frame. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `LANE_CHANGE_DONE` | Offset of a lane change under which the manoeuvre is treated as finished. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `MAX_STEER` | Largest front-wheel steering angle drawn, radians. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `THERE` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `TURN_SIGNAL_MIN` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `TURN_SIGNAL_TIME` | A driver signals a turn this long, or this far, before the stop line. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `TURN_SIGNAL_UNTIL` | And keeps signalling until this share of the turn is driven. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `WheelOdometer` | Distance each vehicle has driven, for the rolling angle of its wheels. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `WheelOdometer.advance` | Total distance driven so far, in world units. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `WheelOdometer.seen` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `blinkOn` | Whether a flashing indicator is lit at this moment of the vehicle's life. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `indicatorSide` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `pathCurvature` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `steerAngle` | Front-wheel steering angle, radians, positive to the left. | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `steerFrom` | Front-wheel steering angle for a path's curvature (`pathCurvature`) and a wheelbase: what the page draws from the worker's numbers | já existia | sim/traffic/vehicle_motion.cpp brake |
| | `turnSide` |  | já existia | sim/traffic/vehicle_motion.cpp brake |
| `src/sim/vehicles/archetypes.ts` | **arquivo** | classes de veículo, com a bicicleta | portado | data/catalog/traffic.json classes |
| | `ARCHETYPES` |  | portado | data/catalog/traffic.json classes |
| | `Archetype` |  | portado | data/catalog/traffic.json classes |
| | `BodyStyle` | Silhouette of a `car`: where the roof ends and how the tail is cut. | portado | data/catalog/traffic.json classes |
| | `VehicleShape` | Body plan the renderer builds from. | portado | data/catalog/traffic.json classes |
| | `archetypeById` | Lookup by id, falling back to the sedan. | portado | data/catalog/traffic.json classes |
| | `archetypeWeights` |  | portado | data/catalog/traffic.json classes |
| | `bodyClassOfArchetype` | The conflict-zone size class a body of this archetype is measured with. | portado | data/catalog/traffic.json classes |
| `src/sim/vehicles/curvature.ts` | **arquivo** | velocidade em curva | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `AGGRESSION_SPREAD` | Spread of comfortable lateral acceleration across drivers, per unit of aggression. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `CHORD` | Half-length of the chord the curvature is measured over. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `HEAVY_SHARE` | Share of the comfortable lateral acceleration a heavy vehicle uses. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `LOOK_MARGIN` |  | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `LOOK_TIME` | Look-ahead beyond the braking distance, seconds of travel plus a margin. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `SAMPLE` | Spacing of the curvature profile along a lanelet. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `STRAIGHT` | Curvature taken as none at all. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `bendSpeed` | Fastest speed at which a bend of curvature `k` is taken within the driver's speed-dependent comfort | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `curveSpeedCap` | Highest speed at which this vehicle can drive on now and still take every bend within its look-ahead at a comfortable | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `curveSpeedCap.before` | Highest speed `d` ahead of a bend taken at `safe`: braking, then the ramp easing off. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `lateralShare` | Share of the fleet-wide figure this driver accepts: personality and vehicle. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `peaks` | Each profile's sharpest sample: below `STRAIGHT` the whole lanelet is straight. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `profileOf` | Curvature, per world unit, at each `SAMPLE` along the lanelet. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `profiles` |  | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `slowestBend` | The speed this vehicle takes the sharpest bend of a lanelet at, within its comfort. | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| | `straight` | Whether a lanelet has no bend anywhere (its profile made first if need be). | já existia | sim/traffic/vehicle_motion.cpp turn_limit |
| `src/sim/vehicles/cycleLane.ts` | **arquivo** | bicicleta na ciclofaixa | portado | sim/traffic/cycling.cpp |
| | `CYCLE_EASE` | Distance at each end of a link over which a bicycle eases into and out of the cycle lane. | portado | sim/traffic/cycling.cpp |
| | `cycleShift` | Sideways shift of a bicycle's body at front arc `s` on `lane` (0 for anything else). | portado | sim/traffic/cycling.cpp |
| | `inside` | 0 at the ends of the link, 1 along its middle. | portado | sim/traffic/cycling.cpp |
| | `isBicycle` |  | portado | sim/traffic/cycling.cpp |
| | `outOfTheWay` | True when `lead` rides in the cycle lane beside `follower`'s path, out of its way. | portado | sim/traffic/cycling.cpp |
| `src/sim/vehicles/driver.ts` | **arquivo** | motorista | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `ACCEL_SPREAD` | Spread of the comfort parameters between the timid and the pushy. | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `BRAKE_SPREAD` |  | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `Driver` |  | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `DriverParams` | The subset of an archetype the longitudinal model actually reads. | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `EMERGENCY_SPREAD` | The emergency brake barely varies. | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `GAP_SPREAD` |  | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `HEADWAY_SPREAD` |  | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `desiredSpeed` | The speed this driver is aiming for right now. | já existia | sim/traffic/demand.cpp (vf, drift) |
| | `makeDriver` | Draws a driver. | já existia | sim/traffic/demand.cpp (vf, drift) |
| `src/sim/vehicles/idm.ts` | **arquivo** | IDM | já existia | sim/traffic/vehicle_motion.cpp |
| | `ConstraintSet` |  | já existia | sim/traffic/vehicle_motion.cpp |
| | `MutableObstacle` | An obstacle the set owns and may write again (its fields as the set reads them). | já existia | sim/traffic/vehicle_motion.cpp |
| | `Obstacle` | Anything a vehicle must not run into | já existia | sim/traffic/vehicle_motion.cpp |
| | `ObstacleKind` |  | já existia | sim/traffic/vehicle_motion.cpp |
| | `RELEASE_FADE` | Follower speed by which the queue-release response has faded out, world units a second. | já existia | sim/traffic/vehicle_motion.cpp |
| | `addObstacle` | An obstacle written into one of the set's slots and appended: the fields it does not carry left undefined (not the last tick's). | já existia | sim/traffic/vehicle_motion.cpp |
| | `canStopComfortably` | True when the vehicle can still stop before `d` without harsh braking. | já existia | sim/traffic/vehicle_motion.cpp |
| | `emptyConstraints` |  | já existia | sim/traffic/vehicle_motion.cpp |
| | `idmAccel` | Intelligent Driver Model acceleration for one obstacle. | já existia | sim/traffic/vehicle_motion.cpp |
| | `nextObstacle` | The next slot of the set, its object made once and written again at every tick: DO NOT make an obstacle a vehicle a tick (hundreds | já existia | sim/traffic/vehicle_motion.cpp |
| | `obstacleInto` | `out` filled as the found obstacle, or a new one when the caller keeps none (`findLeader` without a set): what a finder writes its | já existia | sim/traffic/vehicle_motion.cpp |
| | `resetConstraints` | The set emptied for a tick: its slots kept (nothing made), and nothing of the tick before left in the list. | já existia | sim/traffic/vehicle_motion.cpp |
| | `safeSpeed` | Highest speed from which this vehicle can still stop short of the obstacle | já existia | sim/traffic/vehicle_motion.cpp |
| | `smooth` |  | já existia | sim/traffic/vehicle_motion.cpp |
| `src/sim/vehicles/integrate.ts` | **arquivo** | integração | já existia | sim/traffic/vehicle_motion.cpp |
| | `CLEARANCE_EPSILON` |  | já existia | sim/traffic/vehicle_motion.cpp |
| | `advance` | Moves a vehicle onto the next lanelet on its route. | já existia | sim/traffic/vehicle_motion.cpp |
| | `advanceClearanceTokens` | Advances rear-clearance ownership without consuming the next admission. | já existia | sim/traffic/vehicle_motion.cpp |
| | `advanceTo` |  | já existia | sim/traffic/vehicle_motion.cpp |
| | `applyLaneChange` | Performs a lane change the lane-change stage already cleared as safe. | já existia | sim/traffic/vehicle_motion.cpp |
| | `discardUntravelledFromNewClearances` | Removes overshoot that the next stop line rejected from newly-created tokens. | já existia | sim/traffic/vehicle_motion.cpp |
| | `integrateAll` | The ONLY writer of `s`, `v` and `lanelet`. | já existia | sim/traffic/vehicle_motion.cpp |
| | `sideReach` | How far the body reaches sideways from its own centreline | já existia | sim/traffic/vehicle_motion.cpp |
| `src/sim/vehicles/kerbStops.ts` | **arquivo** | embarque e desembarque na guia, entrega do caminhão; parada de ônibus é do transit | portado | sim/traffic/kerb_stops.cpp |
| | `BUS_FETCH` | How far somebody will walk to catch a bus that has stopped. | de outra funcionalidade | parada de ônibus: funcionalidade `transit` |
| | `BUS_HEADWAY` | Least time between two bus stops, seconds of the bus's own age. | de outra funcionalidade | parada de ônibus: funcionalidade `transit` |
| | `DELIVERY_SHARE` | Share of trucks on a delivery. | portado | sim/traffic/kerb_stops.cpp |
| | `DELIVERY_TIME` | Seconds a truck's mate is away with the load. | portado | sim/traffic/kerb_stops.cpp |
| | `DOOR_CLEAR` | Radius round the door's swing that must be free of people before it opens. | portado | sim/traffic/kerb_stops.cpp |
| | `DOOR_OPEN_TIME` | Seconds a door takes to open and to shut. | portado | sim/traffic/kerb_stops.cpp |
| | `DOOR_SHUT_TIME` |  | portado | sim/traffic/kerb_stops.cpp |
| | `DROP_SHARE` | Share of cars with a passenger that drop one off somewhere on their trip. | portado | sim/traffic/kerb_stops.cpp |
| | `END_CLEARANCE` | Kept clear of the ends of a link: junctions, stop lines, zebras. | portado | sim/traffic/kerb_stops.cpp |
| | `ErrandKind` |  | portado | sim/traffic/kerb_stops.cpp |
| | `GIVE_UP` | Longest a car will stand waiting for room before it gives up and drives on. | portado | sim/traffic/kerb_stops.cpp |
| | `KERB_PACE` | A person's pace to or from the door, world units a second. | portado | sim/traffic/kerb_stops.cpp |
| | `KerbPerson` | The person moving between the seat and the footway. | portado | sim/traffic/kerb_stops.cpp |
| | `KerbStop` |  | portado | sim/traffic/kerb_stops.cpp |
| | `KerbStopPhase` |  | portado | sim/traffic/kerb_stops.cpp |
| | `LONGEST_BODY` | The longest body of any vehicle: past `reach + LONGEST_BODY` ahead nobody's rear reaches back to `reach`. | portado | sim/traffic/kerb_stops.cpp |
| | `PERSON_BASE` | First id of the people who ride in vehicles, clear of any pedestrian id. | portado | sim/traffic/kerb_stops.cpp |
| | `PICK_FETCH` | How far the person to collect may be from the door when the car stops | portado | sim/traffic/kerb_stops.cpp |
| | `PICK_REACH` | How far along the footway a pick-up looks for somebody walking alone. | portado | sim/traffic/kerb_stops.cpp |
| | `PICK_SHARE` | Share of cars with a free kerb-side seat that stop to pick somebody up. | portado | sim/traffic/kerb_stops.cpp |
| | `PLAN_MARGIN` | Least distance ahead a stop is planned, beyond comfortable braking. | portado | sim/traffic/kerb_stops.cpp |
| | `SEAT_STRIDE` | DO NOT go back to a bare 8: a body of nine seats or more gave its last seats the ids of the next vehicle's first ones | portado | sim/traffic/kerb_stops.cpp |
| | `SEAT_TIME` | Seconds to get out of a seat and stand beside the car, or to sit down. | portado | sim/traffic/kerb_stops.cpp |
| | `STEP_ABOARD` | Seconds to step up into a bus from its door. | de outra funcionalidade | parada de ônibus: funcionalidade `transit` |
| | `SeatPerson` | The person a seat holds, as `seatPerson` reads it. | portado | sim/traffic/kerb_stops.cpp |
| | `Seated` | What `seatPerson` reads of a vehicle (the simulation's own, or the page's view of it, `host/simClient.ts`). | portado | sim/traffic/kerb_stops.cpp |
| | `TaskKind` | What one task of a stop does: somebody gets out, or somebody gets in. | portado | sim/traffic/kerb_stops.cpp |
| | `abandon` |  | portado | sim/traffic/kerb_stops.cpp |
| | `assignOccupancy` | Fixed at spawn: which seats are taken, and whether the trip has an errand. | portado | sim/traffic/kerb_stops.cpp |
| | `beginTransfer` |  | portado | sim/traffic/kerb_stops.cpp |
| | `completeTransfer` |  | portado | sim/traffic/kerb_stops.cpp |
| | `doorAlongFromFront` | Where a door is along the body, from the front of the vehicle, as a share of its length: front row and rear row. | portado | sim/traffic/kerb_stops.cpp |
| | `doorPlaces` | World position of a kerb-side door's opening and of the spot beside it on the footway. | portado | sim/traffic/kerb_stops.cpp |
| | `enter` |  | portado | sim/traffic/kerb_stops.cpp |
| | `finishDoors` |  | portado | sim/traffic/kerb_stops.cpp |
| | `footwayBeside` | The nearest point of a walking footway on the kerb side at `s`, if one runs there. | portado | sim/traffic/kerb_stops.cpp |
| | `hail` | The car has stopped for somebody: they see it and walk over. | portado | sim/traffic/kerb_stops.cpp |
| | `kerbSide` | Right of a lane: the side a footway must be on, as a unit normal. | portado | sim/traffic/kerb_stops.cpp |
| | `kerbSideSeats` | Seat (and door) indices on the kerb side, front first. | portado | sim/traffic/kerb_stops.cpp |
| | `kerbStopObstacle` | The stop as an obstacle: the front of the vehicle halts at `at`, and holds there until the door has shut again. | portado | sim/traffic/kerb_stops.cpp |
| | `kerbTransfer` | Where the moving person is, for the renderer. | portado | sim/traffic/kerb_stops.cpp |
| | `newStop` |  | portado | sim/traffic/kerb_stops.cpp |
| | `passengerOrder` | Seats people take after the driver: front passenger, then behind. | portado | sim/traffic/kerb_stops.cpp |
| | `plan` | Chooses where to stop on the current link, or waits for a later one. | portado | sim/traffic/kerb_stops.cpp |
| | `planBusStop` | A bus stop: one or two passengers off by the middle door, and whoever is walking alone nearby on by the front door | de outra funcionalidade | parada de ônibus: funcionalidade `transit` |
| | `queuedAhead` | Somebody standing still or queued in front of the stop would make it a queue, not a stop. | portado | sim/traffic/kerb_stops.cpp |
| | `roomToOpen` | The door's swing and the footway spot are free of people. | portado | sim/traffic/kerb_stops.cpp |
| | `seatAgeClass` | `seatPerson(v, seat).ageClass` without the object (AGT-32: read for every seat of every car drawn, each frame). | portado | sim/traffic/kerb_stops.cpp |
| | `seatPerson` |  | portado | sim/traffic/kerb_stops.cpp |
| | `seatPersonInto` | `seatPerson` written into `out`: DO NOT make an object a seat a frame (the page draws every seated person each frame). | portado | sim/traffic/kerb_stops.cpp |
| | `setDoor` |  | portado | sim/traffic/kerb_stops.cpp |
| | `setSeat` | DO NOT `delete` an entry or write past the end | portado | sim/traffic/kerb_stops.cpp |
| | `stepKerbStops` | One step of every errand. The only writer of `kerbStop` and `doors`. | portado | sim/traffic/kerb_stops.cpp |
| `src/sim/vehicles/laneChange.ts` | **arquivo** | troca de faixa | já existia | sim/traffic/lane_change.cpp |
| | `CLEAR_ROAD` | An obstacle far enough away to be no obstacle at all. | já existia | sim/traffic/lane_change.cpp |
| | `COMMUTE_HOLD` | How near the place it stops at a vehicle no longer changes lane, u. | já existia | sim/traffic/lane_change.cpp |
| | `CRAWL` | Below this a vehicle ahead is treated as standing, not as traffic to follow. | já existia | sim/traffic/lane_change.cpp |
| | `FINISH_MARGIN` | Kept clear of a stop line or a queue's tail when a change must be finished. | já existia | sim/traffic/lane_change.cpp |
| | `KEEP_SIDE_BIAS` | Bias towards the kerb side, in units of acceleration. | já existia | sim/traffic/lane_change.cpp |
| | `LANE_CHANGE_COOLDOWN` | Shortest interval between two discretionary changes by the same driver. | já existia | sim/traffic/lane_change.cpp |
| | `LANE_CHANGE_MIN_ROOM` | Least room a mandatory change needs, whatever the speed. | já existia | sim/traffic/lane_change.cpp |
| | `LANE_CHANGE_TIME` | Seconds of travel needed to complete a change, floored for a standing car. | já existia | sim/traffic/lane_change.cpp |
| | `MOBIL_EVERY` | Ticks between two weighings of a discretionary change by the same driver (MOBIL): a quarter of a second. | já existia | sim/traffic/lane_change.cpp |
| | `Neighbour` |  | já existia | sim/traffic/lane_change.cpp |
| | `OVERTAKE_MIN_ROOM` |  | já existia | sim/traffic/lane_change.cpp |
| | `OVERTAKE_TIME` | Room a DISCRETIONARY change needs — six seconds of travel, and much more than the mandatory floor. | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears` |  | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears.ahead` |  | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears.build` |  | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears.close` |  | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears.data` | Per lane from its offset: the count, then that many (s, rear, id) of its vehicles in the lane's order. | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears.lanes` |  | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears.open` |  | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears.used` |  | já existia | sim/traffic/lane_change.cpp |
| | `RestingRears.w` |  | já existia | sim/traffic/lane_change.cpp |
| | `SAFE_BRAKE_SHARE` | Hardest braking a discretionary change may impose on the car behind. | já existia | sim/traffic/lane_change.cpp |
| | `SIGNAL_LEAD` | Seconds a driver signals before moving across. | já existia | sim/traffic/lane_change.cpp |
| | `SIGNAL_LEAD_CRAWLING` |  | já existia | sim/traffic/lane_change.cpp |
| | `canLeaveLane` | Whether the body can get out of the lane it is in before whatever stands in that lane stops it. | já existia | sim/traffic/lane_change.cpp |
| | `changeLength` | Road a change into `target` from here would be driven over. | já existia | sim/traffic/lane_change.cpp |
| | `decideLaneChanges` |  | já existia | sim/traffic/lane_change.cpp |
| | `discretionary` | The change the driver wants: the best neighbouring lane, or none. | já existia | sim/traffic/lane_change.cpp |
| | `followerIn` | The nearest vehicle behind `rear` in a lane. | já existia | sim/traffic/lane_change.cpp |
| | `gapIsSafe` | Whether the target lane has room beside this vehicle right now. | já existia | sim/traffic/lane_change.cpp |
| | `lastSideways` |  | já existia | sim/traffic/lane_change.cpp |
| | `leaderIn` | The nearest vehicle ahead of `s` in a lane, as an obstacle. | já existia | sim/traffic/lane_change.cpp |
| | `mandatory` | The adjacent lane the route asks for, or null when there is no longer room to get there | já existia | sim/traffic/lane_change.cpp |
| | `nextOutSegment` | Segment the planned next junction movement leaves by, or null when none is planned. | já existia | sim/traffic/lane_change.cpp |
| | `positioningSpeedCap` | The speed a driver holds while working across to the lane the route needs. | já existia | sim/traffic/lane_change.cpp |
| | `pullOutFrame` |  | já existia | sim/traffic/lane_change.cpp |
| | `pullOutRoom` | Room a driver leaves behind a vehicle standing at the kerb | já existia | sim/traffic/lane_change.cpp |
| | `resting` |  | já existia | sim/traffic/lane_change.cpp |
| | `restingReach` | How much further a vehicle rolls before something standing holds it; Infinity when nothing does. | já existia | sim/traffic/lane_change.cpp |
| | `restingRearAhead` | Where the rear of the nearest vehicle ahead of `s` in a lane will come to rest. | já existia | sim/traffic/lane_change.cpp |
| | `roomAt` | `roomNeeded` with the lateral distance to the adjacent lane given (`sideways`): DO NOT project on the lanes again for each speed t | já existia | sim/traffic/lane_change.cpp |
| | `roomNeeded` | Road a mandatory change of `steps` lanes needs at `speed`: each lane is signalled for | já existia | sim/traffic/lane_change.cpp |
| | `shadowClearDistance` | Road driven before no corner of the body is over the line between two lanes `start` apart, on a change `length` long | já existia | sim/traffic/lane_change.cpp |
| | `sideways` | How far across `target`'s centre lies from this vehicle's place on its lane; null when either lane is gone. | já existia | sim/traffic/lane_change.cpp |
| | `sidewaysFrame` |  | já existia | sim/traffic/lane_change.cpp |
| | `standingIntent` | Between two weighings: the change being signalled, while it can still be made from here (its lane beside this one, room left). | já existia | sim/traffic/lane_change.cpp |
| | `stepLaneChange` |  | já existia | sim/traffic/lane_change.cpp |
| `src/sim/vehicles/laneClosures.ts` | **arquivo** | trecho de faixa fechado pelo caminhão | portado | sim/traffic/lane_closures.cpp |
| | `CLEAR_MARGIN` | Kept between a closed stretch and the nearest body on it or past it. | portado | sim/traffic/lane_closures.cpp |
| | `CLOSED` |  | portado | sim/traffic/lane_closures.cpp |
| | `Closure` | Stretches of lane closed for a while by something off the lanes standing on them | portado | sim/traffic/lane_closures.cpp |
| | `NONE` |  | portado | sim/traffic/lane_closures.cpp |
| | `anyClosed` | Whether any lane is closed at all (the commonest answer is no, and costs one look). | portado | sim/traffic/lane_closures.cpp |
| | `closeLane` |  | portado | sim/traffic/lane_closures.cpp |
| | `closedOn` | The stretches closed on a lane now. | portado | sim/traffic/lane_closures.cpp |
| | `laneClear` | Whether a stretch of lane may be closed now | portado | sim/traffic/lane_closures.cpp |
| | `laneClear.stopping` |  | portado | sim/traffic/lane_closures.cpp |
| | `lanesInto` | The lanes a vehicle enters `id` from: a connector's own approach, or the connectors into a link at its junction. | portado | sim/traffic/lane_closures.cpp |
| | `openLanes` | Every stretch `owner` closed, opened again. | portado | sim/traffic/lane_closures.cpp |
| `src/sim/vehicles/leaderIndex.ts` | **arquivo** | quem vai à frente | já existia | sim/traffic/lane_list.cpp |
| | `LOOKAHEAD_HOPS` | How many lanelets ahead the leader search will walk. | já existia | sim/traffic/lane_list.cpp |
| | `SHADOW_SIDE_MARGIN` | Sideways clearance kept from a body in the lane being left, world units. | já existia | sim/traffic/lane_list.cpp |
| | `divergeObstacle` | The body of a vehicle that took a DIFFERENT movement out of the same lane | já existia | sim/traffic/lane_list.cpp |
| | `findLeader` | Nearest obstacle ahead, following the vehicle's own planned lanelet sequence; `out` written as the winner (a set's slot reused, `i | já existia | sim/traffic/lane_list.cpp |
| | `laneSlot` | DO NOT go back to `indexOf` for every vehicle | já existia | sim/traffic/lane_list.cpp |
| | `shadowLeaderObstacle` | A vehicle mid-lane-change is itself an obstacle for its OWN forward motion against whatever it still overlaps in the | já existia | sim/traffic/lane_list.cpp |
| | `shadowsAhead` | Rears of bodies sliding out of a lane, ahead of `s` on it. | já existia | sim/traffic/lane_list.cpp |
| | `upcomingLanelets` | The next `n` lanelets on the vehicle's route, excluding the current one. | já existia | sim/traffic/lane_list.cpp |
| `src/sim/vehicles/obstacles.ts` | **arquivo** | obstáculos: parada, fechamento | portado | sim/traffic/vehicle_motion.cpp |
| | `CLOSURE_STOP` | How far short of a closed stretch of lane the traffic stops. | portado | sim/traffic/vehicle_motion.cpp |
| | `CROSSING_IDS` | DO NOT drop every node's for an edit: the whole map's were spelt again after each road drawn. | portado | sim/traffic/vehicle_motion.cpp |
| | `PED_STOP_MARGIN` | Extra space left in front of the zebra band (`CrossingSpan.along` already clears it). | portado | sim/traffic/vehicle_motion.cpp |
| | `crossingIdsAt` |  | portado | sim/traffic/vehicle_motion.cpp |
| | `longitudinalConstraints` | Turns the world into constraints for one vehicle. | portado | sim/traffic/vehicle_motion.cpp |
| | `mergeObstacle` | The vehicle ahead on ANOTHER movement into the lane this one runs into. | portado | sim/traffic/vehicle_motion.cpp |
| | `pedestrianAhead` | Stop short of a zebra span somebody is on; null when the path is clear. | portado | sim/traffic/vehicle_motion.cpp |
| `src/sim/vehicles/spawn.ts` | **arquivo** | nascer veículos | já existia | sim/traffic/demand.cpp |
| | `<EmptyStatement>` |  | já existia | sim/traffic/demand.cpp |
| | `ADMIT_PER_TICK` | Most vehicles admitted onto the map in one tick, city-wide. | já existia | sim/traffic/demand.cpp |
| | `DestinationPicker` | Where a new car's trip ends, from its first lane: a map exit by default (`chooseVehicleDestination`), the micro window's edge for  | já existia | sim/traffic/demand.cpp |
| | `ENTRY_QUEUE` | Vehicles an entry holds outside the map before further arrivals are lost. | já existia | sim/traffic/demand.cpp |
| | `ENTRY_RATE` | Arrival rate at one boundary entry lane, vehicles per second, at traffic intensity and demand 1 | já existia | sim/traffic/demand.cpp |
| | `FILL_RATE` | Arrivals a second at one entry with a number of cars chosen (`SimWorld.trafficCount`) once the map holds it | já existia | sim/traffic/demand.cpp |
| | `birthSpeed` | How fast a vehicle arrives from off the map | já existia | sim/traffic/demand.cpp |
| | `chosenTraffic` | How many cars the map is kept at: the panel's count, or, following the city | já existia | sim/traffic/demand.cpp |
| | `entries` |  | já existia | sim/traffic/demand.cpp |
| | `entryLanes` | DO NOT walk the map's lanes for an edit. | já existia | sim/traffic/demand.cpp |
| | `isEntry` | A link long enough to arrive on, starting at a node on the edge of the map. | já existia | sim/traffic/demand.cpp |
| | `nextGap` | Seconds to the next arrival of a Poisson stream of `rate` per second. | já existia | sim/traffic/demand.cpp |
| | `roadLength` | The road length of a network, kept by what each rebuild changed (`world/networkLength.ts`). | já existia | sim/traffic/demand.cpp |
| | `spawnAt` | Puts one vehicle on the map at the start of an entry lane, if the lane has room behind whatever is already on it. | já existia | sim/traffic/demand.cpp |
| | `spawnVehicle` | Spawns one vehicle at the first entry lane with room for it. | já existia | sim/traffic/demand.cpp |
| | `spawnVehicleAt` | Puts one vehicle of `arch` on lane `id` with its front at arc `s` | já existia | sim/traffic/demand.cpp |
| | `stepDespawn` | Removes the vehicles that reached the end of a road and stopped there: the only place a car leaves the map | já existia | sim/traffic/demand.cpp |
| | `stepDispatch` | Brings traffic in at the edges of the map. | já existia | sim/traffic/demand.cpp |
| | `trafficTarget` | Density ceiling on the fleet, from total road length. | já existia | sim/traffic/demand.cpp |
| `src/sim/vehicles/state.ts` | **arquivo** | estado do veículo | portado | sim/traffic/traffic_state.h |
| | `ConnectorClearance` |  | portado | sim/traffic/traffic_state.h |
| | `FreePose` | A body centre and heading, now and at the previous tick (for interpolation). | portado | sim/traffic/traffic_state.h |
| | `Kinematics` |  | portado | sim/traffic/traffic_state.h |
| | `StandingCar` | A car standing in a bay as its readers see it (`AmbientWorld.parked`): a vehicle, or a scenery's row that is none (`SceneryCar`). | portado | sim/traffic/traffic_state.h |
| | `Vehicle` |  | portado | sim/traffic/traffic_state.h |
| | `createVehicle` |  | portado | sim/traffic/traffic_state.h |
| | `snapshot` |  | portado | sim/traffic/traffic_state.h |
| | `snapshotInto` | `snapshot` written into the vehicle's own `prev`, every field of it | portado | sim/traffic/traffic_state.h |
| | `strandVehicle` | The driver shot dead at the wheel: the car rolls to a stop where it is and stays there | portado | sim/traffic/traffic_state.h |
| `src/sim/vehicles/vehicleId.ts` | **arquivo** | id do veículo | já existia | índice em TrafficState::vehicles |
| | `VehicleId` | A vehicle's id (`vehicles/state.ts` re-exports it). | já existia | índice em TrafficState::vehicles |
| `src/sim/world.ts` | **arquivo** | mundo da simulação | já existia | CityState, TrafficState |
| | `SimWorld.addShadow` |  | já existia | CityState, TrafficState |
| | `SimWorld.approached` |  | já existia | CityState, TrafficState |
| | `SimWorld.approachedSeen` |  | já existia | CityState, TrafficState |
| | `SimWorld.clearancesAtStart` |  | já existia | CityState, TrafficState |
| | `SimWorld.junctionsApproached` |  | já existia | CityState, TrafficState |
| | `SimWorld.laneHead` |  | já existia | CityState, TrafficState |
| | `SimWorld.nearLanes` |  | já existia | CityState, TrafficState |
| | `SimWorld.nextPersonId` |  | já existia | CityState, TrafficState |
| | `SimWorld.roadLength` |  | já existia | CityState, TrafficState |
| | `SimWorld.signalAtCrossing` |  | já existia | CityState, TrafficState |
| | `SimWorld.someVehicleNear` |  | já existia | CityState, TrafficState |
| | `SimWorld.vehiclesNear` |  | já existia | CityState, TrafficState |
| | `crossingNode` |  | já existia | CityState, TrafficState |
| `src/ui/inspector.ts` | **arquivo** | inspetor: estacionamento da via | portado | project/ui/inspector_panel.gd |
| | `junctionHost.setControl` |  | portado | project/ui/inspector_panel.gd |
| | `junctionHost.setHeight` |  | portado | project/ui/inspector_panel.gd |
| | `junctionHost.setMovementBlocked` |  | portado | project/ui/inspector_panel.gd |
| | `junctionHost.setRules` |  | portado | project/ui/inspector_panel.gd |
| | `junctionHost.setSignal` |  | portado | project/ui/inspector_panel.gd |
| | `junctionHost.setSignals` |  | portado | project/ui/inspector_panel.gd |
| `src/ui/roads/junctionPanel.ts` | **arquivo** | painel do semáforo: tempos, defasagem, onda verde | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `CONTROLS` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `JunctionPanelHost` | THE JUNCTION PANEL, in the inspector: the junction's height, its control as buttons | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `RULES` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `button` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `compassOf` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `div` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `flowOf` | A leg's flow, vehicles an hour, from the junction's answer (`SimClient.junction`); 0 before the first. | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `flowsShown` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `fmt` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `head` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `mountJunctionPanel` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `mountJunctionPanel.set` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `moveChip` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `refreshJunctionFlows` | Rewrites the flows in place (the inspector's refresh), without rebuilding the panel. | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `row` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `rowOf` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `screenOf` | The way a leg points from its junction AS THE PLAYER SEES IT | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `segmented` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `stepper` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `toggle` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| | `withoutFixed` |  | portado | project/ui/junction_panel.gd; sem a escolha automático/parada/preferência, que depende do controlAdvisor |
| `src/ui/roads/profileEditor.ts` | **arquivo** | editor de perfil | de outra funcionalidade | `roads` |
| `src/world/buildings/docks.ts` | **arquivo** | docas do galpão | já existia | sim/buildings/lot_access.cpp (EL_DOCK) |
| | `LORRY_DOCK` | A stall wider than this is a lorry's, not a car's (`sim/agents/parking.ts`). | já existia | sim/buildings/lot_access.cpp (EL_DOCK) |
| | `LorryDock` | A works' lorry docks: the wide bays its loading yard is laid with, each square in front of a loading door of the hall | já existia | sim/buildings/lot_access.cpp (EL_DOCK) |
| | `STALL` |  | já existia | sim/buildings/lot_access.cpp (EL_DOCK) |
| | `docksOf` |  | já existia | sim/buildings/lot_access.cpp (EL_DOCK) |
| | `isDock` |  | já existia | sim/buildings/lot_access.cpp (EL_DOCK) |
| | `lorryDocks` | A building's lorry docks, worked out when its traffic record is sent (`sim/agents/trafficBuildings.ts`); the worker keeps them, th | já existia | sim/buildings/lot_access.cpp (EL_DOCK) |
| | `ringDistance` |  | já existia | sim/buildings/lot_access.cpp (EL_DOCK) |
| `src/world/buildings/sections.ts` | **arquivo** | seções de prédio | já existia | sim/buildings/b40 |
| | `eachSectionRef` |  | já existia | sim/buildings/b40 |
| | `sectionsSize` |  | já existia | sim/buildings/b40 |
| | `sectionsSize.rows` |  | já existia | sim/buildings/b40 |
| `src/world/buildings/streetDoors.ts` | **arquivo** | portas para a rua | de outra funcionalidade | `people` |
| | `PEOPLE_DOORS` | The ways a person goes in and out by (a garage's or a loading bay's door is a vehicle's). | de outra funcionalidade | `people` |
| | `STEP_OUT` | How far out from the door its foot is looked for, clear of the building and its lots. | de outra funcionalidade | `people` |
| | `StreetDoor` | The doors of a building that open straight onto the street: a shop or a bar built up to the footway | de outra funcionalidade | `people` |
| | `streetDoors` |  | de outra funcionalidade | `people` |
| `src/world/conflictPoints.ts` | **arquivo** | pontos de conflito | já existia | sim/traffic/lane_graph.cpp |
| | `CachedPair` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex` | Where two movements through a junction physically conflict. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.add` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.build` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.buildJunction` | One junction's conflicts, from the graph and the caches, with what it made recorded (`ofJunction`). | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.byConnector` | References held by each connector, sorted by arc position. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.conflict` | True when the two connectors have at least one conflict point. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.diverges` | Sibling movements from the same lane whose bodies overlap near the split. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.divergesOf` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.entering` | The junctions of `next` this index does not hold yet: what a window moved to must have prepared first (`SimWorld.moveWindowSteps`) | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.finishOffPage` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.forgetIds` | The ids of these junctions (every one known when none is named) let go when this index no longer holds them (`keysOf`). | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.holds` | Whether this index holds a junction's pairs: outside the window it holds none, and none of its movements reads as conflicting. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.holdsWindow` | Whether the window held is `next`. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.idByKey` | Conflict ids are resources held by live vehicles, so their meaning must survive a topology rebuild. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.inWindow` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.inWindowOf` | Whether a junction is in the window (every one with no window). | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.kept` | The junctions whose conflicts are kept (null: every one): the micro simulation's window round the focus and the vehicles (`SimWorl | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.keysOf` | The keys of `idByKey` each junction made: DO NOT keep the ids of a junction the window left (no vehicle holds them), or the map gr | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.lookup` | Reads the zones kept in an earlier session for these pairs (the derived cache, from the simulation's worker): only the pairs a bui | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.measureOffPage` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.measured` | Pairs measured (not read from the caches) by the last `build` and the last `prepare`: for the probes. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.nextId` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.ofJunction` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.pairCache` | The zones of every pair measured by the previous build, keyed by the two sweeps' sampled geometry. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.pairUses` | How many junctions read each cached pair and sweep: one no junction reads any more is let go. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.points` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.prepare` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.prepared` | DO NOT read them again at the swap | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.pruneKept` | The zones kept from other maps, from roads since undone and from junctions the window left stay in memory only while they are few  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.queueIntrusions` | Movements whose zone reaches a body still waiting behind its stop line. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.recordIntrusions` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.refs` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.sampleOffPage` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.seed` | Zones measured in an earlier session (the game keeps them in the browser, `main.ts`) | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.setWindow` | The window moved: the junctions it left let go (their pairs with them, `unuse`), those it reached built (`update`); false when non | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.store` | Every pair's zones known to this index, kept between sessions by the game (`seed`, `takeMeasured`). | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.sweepCache` | The sampled frames of the movements, keyed by the three centrelines they are sampled along. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.sweepUses` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.takeMeasured` | The zones measured since the last call, to be kept (`seed` reads them back next session). | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.takePrepared` | The junction's record from `prepare` (`prepared`), if it was made from the very junction and lanes built now. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictIndex.update` | DO NOT build the index of the whole map for an edit | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictPoint` |  | já existia | sim/traffic/lane_graph.cpp |
| | `FRAME_LENGTHS` |  | já existia | sim/traffic/lane_graph.cpp |
| | `KeptZones` | The zones kept from earlier sessions and those measured in this one, by pair key | já existia | sim/traffic/lane_graph.cpp |
| | `LINE_DIGESTS` | Each centreline's digest, kept with its point list (`sweepOf`; made by a worker, `sampleOffPage`). | já existia | sim/traffic/lane_graph.cpp |
| | `Movement` | A movement through a junction, swept: its connector, its sweep and shape and the key its frame is kept under. | já existia | sim/traffic/lane_graph.cpp |
| | `PreparedJunction` | A junction as `prepare` read it (`ConflictIndex.prepared`): its connectors, its swept movements, its pairs' keys in `buildJunction | já existia | sim/traffic/lane_graph.cpp |
| | `SHAPES` | The shape of a sweep, kept with its frame | já existia | sim/traffic/lane_graph.cpp |
| | `Sweep` | A movement's sweep (`conflictTable.ts` `SweepSamples`) and the lanelets it was sampled along. | já existia | sim/traffic/lane_graph.cpp |
| | `SweepShape` | Everything `pairTable` reads from a sweep. Two equal shapes give equal zones. | já existia | sim/traffic/lane_graph.cpp |
| | `bodyClassOf` |  | já existia | sim/traffic/lane_graph.cpp |
| | `cachedPair` | A pair's zones, answered from the previous build when neither sweep moved. | já existia | sim/traffic/lane_graph.cpp |
| | `keepPair` | `fresh`: to be kept for the next session by the page (`takeMeasured`); not when its measurer kept it already. | já existia | sim/traffic/lane_graph.cpp |
| | `lineDigest` |  | já existia | sim/traffic/lane_graph.cpp |
| | `measurePair` | A pair measured, or read back from an earlier session. | já existia | sim/traffic/lane_graph.cpp |
| | `measureShape` |  | já existia | sim/traffic/lane_graph.cpp |
| | `pairsMeasured` | Pairs measured by `cachedPair` since the module loaded (`ConflictIndex.measured`). | já existia | sim/traffic/lane_graph.cpp |
| | `sameShape` |  | já existia | sim/traffic/lane_graph.cpp |
| | `sameValues` |  | já existia | sim/traffic/lane_graph.cpp |
| | `shapeFrom` | A sweep's shape from its digest (`shapeDigest`, taken here or by a worker). | já existia | sim/traffic/lane_graph.cpp |
| | `shapeOf` |  | já existia | sim/traffic/lane_graph.cpp |
| | `sweepKey` | The key of a sweep's frame in `sweepCache`: its three centrelines' digests and its sample count. | já existia | sim/traffic/lane_graph.cpp |
| | `sweepOf` |  | já existia | sim/traffic/lane_graph.cpp |
| | `unuse` | One use fewer of a cached key; the entry let go when no junction reads it. | já existia | sim/traffic/lane_graph.cpp |
| | `zoneCentre` |  | já existia | sim/traffic/lane_graph.cpp |
| `src/world/conflictTable.ts` | **arquivo** | tabela de conflitos | já existia | sim/traffic/lane_graph.cpp |
| | `BODY_CLASSES` |  | já existia | sim/traffic/lane_graph.cpp |
| | `BODY_ENVELOPE` |  | já existia | sim/traffic/lane_graph.cpp |
| | `BodyClass` | Size classes a conflict zone is measured for. | já existia | sim/traffic/lane_graph.cpp |
| | `CELL` | Broad-phase cell size, world units. | já existia | sim/traffic/lane_graph.cpp |
| | `ChosenTurn` | What the search for a turn answers (`turnPaths.ts` `chooseTurn`): the choice and the largest body it fits. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictKind` | `cross`: the two centrelines intersect. | já existia | sim/traffic/lane_graph.cpp |
| | `ConflictRef` |  | já existia | sim/traffic/lane_graph.cpp |
| | `ConnectorId` | A connector's id (`lanelets.ts` `ConnectorId`), named here without importing the graph. | já existia | sim/traffic/lane_graph.cpp |
| | `Crossing` | Where two movements' centrelines first cross, with the arc position on each. | já existia | sim/traffic/lane_graph.cpp |
| | `Diverge` | Two movements out of the same lane: resolved by car-following, not claims. | já existia | sim/traffic/lane_graph.cpp |
| | `FinishAsk` | A pair to finish somewhere else (`PairMeasurer.finishPairs`): its two centrelines' own point lists (`Polyline.xy`) and its table ( | já existia | sim/traffic/lane_graph.cpp |
| | `FinishedPair` | A pair finished (`finishPair`): where its centrelines first cross, and what the index makes of its zones. | já existia | sim/traffic/lane_graph.cpp |
| | `HALF_LENGTH` | Half length and half width of each class's swept rectangle, built once: DO NOT read them as a pair destructured (`const [l, w] = . | já existia | sim/traffic/lane_graph.cpp |
| | `HALF_WIDTH` |  | já existia | sim/traffic/lane_graph.cpp |
| | `HEAVY` |  | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionAnswer` | A junction a worker built (`buildJunctionOf`): its key, groups and control, each movement with its path's points, the turns it kep | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionAsk` | One junction of a whole build, as a worker builds it (`buildJunctionOf`) | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionLeg` | A road meeting the junction: what the junction reads of it, and its two ends (its centreline, `segmentLine`). | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionMeasurer` | Builds a whole build's junctions somewhere else - the browser's workers (`conflictPool.ts`) | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionNode` | What a junction reads of its node. | já existia | sim/traffic/lane_graph.cpp |
| | `LaneAsk` | A link lane arriving at or leaving a junction (`LaneletGraph.buildSegmentLinks`), as a worker is sent it: its centre as its own po | já existia | sim/traffic/lane_graph.cpp |
| | `PairFacts` | DO NOT work them out again for each build | já existia | sim/traffic/lane_graph.cpp |
| | `PairMeasurer` | Measures many pairs' tables somewhere else - the browser's workers (`conflictPool.ts`) | já existia | sim/traffic/lane_graph.cpp |
| | `PairWork` | What `pairTable` works in, kept by whoever measures many pairs (a worker's batch, the conflict index): DO NOT make them a pair. `s | já existia | sim/traffic/lane_graph.cpp |
| | `PairWork.hits` |  | já existia | sim/traffic/lane_graph.cpp |
| | `PairWork.projection` |  | já existia | sim/traffic/lane_graph.cpp |
| | `PairWork.seen` |  | já existia | sim/traffic/lane_graph.cpp |
| | `PairWork.stamp` |  | já existia | sim/traffic/lane_graph.cpp |
| | `PairZones` | A pair of movements' zones as the conflict index reads them (`conflictPoints.ts` `ConflictIndex`), over the pair's table. | já existia | sim/traffic/lane_graph.cpp |
| | `QUEUE_BACK` | How far behind its stop line the front of a waiting body is swept, world units | já existia | sim/traffic/lane_graph.cpp |
| | `SWEEP_MARGIN` | Clearance kept around every body, world units (6 cm). | já existia | sim/traffic/lane_graph.cpp |
| | `SWEEP_STEP` | Sampling step of the body centre along a movement, world units. | já existia | sim/traffic/lane_graph.cpp |
| | `SampledSweep` | A movement's sweep sampled (`sampleSweep`): its frame, and the digests the page keys it by. | já existia | sim/traffic/lane_graph.cpp |
| | `SurfaceRings` | A junction's surface as a worker makes it again: its centre and its rings as flat x, y lists. | já existia | sim/traffic/lane_graph.cpp |
| | `SweepAsk` | What one movement's sweep is sampled from, as plain numbers | já existia | sim/traffic/lane_graph.cpp |
| | `SweepGrid` | A sweep's HEAVY rectangles filed by cell over the sweep's own box, as typed arrays (CSR: each cell's samples at `items[start[c]..s | já existia | sim/traffic/lane_graph.cpp |
| | `SweepSamples` | What `pairTable` reads of a movement's sweep (`conflictPoints.ts` `Sweep`), and the caches it keeps on it. | já existia | sim/traffic/lane_graph.cpp |
| | `TABLE_SIZE` | Numbers in a pair's table (`pairTable`): 3 x 3 class pairs x [enterA, exitA, enterB, exitB]. | já existia | sim/traffic/lane_graph.cpp |
| | `TurnAsk` | What a turn is chosen from, as plain numbers, for a worker (`conflictPool.ts`) | já existia | sim/traffic/lane_graph.cpp |
| | `TurnChoice` | A turn as `turnPaths.ts` chose it: one cubic's handle, or two cubics through a point inside the junction. | já existia | sim/traffic/lane_graph.cpp |
| | `TurnKind` | How a movement turns (`junctionBuild.ts` `classifyTurn`). | já existia | sim/traffic/lane_graph.cpp |
| | `Zone` | Centre-arc interval over which a body of one class on one movement overlaps the swept area of a body of another class on | já existia | sim/traffic/lane_graph.cpp |
| | `boundsOf` |  | já existia | sim/traffic/lane_graph.cpp |
| | `clearExits` | Per class, the centre past which the body is clear of any other body. | já existia | sim/traffic/lane_graph.cpp |
| | `eachCell` | Each sample's HEAVY rectangle's cell span, `visit(gx, gy)` for each cell. | já existia | sim/traffic/lane_graph.cpp |
| | `earliestFront` | Earliest body-FRONT arc position at which any class enters the zone. | já existia | sim/traffic/lane_graph.cpp |
| | `finishPair` | A pair finished from its two centrelines' points and its table: `firstCrossing` and `pairFacts`, as the index works them out (`Con | já existia | sim/traffic/lane_graph.cpp |
| | `firstCrossing` | First intersection between two polylines, with arc positions on each (`conflictPoints.ts` keeps it with the pair). | já existia | sim/traffic/lane_graph.cpp |
| | `frameLengthsOf` | Squared tangent length per sample of a frame (`SweepSamples.frameLengthSq`). | já existia | sim/traffic/lane_graph.cpp |
| | `gridOf` |  | já existia | sim/traffic/lane_graph.cpp |
| | `gridOf.count` |  | já existia | sim/traffic/lane_graph.cpp |
| | `gridOf.put` |  | já existia | sim/traffic/lane_graph.cpp |
| | `inRange` | Whether a sample is a place a body of this class can be on the movement. | já existia | sim/traffic/lane_graph.cpp |
| | `joinedPath` | The approach, the movement and the exit as one arc-length parameter. | já existia | sim/traffic/lane_graph.cpp |
| | `joinedPath.sampleAt` |  | já existia | sim/traffic/lane_graph.cpp |
| | `lineDigest` | Digest of a centreline's points: part of a sweep's cache key (`conflictPoints.ts` `sweepOf`). | já existia | sim/traffic/lane_graph.cpp |
| | `overlap` |  | já existia | sim/traffic/lane_graph.cpp |
| | `pairFacts` |  | já existia | sim/traffic/lane_graph.cpp |
| | `pairTable` | The 3 x 3 class pairs' [enterA, exitA, enterB, exitB] (NaN: none), or null when even two heavy bodies never touch. | já existia | sim/traffic/lane_graph.cpp |
| | `pushDiverge` |  | já existia | sim/traffic/lane_graph.cpp |
| | `pushRef` |  | já existia | sim/traffic/lane_graph.cpp |
| | `rangeOf` |  | já existia | sim/traffic/lane_graph.cpp |
| | `sampleSweep` | One movement's sweep sampled from plain numbers, as `conflictPoints.ts` `sweepOf` and `measureShape` would on the page | já existia | sim/traffic/lane_graph.cpp |
| | `sampleSweep.lineOf` |  | já existia | sim/traffic/lane_graph.cpp |
| | `shapeDigest` | Digest of a sweep's frame and its movement's centreline: what a pair's key is made of | já existia | sim/traffic/lane_graph.cpp |
| | `sweepFrame` | The frame of a sweep: `count` body centres `SWEEP_STEP` apart from `c0` along the joined path | já existia | sim/traffic/lane_graph.cpp |
| | `sweepSpan` | A sweep's first sample and count: from a heavy body a little short of its stop line (`QUEUE_BACK`) to its rear past the movement's | já existia | sim/traffic/lane_graph.cpp |
| | `widen` |  | já existia | sim/traffic/lane_graph.cpp |
| | `zonesOf` | DO NOT make a zone each time one is read (two numbers of a table that never changes, read dozens of times a vehicle near a junctio | já existia | sim/traffic/lane_graph.cpp |
| | `zonesOf.get` | Zone on `a` (`onB` false) or on `b` (true) for body `mine` against `theirs`. | já existia | sim/traffic/lane_graph.cpp |
| `src/world/derived.ts` | **arquivo** | dados derivados do documento | já existia | ChangeLog e RoadNetwork |
| | `ChangeSweep` |  | já existia | ChangeLog e RoadNetwork |
| | `ChangeSweep.advance` | Reads what was written since the last advance (once a frame). | já existia | ChangeLog e RoadNetwork |
| | `ChangeSweep.cells` | Rectangles by cell; null: a change of the whole map (or one too wide to file). | já existia | ChangeLog e RoadNetwork |
| | `ChangeSweep.constructor` |  | já existia | ChangeLog e RoadNetwork |
| | `ChangeSweep.from` |  | já existia | ChangeLog e RoadNetwork |
| | `ChangeSweep.now` |  | já existia | ChangeLog e RoadNetwork |
| | `ChangeSweep.serial` | The serial the readers are brought up to. | já existia | ChangeLog e RoadNetwork |
| | `ChangeSweep.touches` | Whether a change of the kinds after `seen` reaches the point. | já existia | ChangeLog e RoadNetwork |
| `src/world/doc.ts` | **arquivo** | documento: estacionamento e semáforo salvos | portado | sim/roads/road_network.h, sim/persistence |
| `src/world/heading.ts` | **arquivo** | direção | já existia | core/math.h |
| | `chordHeading` |  | já existia | core/math.h |
| `src/world/junction/mouthOverlap.ts` | **arquivo** | bocas do cruzamento | já existia | sim/roads/junction.cpp |
| | `REACH` | How far the strip reaches either side of the mouth, along the road, world units. | já existia | sim/roads/junction.cpp |
| | `SHORTEST` | Shortest strip worth adding, world units. | já existia | sim/roads/junction.cpp |
| | `STEP` | Spacing of the strip's cross-sections, world units. | já existia | sim/roads/junction.cpp |
| | `mouthOverlapStrip` | The strip of a curved road's full width across one of its junction mouths. | já existia | sim/roads/junction.cpp |
| `src/world/junctionBuild.ts` | **arquivo** | monta o cruzamento | já existia | sim/traffic/lane_graph.cpp, junction_control.cpp |
| | `isJunction` |  | já existia | sim/traffic/lane_graph.cpp, junction_control.cpp |
| | `junctionAsk` |  | já existia | sim/traffic/lane_graph.cpp, junction_control.cpp |
| | `junctionAsk.lane` |  | já existia | sim/traffic/lane_graph.cpp, junction_control.cpp |
| | `junctionAsk.leg` |  | já existia | sim/traffic/lane_graph.cpp, junction_control.cpp |
| `src/world/junctionTurns.ts` | **arquivo** | conversões | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionTurns.constructor` |  | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionTurns.defer` |  | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionTurns.deferring` |  | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionTurns.find` |  | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionTurns.keep` |  | já existia | sim/traffic/lane_graph.cpp |
| | `JunctionTurns.provisional` |  | já existia | sim/traffic/lane_graph.cpp |
| | `TurnPlan.provisional` |  | já existia | sim/traffic/lane_graph.cpp |
| `src/world/lanelets.ts` | **arquivo** | faixas, com o desvio da ciclofaixa | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `Connector` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `ConnectorId` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `GraphChange` | What one `LaneletGraph.update` built again: the junctions, the lanelets that came or went, the segments. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `JunctionCache` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `JunctionTopology` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `Lanelet` | The atomic unit of vehicle space: a directed, single-lane centreline. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.adopt` | DO NOT seed this graph's caches from `from` and `update` it | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.brought` | The graph now stands for the network as it is. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.build` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.buildJunction` | The connectors of one node (`buildJunctions`): kept whole when its key is known, built otherwise (`junctionBuild.ts`). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.buildJunction.surfaceOf` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.buildJunctions` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.buildJunctions.before` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.buildLinks` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.buildSegmentLinks` | The link lanelets of one segment, both directions (`buildLinks`), under the keys they were built from. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.buildSteps` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.bySegment` | The link lanelets of each segment, both directions, in the order they were built (the order of `lanelets`). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.changeTargets` | The lanes of the same carriageway a vehicle of this shape may change into from `id`: across dashed lines only | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.changes` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.connectors` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.current` | Whether this graph stands for the network as it is now, finished: no junction on stand-in turns (`adopt`). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.exits` | Outgoing connectors of each link lanelet. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.exitsOf` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.findChangeTargets` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.findSiblings` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.forget` | The next `update` builds the whole graph (a world reset: what read the graph forgot it all). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.inbound` | Inbound link lanelets, in deterministic order. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.junctionCache` | What each junction built, by the surface and lanelets it was built from. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.junctionConnectors` | @perInput The connectors of the junction at `node` ALONE, made as the whole graph's build makes them (`buildJunction`) | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.junctionInput` | What a junction's connectors are made of (`junctionBuild.ts`), read off the document and this graph's links | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.junctionInput.lane` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.junctionKeyOf` | The key each junction's cache entry is under (`buildJunction`): found by its node, never by walking the cache. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.junctions` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.junctionsOffPage` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.laneUsable` | Whether a vehicle of this shape may drive a lane (a bus lane is buses only). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.lanelet` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.laneletKeys` | The key each lanelet of this build was made under (`buildLinks`). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.lanelets` | The connector lanelets and the connectors, in creation order. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.letGo` | DO NOT gather every lanelet's key to find out: every edit read the whole map's. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.linkCache` | The link lanelets of each (segment, direction), by the geometry and trims they were built from | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.linksOfSegment` | DO NOT walk `lanelets` to find one road's lanes (the inspector did, every 0.4 s, for the whole map). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.linksOnly` | @rebuild The link lanelets of every segment ALONE (no junction), made as the whole graph's build makes them: what a tile's bays ba | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.netState` | The network's state this graph was last brought up to (`update`). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.outbound` | Link lanelets leaving each node. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.pending` | The junctions built on turns still to be chosen off the page, by node (`buildJunction`, `redoProvisional`). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.place` | A junction's movements (`junctionMovements`, here or in a worker) made this graph's | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.reach` | What a change of the network reaches (`update`, `adopt`): the segments to build again | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.redoProvisional` | Only those junctions: DO NOT build the graph again for them. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.reshape` | DO NOT build the whole junction again for its turns (its surface, its key, every movement) | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.revision` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.siblingLanes` | The other lanes of the same carriageway, adjacent ones first. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.siblings` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.takeKnown` | The junction at `nodeId` handed back whole from the cache when its key is known (a junction an edit did not reach: objects and all | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.takeOut` | DO NOT walk the junction cache to find them: every edit read every junction of the map. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.takeWhole` | Every link, junction and connector of `from`, and its caches, as this graph's own (`adopt` of a graph built whole). | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.takeWhole.copy` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.takeWhole.lists` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.turnPlan` | The turns of the next build chosen off the page (`SimWorld.prepareVehicleTopology`) | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.update` | DO NOT build the whole graph for an edit | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletGraph.updateSteps` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `LaneletId` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `PendingJunction` | A junction built on turns still to be chosen off the page (`TurnPlan`), kept until they are | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `TakenOut` | What `LaneletGraph.takeOut` took out, for `letGo`: the lanelets that went, the junctions' entries and keys, the links' keys. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `allowsDirection` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `clipToEndSections` | A lane's centre between the cross-sections at its two ends. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `drop` | `value` taken out of the list at `key`; the list gone when empty. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `laneletId` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `laneletWork` | The work of the lane graph's builds, counted (`tests/scale/edit.spec.ts`): segments and nodes looked at. | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| | `push` |  | portado | sim/traffic/lane_graph.cpp (DirSeg::cycle_shift) |
| `src/world/networkLength.ts` | **arquivo** | extensão da malha | já existia | LaneGraph::lane_metres |
| | `NetworkLength` | DO NOT sum every ribbon of the map at each revision: every road drawn walked the town's roads. | já existia | LaneGraph::lane_metres |
| | `NetworkLength.bySegment` |  | já existia | LaneGraph::lane_metres |
| | `NetworkLength.net` |  | já existia | LaneGraph::lane_metres |
| | `NetworkLength.of` |  | já existia | LaneGraph::lane_metres |
| | `NetworkLength.put` |  | já existia | LaneGraph::lane_metres |
| | `NetworkLength.seen` |  | já existia | LaneGraph::lane_metres |
| | `NetworkLength.sumAll` | @rebuild Every road summed again. | já existia | LaneGraph::lane_metres |
| | `NetworkLength.total` |  | já existia | LaneGraph::lane_metres |
| | `SCALE` | Lengths are summed in millionths of a unit, as integers: exact, in whatever order an edit adds and takes them. | já existia | LaneGraph::lane_metres |
| `src/world/parking.ts` | **arquivo** | tipos de estacionamento e folgas (CTB) | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `CORNER_CLEAR` | No bay within this distance of a junction's kerb line (CTB art. 181 I). | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `CROSSING_CLEAR` | No bay within this distance of a pedestrian crossing. | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `PARKING_DEPTH` | Depth of a parking lane from the kerb face, world units. | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `PARKING_KINDS` | A band by the kerb is parking (`parallel`) or a CYCLE LANE (`cycle`): the ciclofaixa of the Brazilian code | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `PARKING_PITCH` | Distance along the kerb between bay lines, world units. | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `ParkingKind` |  | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `SegmentParking` | Parking on the two sides of a segment, left and right of its a -> b direction. | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `flipParking` | The same parking seen from the other end of the segment. | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `isParkingKind` |  | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `normalizeParking` | Reads a stored value, or undefined when there is no parking at all. | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `parkingAllowed` | Roads without footways (highways, ramps) do not park at all; every street does. | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `readKind` | A stored kind: the kinds across the kerb an earlier build offered park parallel now. | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| | `sameParking` |  | portado | sim/roads/parking_layout.h e data/catalog/traffic.json |
| `src/world/parkingLayout.ts` | **arquivo** | vagas e pintura de cada rua | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `BAY_LINE` |  | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `CYCLE_RED` | The red of a ciclofaixa's paving. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `ENTRANCE_MARGIN` | Kept clear either side of a lot's frontage for its entrance. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `MER_DASH` | The MER's dashes along the parking lane: 1 m of paint, 1 m clear. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `NO_PARKING_LINE` |  | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `PARKING_PAINT` | Every colour the parking's lines are painted in (`render/markings.ts` `PAINT_COLOURS`). | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `ParkingBay` |  | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `ParkingLayout` |  | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `ParkingLine` |  | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `RUN_STEP` | Step of the paint along a run, so a line follows a curve. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `STRAIGHT` | A point of a run's line nearer than this to the line through its neighbours carries no shape: left out. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `WHITE` |  | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `YELLOW` |  | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `endClear` | How far from a node no bay may start: the corner clearance, or clear of the crossing. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `entrancesNear` | The entrances of the buildings within reach of a street (`frontagesOn`'s reach). | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `frontagesOn` | Intervals along `line` in front of a lot entrance on one side. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `gatesNear` | The entrances of `gates` (`gatesOf`) a street of half width `half` along `line` may read: those `entrancesNear` would hand it, for | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `parkingDigest` | DO NOT read anything in `parkingOf` without adding it here: what one street's bays are laid out from besides the entrances it is h | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `parkingLayout` | DO NOT hand every entrance of the map to every street: each street with parking tested every gate of the city. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `parkingLinesOf` | The paint of one street's parking (`parkingLayout`); `entrances`: the gates within its reach (`gatesNear`), read from the document | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `parkingOf` | One street's bays and lines, onto `bays` and `lines`. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `parksAlong` | Whether a street has parking laid out at all (`parkingOf`): only then do the buildings near it move its paint. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `stations` | Stations from `lo` to `hi`, one a metre and both ends. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| | `subtract` | `from` with every interval of `cut` removed. | portado | sim/roads/parking_layout.cpp e render/roads/road_markings.cpp |
| `src/world/roadSection.ts` | **arquivo** | seção | já existia | sim/roads/profiles.cpp |
| | `laneLineCrossable` |  | já existia | sim/roads/profiles.cpp |
| `src/world/roadTypes.ts` | **arquivo** | tipos de via | já existia | sim/roads/profiles.cpp |
| `src/world/roads/catalog.ts` | **arquivo** | catálogo | já existia | road_profiles.json |
| `src/world/roads/laneEnds.ts` | **arquivo** | pontas das faixas | já existia | sim/traffic/lane_geometry.cpp |
| `src/world/roads/profile.ts` | **arquivo** | perfil | já existia | sim/roads/profiles.cpp |
| `src/world/roads/rules.ts` | **arquivo** | configuração do semáforo do nó | portado | sim/roads/road_network.h SignalSettings |
| | `APPROACH_RULES` |  | portado | sim/roads/road_network.h SignalSettings |
| | `ApproachRule` | THE RULES OF A JUNCTION: who goes first, as the source; the signs are derived from it (V6). | portado | sim/roads/road_network.h SignalSettings |
| | `ApproachRuleEntry` |  | portado | sim/roads/road_network.h SignalSettings |
| | `ApproachSign` | The sign a leg's driver meets at a junction | portado | sim/roads/road_network.h SignalSettings |
| | `GREEN_LIMITS` |  | portado | sim/roads/road_network.h SignalSettings |
| | `OFFSET_LIMIT` |  | portado | sim/roads/road_network.h SignalSettings |
| | `SignalSettings` | A signal's settings; absent: adaptive, the game's own timings. | portado | sim/roads/road_network.h SignalSettings |
| | `approachSign` |  | portado | sim/roads/road_network.h SignalSettings |
| | `arrivesAt` | Whether traffic on a segment arrives at the node (not a one-way leaving it). | portado | sim/roads/road_network.h SignalSettings |
| | `isHighwayClass` | Whether a road is a "rodovia" for CTB art. 29, III, a (the highway class). | portado | sim/roads/road_network.h SignalSettings |
| | `legRule` | The rule of one leg of a junction with signs ('priority' control) | portado | sim/roads/road_network.h SignalSettings |
| | `mainRoadLegs` | The road a junction is ON: the pair of legs whose lesser rank is the highest, the straighter pair on a tie. | portado | sim/roads/road_network.h SignalSettings |
| | `normalizeApproachRules` |  | portado | sim/roads/road_network.h SignalSettings |
| | `normalizeSignalSettings` |  | portado | sim/roads/road_network.h SignalSettings |
| | `roadRank` | A road's rank at a junction: its authored priority, else its class (the order `admission.ts` uses). | portado | sim/roads/road_network.h SignalSettings |
| | `rulesDigest` |  | portado | sim/roads/road_network.h SignalSettings |
| | `signalDigest` |  | portado | sim/roads/road_network.h SignalSettings |
| `src/world/surfaceParts.ts` | **arquivo** | partes da superfície | já existia | render/roads/road_segment.cpp |
| | `surfaceParts` |  | já existia | render/roads/road_segment.cpp |
| | `surfaceParts.road` |  | já existia | render/roads/road_segment.cpp |
| `src/world/sweptBodies.ts` | **arquivo** | corpos varridos | já existia | sim/traffic/lane_graph.cpp conflict_at |
| | `Queue` | The car bodies standing in one waiting lane, and the box their centres span | já existia | sim/traffic/lane_graph.cpp conflict_at |
| | `Rect` |  | já existia | sim/traffic/lane_graph.cpp conflict_at |
| | `clearOfQueue` | Whether `body` is certainly clear of every body in `queue` by `overlap`'s own first test, the bounding circles. | já existia | sim/traffic/lane_graph.cpp conflict_at |
| | `overlap` | Separating-axis test between two oriented rectangles. | já existia | sim/traffic/lane_graph.cpp conflict_at |
| | `queueOf` | A queue of same-sized bodies (`r`, their bounding radius) and the box of their centres. | já existia | sim/traffic/lane_graph.cpp conflict_at |
| | `rect` |  | já existia | sim/traffic/lane_graph.cpp conflict_at |
| | `separatedOn` | Whether the axis (ax, ay) separates the two rectangles `dx, dy` apart. | já existia | sim/traffic/lane_graph.cpp conflict_at |
| `src/world/turnChoice.ts` | **arquivo** | escolha de conversão | já existia | sim/traffic/lane_graph.cpp |
| `src/world/turnDigest.ts` | **arquivo** | resumo para cache de worker | não se aplica | sem cache de worker |
| | `TurnDigest` | FNV-1a over numbers quantised to 1e-5, as two 32-bit lanes. | não se aplica | sem cache de worker |
| | `TurnDigest.a` |  | não se aplica | sem cache de worker |
| | `TurnDigest.add` |  | não se aplica | sem cache de worker |
| | `TurnDigest.b` |  | não se aplica | sem cache de worker |
| | `TurnDigest.mix` | Folds in another digest's two lanes. | não se aplica | sem cache de worker |
| | `TurnDigest.point` |  | não se aplica | sem cache de worker |
| | `TurnDigest.value` |  | não se aplica | sem cache de worker |
| `src/world/turnPaths.ts` | **arquivo** | caminho das conversões | já existia | sim/traffic/lane_geometry.cpp |
| | `ApproachPoses` | A body's positions on an approach lane, before its heading chord reaches the turn | já existia | sim/traffic/lane_geometry.cpp |
| | `Area` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `Box` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `CENTRE_TURN_RADIUS` | The tightest a body of each class turns, on its centreline: AASHTO's design vehicles | já existia | sim/traffic/lane_geometry.cpp |
| | `CORNER_ACROSS` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `CORNER_ALONG` | Where `sweep` checks a body's outline: its four corners and the middle of each side. | já existia | sim/traffic/lane_geometry.cpp |
| | `Candidates` | DO NOT build them per body size again. | já existia | sim/traffic/lane_geometry.cpp |
| | `Candidates.constructor` | The surface round `node`, from its pieces (`surfaceParts`; a worker's, the same pieces sent) | já existia | sim/traffic/lane_geometry.cpp |
| | `Candidates.handle` | The single cubic with this handle (`bezierTurn`). | já existia | sim/traffic/lane_geometry.cpp |
| | `Candidates.handles` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `Candidates.through` | The two cubics through waypoint `w` (of `WAYPOINTS`, `point` its place) at share `k`. | já existia | sim/traffic/lane_geometry.cpp |
| | `Candidates.waypoints` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `FALLBACK_HANDLE` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `FOLLOW_GAP` | The least gap between two bodies following one another on a turn. | já existia | sim/traffic/lane_geometry.cpp |
| | `FailHint` | Where, in its sequence of positions, the last candidate swept for a body left the surface (`sweep`); -1 before any did. | já existia | sim/traffic/lane_geometry.cpp |
| | `HANDLES` | Handle lengths tried, as a fraction of the chord, widest curve first. | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface` | The drivable surface around one node: its junction plate and its legs. | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.approachPoses` | DO NOT sweep them again per candidate | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.approaches` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.areas` | A ring's box and digest, kept with the ring's own point list. | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.centre` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.constructor` | The surface round `node`, from its pieces (`surfaceParts`; a worker's, the same pieces sent) | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.constructor.add` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.contains` | Whether any of its rings holds `p`. | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.empty` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.flatRings` | Its rings as flat x, y lists, in order: what a worker builds the same surface from. Each kept with its ring (`areaOf`): DO NOT fla | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.key` | Digest of every ring point, for `turnPath`'s memo. | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.lastHit` | The ring that held the last point `contains` found (an index into `areas`). | já existia | sim/traffic/lane_geometry.cpp |
| | `JunctionSurface.work` | What the sweeps over this surface work in (`sweep`). | já existia | sim/traffic/lane_geometry.cpp |
| | `KEY_STEP` | Spacing of the samples a lane's digest is taken from, world units. | já existia | sim/traffic/lane_geometry.cpp |
| | `LANE_REACH` | Reach of the sweep along the approach and exit: half the largest body plus the heading chord. | já existia | sim/traffic/lane_geometry.cpp |
| | `NO_ROAD_ENDS` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `Poly` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `QUEUE_REACH` | How far behind a stop line a waiting vehicle is looked for, world units. | já existia | sim/traffic/lane_geometry.cpp |
| | `RoadEnds` | Whether the arriving lane begins, or the departing lane ends, at the end of a road | já existia | sim/traffic/lane_geometry.cpp |
| | `SQUARER` | Tried only when even the fallback overruns. | já existia | sim/traffic/lane_geometry.cpp |
| | `STEPS` | Samples per turn path, cosine-spaced. | já existia | sim/traffic/lane_geometry.cpp |
| | `SWEEP_STEP` | Body-centre spacing of the containment sweep, world units. | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork` | What the sweeps over one surface work in, kept by the surface (`JunctionSurface.work`): the rectangles a sweep writes its position | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork.along` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork.behind` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork.behindAt` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork.poses` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork.probe` | Where the position probed first is written, read for nothing but whether it fits. | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork.rects` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork.take` | The next rectangle of this sweep. | já existia | sim/traffic/lane_geometry.cpp |
| | `SweepWork.taken` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `TurnPathResult` | The widest turn path whose swept body stays on the junction's surface. | já existia | sim/traffic/lane_geometry.cpp |
| | `WAYPOINTS` | The interior waypoints `chooseTurnFor` tries, as offsets from the node: a 5 x 5 grid 2 m apart, nearest the node first. | já existia | sim/traffic/lane_geometry.cpp |
| | `WAYPOINT_SHARES` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `WaitingLane` | Another inbound lane - of another approach, or the one beside this movement's own | já existia | sim/traffic/lane_geometry.cpp |
| | `approachPosesOf` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `approachPosesOf.frameAt` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `areaOf` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `areas` | A ring's box and digest, kept with the ring's own point list. | já existia | sim/traffic/lane_geometry.cpp |
| | `atFrame,behindFrame,aheadFrame` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `bezierTurn` | Cubic Bezier from the end of `inCentre` to the start of `outCentre`. | já existia | sim/traffic/lane_geometry.cpp |
| | `bulbTurnPath` | Turning round in a turning circle at a road's end (`junction/bulb.ts`) | já existia | sim/traffic/lane_geometry.cpp |
| | `bulbTurnPath.angleAt` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `bulbTurnPath.cubic` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `bulbTurnPath.on` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `bulbTurnPath.tangent` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `bulbTurnPath.wrap` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `chooseTurn` | The handle `turnPath` settles on, by sweeping the candidates. | já existia | sim/traffic/lane_geometry.cpp |
| | `chooseTurnFor` | The handle for one body size; null when no candidate keeps that body on the surface. | já existia | sim/traffic/lane_geometry.cpp |
| | `chooseTurnFor.swept` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `corner` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `inBox` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `insideArea` | Whether `p` is inside the area's ring (`insideBands`, its edges sorted into bands once). | já existia | sim/traffic/lane_geometry.cpp |
| | `laneEndKey` | Digest of a lane near its end (`atEnd`) or its start, over `reach`. | já existia | sim/traffic/lane_geometry.cpp |
| | `laneKeys` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `makeChoice` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `medianUturnPath` | A U-turn through an opening in the median (retorno) | já existia | sim/traffic/lane_geometry.cpp |
| | `poseAt` | The body at `c` as a rectangle of its own, kept (an approach's positions, `approachPosesOf`). | já existia | sim/traffic/lane_geometry.cpp |
| | `poseInto` | DO NOT make a rectangle and a heading a candidate position: a turn's search sweeps thousands (the allocation profile, 2026-10-17:  | já existia | sim/traffic/lane_geometry.cpp |
| | `setRect` | A rectangle's fields written as `rect` makes them. | já existia | sim/traffic/lane_geometry.cpp |
| | `surfaceKey` | A surface's key: digest of its centre and every ring point. | já existia | sim/traffic/lane_geometry.cpp |
| | `sweep` | DO NOT fold the three passes back into one loop | já existia | sim/traffic/lane_geometry.cpp |
| | `sweep.frameAt` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `turnFits` | Whether the selected movement contains a body of this size on its drawn surface. | já existia | sim/traffic/lane_geometry.cpp |
| | `turnPath` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `twoPieceTurn` | Two tangent-continuous cubics through a point inside the junction. | já existia | sim/traffic/lane_geometry.cpp |
| | `twoPieceTurn.cubic` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `unflat` |  | já existia | sim/traffic/lane_geometry.cpp |
| | `waitingBodies` | Car-sized bodies standing in a waiting lane, front at the line and further back. | já existia | sim/traffic/lane_geometry.cpp |
| `src/world/walkways.ts` | **arquivo** | caminhos a pé | já existia | sim/traffic/ped_routes.cpp |
| | `WalkGraph.changesSince` |  | já existia | sim/traffic/ped_routes.cpp |
| | `WalkGraph.holds` |  | já existia | sim/traffic/ped_routes.cpp |
| | `WalkGraph.none` |  | já existia | sim/traffic/ped_routes.cpp |
