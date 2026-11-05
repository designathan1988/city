# Cobertura do rastreamento: demolition

Fonte: `converter/out/features/demolition/` (721 declarações próprias, 35 arquivos com declaração própria, 16 entradas).
Totais: não se aplica 292, portado 260, não portado (pendência) 130, já existia 39.
Conferido com o branch `convert/demolition`: `evidence/cobertura_verifica.py` abre cada arquivo citado em "portado" e "já existia" e procura o símbolo.
Estado: portado (funcionalidade no jogo), já existia (o jogo já tinha), não se aplica (motivo) ou não portado (pendência: o que falta e por quê).
Painéis e abas que outro rastreamento lista por inteiro são da sessão dona: o motivo diz qual. A integração é da coordenadora.

## Por arquivo

| Arquivo | Decl. | O que faz para o jogador | Estados | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|---|
| `src/actionsWiring.ts` | 22 | Pistola, bomba e o fogo depois: o que cada golpe faz à cidade | portado 22 | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `src/core/intersect.ts` | 1 | Ponto mais perto num segmento | já existia 1 | `native/src/core/math.h` · `struct V2` |
| `src/core/vec2.ts` | 1 | Vetor 2D | já existia 1 | `native/src/core/math.h` · `struct V2` |
| `src/editor/bulldozer.ts` | 16 | Demolir: clique tira a coisa sob o ponteiro, arrasto tira tudo na caixa | portado 16 | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `src/editor/roads/streetKit.ts` | 1 | A via sai com a rede e o mobiliário | já existia 1 | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `src/main.ts` | 0 | Liga bomba, pistola e demolir ao ponteiro e ao crescimento | portado | `project/game/tools/strike_tool.gd` · `strike`; `project/game/tools/shoot_tool.gd` · `shoot`; `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `src/render/agents.ts` | 4 | Carcaça do veículo, ferimentos na roupa e sangue no chão | portado 4 | `native/src/render/effects/effect_things.cpp` · `EffectWorld::vehicle` |
| `src/render/blast.ts` | 41 | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não portado (pendência) 5, não se aplica 7, portado 29 | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `src/render/buildings/buildingMesh.ts` | 3 | Malha de um prédio para fraturar | não portado (pendência) 3 | o prédio que cai vira um monte de lajes (`effect_ruins.cpp`), não é fraturado em Voronoi com a casca e as janelas dele; falta ler o `b40::Shell` e cortá-lo em job do `JobPool` |
| `src/render/buildings/cellArrays.ts` | 1 | Atributos compactados do three.js | não se aplica 1 | só existe por causa do navegador ou do three.js |
| `src/render/buildings/fracture.ts` | 26 | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) 26 | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `src/render/buildings/fracture.worker.ts` | 1 | Fratura fora do quadro | não portado (pendência) 1 | depende da fratura Voronoi (pendência); o job iria no `JobPool` |
| `src/render/buildings/partPose.ts` | 1 | Matriz da peça instanciada do three.js | não se aplica 1 | só existe por causa do navegador ou do three.js |
| `src/render/buildings/shellParts.ts` | 1 | UV da casca compactada | não se aplica 1 | só existe por causa do navegador ou do three.js |
| `src/render/casualties.ts` | 14 | Manchas de sangue no chão | portado 14 | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `src/render/destruction.ts` | 22 | Prédio que cai: o que fica é um monte de peças sobre o lote | portado 22 | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `src/render/elements.ts` | 0 | Elementos de cenário do 4.0 que aparecem nos efeitos | não se aplica | sessão landscape (ports/landscape) |
| `src/render/exhaust.ts` | 10 | Fumaça, poeira, chama, faíscas, sangue, água e papel | portado 10 | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `src/render/gore.ts` | 19 | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) 17, portado 2 | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `src/render/playEffects.ts` | 42 | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | não se aplica 5, portado 37 | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `src/render/ragdoll.ts` | 154 | Corpos jogados, caídos, que levantam ou rastejam | já existia 4, não portado (pendência) 66, não se aplica 1, portado 83 | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `src/render/ragdollAxes.ts` | 9 | Eixos das juntas do corpo | não portado (pendência) 9 | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `src/render/ragdollGround.ts` | 10 | Altura do chão sob os corpos | já existia 10 | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `src/render/ragdollJolt.ts` | 58 | Ponte com o Jolt em WASM | não se aplica 58 | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `src/render/renderer.ts` | 0 | Porta de entrada dos efeitos na cena | portado | `native/src/render/render_world.cpp` · `effects_` |
| `src/sim/agents/walk.ts` | 220 | Motor inteiro dos pedestres | não portado (pendência) 3, não se aplica 203, portado 14 | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `src/sim/ambient/ambient.ts` | 0 | Carros estacionados e cenário | não se aplica | sessão traffic (ports/traffic) |
| `src/sim/crossings/permission.ts` | 8 | Permissão da travessia | não se aplica 8 | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |
| `src/sim/crossings/state.ts` | 1 | Estado da travessia | não se aplica 1 | já existe em `sim/traffic/ped_crossing.cpp` |
| `src/sim/host/protocol.ts` | 0 | Mensagens com o worker | não se aplica | só existe por causa do navegador ou do three.js: a simulação publica pelo `ChangeLog` |
| `src/sim/host/simClient.ts` | 3 | Cliente do worker | não se aplica 3 | só existe por causa do navegador ou do three.js: a simulação publica pelo `ChangeLog` |
| `src/sim/host/simHost.ts` | 0 | Hospedeiro do worker | não se aplica | só existe por causa do navegador ou do three.js: a simulação publica pelo `ChangeLog` |
| `src/sim/peds/sidewalk.ts` | 2 | Calçadas dos pedestres | já existia 2 | `native/src/sim/traffic/pedestrians.cpp` · `plan_trip` |
| `src/sim/peds/signalPostIndex.ts` | 9 | Índice dos postes de semáforo | já existia 9 | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `src/sim/people/casualties.ts` | 7 | Quem o golpe atingiu, para o desenho | portado 7 | `native/src/sim/demolition/damage_event.h` · `CASUALTY`; `native/src/sim/state/change_log.h` · `damage_events` |
| `src/sim/pose.ts` | 1 | Pose interpolada | já existia 1 | `native/src/sim/state/pose_buffer.h` · `struct Pose` |
| `src/sim/transit/transit.ts` | 3 | Transporte público | não se aplica 3 | sessão transit (ports/transit) |
| `src/sim/world.ts` | 4 | Mundo da simulação | já existia 4 | `native/src/sim/state/city_state.h` · `struct CityState` |
| `src/world/signalPosts.ts` | 2 | Onde ficam os postes de semáforo | já existia 2 | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `src/world/solids.ts` | 1 | Sólidos que o corpo encontra | já existia 1 | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `src/world/walkways.ts` | 3 | Caminhos a pé | já existia 3 | `native/src/sim/traffic/pedestrians.cpp` · `plan_trip` |

## Por declaração própria

### src/actionsWiring.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `Actions` | Pistola, bomba e o fogo depois: o que cada golpe faz à cidade | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `ActionsDeps` | THE ACTIONS (the dock's Actions: the pistol and the bomb, and the city on fire after) | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `FIRE_STEP` | How often the fires flare on, seconds of the simulation's time. | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `RAY_ABOVE` | How high over the ground under the pointer a building can reach: the tallest stack of the tallest storeys, its roof and plinth. | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `RAY_BELOW` | How far under the ground under the pointer a building can stand (one in a valley seen over a ridge). | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `VEHICLE_HALF_SPAN` | The farthest a vehicle's box reaches from its centre on the ground (a rider's shoulders over a two-wheeler's frame, `shootVehicle`). | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions` | Pistola, bomba e o fogo depois: o que cada golpe faz à cidade | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.breakDeferred` | The buildings a blow reached, four a frame (MDN: one `requestAnimationFrame` per frame, asked again while some wait). | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.breakOne` | Pistola, bomba e o fogo depois: o que cada golpe faz à cidade | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.buildingsUnderRay` | DO NOT walk `buildings.all()`: a blow on open ground ray-marched every building of the city. | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.explodeAt` | An explosion at `world`, height `z` (on building `b` when it landed on one). | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.explodeAt.near` | Pistola, bomba e o fogo depois: o que cada golpe faz à cidade | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.explodeAt.throwAboard` | Pistola, bomba e o fogo depois: o que cada golpe faz à cidade | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.ground` | Pistola, bomba e o fogo depois: o que cada golpe faz à cidade | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.ignite` | Buildings on fire the player's order of 2026-10-05: the fire spreading over the neighbouring buildings, as it happens | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.poseOf` | A vehicle where it stands at the end of the last tick (a copy: the frames move on). | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.rayOnBuilding` | The first point of building `b` along the screen ray through (sx, sy): marched down from its top. | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.scheduleBreak` | Pistola, bomba e o fogo depois: o que cada golpe faz à cidade | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.shootAt` | The pistol at a screen point; false when it hit nothing. | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.shootVehicle` | A shot along the line of sight from `a` (height `hi`) to `b` | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.strikeAt` | The bomb at a screen point (`world` the ground under it). | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |
| `createActions.tick` | The fires, at the simulation's time `now` (seconds, `SimClock.time`) | portado | `native/src/sim/commands/strike_commands.cpp` · `strike_at`; `native/src/sim/commands/shoot_commands.cpp` · `shoot_at`; `native/src/sim/commands/shoot_vehicles.cpp` · `shoot_vehicle` |

### src/core/intersect.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `closestOnSegment` | Ponto mais perto num segmento | já existia | `native/src/core/math.h` · `struct V2` |

### src/core/vec2.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `lenSq` | Vetor 2D | já existia | `native/src/core/math.h` · `struct V2` |

### src/editor/bulldozer.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `Bulldozer` | THE BULLDOZER: a click removes the one thing under the pointer | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.box` | The box on the map while it is dragged (no longer a click), for the overlay. | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.boxed` | Everything inside the box removed in one undo step: roads | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.boxed.inside` | Demolir: clique tira a coisa sob o ponteiro, arrasto tira tudo na caixa | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.cancel` | Demolir: clique tira a coisa sob o ponteiro, arrasto tira tudo na caixa | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.click` | @perInput The bulldozer's click: the one thing under the pointer. | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.constructor` | Demolir: clique tira a coisa sob o ponteiro, arrasto tira tudo na caixa | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.down` | A press at screen point `screen` over `world`, `anchor` what is under it. | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.drag` | The box being dragged (screen pixels in the canvas, and the ground under its corners), or the press while it stays a click. | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.gesture` | Demolir: clique tira a coisa sob o ponteiro, arrasto tira tudo na caixa | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.inProgress` | Demolir: clique tira a coisa sob o ponteiro, arrasto tira tudo na caixa | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.move` | @perInput The pointer moved; true when it was this tool's drag. | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `Bulldozer.up` | The pointer let go: the click or the box is carried out (`commit` false: dropped). | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `BulldozerHost` | What the bulldozer reads of the game and does to it. | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `CLICK_PIXELS` | A press that moved less than this many screen pixels is a click. | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |
| `removeRoads` | Roads taken away as the player demolishes them, by any tool: their power line and furniture with them, a share of their price back (`editor roads econ | portado | `native/src/sim/commands/demolish_box.cpp` · `demolish_box`; `project/game/tools/demolish_tool.gd` · `demolish_box` |

### src/editor/roads/streetKit.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `removeWithStreetKit` | A via sai com a rede e o mobiliário | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |

### src/render/agents.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `createAgentMeshes.carcass` | Carcaça do veículo, ferimentos na roupa e sangue no chão | portado | `native/src/render/effects/effect_things.cpp` · `EffectWorld::vehicle` |
| `createAgentMeshes.carcass.put` | Carcaça do veículo, ferimentos na roupa e sangue no chão | portado | `native/src/render/effects/effect_things.cpp` · `EffectWorld::vehicle` |
| `createAgentMeshes.carcass.wave` | Carcaça do veículo, ferimentos na roupa e sangue no chão | portado | `native/src/render/effects/effect_things.cpp` · `EffectWorld::vehicle` |
| `createAgentMeshes.setBleed` | Carcaça do veículo, ferimentos na roupa e sangue no chão | portado | `native/src/render/effects/effect_things.cpp` · `EffectWorld::vehicle` |

### src/render/blast.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `Blast` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `BlastWorld` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `COLORS` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `Crater` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/marks.cpp` · `Marks::` |
| `DebrisKind` | Explosions, and what they throw | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `DebrisSpec` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `GEYSER_SECONDS,GEYSER_FADE` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `GRAVITY` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `MAX_CRATERS` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/marks.cpp` · `Marks::` |
| `MAX_PIECES` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `MAX_SOOT` | Soot blots kept on the ground at once (oldest go first). | portado | `native/src/render/effects/marks.cpp` · `Marks::` |
| `MAX_WIRES` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não portado (pendência) | os fios soltos dos postes derrubados não balançam: o poste cai, os fios continuam do mapa |
| `Piece` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `Rope` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não portado (pendência) | os fios soltos dos postes derrubados não balançam: o poste cai, os fios continuam do mapa |
| `WIRE_POINTS` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não portado (pendência) | os fios soltos dos postes derrubados não balançam: o poste cai, os fios continuam do mapa |
| `craterTexture` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não se aplica | só existe por causa do navegador ou do three.js |
| `craterTexture.rnd` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não se aplica | só existe por causa do navegador ou do three.js |
| `createBlast` | `light`: the flash, made by the renderer and in the scene since it opened (`PlayEffectsContext.lights`). | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.active` | Whether anything is still moving, burning or flashing. | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.arc` | An electric discharge at a point, repeated for a while (a broken line on the ground). | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.burn` | Seconds of fire on it (a burning wreck), 0 for none. | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.carcassMaterial` | A burnt-out vehicle (as GTA leaves them) | não se aplica | só existe por causa do navegador ou do three.js |
| `createBlast.casting` | Whether anything that casts a shadow moves (debris and wrecks not yet at rest): fire, smoke, water, sparks and wires cast none (AGT-01). | não se aplica | só existe por causa do navegador ou do three.js |
| `createBlast.clear` | Everything gone: debris, wrecks, craters, soot, fires, wires, jets (the weapons lab's clean slate). | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.crater` | A crater (a hole and its broken rim) and the scorch round it. | portado | `native/src/render/effects/marks.cpp` · `Marks::` |
| `createBlast.dispose` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.dropObject` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createBlast.explode` | The explosion itself at world (x, y), height z: `radius` its reach; `ground` what it tears up. | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.flashAt` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.geyser` | A broken hydrant at world (x, y): a jet of water for `seconds` (`GEYSER_SECONDS` if not given), dying down at its end. | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.leak` | A burst pipe: a thin spurt of water for a moment, not a hydrant's jet. | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.piece` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createBlast.shake` | How hard the camera shakes now (world units). | portado | `project/game/camera/camera_shake.gd` · `effects_shake` |
| `createBlast.soot` | Soot and ash laid on the ground at world (x, y): the street left dirty by the blast, for good. | portado | `native/src/render/effects/marks.cpp` · `Marks::` |
| `createBlast.stepPiece` | One step of a rigid piece: gravity, then each corner below the ground struck with an impulse. | portado | `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createBlast.stepRope` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não portado (pendência) | os fios soltos dos postes derrubados não balançam: o poste cai, os fios continuam do mapa |
| `createBlast.update` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast` |
| `createBlast.wire` | A wire torn loose: pinned at `from` (world, three's frame), its loose end at `to`, thrown by `kick`. | não portado (pendência) | os fios soltos dos postes derrubados não balançam: o poste cai, os fios continuam do mapa |
| `spinQ` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não se aplica | só existe por causa do navegador ou do three.js |
| `tmp1,tmp2,tmp3,tmp4,tmp5` | Bola de fogo, onda, clarão, tremor, estilhaços, cratera, fuligem, fogo, hidrante | não se aplica | só existe por causa do navegador ou do three.js |
| `uploadWritten` | The instances written (`count`) to the GPU, not the whole buffer (1 400 pieces' worth) each frame one moves. | não se aplica | só existe por causa do navegador ou do three.js |

### src/render/buildings/buildingMesh.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `interiorFurniture` | Malha de um prédio para fraturar | não portado (pendência) | o prédio que cai vira um monte de lajes (`effect_ruins.cpp`), não é fraturado em Voronoi com a casca e as janelas dele; falta ler o `b40::Shell` e cortá-lo em job do `JobPool` |
| `shellPartFaces` | Malha de um prédio para fraturar | não portado (pendência) | o prédio que cai vira um monte de lajes (`effect_ruins.cpp`), não é fraturado em Voronoi com a casca e as janelas dele; falta ler o `b40::Shell` e cortá-lo em job do `JobPool` |
| `withShellParts` | Malha de um prédio para fraturar | não portado (pendência) | o prédio que cai vira um monte de lajes (`effect_ruins.cpp`), não é fraturado em Voronoi com a casca e as janelas dele; falta ler o `b40::Shell` e cortá-lo em job do `JobPool` |

### src/render/buildings/cellArrays.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `octDecode` | Atributos compactados do three.js | não se aplica | só existe por causa do navegador ou do three.js |

### src/render/buildings/fracture.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `BROKEN` | The grey of broken masonry, on the cut faces. | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `FRAGMENT` | Edge of a Voronoi cell: the size of a fragment. | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `FractureInput` | What a fracture needs, as plain arrays (so it can run in a worker, `fracture.worker.ts`). | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `Fragment` | What a ruin knows of each fragment besides its vertices (which are in the ruin's buffer, `StillData`). | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `FragmentData` | A fragment as arrays, vertices round its centre: gathered into its ruin's buffer (`mergeFragments`). | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `MAX_EDGE` | Longest triangle edge after subdivision. | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `MODEL_ARRAYS` | A kit model's vertices as plain arrays, made once per model. | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `STRIDE` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `StillData` | Every fragment of a building in one buffer, in world space (three's, y up), one draw group per material | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `Tri` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `WALL` | Thickness given to a wall fragment. | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `fractureData` | The fracture itself, pure arrays in and out: run in a worker. | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `fractureData.cellOf` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `fractureData.centroid` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `fractureData.push` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `fractureData.rand` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `fractureData.seedOf` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `fractureData.vkey` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `fragmentGeometry` | A fragment's own geometry, round its centre, from its ranges in the ruin's buffer as they stand | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `lerp` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `mergeFragments` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `modelArrays` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `prepareFracture` | The arrays of a building's chunk and kit a fracture reads, and the materials its fragments are drawn with. | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `prepareFracture.materialIndex` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `prepareFracture.model` | Prédio cortado em pedaços Voronoi com espessura | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |
| `subdivide` | Splits a triangle along its longest edge until every edge is under `max`. | não portado (pendência) | o jogo derruba o prédio em lajes; a fratura Voronoi da fachada (casca, janelas) fica como pendência |

### src/render/buildings/fracture.worker.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `serve` | This service of the job system (`job.worker.ts`): its messages answered on the scope it is given. | não portado (pendência) | depende da fratura Voronoi (pendência); o job iria no `JobPool` |

### src/render/buildings/partPose.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `poseMatrices` | Matriz da peça instanciada do three.js | não se aplica | só existe por causa do navegador ou do three.js |

### src/render/buildings/shellParts.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `shellUv` | UV da casca compactada | não se aplica | só existe por causa do navegador ou do three.js |

### src/render/casualties.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `BloodDecal` | A patch of blood on the ground: where, how big at full spread, and how long it takes to spread. | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `CasualtyLayer` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `MAX` | What a blow leaves on the ground (the player's order of 2026-10-05) | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `VARIANTS` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `Variant` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `bloodTexture` | Blood as it lies on a street: nearly black where it is deep, a dark crimson at thin edges | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `bloodTexture.blob` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `bloodTexture.deep` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `bloodTexture.drop` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `bloodTexture.rnd` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `bloodTexture.thin` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `createCasualties` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `createCasualties.dispose` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |
| `createCasualties.sync` | Manchas de sangue no chão | portado | `native/src/render/effects/marks.cpp` · `Marks::add_blood`; `project/render/shaders/marks.gdshader` · `void` |

### src/render/destruction.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `COLLAPSE_BELOW` | Share of the fragments left standing below which the rest comes down. | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `Destruction` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `GRAVITY` | Buildings that break for real (the player's order of 2026-10-05) | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `NO_GEOMETRY` | What a piece's mesh holds while it is not flying: nothing. | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `Piece` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `RELEASE_MIN` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `RELEASE_SLICE_MS` | Milliseconds a frame for cutting out and sending off the pieces of a blow (`release`), and the fewest a frame. | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `Ruin` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.blank` | A piece out of its ruin's still mesh: its triangles collapsed to a point, drawing nothing. | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.dispose` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.fail` | DO NOT keep it: its blows piled up and the building never broke. | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.hit` | A blow on building `b` (its floor at `floor`) at world (x, y, z), force 1..10. | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.land` | A piece landed, written into its ruin's still mesh where it lies. | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.loosen` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.make` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.place` | Static batching Unity's manual, Draw call batching: meshes that do not move and share a material, combined into one | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.release` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.strike` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.unsupported` | Every standing piece not joined to the ground through standing pieces. | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.update` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |
| `createDestruction.workerOf` | Prédio que cai: o que fica é um monte de peças sobre o lote | portado | `native/src/render/effects/effect_ruins.cpp` · `EffectWorld::pile` |

### src/render/exhaust.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `EXHAUST_LIFE` | Seconds an exhaust puff lives. | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `Exhaust` | Fumaça, poeira, chama, faíscas, sangue, água e papel | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `POOL` | Puffs alive at once, map-wide. | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `PuffKind` | 0 exhaust, 1 dust (brown-grey, low and wide), 2 dark smoke (rises), 3 concrete dust (pale, billowing), 4 blood spray | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `createExhaust` | Fumaça, poeira, chama, faíscas, sangue, água e papel | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `createExhaust.burst` | `count` puffs of `kind` at once, scattered over `spread` round (x, y, z) | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `createExhaust.dispose` | Fumaça, poeira, chama, faíscas, sangue, água e papel | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `createExhaust.emit` | A vehicle at (x, y) on a deck at height `z`, heading `angle`, `length` long, moving at `speed` (units s) | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `createExhaust.spawn` | Fumaça, poeira, chama, faíscas, sangue, água e papel | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |
| `createExhaust.tick` | Advances the clock the shader ages puffs by. | portado | `native/src/render/effects/puffs.cpp` · `Puffs::burst`; `project/render/shaders/puffs.gdshader` · `void vertex` |

### src/render/gore.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `Chain` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `Ends` | Three's frame (y up). | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `GRAVITY` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `Gore` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `LINKS` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `MAX_BONES` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `MAX_ORGANS` | Órgãos, tripas e ossos de corpos abertos | portado | `native/src/render/effects/effect_things.cpp` · `EffectWorld::casualty` |
| `MAX_SEGMENTS` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `Organ` | Órgãos, tripas e ossos de corpos abertos | portado | `native/src/render/effects/effect_things.cpp` · `EffectWorld::casualty` |
| `createGore` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `createGore.between` | A cylinder from `a` to `b`, `r` round. | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `createGore.bone` | A bone's end out of a stump: drawn from `ends()` (the joint, the tip) while it returns them. | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `createGore.clear` | All of it gone. | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `createGore.dispose` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `createGore.sameAsSent` | Whether a mesh's pieces are the ones it last sent (count, matrices, colours); if not, they are kept as sent now. | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `createGore.scatter` | Organs dropped from a point (a body torn open), thrown by `kick`. | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `createGore.spill` | Guts out of an opened belly: hanging from `anchor()` | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `createGore.update` | Órgãos, tripas e ossos de corpos abertos | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |
| `equalFloats` | Whether two arrays of floats hold the same numbers. | não portado (pendência) | a barriga aberta e as vísceras presas por Verlet não são desenhadas; um corpo despedaçado só lança órgãos soltos (`effect_things.cpp`) |

### src/render/playEffects.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `PlayEffects` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `PlayEffectsContext` | What shots and blows leave behind: the bodies they throw | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `TRACER_LIFE` | A tracer's life, s, and how many can be in the air at once (the oldest is taken again past it). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer` |
| `TRACER_SLOTS` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer` |
| `Tracers` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer` |
| `createPlayEffects` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.burn` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.busy` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.castersMoving` | Whether anything that casts a shadow moves: debris and wrecks not at rest, bodies falling (`busy` asks frames; this, the shadow cascades again, AGT-01 | não se aplica | sonda e ajustes do navegador |
| `createPlayEffects.clearCasualties` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.clock` | The effects' own seconds for a frame of `wallDt` (the weapons lab slows, stops and steps them). | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.dispose` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.dropVehicle` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_things.cpp` · `EffectWorld::vehicle` |
| `createPlayEffects.explode` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.explode.away` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.explode.ground` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.flingOccupants` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.forgetRuin` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | não se aplica | sonda e ajustes do navegador |
| `createPlayEffects.frame` | The bodies posed and drawn with the crowd's meshes (`AgentRenderOptions.ragdolls`). | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.geyser` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.groundChanged` | The heights found forgotten once the ground under them changed (an edit, a building put up or knocked down). | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.hides` | Somebody whose own body lies on the ground: not drawn standing as well. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.isDown` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.leak` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.lotsAt` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | não se aplica | sonda e ajustes do navegador |
| `createPlayEffects.onBuildingDown` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.ragdollProbe` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | não se aplica | sonda e ajustes do navegador |
| `createPlayEffects.setEffectsSpeed` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | não se aplica | sonda e ajustes do navegador |
| `createPlayEffects.shake` | How far an explosion shakes the camera now. | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.shootBody` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.shot` | A shot's line, world coordinates (x, y, height). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer` |
| `createPlayEffects.soot` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.sparkAt` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.stepEffects` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.strikeBuilding` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.update` | Debris, ruins, blood stains, tracers: after the agents. | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createPlayEffects.vehicleHit` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_things.cpp` · `EffectWorld::vehicle` |
| `createPlayEffects.wound` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_world.cpp` · `EffectWorld::sync`; `native/src/render/effects/effect_blast.cpp` · `EffectWorld::blast`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer`; `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body` |
| `createTracers` | THE TRACERS: one set of line segments made once, a slot a shot | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer` |
| `createTracers.dispose` | O que os golpes deixam: corpos, carcaças, tiros, estilhaços | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer` |
| `createTracers.shot` | A shot's line, world coordinates (x, y, height). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer` |
| `createTracers.update` | Debris, ruins, blood stains, tracers: after the agents. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::tracer` |

### src/render/ragdoll.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `AIR` | Speed kept per step in the air. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `ANKLE_UP,ANKLE_DOWN,ANKLE_SIDE` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `BEL` | In front of the pelvis: braces the torso against folding back and forth. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `Body` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `CAPSULE_BONES` | The bones that collide along their length (`capsules`), and where along each the body is tested. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `CAPSULE_SAMPLES` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `CRAWL_AFTER` | Seconds the gravely hurt lie writhing before they try to crawl off (face down). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `CRAWL_UP` | Seconds from lying face down to on hands and knees, ready to crawl. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `FALL_MOST` | The living: most seconds falling before they lie still, and the blend from lying into the get-up clip. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `FEET` | Each leg's knee, ankle and toe (`ankles`). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `Fate` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `GRAVITY` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `GROUND_SAMPLES,GROUND_SPACING` | Corpos jogados, caídos, que levantam ou rastejam | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `GROUND_TILE` | A tile's side, metres: its last samples are the next tile's first. | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `GetUp` | Somebody alive getting up at world (x, y) facing `heading`, taking `seconds` (`PeopleEngine.getUp`). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `GroundTile` | One tile of the physics' ground, the ground's serial it was made at, and the bodies on it. | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `ITERATIONS` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `JOINTS` | The skeleton's joints the particles sit on ('' for a particle with no bone of its own). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `JOLT` | Bodies simulated by Jolt (`ragdollJolt.ts`), not the stick figure | não se aplica | o jogo usa o passo rígido de `debris.h`, não o Jolt |
| `J_PAIRS` | Parts joined to each other (parent, child): they do not collide. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `J_PART` | The Jolt part each drawn part moves with. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `J_STUMP` | What each limb is torn from: the part left with the stump. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `J_TORSO,J_HEAD,J_LUA,J_LFA,J_RUA,J_RFA,J_LTH,J_LCA,J_LFO,J_RTH,J_RCA,J_RFO` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `KEY_PART` | Which part each key bone follows; a bone not listed follows its nearest listed ancestor. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `LH,LK,LA,LT,RH,RK,RA,RT` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `LIE` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `LIMBS` | What tears off a body right under a blow: the particles that go with it. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `LIMB_JOINTS` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `LOST_PARTS` | The parts gone with each limb shot off (as the living lose them, `riggedCitizens` maim). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `LS,LE,LW,RS,RE,RW` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `Limb` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `MAX_BODIES` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `METRE` | World units in a metre (Jolt works in metres). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `Occupant` | Somebody inside a building a blow struck: thrown out of it from where they were. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `PARTS` | Each part: the particle its frame sits on, the particle its long axis points to, what turns it about that axis. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `PART_NAMES` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `PART_PARENT` | Parents first (`PART_NAMES` follows this order): each part's parent part, for `bonesWorld`. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `PEL,CHE,NEC,HEA,TOP` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `PartName` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `RISE_BLEND` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `RISE_CLIP` | The get-up clip's length when it has not loaded yet. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `RISE_KEY` | The clip a body that fell gets up with. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `RagdollCitizens` | Ragdolls: people knocked over or killed by a blow, or tripping, their own bodies falling. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `RagdollProbe` | One body as the weapons lab measures it (`Ragdolls.probe`). Distances in metres. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `RagdollWall` | A wall a body can strike: a ring on the ground (world x, y), up to a height. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `RagdollWorld` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `Ragdolls` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `SETTLE` | Seconds of simulation after which a body is put to rest whatever it is doing. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `SINK` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `STEP` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `STEP_FACE` | Deeper than this under the middle of a bone: a bank beside it, not ground under it - left to its ends (`collide`). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `Stick` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `Survivor` | Somebody alive on the ground: falling, lying a moment, then getting up. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `TILE_SHIFT` | Tile indices shifted by this much into a tile's key: 2^20 tiles either way. | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `UP` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `WORLD_SIDE` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `WORLD_UP` | The axes a body's frame falls back on when its particles give none (three's space: y up). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `WRITHE_LEGS` | Each knee's pull while somebody writhes: knee, hip, phase. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `Wall` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `boneOn,riseSolved` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `boneWorld,riseTarget,riseWorld` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `clampAngle` | An angle brought within [lo, hi] (radians, the range under a turn): itself when inside, else the nearer end round the circle. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `createRagdolls` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.absorb` | Takes in the casualties of blows (each once): bodies thrown from the pose they were last drawn in. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.active` | Whether a body still moves or somebody alive is on the ground (`stats` moving or living), asked every frame. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.add` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.ankles` | Ankles: each foot kept within the reach a real ankle has, in its shin's frame | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.blast` | A blast's push on a body: away from it and up, the legs taken from under it, arms and legs scattered. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.bleed` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.boneOut` | A bone's end out of a stump: from joint `at`, along `from` to `at` and on, `len` long, while the body lasts. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.bonesWorld` | Every bone's world matrix, turned and placed from the particles: the shared scratch, read before the next body is posed. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.build` | A body standing as it was last drawn, at rest; null when that pose is not known. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.build.at` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.build.fold` | How close a limb's ends may come as its middle joint folds. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.build.len` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.build.range` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.build.rigid` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.capsules` | The bones as capped cylinders Jakobsen, Advanced Character Physics: each limb a capsule projected out of what it | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.captured` | Getting up as captured (CMU 140_01 face down, 140_08 face up): the clip's first frame, lying, laid where the body lies | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.captured.along` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.captured.ang` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.captured.rootAt` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.clear` | Every body, piece and stain gone (the weapons lab's clean slate). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.collide` | The ground, the walls and the roofs, as projections; speed into a surface is mostly lost, and some along it. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.colliders` | The ground under a body and round it (tiles shared with every body over them), and its walls | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.components` | Which piece each particle is on, from the sticks still whole. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.components.find` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.curl` | A body burnt black draws up as burnt bodies do | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.debug` | Each body's pelvis, phase and fate (for probes). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.detach` | A limb (or the head) torn off at a blow: a body of its own from here | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.draw` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.drip` | A drop of blood on the ground at world (x, y) (a trail behind somebody bleeding). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.faceUpOf` | Whether a body lies face up (its chest's front to the sky). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.fling` | People inside a struck building, thrown out of it - through its walls and windows - from the floor they were on. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.groundTile` | DO NOT make a heightfield per body: tile (tx, tz) of the physics' ground, taken by one more body (made when none has it). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.handsAndKnees` | On hands and knees over where the crouch will be | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.handsAndKnees.L` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.hides` | Whether somebody's own body is on the ground here, so the crowd does not draw them standing too. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.hinges` | Knees bend forwards only and elbows backwards only: one on the wrong side of its limb's line is pushed back across. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.joints` | Joint friction: across every bone (a rigid stick), the two ends' speeds drawn a little towards their mean | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.joltSpec` | A body as Jolt parts and joints, from where its particles are (metres) | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.joltSpec.R` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.joltSpec.capsule` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.joltSpec.cone` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.joltSpec.dir` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.joltSpec.hand` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.joltSpec.hinge` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.keptBones` | The bones as they are now, kept: the lying pose a get-up blends from (`Survivor.from`). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.keyPose` | The body at time `t` of its getting up from lying (`Survivor.keys`): key poses eased into one another, the body kept a body. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.launch` | Sets every particle moving: `v(k)` its velocity. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.localBone` | Bone `i`'s matrix in its parent's frame, of a list of world matrices. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.mush` | Shot to a heap of meat (`mush`). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.openBelly` | The belly opened: the guts hanging out of it, swinging with the body, some spilled on the ground. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.palettes` | The skin matrices of the body (one palette per piece) from its bones' world matrices, into its own palettes. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.partFrame` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.particlesOf` | The particles of the body in a pose read off its skeleton (a clip's palette under `root`). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.particlesOf.at` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.prepare` | A body into Jolt, or a change since its last step passed on: made the first time | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.probe` | Everything about each body for a test bench (the weapons lab): its particles | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.release` | Lets go of those who are up again once the simulation has them up too (`down` false). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.release@909` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.remove` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.rise` | Getting up: which way, from how the body lies | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.riseBone` | Bone `i` of a get-up blended from the lying pose into the clip's, its parent first (into `out`, marked in `riseSolved`). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.risePalette` | A getting-up body: the lying pose blended into the get-up clip's first frame, then the clip. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.settle` | The body's sticks (lengths, the ranges of the joints) satisfied by `p`, kept above the ground. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.sever` | What came off at this blow thrown off the body (`detach`), and the body burnt black right under a bomb. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.shootBody` | A shot along the line of sight from `a` to `b` (three's frame) at the bodies on the ground | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.shove` | A bullet's push: the part it went into knocked back from the shooter, the chest a little with it | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.spawn` | The body of a casualty of a blow. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.stats` | Bodies drawn, still moving, pieces, and the living among them (for probes). | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.step` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.sync` | The particles from the parts after a Jolt step; asleep when Jolt has put the parts to sleep, or they have lain still. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.torsoSide` | The torso's side axis (left to right), from the hips and the shoulders still on it. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.trip` | Somebody falling (a `fall` pause), from the pose they were last drawn in: tripping forwards, or knocked towards `away` | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.update` | Corpos jogados, caídos, que levantam ou rastejam | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.updateNormals` | Each limb's bend normal, carried on so it keeps its sign and survives the limb going straight. | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `createRagdolls.writhe` | Somebody down for good and hurt, where they lie (GTA's writhe: on the ground in pain, going nowhere) | portado | `native/src/render/effects/effect_bodies.cpp` · `EffectWorld::body`; `native/src/render/effects/debris.cpp` · `Debris::add`; `project/render/shaders/debris.gdshader` · `void vertex` |
| `edgePoint` | The nearest point of a ring's edge (`nearestEdge`): read before the next call. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `frameOf` | A frame with its x along `x` and its z as near `ref` as stays square to x. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `inside` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `matrices` | `list` with at least `n` matrices, the ones there kept. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `nearWalls` | The walls within reach of the body stepped now (`step`). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `nearestEdge` | The nearest point of a ring's edge, in the shared `edgePoint` (read it before the next call). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `partOrigin` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `partTurn` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `poseFrame,poseRest,poseBack,poseAnchor,poseSkin` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `poseSide,poseAxis,posePos,poseQ` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `riseA,riseB,riseOwn` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `riseP0,riseQ0,riseS0,riseP1,riseQ1,riseS1` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `smooth` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `stepVelocity` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `tmpA,tmpB,tmpC,tmpD,tmpE` | Corpos jogados, caídos, que levantam ou rastejam | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `warmPhysics` | Starts loading the physics the bodies fall with (Jolt): when the Actions are chosen, or the first time a body falls. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |

### src/render/ragdollAxes.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `EPS` | THE FRAMES OF A RAGDOLL'S JOINTS, ALWAYS VALID. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `across` | `v` with its part along the unit `axis` removed, unit; a perpendicular of `axis` when nothing is left. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `coneFrames` | A cone (swing-twist) joint's two frames: `twist2` along the bone (unit), its plane axis across it towards `side` | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `direction` | The direction from `a` to `b`, unit; `fallback` (unit) when they coincide. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `hingeFrame` | A hinge from the upper bone `t` and the lower bone `s` (both unit): the axis it bends about the right way | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `perpendicular` | A unit vector perpendicular to the unit vector `axis`. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `unitOr` | `v` normalized, or `fallback` (unit) when `v` has no length to speak of. Writes into `v`. | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `validFrame` | Whether `a` and `b` are unit and perpendicular, within what Jolt accepts (1e-4 on a normalization). | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |
| `validFrame.ok` | Eixos das juntas do corpo | não portado (pendência) | corpo de peças rígidas sem juntas (`effect_bodies.cpp`): as juntas, o Jolt, a captura de pose e o levantar animado ficam como pendência |

### src/render/ragdollGround.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `CORNERS_PER_UNIT` | Corners a world unit along each axis (a corner every 0.2 units). | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `GroundCorners` | Altura do chão sob os corpos | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `SHIFT` | Tile indices are shifted by this much into the key (`key`): maps up to 2^20 tiles across either way. | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `TILE` | Corners along a tile's side. | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `Tile` | Altura do chão sob os corpos | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `createGroundCorners` | DO NOT cap it and clear it whole: a cap thrown away mid-fall is found again corner by corner, every body at once | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `createGroundCorners.at` | Height at world (x, y): between its cell's four corners, as a physics engine's heightfield is between its samples. | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `createGroundCorners.forget` | Every height found forgotten: the ground under them changed. | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `createGroundCorners.height` | Altura do chão sob os corpos | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |
| `createGroundCorners.sweep` | Lets go of the tiles nobody read since the last sweep (once a frame): what still moves on the ground keeps its own. | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |

### src/render/ragdollJolt.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `JBody` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JBodyID` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JConstraint` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JointSpec` | A joint between two parts, at `at` (metres, world). | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltApi` | The bodies on the ground simulated by Jolt Physics | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll` | One body's parts in Jolt, and which part carries each of its particles. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.active` | Whether any part is still moving (Jolt has not put it to sleep). | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.carried` | How many particles part `i` carries. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.constructor` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.destroy` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.owner` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.partOf` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.point` | Where particle `k` is now (metres, world); null when no part carries it. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.pull` | A muscle's pull: particles `a` and `b` drawn towards each other by `impulse` (newton seconds) | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.push` | Particle `k`'s speed changed by `dv` (metres a second): an impulse on its part, at it, for its share of the part's mass. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.sleep` | Every part put to sleep (the body lies still; a push wakes it). | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.tear` | Parts torn off (`ragdoll.ts` detach): the joints holding them to the rest let go, and they go to a ragdoll of their own | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltRagdoll.velocity` | How fast particle `k` moves (metres a second, world). | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltStatic` | A static collider made for a body (its ground, the walls round it), released with it. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.J` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.active` | Whether any part is still moving (Jolt has not put it to sleep). | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.addStatic` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.bodies` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.constructor` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.deactivate` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.destroyBody` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.ground` | The ground under a square (metres, world): `n` x `n` samples `cell` apart from (x0, z0), heights from `height`. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.groups` | Parts next to each other in a ragdoll do not collide (subgroups by part index). | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.impulse` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.joint` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.jolt` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.live` | The joints in the physics now, by pointer: each is taken out exactly once (`removeConstraint`). | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.liveConstraints` | How many joints are in the physics now. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.mass` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.nextGroup` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.pointVelocity` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.position` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.prism` | A wall, a pole, a car: the ring (metres, world x and z) stood from `bottom` to `top`. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.r3` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.ragdoll` | A ragdoll from its spec, moving at rest (the particles' own velocities come in as pushes). | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.release` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.removeConstraint` | A joint out of the physics: refused, loudly, when it is not in it. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.rotation` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.separate` | Parts of a ragdoll that do not collide with each other (parent and child), set once per pair of part indices. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.step` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.system` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `JoltWorld.v3` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `LAYER_STATIC,LAYER_MOVING` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `MAX_PARTS` | Parts a ragdoll has (`RagdollSpec.parts`), for its collision groups. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `PartSpec` | A part: a capsule from `a` to `b` (metres, world), or a box; the particles it carries. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `RagdollSpec` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `api` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `joltWorld` | The physics world, once Jolt is loaded. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `loading` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `startJolt` | Starts loading Jolt (some 3 MB, once); `joltWorld` is null until it is in. | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `tmpQ,tmpV,tmpW,tmpM` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |
| `world` | Ponte com o Jolt em WASM | não se aplica | o corpo do jogo usa o passo rígido de `debris.h`, não o Jolt |

### src/sim/agents/walk.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `ACCEL` | Acceleration and braking of a walker, u s². | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ACROSS_PROJECTION` | `projectAcross`'s: read at once by its one caller in the walkers' step. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `AHEAD` | How far ahead on the route the body walks towards, u. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `AIM_FRAME` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `AIM_HOLD` | Seconds a stripe just taken is kept, unless it shuts. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `AIM_PROJECTION` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ALIGHT_SNAP` | How far from the door the walkway somebody gets out onto may be (`alight`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ALIGHT_TRIES` | Ways tried for somebody getting out of a car (`alight`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ASK_EVERY` | How often a waiting walker asks whether they may cross, seconds. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ASK_FROM` | A walker this far from the waiting place starts asking whether they may cross. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `BENDS` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `BEND_WINDOW` | Arc over which a line's bend is measured, u. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `BODY` | A body's radius: two people closer than twice this, side by side, touch. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `BODY_SHOCK` | Motor inteiro dos pedestres | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `BRAKE` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `CELL` | As wide as a walker looks ahead (`LOOK`) | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `CRAWL_PACE` | A crawl's speed, u s: the captured crawl's own (its feet and knees do not slide). | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `CROSSING_COST` | Extra cost of a crossing on a route, u: a zebra is walked to only when worth it. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `CarZone` | Ground a walker keeps off: a car's body (and the way it holds), or a train's. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `DOOR_CLEAR` | Somebody this near the spot a walker steps out onto keeps them inside a moment, u. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `FLINCH` | A first wound's reaction (`flinch`): struck, hunched over the wound standing, then on, wounded (seconds). | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `FOLD_SHARE` | Share of a bend's radius a walker may be inside it on a counter-bend: short of the fold. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `GAP_MARGIN` | Margin of clear road a walker wants beyond their own time across, s; after `IMPATIENT` waiting, none. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `GAP_NEAR` | A vehicle this near the crossing keeps a walker on the kerb whatever it is doing, u. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `GATE_JOIN` | Farthest a gate's foot may be from the walkway it is joined to. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `GRANT_LIFE` | Seconds a leave to cross holds for one standing still who has not yet stepped onto the carriageway. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `HAIL_REACH` | How far from a lane's centre somebody on the footway may be hailed from (`hailable`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `HEADWAY` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `HELD_SWITCH` | Held still this long, a quarter of that will do. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `IMPATIENT` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `INAPPROPRIATE` | Penalty of a stripe a walker must not take (SUMO's INAPPROPRIATE_PENALTY, as room ahead). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `JAM_AFTER` | Held still this long, a walker is let through others slowly (SUMO's jamtime: 10 s on a crossing). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `JAM_AFTER_CROSSING` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `JAM_AFTER_NARROW` | Where only one person fits abreast - a lot's path and its gate - SUMO's `pedestrian.striping.jamtime.narrow`, 1 s | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `JAM_NO_PROGRESS` | Face to face and getting no further along the way for this long, s | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `JAM_SHARE` | Speed of a jammed walker, as a share of their own. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `KEEP` | The gap a walker keeps to the body ahead, and the time headway behind it. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `KERB_BACK` | Where a walker stands to wait: this far back from the kerb. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `KNOCKED_MOST` | Knocked down by a light wound: the longest they can be down before they are up (`getUp` sets the real end). | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `LATERAL_PENALTY` | Room given up for each unit across from where the body is: SUMO's LATERAL_PENALTY, a metre a stripe. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `LATERAL_SHARE` | Most a walker moves across towards its stripe, as a share of its forward speed (SUMO's LATERAL_SPEED_FACTOR). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `LOOK` | How far ahead others are looked for. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `LOT_AHEAD` | How far ahead, at most, inside a lot and through its gate (`target`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `MOVING` | Faster than this, somebody is walking (coming the other way, say), u s. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `NEAREST` | Where `projectInto` reads the way's nearest point: made once. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `NO_POINTS` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ONCOMING_CONFLICT` | Penalty of the leftmost stripe while somebody comes the other way in it (SUMO's ONCOMING_CONFLICT_PENALTY, -1000). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `OTHERS` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `Other` | A body near the walker, in its own frame (`addOther`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `PANIC_RUN` | Running for their lives from a blow: a sprint. | portado | `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `PROGRESS` | Getting this much further along the way is progress (`JAM_NO_PROGRESS`), u. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `PROJECT_FRAME` | The step frames of the hot callers, each its own (`project` runs inside `stepWalkers`' use of its own). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedEntry` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedSource` | The parked cars as their keeper tells their changes (`ambient ambient.ts` AmbientWorld). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex` | DO NOT file every parked car again when one of them changes | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.byCar` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.cells` | The zones by cell, each in every cell its box reaches with a walker's look ahead round it. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.follow` | Brought up to the cars standing, from what their keeper says changed | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.keep` | A car filed where it stands, if it is not, or again if it moved. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.list` | What was seen, in the list's order. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.pass` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.place` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.serial` | The serial of the source last followed (`follow`); -1: never. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.sync` | Brings the cells up to `cars`, the cars standing now (`AmbientWorld.parked`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ParkedZoneIndex.unfile` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `QUEUE_GAP` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `QUEUE_REACH` | Behind somebody waiting for the zebra it is going to, this near its waiting place, a walker is in the queue | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `REACH` | Farthest a door or a car may be from the walkways, by default. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `RESERVE_MAX` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `RESERVE_ONCOMING` | Share of a corner's stripes, and most width, left to those coming the other way (SUMO's defaults). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ROOMS` | Each step's room, worked out once (it depends on the step alone) | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ROUTE_SCRATCH` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `RUN` | How much faster than their walk somebody runs away. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `RouteScratch` | DO NOT go back to `new Float64Array(n).fill(Infinity)` per search | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `SHOCK_LIFE` | A shocking event's life (seconds), and the reach of a body lying in the street. | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `SHOULDERS` | Two people this far apart side by side pass each other, u. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `SHUFFLE` | Slower than this a walker faces the way its walk goes and steps aside sideways | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `SHUFFLE_COS` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `SIDESTEP` | Fastest a body steps sideways, u s. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `SPAN` | The walker's span across its step, widened at a kerb or on a zebra: its own copy of the step's room. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `STAGGER_DISTANCE` | Motor inteiro dos pedestres | não portado (pendência) | o tropeço de um passo para trás quando a bala acerta não é animado (pedestre cozido sem esse clipe) |
| `STAGGER_FROM` | It starts after the hit itself (the captured jerk, `render agents.ts`). | não portado (pendência) | o tropeço de um passo para trás quando a bala acerta não é animado (pedestre cozido sem esse clipe) |
| `STAGGER_TIME` | Struck by the round: a stagger of a couple of steps back, the way it went | não portado (pendência) | o tropeço de um passo para trás quando a bala acerta não é animado (pedestre cozido sem esse clipe) |
| `STATES` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `STEP_PROJECTION` | The projections of the walkers' step and of `target`: made once (each read before the next is made). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `STRIPE` | Width of one stripe, world units: SUMO's default, a person's room abreast. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `SWITCH_GAIN` | A new stripe must be this much better to be taken: no dithering between two. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `State` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `Step` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `StepFrame` | A place on a step and its way ahead (`frameInto`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `TARGET` | `target`'s point: made once, read by the walkers' step before it is asked again. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `THERE` | The end of a walk is reached this near it. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `TRAFFIC_POSE` | Each vehicle's pose as `gapOpen` files it, written in place. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `TRAIN_GROUND` | The ground of a train (its cars, the track just ahead): whoever is on it walks off it. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `TURN_RATE` | Fastest a body turns walking, and standing (on the spot), rad s. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `TURN_STANDING` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WAIT_HALF` | Half the length of kerb people wait along at a zebra, u | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WALKER_SHAPE` | Every field a walker may be given later, there from its birth and in one order: added as things happened to each | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WALK_COMPONENTS` | Each node's component in the walk graph as it is (`WalkGraph.state`): labelled again when the graph changed and a route is asked. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WALK_FRAME` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WAY_CELL` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WAY_SAMPLE` | The way's frame `frameInto` samples into: made once. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WOUNDED_PACE` | A wounded walker's pace walking on, as a share of their own: slow, bent over the wound, never their own walk again. | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `WOUNDED_RUSH` | Getting away hurt, against their own pace: a run bent over the wound, as GTA's shot peds flee clutching it | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `WalkTrace` | A test bench's trace of what happens to chosen walkers (the weapons lab, `traceWalkers`) | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `Walker` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid` | The walkers out on the street by cell of `CELL`, as Matthias Müller's dense spatial hash files particles | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.build` | Files everybody out on the street (not inside, not done) where they are now. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.cx` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.cy` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.entries` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.list` | What was seen, in the list's order. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.size` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.slot` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.someIn` | Whether `fn` is true of anybody filed in the cells [gx0, gx1] x [gy0, gy1]; stops at the first. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.someNear` | Whether `fn` is true of anybody filed within `reach` cells of (x, y); stops at the first. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerGrid.start` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `WalkerSeen` | Somebody walking now, as the player and the police see them. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ZEBRAS` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ZONES` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ZONE_CELL` | Cells for the cars' zones (`zonesNear`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `Zebra` | A crossing on a walker's way: a zebra as the vehicles and the signals know it (`edge`), or a crossing with no zebra | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `actName` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `addOther` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `addPlayerWalker` | A body for the player to move, out at a point (stepping out of a door or a car). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `bendRadii` | The tightest radius of a walkway's line turning left and turning right (along it from `a` to `b`) | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `bendRadii.heading` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `cell` | A grid cell as one number: a string key | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `cellKey` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `clamp` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `createAgentWalkEngine` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `createAgentWalkEngine.walk` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `crossingOf` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `ensureGraph` | DO NOT index every walkway again when the network changes: every road drawn filed the town's walkways again. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `fileWay` | A walkway filed in (or out of) the cells its usable width reaches. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `fileZones` | Zones by cell: each in every cell its box reaches with a walker's look ahead round it. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `finish` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `frame` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `frameInto` | `frame` into `out`: the walkers' step asks it several times a tick for every walker | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `freeAhead` | Free distance ahead in a stripe centred `c` across the step. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `frighten` | Somebody who saw a fright at (x, y), `d` off: running away from it, some tripping, crouching or fainting. | portado | `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `gapOpen` | Whether a walker may step onto a crossing with no zebra | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `hypot` | Length of (x, y). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `insideDiscs` | Whether (x, y) is within `margin` of any of the first `count` discs. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `inspectAgentWalkers` | A read-only look at the walkers, for tests and diagnosis. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `joinFootway` | A lot's way (door ... gate, foot) joined to the footway | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `joinParty` | `p` filed with its party (`State.parties`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `joinsFootway` | Whether a lot's way out ending at `p` reaches a footway a walker can join there (`joinFootway`, within `GATE_JOIN`) | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `kerbInset` | How far a footway's walking line is from its kerb (where a crossing's end meets the road). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `lastOf` | The end of a walker's route: where they are going. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `legsLostOf` | How many legs a walker has lost (a hobble on one, no walking on none). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `lotSteps` | The straight stretches through a lot along `points` (a door to a gate's foot, or back). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `mourn` | Those walking with somebody shot (`startWalk` with): down beside them, crying, a few seconds, then away | portado | `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `movePlayerWalker` | The player's body moved: where it is now, the way it faces, how fast it goes. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `moved` | Whether a car stands otherwise than when it was filed (came, went, or moved). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `nearMoving` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `nearNow` | The walkers by cell for the cars' questions, filed at the first one after the walkers stepped (`State.near`) | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `nearParked` | The cars' zones by cell this tick (`stepWalkers`), and the ones in a walker's cell (`zonesNearInto`). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `nearestWay` | The walkway nearest a point within `reach` (crossings excluded: nobody starts on a zebra). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `newStepFrame` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `onLastStretch` | Whether somebody out on the street is on the last stretch of their walk, where `walkOn` takes them on. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `onZebra` | Whether a walker is out on a zebra's carriageway, kerb to kerb. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `othersN` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `parkedZone` | A car's body as a zone: three discs along its length, half its width round. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `plan` | The steps from a point to a point: to the walkways, along them, and off to the end. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `project` | A body projected on a step: how far along it, and how far left of it. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `projectAcross` | `project` of a body onto a step. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `projectInto` | `project` into `out` (every walker, several times a tick). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `prune` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `publish` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `queuedForZebra` | Whether the nearest body ahead in its own stripe (`d` across), near, stands waiting for `z`. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `releaseWalker` | Gives a body back from the player's hand, walking on to `to` (their home, say); null: off the street. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `removeWalker` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `replan` | A walker's route made again from where they are to `to`. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `reroute` | A walker's route found again to `to`, keeping what runs through a lot (`Step.lot`) | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `reserveCost` | What a stripe centred `c` costs for lying in the stripes kept for those coming the other way. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `reservedFor` | The width at the left of a step, in the way walked, left to those coming the other way: corners only. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `room` | The lateral room of a step in its own walking frame, [low, high], left positive, for a body's centre. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `roomOf` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `route` | DO NOT search without it: the scenery's people come in at the map's edge for the far side | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `route.distOf` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `route.estimate` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `route.push` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `routeScratch` | The graph's search arrays, on a new generation, the heap empty. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `rushName` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `seeWalker` | Another walker as `seer` sees it: one of its others if ahead within `LOOK`. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `seen` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `seer` | The walker looking round (`seeWalker`), and its frame. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `seerFrame` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `sendRunning` | Somebody sent running towards a point (a police officer after the player), their own route on the walkways. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `shock` | Motor inteiro dos pedestres | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `staggerSpeed` | The stagger's speed `t` seconds into a flinch: fast at first, slowing to a stop. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `startWalk` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `startle` | Everybody within `radius` of a fright | portado | `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `stateOf` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `stepLength` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `stepWalkers` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `stepWalkers.enter` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `takeWalker` | Puts somebody walking under the player's hand: they stop following their route. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `target` | The point the body walks towards: a little ahead on its route, in its stripe. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `trace` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `traceSink` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `traceTick` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `traceWalkers` | Traces these walkers' changes into `sink` (an empty list stops it). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `traced` | Motor inteiro dos pedestres | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkComponents` | @rebuild The graph's nodes labelled by their component (the ways joining them), once per state of the graph. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkOn` | On their last stretch, a walk goes on to somewhere else instead of ending | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkThrough` | A walk that begins by coming out through a lot | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkWork` | The work of the walkers' derived products, counted (counts, not times: the same on every machine), for the scale specs | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkerAct` | Somebody stops a while to do something (talk, fall down), facing a point. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkerAlive` | Whether a walker is still walking (out on the street, or waiting inside to step out). | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkerOf` | One walker, by id, or null when nobody of that id is out. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkerState` | One walker as a test bench reads them (the weapons lab) | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkersNear` | The walkers within `radius` of a point, out on the street. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `walkersWithin` | DO NOT walk `State.walkers` for a place: every walker of the map, for each blow and each look. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `wayCost` | The cost of walking a way: its length, and more for a zebra. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `woundOf` | How a shot walker is hurt, from their health and what struck them: none, light (most of their health left) or grave | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `woundPace` | Which of those a wound sets. | portado | `native/src/sim/commands/shoot_people.cpp` · `shoot_walker`; `native/src/sim/demolition/wounds.cpp` · `bleed_step`; `native/src/sim/traffic/ped_wounds.cpp` · `wounded_pace`; `native/src/sim/commands/strike_people.cpp` · `strike_people` |
| `zebraAhead` | The zebra a walker is coming up to and has not been let onto, and the distance along its route to where it waits for it | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |
| `zonesNearInto` | The zones in `p`'s cell, the standing cars' first, into `ZONES`; how many. | não se aplica | sessão people: o rastreamento people lista walk.ts por inteiro (ports/people); aqui só a parte dos feridos e das vítimas |

### src/sim/crossings/permission.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `alreadyReserved` | Permissão da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |
| `crossingReservedByVehicle` | Permissão da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |
| `heldOnlyByPedestrians` | Permissão da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |
| `indexReservations` | Permissão da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |
| `mayEnterCrossing` | Permissão da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |
| `pedGapAccepted` | Permissão da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |
| `turnOwedToVehicle` | Permissão da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |
| `vehicleBodyOnCrossing` | Permissão da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp`; o restante é da sessão people |

### src/sim/crossings/state.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `emptyCrossingState` | Estado da travessia | não se aplica | já existe em `sim/traffic/ped_crossing.cpp` |

### src/sim/host/simClient.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `SimClient.act` | Cliente do worker | não se aplica | só existe por causa do navegador ou do three.js: a simulação publica pelo `ChangeLog` |
| `SimClient.casualties` | Cliente do worker | não se aplica | só existe por causa do navegador ou do three.js: a simulação publica pelo `ChangeLog` |
| `SimClient.takeWounds` | Cliente do worker | não se aplica | só existe por causa do navegador ou do three.js: a simulação publica pelo `ChangeLog` |

### src/sim/peds/sidewalk.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `SidewalkGraph.crossingEdge` | Calçadas dos pedestres | já existia | `native/src/sim/traffic/pedestrians.cpp` · `plan_trip` |
| `SidewalkGraph.madeAt` | Calçadas dos pedestres | já existia | `native/src/sim/traffic/pedestrians.cpp` · `plan_trip` |

### src/sim/peds/signalPostIndex.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `SignalPostIndex.file` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `SignalPostIndex.follow` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `SignalPostIndex.found` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `SignalPostIndex.lift` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `SignalPostIndex.near` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `SignalPostIndex.placeAll` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `SignalPostIndex.placeJunction` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `SignalPostWorld` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `cellOf` | Índice dos postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |

### src/sim/people/casualties.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `Casualty` | Somebody a blow killed or knocked down: who they were (for their body) and the blast that threw them. | portado | `native/src/sim/demolition/damage_event.h` · `CASUALTY`; `native/src/sim/state/change_log.h` · `damage_events` |
| `RECORDS` | Quem o golpe atingiu, para o desenho | portado | `native/src/sim/demolition/damage_event.h` · `CASUALTY`; `native/src/sim/state/change_log.h` · `damage_events` |
| `WOUNDS` | Quem o golpe atingiu, para o desenho | portado | `native/src/sim/demolition/damage_event.h` · `CASUALTY`; `native/src/sim/state/change_log.h` · `damage_events` |
| `impactCasualties` | The casualties of blows on the map; their clocks advanced by `dt`, a minute's worth kept. | portado | `native/src/sim/demolition/damage_event.h` · `CASUALTY`; `native/src/sim/state/change_log.h` · `damage_events` |
| `recordCasualty` | Records somebody struck by a blow. | portado | `native/src/sim/demolition/damage_event.h` · `CASUALTY`; `native/src/sim/state/change_log.h` · `damage_events` |
| `recordWound` | A bullet wound on somebody (alive or not): their clothes holed and bloodied by the renderer. | portado | `native/src/sim/demolition/damage_event.h` · `CASUALTY`; `native/src/sim/state/change_log.h` · `damage_events` |
| `takeWounds` | The wounds recorded since the last call (`recordWound`), taken. | portado | `native/src/sim/demolition/damage_event.h` · `CASUALTY`; `native/src/sim/state/change_log.h` · `damage_events` |

### src/sim/pose.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `vehiclePoseNowInto` | Pose interpolada | já existia | `native/src/sim/state/pose_buffer.h` · `struct Pose` |

### src/sim/transit/transit.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `TransitSim.trainZones` | Transporte público | não se aplica | sessão transit (ports/transit) |
| `TransitSim.trainZones.far` | Transporte público | não se aplica | sessão transit (ports/transit) |
| `TransitSim.trainZones.zone` | Transporte público | não se aplica | sessão transit (ports/transit) |

### src/sim/world.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `SimWorld.nextPersonId` | Mundo da simulação | já existia | `native/src/sim/state/city_state.h` · `struct CityState` |
| `SimWorld.signalAtCrossing` | Mundo da simulação | já existia | `native/src/sim/state/city_state.h` · `struct CityState` |
| `SimWorld.someVehicleNear` | Mundo da simulação | já existia | `native/src/sim/state/city_state.h` · `struct CityState` |
| `crossingNode` | Mundo da simulação | já existia | `native/src/sim/state/city_state.h` · `struct CityState` |

### src/world/signalPosts.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `SignalJunctions` | Onde ficam os postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |
| `signalPosts` | Onde ficam os postes de semáforo | já existia | `native/src/sim/roads/street_furniture.cpp` · `street_furniture` |

### src/world/solids.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `solidsOf` | Sólidos que o corpo encontra | já existia | `native/src/sim/terrain/heightfield.h` · `height_at` |

### src/world/walkways.ts

| Declaração | O que faz | Estado | Dono no jogo (arquivo · símbolo) / motivo |
|---|---|---|---|
| `WalkGraph.changesSince` | Caminhos a pé | já existia | `native/src/sim/traffic/pedestrians.cpp` · `plan_trip` |
| `WalkGraph.holds` | Caminhos a pé | já existia | `native/src/sim/traffic/pedestrians.cpp` · `plan_trip` |
| `WalkGraph.none` | Caminhos a pé | já existia | `native/src/sim/traffic/pedestrians.cpp` · `plan_trip` |

