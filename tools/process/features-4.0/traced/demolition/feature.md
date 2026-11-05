# Feature: src/editor/bulldozer.ts, src/actionsWiring.ts, src/render/playEffects.ts, src/render/blast.ts, src/render/destruction.ts, src/render/gore.ts, src/render/casualties.ts, src/render/ragdoll.ts, src/render/ragdollAxes.ts, src/render/ragdollGround.ts, src/render/ragdollJolt.ts, src/render/buildings/fracture.ts, src/render/buildings/fracture.worker.ts, src/sim/people/casualties.ts, src/render/exhaust.ts, src/sim/agents/walk.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 721 declarations
- Slices of shared code: 40
- Code: 9272 lines in 41 files (source/)
- Resources: 21 (16 entry, 5 style, 0 import; 10 only the feature uses)
- Uses: 392 shared declarations (interface/: 613 declarations in 159 files, the types they name included); hooks: 375; external APIs: 502

## Code

- src/actionsWiring.ts — whole, 476 lines
- src/core/intersect.ts — part, 16 lines
- src/core/vec2.ts — part, 1 lines
- src/editor/bulldozer.ts — whole, 151 lines
- src/editor/roads/streetKit.ts — part, 9 lines
- src/main.ts — part, 106 lines
- src/render/agents.ts — part, 154 lines
- src/render/blast.ts — whole, 758 lines
- src/render/buildings/buildingMesh.ts — part, 86 lines
- src/render/buildings/cellArrays.ts — part, 7 lines
- src/render/buildings/fracture.ts — whole, 418 lines
- src/render/buildings/fracture.worker.ts — whole, 20 lines
- src/render/buildings/partPose.ts — part, 5 lines
- src/render/buildings/shellParts.ts — part, 12 lines
- src/render/casualties.ts — whole, 165 lines
- src/render/destruction.ts — whole, 410 lines
- src/render/elements.ts — part, 22 lines
- src/render/exhaust.ts — whole, 240 lines
- src/render/gore.ts — whole, 259 lines
- src/render/playEffects.ts — whole, 577 lines
- src/render/ragdoll.ts — whole, 1981 lines
- src/render/ragdollAxes.ts — whole, 61 lines
- src/render/ragdollGround.ts — whole, 73 lines
- src/render/ragdollJolt.ts — whole, 399 lines
- src/render/renderer.ts — part, 100 lines
- src/sim/agents/walk.ts — whole, 2276 lines
- src/sim/ambient/ambient.ts — part, 62 lines
- src/sim/crossings/permission.ts — part, 111 lines
- src/sim/crossings/state.ts — part, 3 lines
- src/sim/host/protocol.ts — part, 12 lines
- src/sim/host/simClient.ts — part, 18 lines
- src/sim/host/simHost.ts — part, 17 lines
- src/sim/peds/sidewalk.ts — part, 7 lines
- src/sim/peds/signalPostIndex.ts — part, 71 lines
- src/sim/people/casualties.ts — whole, 76 lines
- src/sim/pose.ts — part, 10 lines
- src/sim/transit/transit.ts — part, 39 lines
- src/sim/world.ts — part, 22 lines
- src/world/signalPosts.ts — part, 27 lines
- src/world/solids.ts — part, 6 lines
- src/world/walkways.ts — part, 9 lines

## Slices of shared code

- `src/main.ts#actions` — 16 of 16 lines
- `src/main.ts#bulldozer` — 10 of 10 lines
- `src/main.ts#cancelGestures` — 3 of 13 lines
- `src/main.ts#currentGesture` — 4 of 18 lines
- `src/main.ts#drawOverlayScreen` — 26 of 469 lines
- `src/main.ts#endPointer` — 3 of 30 lines
- `src/main.ts#frame` — 3 of 102 lines
- `src/main.ts#gameKeys` — 5 of 77 lines
- `src/main.ts#gestureInProgress` — 3 of 3 lines
- `src/main.ts#growZones` — 3 of 10 lines
- `src/main.ts#pointerDown` — 13 of 180 lines
- `src/main.ts#pointerMove` — 3 of 72 lines
- `src/main.ts#showInspector` — 14 of 246 lines
- `src/render/agents.ts#AgentRenderOptions` — 3 of 30 lines
- `src/render/agents.ts#createAgentMeshes` — 75 of 2045 lines
- `src/render/agents.ts#createAgentMeshes.procPose` — 17 of 124 lines
- `src/render/elements.ts#EFFECTS` — 9 of 9 lines
- `src/render/elements.ts#ElementLayer` — 3 of 11 lines
- `src/render/elements.ts#createElementLayer.give` — 10 of 10 lines
- `src/render/renderer.ts#SceneHandle` — 4 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 62 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.loadEffects` — 17 of 18 lines
- `src/render/renderer.ts#createSceneRenderer.movingCasters` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.company` — 7 of 17 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking` — 15 of 20 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.people` — 16 of 40 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace` — 3 of 8 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.reset` — 3 of 34 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.step` — 3 of 41 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings` — 7 of 25 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.walkFrom` — 8 of 36 lines
- `src/sim/host/protocol.ts#FromSim` — 12 of 12 lines
- `src/sim/host/simClient.ts#SimClient.casualtyList` — 1 of 1 lines
- `src/sim/host/simClient.ts#SimClient.readTick` — 3 of 160 lines
- `src/sim/host/simHost.ts#SimHost.casualtiesSent` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.frame` — 6 of 39 lines
- `src/sim/host/simHost.ts#SimHost.init` — 4 of 32 lines
- `src/sim/host/simHost.ts#SimHost.publish` — 6 of 80 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.reset` — 3 of 5 lines

## Resources

- feature entry `hint.shoot.miss` — src/ui/i18n/en.ts:31: `'hint.shoot.miss': 'Nobody there: click a person.'`
- shared entry `hint.lot.deleted` — src/ui/i18n/en.ts:40: `'hint.lot.deleted': 'Deleted.'`
- shared entry `hint.streetscape.removed` — src/ui/i18n/en.ts:335: `'hint.streetscape.removed': 'Item removed'`
- shared entry `hint.pole.removed` — src/ui/i18n/en.ts:906: `'hint.pole.removed': 'Pole removed'`
- feature entry `hint.strike.hit` — src/ui/i18n/en.ts:908: `'hint.strike.hit': 'Hit: keep striking to bring it down'`
- feature entry `hint.strike.down` — src/ui/i18n/en.ts:909: `'hint.strike.down': 'The building came down'`
- feature entry `hint.strike.deaths` — src/ui/i18n/en.ts:910: `'hint.strike.deaths': 'Hit: there were casualties'`
- feature entry `hint.strike.ground` — src/ui/i18n/en.ts:911: `'hint.strike.ground': 'Struck the ground'`
- feature entry `hint.shoot.miss` — src/ui/i18n/pt-BR.ts:31: `'hint.shoot.miss': 'Ninguém aí: clique numa pessoa.'`
- shared entry `hint.lot.deleted` — src/ui/i18n/pt-BR.ts:40: `'hint.lot.deleted': 'Apagado.'`
- shared entry `hint.streetscape.removed` — src/ui/i18n/pt-BR.ts:333: `'hint.streetscape.removed': 'Item removido'`
- shared entry `hint.pole.removed` — src/ui/i18n/pt-BR.ts:899: `'hint.pole.removed': 'Poste removido'`
- feature entry `hint.strike.hit` — src/ui/i18n/pt-BR.ts:901: `'hint.strike.hit': 'Atingido: continue batendo para derrubar'`
- feature entry `hint.strike.down` — src/ui/i18n/pt-BR.ts:902: `'hint.strike.down': 'Construção desabou'`
- feature entry `hint.strike.deaths` — src/ui/i18n/pt-BR.ts:903: `'hint.strike.deaths': 'Atingido: houve vítimas'`
- feature entry `hint.strike.ground` — src/ui/i18n/pt-BR.ts:904: `'hint.strike.ground': 'Impacto no chão'`
- shared style `side` — src/peopleLab/peopleLab.css:142: `.side { background: var(--panel); border-left: 1px solid var(--line); display: flex; flex-direction: column; padding: 14px; gap: 8px; }`
- shared style `side` — src/peopleLab/peopleLab.css:143: `.side h2 { margin: 0 0 2px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `
- shared style `xs-label` — src/ui/roads/roads.css:61: `.xs-label { fill: #e8f0ee; font: 600 12.5px Inter, ui-sans-serif, sans-serif; font-variant-numeric: tabular-nums; }`
- shared style `xs-glyph` — src/ui/roads/roads.css:63: `.xs-glyph { fill: #f2efe6; font: 800 13px Inter, ui-sans-serif, sans-serif; }`

## Where the program plugs the feature in

### src/actionsWiring.ts

- `src/actionsWiring.ts#Actions` (interface, line 57) ← call from `src/main.ts#frame`, `src/main.ts#pointerDown`
- `src/actionsWiring.ts#createActions` (function, line 68) ← call from `src/main.ts#actions`
- `src/actionsWiring.ts#createActions.shootAt` (function, line 89) ← implementation from `src/main.ts#pointerDown`
- `src/actionsWiring.ts#createActions.strikeAt` (function, line 213) ← implementation from `src/main.ts#pointerDown`
- `src/actionsWiring.ts#createActions.tick` (function, line 422) ← implementation from `src/main.ts#frame`

### src/editor/bulldozer.ts

- `src/editor/bulldozer.ts#Bulldozer` (class, line 42) ← construct from `src/main.ts#bulldozer`
- `src/editor/bulldozer.ts#Bulldozer.box` (method, line 87) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/bulldozer.ts#Bulldozer.cancel` (method, line 56) ← call from `src/main.ts#cancelGestures`
- `src/editor/bulldozer.ts#Bulldozer.down` (method, line 61) ← call from `src/main.ts#pointerDown`
- `src/editor/bulldozer.ts#Bulldozer.gesture` (method, line 48) ← call from `src/main.ts#currentGesture`
- `src/editor/bulldozer.ts#Bulldozer.inProgress` (method, line 52) ← call from `src/main.ts#gestureInProgress`
- `src/editor/bulldozer.ts#Bulldozer.move` (method, line 66) ← call from `src/main.ts#pointerMove`
- `src/editor/bulldozer.ts#Bulldozer.up` (method, line 75) ← call from `src/main.ts#endPointer`
- `src/editor/bulldozer.ts#removeRoads` (function, line 28) ← call from `src/main.ts#gameKeys`, `src/main.ts#showInspector`

### src/render/buildings/fracture.worker.ts

- `src/render/buildings/fracture.worker.ts#serve` (function, line 5) ← callbackAssign from `src/bays.worker.ts#serve`, `src/cityGen.worker.ts#serve`, `src/conflictTable.worker.ts#serve`, `src/editor/save.worker.ts#serve`, …
- `src/render/buildings/fracture.worker.ts#serve` (function, line 5) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/render/exhaust.ts

- `src/render/exhaust.ts#createExhaust` (function, line 33) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#createExhaust.burst` (method, line 219) ← implementation from `src/render/elements.ts#createElementLayer.give`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#createExhaust.dispose` (method, line 234) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#createExhaust.emit` (method, line 204) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#createExhaust.tick` (method, line 226) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#Exhaust` (interface, line 22) ← call from `src/render/elements.ts#createElementLayer.give`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#Exhaust` (interface, line 22) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/exhaust.ts#Exhaust` (interface, line 22) ← typeRef from `src/render/elements.ts#createElementLayer.give`, `src/render/elements.ts#ElementLayer`
- `src/render/exhaust.ts#PuffKind` (type, line 20) ← typeRef from `src/render/elements.ts#EFFECTS`

### src/render/playEffects.ts

- `src/render/playEffects.ts#createPlayEffects` (function, line 160) ← call from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/playEffects.ts#createPlayEffects` (function, line 160) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/playEffects.ts#createPlayEffects.burn` (method, line 556) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.busy` (method, line 351) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.castersMoving` (method, line 352) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.movingCasters`
- `src/render/playEffects.ts#createPlayEffects.clearCasualties` (method, line 359) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.clock` (method, line 308) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.dispose` (method, line 567) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.dropVehicle` (method, line 373) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.explode` (method, line 413) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.flingOccupants` (method, line 562) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.forgetRuin` (method, line 563) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.frame` (method, line 315) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.geyser` (method, line 558) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.hides` (method, line 341) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.leak` (method, line 560) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.onBuildingDown` (method, line 561) ← implementation from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/playEffects.ts#createPlayEffects.ragdollProbe` (method, line 358) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.setEffectsSpeed` (method, line 313) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.shake` (method, line 350) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.shootBody` (method, line 353) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.shot` (method, line 399) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.soot` (method, line 557) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.sparkAt` (method, line 559) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.stepEffects` (method, line 314) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.strikeBuilding` (method, line 406) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.update` (method, line 342) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.vehicleHit` (method, line 360) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createPlayEffects.wound` (method, line 387) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#createTracers` (function, line 107) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/playEffects.ts#PlayEffects` (interface, line 58) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.loadEffects`, `src/render/renderer.ts#createSceneRenderer.movingCasters`
- `src/render/playEffects.ts#PlayEffects` (interface, line 58) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/playEffects.ts#PlayEffects` (interface, line 58) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/playEffects.ts#PlayEffectsContext` (interface, line 40) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/playEffects.ts#TRACER_LIFE` (variable, line 95) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/playEffects.ts#TRACER_SLOTS` (variable, line 96) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/playEffects.ts#Tracers` (interface, line 98) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer.loadEffects`

### src/render/ragdoll.ts

- `src/render/ragdoll.ts#AIR` (variable, line 140) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#ANKLE_UP,ANKLE_DOWN,ANKLE_SIDE` (variable, line 117) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#BEL` (variable, line 85) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#Body` (interface, line 222) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#boneOn,riseSolved` (variable, line 328) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#boneWorld,riseTarget,riseWorld` (variable, line 327) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#CAPSULE_BONES` (variable, line 127) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#CAPSULE_SAMPLES` (variable, line 131) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#clampAngle` (function, line 299) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#CRAWL_AFTER` (variable, line 156) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#CRAWL_UP` (variable, line 158) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#createRagdolls` (function, line 401) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#edgePoint` (variable, line 335) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#FALL_MOST` (variable, line 148) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#Fate` (type, line 183) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#FEET` (variable, line 115) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#frameOf` (function, line 310) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#GetUp` (type, line 72) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#GRAVITY` (variable, line 136) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#GROUND_SAMPLES,GROUND_SPACING` (variable, line 190) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#GROUND_TILE` (variable, line 192) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#GroundTile` (interface, line 196) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#inside` (function, line 1958) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#ITERATIONS` (variable, line 138) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#J_PAIRS` (variable, line 178) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#J_PART` (variable, line 171) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#J_STUMP` (variable, line 176) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#J_TORSO,J_HEAD,J_LUA,J_LFA,J_RUA,J_RFA,J_LTH,J_LCA,J_LFO,J_RTH,J_RCA,J_RFO` (variable, line 169) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#JOINTS` (variable, line 75) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#JOLT` (variable, line 160) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#KEY_PART` (variable, line 90) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#LH,LK,LA,LT,RH,RK,RA,RT` (variable, line 83) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#LIE` (variable, line 143) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#Limb` (type, line 88) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#LIMB_JOINTS` (variable, line 113) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#LIMBS` (variable, line 134) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#LOST_PARTS` (variable, line 122) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#LS,LE,LW,RS,RE,RW` (variable, line 82) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#matrices` (function, line 340) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#MAX_BODIES` (variable, line 154) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#METRE` (variable, line 167) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#nearestEdge` (function, line 1968) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#nearWalls` (variable, line 333) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#Occupant` (interface, line 36) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#Occupant` (interface, line 36) ← typeRef from `src/render/renderer.ts#SceneHandle`
- `src/render/ragdoll.ts#PART_NAMES` (variable, line 112) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#PART_PARENT` (variable, line 108) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#PartName` (type, line 87) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#partOrigin` (variable, line 325) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#PARTS` (variable, line 98) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#partTurn` (variable, line 324) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#PEL,CHE,NEC,HEA,TOP` (variable, line 81) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#poseFrame,poseRest,poseBack,poseAnchor,poseSkin` (variable, line 323) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#poseSide,poseAxis,posePos,poseQ` (variable, line 322) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#RagdollCitizens` (interface, line 14) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#RagdollCitizens` (interface, line 14) ← typeRef from `src/render/agents.ts#AgentRenderOptions`, `src/render/agents.ts#createAgentMeshes`
- `src/render/ragdoll.ts#RagdollProbe` (interface, line 346) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#RagdollProbe` (interface, line 346) ← typeRef from `src/render/renderer.ts#SceneHandle`
- `src/render/ragdoll.ts#Ragdolls` (interface, line 368) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#RagdollWall` (interface, line 54) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#RagdollWorld` (interface, line 62) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#RISE_BLEND` (variable, line 149) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#RISE_CLIP` (variable, line 151) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#RISE_KEY` (variable, line 153) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#riseA,riseB,riseOwn` (variable, line 329) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#riseP0,riseQ0,riseS0,riseP1,riseQ1,riseS1` (variable, line 330) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#SETTLE` (variable, line 146) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#SINK` (variable, line 144) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#smooth` (function, line 319) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#STEP` (variable, line 137) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#STEP_FACE` (variable, line 119) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#stepVelocity` (variable, line 331) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#Stick` (interface, line 185) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#Survivor` (interface, line 199) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#TILE_SHIFT` (variable, line 194) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#tmpA,tmpB,tmpC,tmpD,tmpE` (variable, line 293) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#UP` (variable, line 307) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#Wall` (interface, line 187) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#warmPhysics` (function, line 163) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#warmPhysics` (function, line 163) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#WORLD_SIDE` (variable, line 296) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#WORLD_UP` (variable, line 295) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ragdoll.ts#WRITHE_LEGS` (variable, line 337) ← moduleLoad from `src/render/renderer.ts#createSceneRenderer`

### src/sim/agents/walk.ts

- `src/sim/agents/walk.ts#ACCEL` (variable, line 39) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ACROSS_PROJECTION` (variable, line 723) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#actName` (function, line 409) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#addOther` (function, line 1451) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#addPlayerWalker` (function, line 2024) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#AHEAD` (variable, line 61) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#AIM_FRAME` (variable, line 685) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#AIM_HOLD` (variable, line 44) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#AIM_PROJECTION` (variable, line 734) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ALIGHT_SNAP` (variable, line 913) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ALIGHT_TRIES` (variable, line 915) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ASK_EVERY` (variable, line 85) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ASK_FROM` (variable, line 77) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BEND_WINDOW` (variable, line 796) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#bendRadii` (function, line 799) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BENDS` (variable, line 797) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BODY` (variable, line 26) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BODY_SHOCK` (variable, line 450) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BRAKE` (variable, line 40) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#CarZone` (interface, line 102) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#cell` (function, line 473) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#CELL` (variable, line 96) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#cellKey` (function, line 474) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#clamp` (function, line 825) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#CRAWL_PACE` (variable, line 434) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#createAgentWalkEngine` (function, line 917) ← call from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#createAgentWalkEngine` (function, line 917) ← implementation from `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo`, `src/sim/ambient/ambient.ts#AmbientWorld.across`, `src/sim/ambient/ambient.ts#AmbientWorld.company`, …
- `src/sim/agents/walk.ts#createAgentWalkEngine` (function, line 917) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#CROSSING_COST` (variable, line 73) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#crossingOf` (function, line 836) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#DOOR_CLEAR` (variable, line 91) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ensureGraph` (function, line 479) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#fileWay` (function, line 498) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#fileZones` (function, line 1354) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#finish` (function, line 1299) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#FLINCH` (variable, line 432) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#FOLD_SHARE` (variable, line 780) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#frame` (function, line 688) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#frameInto` (function, line 693) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#freeAhead` (function, line 1476) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#frighten` (function, line 2148) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#GAP_MARGIN` (variable, line 853) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#GAP_NEAR` (variable, line 856) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#gapOpen` (function, line 859) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#GATE_JOIN` (variable, line 1225) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#GRANT_LIFE` (variable, line 87) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#HAIL_REACH` (variable, line 911) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#HEADWAY` (variable, line 37) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#HELD_SWITCH` (variable, line 46) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#hypot` (function, line 471) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#IMPATIENT` (variable, line 854) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#INAPPROPRIATE` (variable, line 785) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#insideDiscs` (function, line 1504) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#inspectAgentWalkers` (function, line 1962) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_AFTER` (variable, line 50) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_AFTER_CROSSING` (variable, line 51) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_AFTER_NARROW` (variable, line 53) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_NO_PROGRESS` (variable, line 55) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_SHARE` (variable, line 59) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#joinFootway` (function, line 1228) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#joinParty` (function, line 1294) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#joinsFootway` (function, line 2090) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace`
- `src/sim/agents/walk.ts#joinsFootway` (function, line 2090) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#KEEP` (variable, line 36) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#KERB_BACK` (variable, line 75) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#kerbInset` (function, line 828) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#KNOCKED_MOST` (variable, line 436) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#lastOf` (function, line 2216) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#LATERAL_PENALTY` (variable, line 48) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#LATERAL_SHARE` (variable, line 65) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#legsLostOf` (function, line 1528) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#LOOK` (variable, line 32) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#LOT_AHEAD` (variable, line 63) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#lotSteps` (function, line 1213) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#mourn` (function, line 2135) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#moved` (function, line 256) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#movePlayerWalker` (function, line 2049) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#MOVING` (variable, line 34) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#NEAREST` (variable, line 731) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#nearestWay` (function, line 517) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#nearMoving` (variable, line 1515) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#nearNow` (function, line 1437) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#nearParked` (variable, line 1514) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#newStepFrame` (function, line 680) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#NO_POINTS` (variable, line 109) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ONCOMING_CONFLICT` (variable, line 787) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#onLastStretch` (function, line 2083) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`
- `src/sim/agents/walk.ts#onLastStretch` (function, line 2083) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#onZebra` (function, line 900) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#Other` (interface, line 1448) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#OTHERS` (variable, line 1449) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#othersN` (variable, line 1450) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#PANIC_RUN` (variable, line 2118) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedEntry` (interface, line 124) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedSource` (interface, line 127) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#parkedZone` (function, line 112) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex` (class, line 139) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.byCar` (field, line 144) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.cells` (field, line 141) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.follow` (method, line 150) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.keep` (method, line 179) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.list` (field, line 143) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.pass` (field, line 145) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.place` (method, line 222) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.serial` (field, line 147) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.sync` (method, line 189) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.unfile` (method, line 238) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#plan` (function, line 1195) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#PROGRESS` (variable, line 57) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#project` (function, line 726) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#PROJECT_FRAME` (variable, line 684) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#projectAcross` (function, line 719) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#projectInto` (function, line 737) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#prune` (function, line 1323) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#publish` (function, line 1884) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#QUEUE_GAP` (variable, line 83) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#QUEUE_REACH` (variable, line 79) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#queuedForZebra` (function, line 1488) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#REACH` (variable, line 89) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#releaseWalker` (function, line 2251) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#removeWalker` (function, line 2096) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.people`, `src/sim/ambient/ambient.ts#AmbientWorld.reset`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/agents/walk.ts#removeWalker` (function, line 2096) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#replan` (function, line 2223) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#reroute` (function, line 2228) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#RESERVE_MAX` (variable, line 783) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#RESERVE_ONCOMING` (variable, line 782) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#reserveCost` (function, line 1499) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#reservedFor` (function, line 789) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#room` (function, line 754) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#roomOf` (function, line 762) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ROOMS` (variable, line 760) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#route` (function, line 544) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ROUTE_SCRATCH` (variable, line 654) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#routeScratch` (function, line 656) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#RouteScratch` (interface, line 653) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#RUN` (variable, line 2116) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#rushName` (function, line 410) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#seen` (function, line 1995) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#seer` (variable, line 1459) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#seerFrame` (variable, line 1460) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#seeWalker` (function, line 1462) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#sendRunning` (function, line 2207) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#shock` (function, line 452) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SHOCK_LIFE` (variable, line 449) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SHOULDERS` (variable, line 28) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SHUFFLE` (variable, line 67) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SHUFFLE_COS` (variable, line 68) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SIDESTEP` (variable, line 30) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SPAN` (variable, line 1536) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STAGGER_DISTANCE` (variable, line 441) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STAGGER_FROM` (variable, line 440) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STAGGER_FROM` (variable, line 440) ← read from `src/render/agents.ts#createAgentMeshes.procPose`
- `src/sim/agents/walk.ts#STAGGER_TIME` (variable, line 438) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STAGGER_TIME` (variable, line 438) ← read from `src/render/agents.ts#createAgentMeshes.procPose`
- `src/sim/agents/walk.ts#staggerSpeed` (function, line 443) ← call from `src/render/agents.ts#createAgentMeshes.procPose`
- `src/sim/agents/walk.ts#staggerSpeed` (function, line 443) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#startle` (function, line 2121) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#startWalk` (function, line 1237) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#State` (interface, line 355) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#stateOf` (function, line 459) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STATES` (variable, line 458) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#Step` (interface, line 259) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STEP_PROJECTION` (variable, line 733) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#StepFrame` (interface, line 679) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#stepLength` (function, line 675) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#stepWalkers` (function, line 1538) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STRIPE` (variable, line 24) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SWITCH_GAIN` (variable, line 42) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#takeWalker` (function, line 2010) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#target` (function, line 1328) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TARGET` (variable, line 1349) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#THERE` (variable, line 93) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#trace` (function, line 411) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#traced` (variable, line 400) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#traceSink` (variable, line 401) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#traceTick` (variable, line 402) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#traceWalkers` (function, line 404) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TRAFFIC_POSE` (variable, line 850) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TRAIN_GROUND` (variable, line 2113) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TURN_RATE` (variable, line 70) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TURN_STANDING` (variable, line 71) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WAIT_HALF` (variable, line 81) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WALK_COMPONENTS` (variable, line 624) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WALK_FRAME` (variable, line 686) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkComponents` (function, line 626) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#Walker` (interface, line 274) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WALKER_SHAPE` (variable, line 349) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerAct` (function, line 2103) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerAlive` (function, line 2077) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.step`
- `src/sim/agents/walk.ts#walkerAlive` (function, line 2077) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid` (class, line 1369) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.build` (method, line 1382) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.cx` (field, line 1371) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.cy` (field, line 1372) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.entries` (field, line 1374) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.list` (field, line 1370) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.size` (field, line 1375) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.slot` (method, line 1377) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.someIn` (method, line 1421) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.someNear` (method, line 1415) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.start` (field, line 1373) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerOf` (function, line 2004) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, `src/sim/ambient/ambient.ts#AmbientWorld.people`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/agents/walk.ts#walkerOf` (function, line 2004) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerSeen` (interface, line 1983) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerSeen` (interface, line 1983) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.people`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/agents/walk.ts#walkersNear` (function, line 1999) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerState` (function, line 2261) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkersWithin` (function, line 1311) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkOn` (function, line 2058) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, `src/sim/ambient/ambient.ts#AmbientWorld.people`
- `src/sim/agents/walk.ts#walkOn` (function, line 2058) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkThrough` (function, line 1220) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.company`, `src/sim/ambient/ambient.ts#AmbientWorld.walkFrom`
- `src/sim/agents/walk.ts#walkThrough` (function, line 1220) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkTrace` (interface, line 389) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkWork` (variable, line 122) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WAY_CELL` (variable, line 97) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WAY_SAMPLE` (variable, line 682) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#wayCost` (function, line 541) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WOUNDED_PACE` (variable, line 423) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WOUNDED_RUSH` (variable, line 425) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#woundOf` (function, line 418) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#woundPace` (function, line 427) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#Zebra` (interface, line 834) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#zebraAhead` (function, line 885) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ZEBRAS` (variable, line 848) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ZONE_CELL` (variable, line 99) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ZONES` (variable, line 1516) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#zonesNearInto` (function, line 1518) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`

### src/sim/people/casualties.ts

- `src/sim/people/casualties.ts#Casualty` (interface, line 8) ← typeRef from `src/sim/host/protocol.ts#FromSim`, `src/sim/host/simClient.ts#SimClient.casualtyList`, `src/sim/host/simHost.ts#SimHost.casualtiesSent`
- `src/sim/people/casualties.ts#impactCasualties` (function, line 69) ← call from `src/sim/host/simHost.ts#SimHost.frame`, `src/sim/host/simHost.ts#SimHost.publish`
- `src/sim/people/casualties.ts#takeWounds` (function, line 53) ← call from `src/sim/host/simHost.ts#SimHost.publish`

## Shared declarations the feature uses

### src/bays.worker.ts

- `src/bays.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/cityGen.worker.ts

- `src/cityGen.worker.ts#serve` (function, line 19) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/conflictTable.worker.ts

- `src/conflictTable.worker.ts#serve` (function, line 13) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 2: `src/sim/agents/walk.ts#fileWay`, `src/sim/agents/walk.ts#gapOpen`

### src/core/heap.ts

- `src/core/heap.ts#Heap` (class, line 3) — construct by 1: `src/sim/agents/walk.ts#routeScratch`
- `src/core/heap.ts#Heap` (class, line 3) — typeRef by 1: `src/sim/agents/walk.ts#RouteScratch`
- `src/core/heap.ts#Heap.clear` (method, line 13) — call by 1: `src/sim/agents/walk.ts#routeScratch`
- `src/core/heap.ts#Heap.peekKey` (method, line 12) — call by 1: `src/sim/agents/walk.ts#route`
- `src/core/heap.ts#Heap.pop` (method, line 24) — call by 1: `src/sim/agents/walk.ts#route`
- `src/core/heap.ts#Heap.push` (method, line 14) — call by 1: `src/sim/agents/walk.ts#route.push`
- `src/core/heap.ts#Heap.size` (getter, line 10) — call by 1: `src/sim/agents/walk.ts#route`

### src/core/polygon.ts

- `src/core/polygon.ts#pointInPolygon` (function, line 40) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 5: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#joinFootway`, `src/sim/agents/walk.ts#plan`, …
- `src/core/polyline.ts#FrameOut` (interface, line 23) — read by 2: `src/sim/agents/walk.ts#frameInto`, `src/sim/pose.ts#vehiclePoseNowInto`
- `src/core/polyline.ts#newFrame` (function, line 33) — call by 1: `src/sim/agents/walk.ts#WAY_SAMPLE`
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 2: `src/sim/agents/walk.ts#fileWay`, `src/sim/agents/walk.ts#gapOpen`
- `src/core/polyline.ts#Polyline.closestInto` (method, line 347) — call by 1: `src/sim/agents/walk.ts#projectInto`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 4: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/agents/walk.ts#nearestWay`, `src/sim/agents/walk.ts#publish`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 4: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/agents/walk.ts#nearestWay`, `src/sim/agents/walk.ts#publish`
- `src/core/polyline.ts#Polyline.cum` (getter, line 81) — call by 2: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 5: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/agents/walk.ts#route`, …
- `src/core/polyline.ts#Polyline.n` (field, line 40) — read by 3: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#frameInto`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 5: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#joinFootway`, `src/sim/agents/walk.ts#plan`, …
- `src/core/polyline.ts#Polyline.sampleAtInto` (method, line 157) — call by 1: `src/sim/agents/walk.ts#frameInto`
- `src/core/polyline.ts#Polyline.tan` (getter, line 96) — call by 1: `src/sim/agents/walk.ts#frameInto`
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 2: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`

### src/core/rng.ts

- `src/core/rng.ts#Rng.float` (method, line 57) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`

### src/core/scalar.ts

- `src/core/scalar.ts#EPS` (variable, line 6) — read by 1: `src/core/intersect.ts#closestOnSegment`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 109: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.explodeAt.throwAboard`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`, …
- `src/core/units.ts#stepsIn` (function, line 15) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/core/units.ts#TUNED_UNIT` (variable, line 12) — read by 4: `src/render/playEffects.ts#createPlayEffects.shot`, `src/render/blast.ts#createBlast.update`, `src/render/exhaust.ts#createExhaust`, `src/render/exhaust.ts#createExhaust.emit`

### src/core/vec2.ts

- `src/core/vec2.ts#add` (function, line 9) — call by 1: `src/core/intersect.ts#closestOnSegment`
- `src/core/vec2.ts#dot` (function, line 14) — call by 1: `src/core/intersect.ts#closestOnSegment`
- `src/core/vec2.ts#scale` (function, line 11) — call by 1: `src/core/intersect.ts#closestOnSegment`
- `src/core/vec2.ts#sub` (function, line 10) — call by 1: `src/core/intersect.ts#closestOnSegment`
- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 28: `src/editor/bulldozer.ts#Bulldozer.box`, `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.boxed.inside`, `src/editor/bulldozer.ts#Bulldozer.up`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 33: `src/editor/bulldozer.ts#Bulldozer.box`, `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.boxed.inside`, `src/editor/bulldozer.ts#Bulldozer.click`, …

### src/editor/editStream.ts

- `src/editor/editStream.ts#editRng` (function, line 5) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`

### src/editor/roads/economy.ts

- `src/editor/roads/economy.ts#roadsBefore` (function, line 11) — call by 1: `src/editor/bulldozer.ts#removeRoads`
- `src/editor/roads/economy.ts#settleRoadEdit` (function, line 16) — call by 1: `src/editor/bulldozer.ts#removeRoads`

### src/editor/roads/streetKit.ts

- `src/editor/roads/streetKit.ts#Kit` (interface, line 21) — read by 1: `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/editor/roads/streetKit.ts#kitOf` (function, line 41) — call by 1: `src/editor/roads/streetKit.ts#removeWithStreetKit`

### src/editor/save.worker.ts

- `src/editor/save.worker.ts#serve` (function, line 12) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/editor/snap.ts

- `src/editor/snap.ts#Anchor` (interface, line 16) — read by 1: `src/editor/bulldozer.ts#Bulldozer.click`
- `src/editor/snap.ts#Anchor` (interface, line 16) — typeRef by 3: `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/bulldozer.ts#Bulldozer.down`, `src/editor/bulldozer.ts#Bulldozer.drag`
- `src/editor/snap.ts#ribbonsNear` (function, line 74) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/growth.worker.ts

- `src/growth.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/job.worker.ts

- `src/job.worker.ts#<ExpressionStatement>` (statement, line 64) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`
- `src/job.worker.ts#<ExpressionStatement>` (statement, line 64) — implementation by 1: `src/render/buildings/fracture.worker.ts#serve`
- `src/job.worker.ts#adopt` (function, line 40) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`
- `src/job.worker.ts#adopt` (function, line 40) — implementation by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/jobs/jobPool.ts

- `src/jobs/jobPool.ts#jobPort` (function, line 669) — call by 1: `src/render/destruction.ts#createDestruction.workerOf`

### src/jobs/jobScope.ts

- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — typeRef by 1: `src/render/buildings/fracture.worker.ts#serve`
- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — write by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/main.ts

- `src/main.ts#actions` (variable, line 4259) — implementation by 9: `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.explodeAt`, …
- `src/main.ts#bulldozer` (variable, line 1105) — implementation by 4: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/bulldozer.ts#Bulldozer.move`, `src/editor/bulldozer.ts#Bulldozer.up`

### src/networkBake.worker.ts

- `src/networkBake.worker.ts#serve` (function, line 32) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/agents.ts

- `src/render/agents.ts#AgentMeshes` (interface, line 168) — call by 4: `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.dispose`, `src/render/playEffects.ts#createPlayEffects.dropVehicle`, `src/render/playEffects.ts#createPlayEffects.explode`
- `src/render/agents.ts#AgentMeshes` (interface, line 168) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#AgentMeshes` (interface, line 168) — typeRef by 1: `src/render/playEffects.ts#PlayEffectsContext`
- `src/render/agents.ts#BodyPlan` (interface, line 379) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#CarParts` (interface, line 621) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#createAgentMeshes` (function, line 552) — implementation by 12: `src/render/playEffects.ts#createPlayEffects.frame`, `src/render/ragdoll.ts#createRagdolls.build`, `src/render/ragdoll.ts#createRagdolls.captured`, `src/render/ragdoll.ts#createRagdolls.detach`, …
- `src/render/agents.ts#createAgentMeshes` (function, line 552) — read by 2: `src/render/agents.ts#createAgentMeshes.carcass`, `src/render/agents.ts#createAgentMeshes.setBleed`
- `src/render/agents.ts#createAgentMeshes` (function, line 552) — write by 1: `src/render/agents.ts#createAgentMeshes.setBleed`
- `src/render/agents.ts#createAgentMeshes.bodyKey` (function, line 651) — call by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#planOf` (function, line 438) — call by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#SIDES` (variable, line 112) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#TwoWheelerParts` (interface, line 817) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`

### src/render/buildings/buildingMesh.ts

- `src/render/buildings/buildingMesh.ts#Emitter` (class, line 847) — construct by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/render/buildings/buildingMesh.ts#FurnitureSink` (type, line 478) — typeRef by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/render/buildings/buildingMesh.ts#Paint` (interface, line 146) — typeRef by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/buildingMesh.ts#PartRows.batch` (method, line 468) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/render/buildings/buildingMesh.ts#PartRows.count` (field, line 383) — read by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/render/buildings/buildingMesh.ts#PartSink` (type, line 476) — typeRef by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/render/buildings/buildingMesh.ts#placeFurniture` (function, line 1458) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/render/buildings/buildingMesh.ts#Shell` (class, line 666) — construct by 2: `src/render/buildings/buildingMesh.ts#interiorFurniture`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/buildingMesh.ts#SHELL_PART_MATRIX` (variable, line 3834) — read by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/buildingMesh.ts#Shell.decay` (field, line 677) — write by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/buildingMesh.ts#Shell.face` (method, line 704) — call by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/buildingMesh.ts#Shell.ground` (field, line 675) — write by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/buildingMesh.ts#shellChunks` (function, line 3814) — call by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/buildingMesh.ts#V3` (type, line 805) — typeRef by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`

### src/render/buildings/cell.worker.ts

- `src/render/buildings/cell.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/buildings/cellArrays.ts

- `src/render/buildings/cellArrays.ts#COLOUR_RANGE` (variable, line 90) — read by 1: `src/render/buildings/fracture.ts#fractureData`
- `src/render/buildings/cellArrays.ts#FINISH_ATTRIBUTE` (variable, line 80) — read by 2: `src/render/destruction.ts#createDestruction.place`, `src/render/buildings/fracture.ts#fragmentGeometry`
- `src/render/buildings/cellArrays.ts#finishId` (function, line 87) — call by 1: `src/render/buildings/fracture.ts#prepareFracture`
- `src/render/buildings/cellArrays.ts#NORMAL_SHORTS` (variable, line 104) — read by 1: `src/render/buildings/fracture.ts#fractureData`
- `src/render/buildings/cellArrays.ts#NORMAL_UNIT` (variable, line 94) — read by 1: `src/render/buildings/cellArrays.ts#octDecode`
- `src/render/buildings/cellArrays.ts#PAINT_UNIT` (variable, line 92) — read by 1: `src/render/buildings/fracture.ts#fractureData`
- `src/render/buildings/cellArrays.ts#SHELL_SHORTS` (variable, line 102) — read by 1: `src/render/buildings/fracture.ts#fractureData`
- `src/render/buildings/cellArrays.ts#WEAR_STEPS` (variable, line 106) — read by 1: `src/render/buildings/fracture.ts#fractureData`

### src/render/buildings/chunk.ts

- `src/render/buildings/chunk.ts#BuildingChunk` (interface, line 108) — read by 2: `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#withShellParts`
- `src/render/buildings/chunk.ts#BuildingChunk` (interface, line 108) — typeRef by 3: `src/render/destruction.ts#createDestruction`, `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#withShellParts`
- `src/render/buildings/chunk.ts#PartBatch` (interface, line 75) — read by 2: `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/chunk.ts#PartBatch` (interface, line 75) — typeRef by 3: `src/render/buildings/buildingMesh.ts#interiorFurniture`, `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/chunk.ts#PartKind` (type, line 8) — typeRef by 3: `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/chunk.ts#ShellChunk` (interface, line 89) — read by 2: `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#withShellParts`
- `src/render/buildings/chunk.ts#ShellChunk` (interface, line 89) — typeRef by 2: `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/buildingMesh.ts#shellPartFaces`

### src/render/buildings/kit.ts

- `src/render/buildings/kit.ts#BuildingKit` (interface, line 146) — call by 1: `src/render/buildings/fracture.ts#prepareFracture`
- `src/render/buildings/kit.ts#BuildingKit` (interface, line 146) — read by 1: `src/render/buildings/fracture.ts#prepareFracture`
- `src/render/buildings/kit.ts#BuildingKit` (interface, line 146) — typeRef by 2: `src/render/destruction.ts#createDestruction`, `src/render/buildings/fracture.ts#prepareFracture`
- `src/render/buildings/kit.ts#createBuildingKit.furniture` (method, line 689) — implementation by 1: `src/render/buildings/fracture.ts#prepareFracture`

### src/render/buildings/partPose.ts

- `src/render/buildings/partPose.ts#POSE_FLOATS` (variable, line 6) — read by 2: `src/render/buildings/partPose.ts#poseMatrices`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/partPose.ts#poseMatrixInto` (function, line 13) — call by 2: `src/render/buildings/partPose.ts#poseMatrices`, `src/render/buildings/buildingMesh.ts#shellPartFaces`

### src/render/buildings/shellParts.ts

- `src/render/buildings/shellParts.ts#COVER_INSET` (variable, line 33) — read by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/shellParts.ts#LOOK_FLOATS` (variable, line 16) — read by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/shellParts.ts#NO_GROUND` (variable, line 26) — read by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/shellParts.ts#SHELL_PART_FACES` (variable, line 107) — read by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/shellParts.ts#SHELL_PART_KINDS` (variable, line 12) — read by 2: `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/shellParts.ts#ShellPartFace` (interface, line 90) — read by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`

### src/render/citizenBake.worker.ts

- `src/render/citizenBake.worker.ts#serve` (function, line 89) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/depthConvention.ts

- `src/render/depthConvention.ts#pullTowardEye` (function, line 28) — call by 2: `src/render/blast.ts#createBlast`, `src/render/casualties.ts#createCasualties`

### src/render/derivedWriter.worker.ts

- `src/render/derivedWriter.worker.ts#serve` (function, line 23) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/ecology.worker.ts

- `src/render/ecology.worker.ts#serve` (function, line 8) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/flatGround.worker.ts

- `src/render/flatGround.worker.ts#serve` (function, line 7) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/groundShape.worker.ts

- `src/render/groundShape.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/instanceBounds.ts

- `src/render/instanceBounds.ts#fitInstances` (function, line 5) — call by 1: `src/render/gore.ts#createGore.update`

### src/render/isoViewport.ts

- `src/render/isoViewport.ts#createIsoRig` (function, line 78) — implementation by 3: `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`

### src/render/landStroke.worker.ts

- `src/render/landStroke.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/networkFarRoads.worker.ts

- `src/render/networkFarRoads.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/people/lodWorker.ts

- `src/render/people/lodWorker.ts#serve` (function, line 82) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/renderer.ts

- `src/render/renderer.ts#BlastHit` (interface, line 187) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/render/renderer.ts#BlastHit` (interface, line 187) — typeRef by 2: `src/actionsWiring.ts#createActions.explodeAt`, `src/render/playEffects.ts#PlayEffects`
- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — callbackAssign by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — implementation by 6: `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootAt`, …
- `src/render/renderer.ts#createSceneRenderer.loadEffects` (function, line 1001) — callbackAssign by 2: `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.strikeBuilding`
- `src/render/renderer.ts#createSceneRenderer.loadEffects` (function, line 1001) — implementation by 3: `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.forgetRuin`, `src/render/playEffects.ts#createPlayEffects.strikeBuilding`
- `src/render/renderer.ts#SceneHandle` (interface, line 227) — call by 6: `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootAt`, …
- `src/render/renderer.ts#SceneHandle` (interface, line 227) — read by 1: `src/actionsWiring.ts#createActions.shootAt`
- `src/render/renderer.ts#SceneHandle` (interface, line 227) — typeRef by 2: `src/actionsWiring.ts#ActionsDeps`, `src/actionsWiring.ts#createActions.shootAt`

### src/render/riggedCitizens.ts

- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) — call by 1: `src/render/agents.ts#createAgentMeshes.setBleed`

### src/render/roadStations.worker.ts

- `src/render/roadStations.worker.ts#serve` (function, line 15) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/roadTile.worker.ts

- `src/render/roadTile.worker.ts#serve` (function, line 102) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/terrainLight.worker.ts

- `src/render/terrainLight.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/uploads.ts

- `src/render/uploads.ts#compileAhead` (function, line 242) — call by 1: `src/render/playEffects.ts#createPlayEffects`

### src/render/vehicleModels.ts

- `src/render/vehicleModels.ts#merge` (function, line 221) — call by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/vehicleModels.ts#TwoWheelerModel` (interface, line 535) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/vehicleModels.ts#VehicleModel` (interface, line 119) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`

### src/render/wind.ts

- `src/render/wind.ts#windUniforms` (variable, line 13) — read by 1: `src/render/exhaust.ts#createExhaust`

### src/sim/agents/laneGrid.ts

- `src/sim/agents/laneGrid.ts#lanesNearInto` (function, line 178) — call by 1: `src/sim/world.ts#SimWorld.someVehicleNear`

### src/sim/audit.ts

- `src/sim/audit.ts#AuditIssue` (interface, line 29) — typeRef by 1: `src/sim/agents/walk.ts#createAgentWalkEngine`

### src/sim/city/city.ts

- `src/sim/city/city.ts#City.transit` (field, line 14) — read by 1: `src/sim/agents/walk.ts#stepWalkers`

### src/sim/clock.ts

- `src/sim/clock.ts#SimClock.tick` (field, line 7) — read by 1: `src/sim/crossings/permission.ts#heldOnlyByPedestrians`

### src/sim/crossings/permission.ts

- `src/sim/crossings/permission.ts#PED_COURTESY` (variable, line 106) — read by 2: `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/sim/crossings/permission.ts#Reservation` (type, line 45) — read by 1: `src/sim/crossings/permission.ts#crossingReservedByVehicle`
- `src/sim/crossings/permission.ts#Reservation` (type, line 45) — typeRef by 1: `src/sim/crossings/permission.ts#indexReservations`
- `src/sim/crossings/permission.ts#RESERVATIONS` (variable, line 46) — read by 2: `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#crossingReservedByVehicle`
- `src/sim/crossings/permission.ts#RESERVATIONS` (variable, line 46) — write by 1: `src/sim/crossings/permission.ts#indexReservations`
- `src/sim/crossings/permission.ts#reservedIds` (variable, line 48) — read by 2: `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#alreadyReserved`
- `src/sim/crossings/permission.ts#reservedIds` (variable, line 48) — write by 1: `src/sim/crossings/permission.ts#indexReservations`
- `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing` (function, line 148) — call by 1: `src/sim/crossings/permission.ts#vehicleBodyOnCrossing`

### src/sim/crossings/state.ts

- `src/sim/crossings/state.ts#CrossingState` (interface, line 16) — read by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/crossings/state.ts#CrossingState` (interface, line 16) — typeRef by 2: `src/sim/agents/walk.ts#State`, `src/sim/crossings/state.ts#emptyCrossingState`
- `src/sim/crossings/state.ts#CrossingState` (interface, line 16) — write by 1: `src/sim/agents/walk.ts#publish`

### src/sim/dt.ts

- `src/sim/dt.ts#DT` (variable, line 2) — read by 2: `src/sim/agents/walk.ts#movePlayerWalker`, `src/sim/agents/walk.ts#stepWalkers`

### src/sim/host/fleetGrid.ts

- `src/sim/host/fleetGrid.ts#GroundBox` (interface, line 7) — typeRef by 2: `src/actionsWiring.ts#createActions.shootVehicle`, `src/render/playEffects.ts#createPlayEffects`
- `src/sim/host/fleetGrid.ts#GroundBox` (interface, line 7) — write by 1: `src/render/playEffects.ts#createPlayEffects`

### src/sim/host/protocol.ts

- `src/sim/host/protocol.ts#SimAct` (type, line 135) — typeRef by 1: `src/sim/host/simClient.ts#SimClient.act`
- `src/sim/host/protocol.ts#Wound` (interface, line 151) — read by 1: `src/render/playEffects.ts#createPlayEffects.frame`
- `src/sim/host/protocol.ts#Wound` (interface, line 151) — typeRef by 1: `src/sim/host/simClient.ts#SimClient.takeWounds`

### src/sim/host/simClient.ts

- `src/sim/host/simClient.ts#SimClient` (class, line 84) — typeRef by 3: `src/actionsWiring.ts#ActionsDeps`, `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#PlayEffects`
- `src/sim/host/simClient.ts#SimClient.casualtyList` (field, line 229) — read by 1: `src/sim/host/simClient.ts#SimClient.casualties`
- `src/sim/host/simClient.ts#SimClient.casualtyList` (field, line 229) — write by 1: `src/sim/host/simClient.ts#SimClient.casualties`
- `src/sim/host/simClient.ts#SimClient.constructor` (constructor, line 232) — read by 3: `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.groundChanged`, `src/render/playEffects.ts#createPlayEffects.lotsAt`
- `src/sim/host/simClient.ts#SimClient.nextAsk` (field, line 104) — read by 1: `src/sim/host/simClient.ts#SimClient.act`
- `src/sim/host/simClient.ts#SimClient.nextAsk` (field, line 104) — write by 1: `src/sim/host/simClient.ts#SimClient.act`
- `src/sim/host/simClient.ts#SimClient.pedViews` (field, line 129) — read by 3: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootAt`, `src/render/playEffects.ts#createPlayEffects.frame`
- `src/sim/host/simClient.ts#SimClient.pending` (field, line 103) — read by 1: `src/sim/host/simClient.ts#SimClient.act`
- `src/sim/host/simClient.ts#SimClient.pending` (field, line 103) — write by 1: `src/sim/host/simClient.ts#SimClient.act`
- `src/sim/host/simClient.ts#SimClient.send` (method, line 423) — call by 1: `src/sim/host/simClient.ts#SimClient.act`
- `src/sim/host/simClient.ts#SimClient.signalJunctions` (field, line 409) — read by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/sim/host/simClient.ts#SimClient.time` (field, line 185) — read by 1: `src/actionsWiring.ts#createActions.ignite`
- `src/sim/host/simClient.ts#SimClient.vehiclesIn` (method, line 138) — call by 3: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootVehicle`, `src/render/playEffects.ts#createPlayEffects`
- `src/sim/host/simClient.ts#SimClient.wounds` (field, line 228) — read by 1: `src/sim/host/simClient.ts#SimClient.takeWounds`
- `src/sim/host/simClient.ts#SimClient.wounds` (field, line 228) — write by 1: `src/sim/host/simClient.ts#SimClient.takeWounds`
- `src/sim/host/simClient.ts#VehicleView` (interface, line 28) — read by 4: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.poseOf`, `src/actionsWiring.ts#createActions.shootVehicle`, `src/render/playEffects.ts#createPlayEffects`
- `src/sim/host/simClient.ts#VehicleView` (interface, line 28) — typeRef by 4: `src/actionsWiring.ts#createActions.poseOf`, `src/actionsWiring.ts#createActions.shootVehicle`, `src/render/playEffects.ts#createPlayEffects`, `src/render/agents.ts#createAgentMeshes.carcass`

### src/sim/intersections/crossingSpans.ts

- `src/sim/intersections/crossingSpans.ts#CrossingSpans.span` (method, line 138) — call by 2: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing` (function, line 203) — call by 1: `src/sim/crossings/permission.ts#crossingReservedByVehicle`

### src/sim/params.ts

- `src/sim/params.ts#PED` (variable, line 93) — read by 3: `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`, `src/sim/crossings/permission.ts#pedGapAccepted`

### src/sim/peds/sidewalk.ts

- `src/sim/peds/sidewalk.ts#SidewalkEdge` (interface, line 30) — read by 4: `src/sim/agents/walk.ts#publish`, `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#vehicleBodyOnCrossing`
- `src/sim/peds/sidewalk.ts#SidewalkEdge` (interface, line 30) — typeRef by 6: `src/sim/agents/walk.ts#Zebra`, `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, …
- `src/sim/peds/sidewalk.ts#SidewalkGraph.crossings` (field, line 50) — read by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingEdge`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.edges` (field, line 49) — read by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingEdge`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.ofNode` (field, line 55) — read by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.madeAt`

### src/sim/peds/signalPostIndex.ts

- `src/sim/peds/signalPostIndex.ts#POST_CELL` (variable, line 17) — read by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.near`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.byNode` (field, line 23) — read by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.byNode` (field, line 23) — write by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.cells` (field, line 24) — read by 4: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.near`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.cells` (field, line 24) — write by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.serial` (field, line 22) — read by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.serial` (field, line 22) — write by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`

### src/sim/people/engine.ts

- `src/sim/people/engine.ts#Boarder` (interface, line 56) — read by 2: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#createAgentWalkEngine.walk`
- `src/sim/people/engine.ts#PedestrianEngine` (interface, line 8) — typeRef by 1: `src/sim/agents/walk.ts#createAgentWalkEngine`
- `src/sim/people/engine.ts#PeopleBridge` (interface, line 68) — read by 1: `src/sim/agents/walk.ts#createAgentWalkEngine`
- `src/sim/people/engine.ts#PeopleBridge` (interface, line 68) — typeRef by 1: `src/sim/agents/walk.ts#createAgentWalkEngine`
- `src/sim/people/engine.ts#ResidentWalk` (interface, line 38) — read by 1: `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/engine.ts#ResidentWalk` (interface, line 38) — typeRef by 2: `src/sim/agents/walk.ts#startWalk`, `src/sim/agents/walk.ts#walkThrough`

### src/sim/people/view.ts

- `src/sim/people/view.ts#BodyPart` (type, line 53) — typeRef by 16: `src/actionsWiring.ts#createActions.shootAt`, `src/render/playEffects.ts#PlayEffects`, `src/render/ragdoll.ts#Body`, `src/render/ragdoll.ts#createRagdolls`, …
- `src/sim/people/view.ts#GestureKind` (type, line 19) — typeRef by 2: `src/sim/agents/walk.ts#Walker`, `src/sim/agents/walk.ts#walkerAct`
- `src/sim/people/view.ts#GestureView` (interface, line 32) — read by 1: `src/render/playEffects.ts#createPlayEffects.frame`
- `src/sim/people/view.ts#GestureView` (interface, line 32) — write by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/people/view.ts#PartyView` (interface, line 11) — read by 6: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#finish`, `src/sim/agents/walk.ts#joinParty`, `src/sim/agents/walk.ts#mourn`, …
- `src/sim/people/view.ts#PartyView` (interface, line 11) — typeRef by 1: `src/sim/people/casualties.ts#Casualty`
- `src/sim/people/view.ts#PedView` (interface, line 55) — read by 10: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootAt`, `src/render/playEffects.ts#createPlayEffects.frame`, `src/sim/agents/walk.ts#createAgentWalkEngine`, …
- `src/sim/people/view.ts#PedView` (interface, line 55) — typeRef by 3: `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`, `src/sim/agents/walk.ts#Walker`
- `src/sim/people/view.ts#PedView` (interface, line 55) — write by 2: `src/sim/agents/walk.ts#publish`, `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/view.ts#PersonAgeClass` (type, line 6) — typeRef by 3: `src/sim/people/casualties.ts#Casualty`, `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/view.ts#PersonGender` (type, line 7) — typeRef by 3: `src/sim/people/casualties.ts#Casualty`, `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/view.ts#personHash` (function, line 105) — call by 5: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#frighten`, `src/sim/agents/walk.ts#mourn`, `src/sim/agents/walk.ts#startWalk`, …
- `src/sim/people/view.ts#Severable` (type, line 51) — typeRef by 11: `src/render/playEffects.ts#PlayEffects`, `src/render/ragdoll.ts#createRagdolls`, `src/render/ragdoll.ts#createRagdolls.detach`, `src/render/ragdoll.ts#createRagdolls.fling`, …

### src/sim/pose.ts

- `src/sim/pose.ts#AXLE_NOW` (variable, line 146) — read by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#axleFrame` (function, line 151) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#bodyOffset` (function, line 39) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#framesPoseInto` (function, line 122) — call by 2: `src/actionsWiring.ts#createActions.poseOf`, `src/render/playEffects.ts#createPlayEffects`
- `src/sim/pose.ts#laneChangeYaw` (function, line 31) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#newPose` (function, line 23) — call by 3: `src/actionsWiring.ts#createActions.poseOf`, `src/render/playEffects.ts#createPlayEffects`, `src/sim/agents/walk.ts#TRAFFIC_POSE`
- `src/sim/pose.ts#PoseOut` (interface, line 17) — read by 3: `src/render/playEffects.ts#createPlayEffects`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#PoseOut` (interface, line 17) — typeRef by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#PoseOut` (interface, line 17) — write by 2: `src/render/playEffects.ts#createPlayEffects`, `src/sim/pose.ts#vehiclePoseNowInto`

### src/sim/signals/fsm.ts

- `src/sim/signals/fsm.ts#SignalController` (interface, line 21) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/signals/fsm.ts#SignalController` (interface, line 21) — typeRef by 1: `src/sim/crossings/permission.ts#turnOwedToVehicle`

### src/sim/signals/plan.ts

- `src/sim/signals/plan.ts#CrossingId` (type, line 8) — typeRef by 7: `src/sim/agents/walk.ts#State`, `src/sim/agents/walk.ts#Walker`, `src/sim/agents/walk.ts#Zebra`, `src/sim/crossings/permission.ts#crossingReservedByVehicle`, …
- `src/sim/signals/plan.ts#makeCrossingId` (function, line 370) — call by 2: `src/sim/agents/walk.ts#crossingOf`, `src/sim/crossings/permission.ts#mayEnterCrossing`
- `src/sim/signals/plan.ts#SignalPlan` (interface, line 26) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/signals/plan.ts#Stage` (interface, line 10) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`

### src/sim/signals/query.ts

- `src/sim/signals/query.ts#pedestrianSignalState` (function, line 40) — call by 1: `src/sim/crossings/permission.ts#mayEnterCrossing`
- `src/sim/signals/query.ts#signalStateFor` (function, line 9) — call by 1: `src/sim/crossings/permission.ts#turnOwedToVehicle`

### src/sim/transit/transit.ts

- `src/sim/transit/transit.ts#AHEAD_TIME` (variable, line 33) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#RailLine` (interface, line 79) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`
- `src/sim/transit/transit.ts#RailLine` (interface, line 79) — typeRef by 1: `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#Train` (interface, line 66) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#Train` (interface, line 66) — typeRef by 1: `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TRAIN_AHEAD` (variable, line 31) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TRAIN_CARS,METRO_CARS,CAR_LENGTH` (variable, line 42) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TransitSim.pointAt` (method, line 446) — call by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TransitSim.pointAt` (method, line 446) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`
- `src/sim/transit/transit.ts#TransitSim.rails` (field, line 109) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`

### src/sim/vehicles/archetypes.ts

- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) — read by 8: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootVehicle`, `src/actionsWiring.ts#VEHICLE_HALF_SPAN`, `src/render/playEffects.ts#createPlayEffects`, …
- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) — typeRef by 1: `src/render/playEffects.ts#PlayEffects`
- `src/sim/vehicles/archetypes.ts#ARCHETYPES` (variable, line 50) — read by 1: `src/actionsWiring.ts#VEHICLE_HALF_SPAN`

### src/sim/vehicles/cycleLane.ts

- `src/sim/vehicles/cycleLane.ts#cycleShift` (function, line 22) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`

### src/sim/vehicles/idm.ts

- `src/sim/vehicles/idm.ts#canStopComfortably` (function, line 108) — call by 1: `src/sim/crossings/permission.ts#pedGapAccepted`

### src/sim/vehicles/state.ts

- `src/sim/vehicles/state.ts#ConnectorClearance` (interface, line 23) — read by 1: `src/sim/crossings/permission.ts#indexReservations`
- `src/sim/vehicles/state.ts#FreePose` (interface, line 155) — read by 4: `src/sim/agents/walk.ts#moved`, `src/sim/agents/walk.ts#parkedZone`, `src/sim/agents/walk.ts#ParkedZoneIndex.place`, `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/vehicles/state.ts#FreePose` (interface, line 155) — typeRef by 3: `src/sim/agents/walk.ts#moved`, `src/sim/agents/walk.ts#parkedZone`, `src/sim/agents/walk.ts#ParkedZoneIndex.place`
- `src/sim/vehicles/state.ts#StandingCar` (interface, line 147) — read by 4: `src/sim/agents/walk.ts#parkedZone`, `src/sim/agents/walk.ts#ParkedZoneIndex.follow`, `src/sim/agents/walk.ts#ParkedZoneIndex.keep`, `src/sim/agents/walk.ts#ParkedZoneIndex.sync`
- `src/sim/vehicles/state.ts#StandingCar` (interface, line 147) — typeRef by 6: `src/sim/agents/walk.ts#ParkedEntry`, `src/sim/agents/walk.ts#ParkedSource`, `src/sim/agents/walk.ts#parkedZone`, `src/sim/agents/walk.ts#ParkedZoneIndex.byCar`, …
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) — read by 6: `src/sim/agents/walk.ts#gapOpen`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, …
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) — typeRef by 3: `src/sim/crossings/permission.ts#heldOnlyByPedestrians`, `src/sim/pose.ts#vehiclePoseNowInto`, `src/sim/world.ts#SimWorld.someVehicleNear`

### src/sim/world.ts

- `src/sim/world.ts#BODY_OFF_LANE` (variable, line 39) — read by 1: `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#LaneletRuntime` (interface, line 78) — read by 1: `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#SimWorld` (class, line 153) — typeRef by 46: `src/sim/people/casualties.ts#impactCasualties`, `src/sim/people/casualties.ts#recordCasualty`, `src/sim/people/casualties.ts#RECORDS`, `src/sim/people/casualties.ts#recordWound`, …
- `src/sim/world.ts#SimWorld.ambient` (field, line 267) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/sim/world.ts#SimWorld.asleep` (field, line 199) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/world.ts#SimWorld.bodiesIn` (method, line 1261) — call by 1: `src/sim/crossings/permission.ts#vehicleBodyOnCrossing`
- `src/sim/world.ts#SimWorld.bodiesIn` (method, line 1261) — read by 1: `src/sim/crossings/permission.ts#vehicleBodyOnCrossing`
- `src/sim/world.ts#SimWorld.city` (field, line 260) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/sim/world.ts#SimWorld.clock` (field, line 154) — read by 1: `src/sim/crossings/permission.ts#heldOnlyByPedestrians`
- `src/sim/world.ts#SimWorld.connector` (method, line 371) — call by 3: `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/sim/world.ts#SimWorld.constructor` (constructor, line 293) — read by 1: `src/sim/crossings/permission.ts#crossingReservedByVehicle`
- `src/sim/world.ts#SimWorld.controller` (method, line 395) — call by 1: `src/sim/crossings/permission.ts#mayEnterCrossing`
- `src/sim/world.ts#SimWorld.crossingSpans` (field, line 210) — read by 2: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/sim/world.ts#SimWorld.crossingStates` (field, line 204) — read by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.crossingStates` (field, line 204) — write by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.graph` (field, line 155) — read by 2: `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/sim/world.ts#SimWorld.junctionHolders` (method, line 434) — call by 1: `src/sim/crossings/permission.ts#indexReservations`
- `src/sim/world.ts#SimWorld.laneHead` (method, line 1230) — call by 2: `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/sim/world.ts#SimWorld.lanelet` (method, line 361) — call by 6: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, …
- `src/sim/world.ts#SimWorld.nearLanes` (field, line 478) — read by 1: `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#SimWorld.pedViewById` (field, line 208) — read by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.pedViews` (field, line 206) — read by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.rt` (method, line 376) — call by 1: `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#SimWorld.sidewalks` (field, line 157) — read by 1: `src/sim/agents/walk.ts#crossingOf`
- `src/sim/world.ts#SimWorld.signalPosts` (field, line 159) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/sim/world.ts#SimWorld.vehicles` (field, line 163) — read by 2: `src/sim/agents/walk.ts#gapOpen`, `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#SimWorld.wakeSignal` (method, line 601) — call by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/world.ts#SimWorld.walkwaysNow` (method, line 685) — call by 1: `src/sim/agents/walk.ts#ensureGraph`

### src/view/viewport.ts

- `src/view/viewport.ts#flatViewport.toWorld` (method, line 55) — implementation by 1: `src/actionsWiring.ts#createActions.shootAt`
- `src/view/viewport.ts#flatViewport.toWorldAt` (method, line 57) — implementation by 3: `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`
- `src/view/viewport.ts#Viewport` (interface, line 8) — call by 3: `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`
- `src/view/viewport.ts#Viewport` (interface, line 8) — typeRef by 1: `src/actionsWiring.ts#ActionsDeps`

### src/world/approachGroups.ts

- `src/world/approachGroups.ts#ApproachGroup` (interface, line 5) — read by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`

### src/world/barriers.ts

- `src/world/barriers.ts#Barrier` (interface, line 9) — read by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/buildings/blocks.ts

- `src/world/buildings/blocks.ts#resolveBlocks` (variable, line 11) — call by 2: `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/render/destruction.ts#createDestruction.make`

### src/world/buildings/elements.ts

- `src/world/buildings/elements.ts#elementRing` (function, line 119) — call by 1: `src/render/playEffects.ts#createPlayEffects`

### src/world/buildings/foundation.ts

- `src/world/buildings/foundation.ts#floorHeight` (function, line 289) — call by 2: `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.strikeBuilding`

### src/world/buildings/geometry.ts

- `src/world/buildings/geometry.ts#levelElevation` (function, line 97) — call by 2: `src/actionsWiring.ts#createActions.explodeAt`, `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/geometry.ts#levelHeight` (function, line 90) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/geometry.ts#liftAt` (function, line 120) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/geometry.ts#localToWorld` (function, line 32) — call by 1: `src/render/destruction.ts#createDestruction.make`
- `src/world/buildings/geometry.ts#roofHeightAt` (function, line 775) — call by 1: `src/render/playEffects.ts#createPlayEffects`
- `src/world/buildings/geometry.ts#roofRise` (function, line 757) — call by 1: `src/render/playEffects.ts#createPlayEffects`
- `src/world/buildings/geometry.ts#solidFootprints` (function, line 171) — call by 2: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.tick`
- `src/world/buildings/geometry.ts#volumeCorners` (function, line 153) — call by 1: `src/render/playEffects.ts#createPlayEffects`
- `src/world/buildings/geometry.ts#volumeElevation` (function, line 117) — call by 3: `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/render/playEffects.ts#createPlayEffects`, `src/render/destruction.ts#createDestruction.make`
- `src/world/buildings/geometry.ts#worldToLocal` (function, line 59) — call by 2: `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/render/playEffects.ts#createPlayEffects`

### src/world/buildings/interior.ts

- `src/world/buildings/interior.ts#FloorInterior` (interface, line 42) — read by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/interior.ts#Furniture` (interface, line 21) — read by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/interior.ts#FURNITURE_KINDS` (variable, line 13) — read by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/interior.ts#FurnitureKind` (type, line 19) — typeRef by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/interior.ts#interiorAt` (function, line 194) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`

### src/world/buildings/materials.ts

- `src/world/buildings/materials.ts#Finish` (type, line 7) — typeRef by 3: `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/world/buildings/materials.ts#FINISHES` (variable, line 6) — read by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`

### src/world/buildings/store.ts

- `src/world/buildings/store.ts#BuildingStore.get` (method, line 115) — call by 3: `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.strikeAt`, `src/actionsWiring.ts#createActions.tick`
- `src/world/buildings/store.ts#BuildingStore.has` (method, line 119) — call by 2: `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/buildings/store.ts#BuildingStore.near` (method, line 133) — call by 5: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.explodeAt`, `src/render/playEffects.ts#createPlayEffects`, …
- `src/world/buildings/store.ts#BuildingStore.remove` (method, line 246) — call by 4: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/buildings/store.ts#BuildingStore.size` (getter, line 99) — call by 1: `src/render/playEffects.ts#createPlayEffects.groundChanged`
- `src/world/buildings/store.ts#BuildingStore.wear` (method, line 179) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`

### src/world/buildings/types.ts

- `src/world/buildings/types.ts#Building` (interface, line 284) — read by 11: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.rayOnBuilding`, …
- `src/world/buildings/types.ts#Building` (interface, line 284) — typeRef by 10: `src/actionsWiring.ts#Actions`, `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.rayOnBuilding`, …
- `src/world/buildings/types.ts#BuildingElement` (interface, line 259) — read by 1: `src/render/playEffects.ts#createPlayEffects`
- `src/world/buildings/types.ts#BuildingId` (type, line 9) — typeRef by 5: `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.strikeAt`, `src/actionsWiring.ts#createActions.tick`, …
- `src/world/buildings/types.ts#MAX_STOREY_HEIGHT` (variable, line 336) — read by 1: `src/actionsWiring.ts#RAY_ABOVE`
- `src/world/buildings/types.ts#MAX_STOREYS` (variable, line 338) — read by 1: `src/actionsWiring.ts#RAY_ABOVE`
- `src/world/buildings/types.ts#Volume` (interface, line 150) — read by 5: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/render/playEffects.ts#createPlayEffects`, `src/render/destruction.ts#createDestruction.make`, …
- `src/world/buildings/types.ts#volumeTop` (function, line 447) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`

### src/world/carriageway.ts

- `src/world/carriageway.ts#onCarriageway` (function, line 9) — call by 1: `src/editor/bulldozer.ts#Bulldozer.click`

### src/world/changes.ts

- `src/world/changes.ts#ChangeJournal.serialOf` (method, line 203) — call by 1: `src/render/playEffects.ts#createPlayEffects.groundChanged`

### src/world/derived.ts

- `src/world/derived.ts#Derived.refresh` (method, line 93) — call by 1: `src/world/solids.ts#solidsOf`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 6: `src/editor/bulldozer.ts#BulldozerHost`, `src/editor/bulldozer.ts#removeRoads`, `src/actionsWiring.ts#ActionsDeps`, `src/sim/peds/signalPostIndex.ts#SignalPostWorld`, …
- `src/world/doc.ts#RoadDoc.addTerrainStamp` (method, line 1327) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/doc.ts#RoadDoc.barriersIn` (method, line 535) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — read by 10: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.buildingsUnderRay`, …
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 1: `src/render/playEffects.ts#createPlayEffects.groundChanged`
- `src/world/doc.ts#RoadDoc.landscapeIn` (method, line 605) — call by 2: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/doc.ts#RoadDoc.lotsIn` (method, line 321) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 4: `src/actionsWiring.ts#createActions.explodeAt`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`, `src/world/signalPosts.ts#signalPosts`, `src/sim/crossings/permission.ts#crossingReservedByVehicle`
- `src/world/doc.ts#RoadDoc.poleNear` (method, line 671) — call by 1: `src/editor/bulldozer.ts#Bulldozer.click`
- `src/world/doc.ts#RoadDoc.poles` (getter, line 164) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/doc.ts#RoadDoc.polesIn` (method, line 450) — call by 2: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/doc.ts#RoadDoc.pruneOrphanNodes` (method, line 1386) — call by 1: `src/editor/bulldozer.ts#removeRoads`
- `src/world/doc.ts#RoadDoc.removeBarrier` (method, line 521) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.removeLandscape` (method, line 596) — call by 4: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/actionsWiring.ts#createActions.explodeAt`, `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/world/doc.ts#RoadDoc.removePole` (method, line 654) — call by 4: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/actionsWiring.ts#createActions.explodeAt`, `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/world/doc.ts#RoadDoc.removeSegment` (method, line 767) — call by 1: `src/editor/bulldozer.ts#removeRoads`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 1: `src/world/signalPosts.ts#signalPosts`
- `src/world/doc.ts#RoadDoc.setNodeControl` (method, line 1347) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/doc.ts#RoadDoc.spansAt` (method, line 455) — call by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 4: `src/actionsWiring.ts#createActions.explodeAt`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`, `src/world/signalPosts.ts#signalPosts`, `src/sim/crossings/permission.ts#crossingReservedByVehicle`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 1: `src/world/signalPosts.ts#signalPosts`

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 9: `src/sim/crossings/permission.ts#indexReservations`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`, `src/sim/peds/signalPostIndex.ts#SignalPostWorld`, …
- `src/world/ids.ts#PoleId` (type, line 10) — typeRef by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 4: `src/editor/bulldozer.ts#removeRoads`, `src/sim/agents/walk.ts#publish`, `src/world/signalPosts.ts#SignalJunctions`, `src/editor/roads/streetKit.ts#removeWithStreetKit`

### src/world/landscape.ts

- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — read by 3: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/landscape.ts#landscapeNearIn` (function, line 398) — call by 1: `src/editor/bulldozer.ts#Bulldozer.click`

### src/world/lanelets.ts

- `src/world/lanelets.ts#Connector` (interface, line 62) — read by 3: `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/world/lanelets.ts#JunctionTopology` (interface, line 78) — read by 3: `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/world/lanelets.ts#Lanelet` (interface, line 31) — read by 6: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, …
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) — typeRef by 1: `src/sim/peds/signalPostIndex.ts#SignalPostWorld`
- `src/world/lanelets.ts#LaneletGraph.junctions` (field, line 140) — read by 3: `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`, `src/sim/crossings/permission.ts#turnOwedToVehicle`

### src/world/lotRecord.ts

- `src/world/lotRecord.ts#Lot` (interface, line 6) — read by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/lotRings.ts

- `src/world/lotRings.ts#lotCentre` (function, line 56) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/lots.ts

- `src/world/lots.ts#deleteLot` (function, line 1027) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 6: `src/editor/bulldozer.ts#BulldozerHost`, `src/editor/bulldozer.ts#removeRoads`, `src/actionsWiring.ts#ActionsDeps`, `src/sim/peds/signalPostIndex.ts#SignalPostWorld`, …
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 1: `src/world/signalPosts.ts#signalPosts`
- `src/world/network.ts#Network.rebuild` (method, line 119) — call by 1: `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/world/network.ts#Network.ribbonsMeeting` (method, line 62) — call by 1: `src/world/signalPosts.ts#signalPosts`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 2: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/world/signalPosts.ts#signalPosts`

### src/world/signalPosts.ts

- `src/world/signalPosts.ts#SIGNAL_POST_RADIUS` (variable, line 13) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/world/signalPosts.ts#SignalPost` (interface, line 15) — read by 4: `src/actionsWiring.ts#createActions.explodeAt`, `src/sim/agents/walk.ts#stepWalkers`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/world/signalPosts.ts#SignalPost` (interface, line 15) — typeRef by 4: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.near`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.found`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`, `src/world/signalPosts.ts#signalPosts`
- `src/world/signalPosts.ts#signalPostPlace` (function, line 25) — call by 2: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`, `src/world/signalPosts.ts#signalPosts`

### src/world/solids.ts

- `src/world/solids.ts#createSolids` (function, line 96) — call by 1: `src/world/solids.ts#solidsOf`
- `src/world/solids.ts#createSolids` (function, line 96) — implementation by 1: `src/render/playEffects.ts#createPlayEffects`
- `src/world/solids.ts#createSolids` (function, line 96) — read by 1: `src/world/solids.ts#solidsOf`
- `src/world/solids.ts#known` (variable, line 144) — read by 1: `src/world/solids.ts#solidsOf`
- `src/world/solids.ts#known` (variable, line 144) — write by 1: `src/world/solids.ts#solidsOf`
- `src/world/solids.ts#Solid` (type, line 12) — read by 1: `src/render/playEffects.ts#createPlayEffects`
- `src/world/solids.ts#Solids` (interface, line 27) — call by 1: `src/render/playEffects.ts#createPlayEffects`
- `src/world/solids.ts#Solids` (interface, line 27) — typeRef by 1: `src/world/solids.ts#solidsOf`

### src/world/transit.ts

- `src/world/transit.ts#TransitLine` (interface, line 35) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`

### src/world/utilities.ts

- `src/world/utilities.ts#POLE_ARM_DROP` (variable, line 16) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_ARM_HALF` (variable, line 14) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_HEIGHT` (variable, line 9) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_LAMP_REACH` (variable, line 24) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#UtilityPole` (interface, line 43) — read by 3: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/utilities.ts#UtilitySpan` (interface, line 51) — read by 1: `src/actionsWiring.ts#createActions.explodeAt`

### src/world/walkways.ts

- `src/world/walkways.ts#WalkChange` (interface, line 72) — read by 1: `src/sim/agents/walk.ts#ensureGraph`
- `src/world/walkways.ts#WalkChange` (interface, line 72) — typeRef by 2: `src/world/walkways.ts#WalkGraph.changesSince`, `src/world/walkways.ts#WalkGraph.none`
- `src/world/walkways.ts#WalkGraph` (class, line 81) — typeRef by 7: `src/sim/agents/walk.ts#ensureGraph`, `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#ROUTE_SCRATCH`, `src/sim/agents/walk.ts#routeScratch`, …
- `src/world/walkways.ts#WalkGraph.at` (field, line 86) — read by 2: `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#walkComponents`
- `src/world/walkways.ts#WalkGraph.log` (field, line 93) — read by 1: `src/world/walkways.ts#WalkGraph.changesSince`
- `src/world/walkways.ts#WalkGraph.nodes` (field, line 83) — read by 4: `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#route.estimate`, `src/sim/agents/walk.ts#routeScratch`, `src/sim/agents/walk.ts#walkComponents`
- `src/world/walkways.ts#WalkGraph.state` (getter, line 100) — call by 2: `src/sim/agents/walk.ts#ensureGraph`, `src/sim/agents/walk.ts#walkComponents`
- `src/world/walkways.ts#WalkGraph.token` (field, line 96) — read by 1: `src/world/walkways.ts#WalkGraph.changesSince`
- `src/world/walkways.ts#WalkGraph.ways` (field, line 84) — read by 5: `src/sim/agents/walk.ts#ensureGraph`, `src/sim/agents/walk.ts#nearestWay`, `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#walkComponents`, …
- `src/world/walkways.ts#WalkNode` (interface, line 26) — read by 1: `src/sim/agents/walk.ts#route.estimate`
- `src/world/walkways.ts#Walkway` (interface, line 32) — read by 20: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#crossingOf`, `src/sim/agents/walk.ts#fileWay`, …
- `src/world/walkways.ts#Walkway` (interface, line 32) — typeRef by 12: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#BENDS`, `src/sim/agents/walk.ts#crossingOf`, `src/sim/agents/walk.ts#fileWay`, …

## External APIs

- `es:Math` × 827
- `three:Vector3` × 389
- `es:Math.random` × 215
- `es:Array.length` × 189
- `three:Vector3.x` × 180
- `three:Vector3.z` × 175
- `three:Vector3.y` × 166
- `es:Math.max` × 137
- `es:Array.push` × 135
- `three:Vector3.clone` × 108
- `es:Math.min` × 99
- `es:Float32Array` × 90
- `es:Map.get` × 81
- `es:Math.floor` × 69
- `es:Math.sin` × 68
- `es:Math.cos` × 66
- `three:Vector3.addScaledVector` × 64
- `three:Matrix4` × 63
- `three:Vector3.multiplyScalar` × 54
- `es:Array.map` × 53
- `es:Infinity` × 52
- `three:Vector3.subVectors` × 51
- `three:Vector3.copy` × 48
- `es:Math.abs` × 44
- `es:Math.PI` × 44
- `three:Vector3.add` × 41
- `three:Vector3.normalize` × 41
- `es:Map` × 39
- `es:Map.set` × 38
- `three:Vector3.set` × 38
- `es:Math.hypot` × 37
- `es:ReadonlyArray.length` × 30
- `es:Record` × 30
- `three:Quaternion` × 30
- `three:Object3D.position` × 29
- `es:Array.filter` × 28
- `three:BufferAttribute` × 26
- `jolt-physics:Jolt.destroy` × 25
- `three:InstancedMesh.count` × 25
- `es:Set` × 24
- `three:Vector3.lengthSq` × 24
- `es:Map.delete` × 23
- `es:Set.has` × 23
- `three:Vector3.dot` × 23
- `three:Vector3.length` × 23
- `es:Math.atan2` × 21
- `three:BufferGeometry.setAttribute` × 21
- `es:Set.add` × 18
- `es:Int32Array` × 17
- `es:ReadonlySet.has` × 17
- `three:BufferGeometry.dispose` × 17
- `three:Vector3.crossVectors` × 17
- `es:WeakMap.get` × 16
- `three:Object3D.visible` × 16
- `three:Vector3.lerp` × 16
- `jolt-physics:Body.GetID` × 15
- `three:Color` × 15
- `three:InstancedMesh` × 15
- `three:Object3D.add` × 15
- `es:Map.clear` × 14
- `es:Number.toFixed` × 14
- `es:ReadonlyArray.map` × 14
- `three:BufferGeometry` × 14
- `three:Quaternion.setFromAxisAngle` × 14
- `three:Vector3.setY` × 14
- `es:Array.forEach` × 13
- `es:Readonly` × 13
- `es:WeakMap.set` × 13
- `three:Matrix4.multiply` × 13
- `three:Mesh.geometry` × 13
- `three:Vector3.distanceTo` × 13
- `es:Array.splice` × 12
- `es:Number` × 12
- `three:BufferAttribute.needsUpdate` × 12
- `es:Array.some` × 11
- `es:InstanceType` × 11
- `three:InstancedMesh.instanceMatrix` × 11
- `three:Vector3.sub` × 11
- `dom:CanvasGradient.addColorStop` × 10
- `dom:MessageEvent.data` × 10
- `es:Array.shift` × 10
- `es:Array.slice` × 10
- `es:WeakMap` × 10
- `jolt-physics:Jolt.Vec3` × 10
- `three:Material.dispose` × 10
- `three:MeshStandardMaterial` × 10
- `three:Object3D.name` × 10
- `three:Vector3.divideScalar` × 10
- `es:Array.includes` × 9
- `es:Math.round` × 9
- `es:NumberConstructor.isFinite` × 9
- `es:Object` × 9
- `three:BufferAttribute.addUpdateRange` × 9
- `three:BufferGeometry.getAttribute` × 9
- `three:InstancedMesh.setColorAt` × 9
- `three:InstancedMesh.setMatrixAt` × 9
- `three:Matrix4.copy` × 9
- `three:Mesh` × 9
- `three:Object3D.matrix` × 9
- `three:Object3D.scale` × 9
- `es:Array.indexOf` × 8
- `es:Array.sort` × 8
- `es:Float32Array.length` × 8
- `es:ReadonlyArray.includes` × 8
- `three:BufferAttribute.array` × 8
- `three:Group` × 8
- `three:Object3D.frustumCulled` × 8
- `three:Vector3.applyQuaternion` × 8
- `es:Float32Array.set` × 7
- `es:Float32Array.subarray` × 7
- `es:Float32ArrayConstructor.from` × 7
- `es:Int32Array.length` × 7
- `es:Partial` × 7
- `es:ReadonlyMap.get` × 7
- `three:InstancedMesh.instanceColor` × 7
- `three:Matrix4.compose` × 7
- `three:Object3D.receiveShadow` × 7
- `dom:CanvasFillStrokeStyles.fillStyle` × 6
- `es:Map.size` × 6
- `es:Map.values` × 6
- `es:Math.sqrt` × 6
- `es:String.replace` × 6
- `es:Uint32Array` × 6
- `es:Uint32Array.length` × 6
- `three:DoubleSide` × 6
- `three:Material` × 6
- `three:Matrix4.invert` × 6
- `three:Matrix4.multiplyMatrices` × 6
- `three:Object3D.castShadow` × 6
- `dom:CanvasDrawPath.beginPath` × 5
- `dom:document` × 5
- `dom:Worker` × 5
- `es:Array.reduce` × 5
- `es:NaN` × 5
- `es:Uint8Array` × 5
- `three:BufferAttribute.clearUpdateRanges` × 5
- `three:BufferAttribute.setUsage` × 5
- `three:CanvasTexture` × 5
- `three:DynamicDrawUsage` × 5
- `three:Matrix4.clone` × 5
- `three:Object3D.renderOrder` × 5
- `three:Object3D.rotation` × 5
- `three:Object3D.updateMatrix` × 5
- `dom:CanvasDrawPath.fill` × 4
- `dom:CanvasFillStrokeStyles.createRadialGradient` × 4
- `dom:CanvasPath.moveTo` × 4
- `dom:performance` × 4
- `es:Float64Array` × 4
- `es:NonNullable` × 4
- `es:ObjectConstructor.entries` × 4
- `es:Promise.then` × 4
- `es:ReadonlyArray.forEach` × 4
- `es:ReadonlyArray.some` × 4
- `es:ReadonlySet` × 4
- `es:String` × 4
- `jolt-physics:Jolt.RVec3` × 4
- `jolt-physics:ShapeResult.Get` × 4
- `jolt-physics:ShapeSettings.Create` × 4
- `three:BufferGeometry.rotateX` × 4
- `three:BufferGeometry.translate` × 4
- `three:CylinderGeometry` × 4
- `three:Light.intensity` × 4
- `three:Matrix4.fromArray` × 4
- `three:Object3D.quaternion` × 4
- `three:Object3D.remove` × 4
- `three:Quaternion.clone` × 4
- `three:Sphere` × 4
- `three:Vector3.applyAxisAngle` × 4
- `three:Vector3.negate` × 4
- `dom:CanvasPath.arc` × 3
- `dom:CanvasPath.lineTo` × 3
- `dom:CanvasPathDrawingStyles.lineWidth` × 3
- `dom:console` × 3
- `dom:Document.createElement` × 3
- `dom:HTMLCanvasElement.getContext` × 3
- `dom:HTMLCanvasElement.height` × 3
- `dom:HTMLCanvasElement.width` × 3
- `dom:Performance.now` × 3
- `dom:Worker.postMessage` × 3
- `es:Array.findIndex` × 3
- `es:Array.flatMap` × 3
- `es:Array.pop` × 3
- `es:Error` × 3
- `es:Int16Array` × 3
- `es:Math.imul` × 3
- `es:Parameters` × 3
- `es:Promise` × 3
- `es:ReadonlyArray.every` × 3
- `es:ReadonlyMap` × 3
- `es:Set.clear` × 3
- `es:String.slice` × 3
- `es:Uint16Array` × 3
- `jolt-physics:Jolt.getPointer` × 3
- `jolt-physics:Vec3.Set` × 3
- `three:BufferAttribute.itemSize` × 3
- `three:BufferAttribute.setXYZ` × 3
- `three:BufferGeometry.boundingBox` × 3
- `three:BufferGeometry.boundingSphere` × 3
- `three:BufferGeometry.computeBoundingBox` × 3
- `three:Color.getHex` × 3
- `three:LineBasicMaterial` × 3
- `three:Matrix4.decompose` × 3
- `three:PointLight` × 3
- `three:Quaternion.invert` × 3
- `three:Quaternion.setFromRotationMatrix` × 3
- `three:SRGBColorSpace` × 3
- `three:Texture.colorSpace` × 3
- `three:Vector3.addVectors` × 3
- `three:Vector3.applyMatrix4` × 3
- `three:Vector3.lerpVectors` × 3
- `three:Vector3.setFromMatrixPosition` × 3
- `dom:CanvasDrawPath.stroke` × 2
- `dom:CanvasFillStrokeStyles.strokeStyle` × 2
- `dom:CanvasRect.fillRect` × 2
- `dom:Console.error` × 2
- `dom:location` × 2
- `dom:MessageEvent` × 2
- `es:Array.every` × 2
- `es:Array.find` × 2
- `es:ArrayBuffer` × 2
- `es:Float32Array.buffer` × 2
- `es:Float32Array.fill` × 2
- `es:Float32Array.slice` × 2
- `es:Int32Array.fill` × 2
- `es:Map.keys` × 2
- `es:Math.asin` × 2
- `es:Math.ceil` × 2
- `es:Math.sign` × 2
- `es:ObjectConstructor.fromEntries` × 2
- `es:parseInt` × 2
- `es:ReadonlyArray.filter` × 2
- `es:ReadonlyArray.slice` × 2
- `es:Set.delete` × 2
- `es:Set.size` × 2
- `es:Uint32Array.fill` × 2
- `es:Uint8Array.length` × 2
- `jolt-physics:BodyCreationSettings.mFriction` × 2
- `jolt-physics:BodyInterface.AddBody` × 2
- `jolt-physics:BodyInterface.CreateBody` × 2
- `jolt-physics:BodyInterface.DestroyBody` × 2
- `jolt-physics:BodyInterface.RemoveBody` × 2
- `jolt-physics:BroadPhaseLayerInterfaceTable.MapObjectToBroadPhaseLayer` × 2
- `jolt-physics:HeightFieldShapeSettings.mHeightSamples` × 2
- `jolt-physics:Jolt` × 2
- `jolt-physics:Jolt.BodyCreationSettings` × 2
- `jolt-physics:Jolt.BroadPhaseLayer` × 2
- `jolt-physics:Jolt.EConstraintSpace_WorldSpace` × 2
- `jolt-physics:Jolt.Quat` × 2
- `jolt-physics:JoltSettings.mBroadPhaseLayerInterface` × 2
- `jolt-physics:JoltSettings.mObjectLayerPairFilter` × 2
- `jolt-physics:ObjectLayerPairFilterTable.EnableCollision` × 2
- `jolt-physics:RVec3.Set` × 2
- `jolt-physics:TwoBodyConstraintSettings.Create` × 2
- `three:AdditiveBlending` × 2
- `three:Box3.getSize` × 2
- `three:BufferAttribute.setW` × 2
- `three:BufferAttribute.setXYZW` × 2
- `three:BufferGeometry.addGroup` × 2
- `three:BufferGeometry.computeVertexNormals` × 2
- `three:BufferGeometry.toNonIndexed` × 2
- `three:CircleGeometry` × 2
- `three:Color.b` × 2
- `three:Color.g` × 2
- `three:Color.lerp` × 2
- `three:Color.multiplyScalar` × 2
- `three:Color.r` × 2
- `three:CurvePath.closePath` × 2
- `three:Euler.x` × 2
- `three:Euler.y` × 2
- `three:Euler.z` × 2
- `three:ExtrudeGeometry` × 2
- `three:IUniform.value` × 2
- `three:Light.color` × 2
- `three:LineSegments` × 2
- `three:Matrix3` × 2
- `three:Matrix3.getNormalMatrix` × 2
- `three:Matrix4.elements` × 2
- `three:Matrix4.makeBasis` × 2
- `three:Matrix4.toArray` × 2
- `three:MeshStandardMaterialProperties.map` × 2
- `three:NormalBlending` × 2
- `three:Object3D` × 2
- `three:Path.lineTo` × 2
- `three:Path.moveTo` × 2
- `three:PointLight.decay` × 2
- `three:Points` × 2
- `three:Quaternion.copy` × 2
- `three:Quaternion.setFromUnitVectors` × 2
- `three:Scene` × 2
- `three:ShaderMaterialProperties.uniforms` × 2
- `three:Shape` × 2
- `three:Sphere.set` × 2
- `three:Sphere.union` × 2
- `three:Texture.dispose` × 2
- `three:Vector3.applyMatrix3` × 2
- `three:Vector3.cross` × 2
- `three:Vector3.equals` × 2
- `three:Vector3.toArray` × 2
- `three:WebGLProgramParameters.fragmentShader` × 2
- `three:WebGLProgramParameters.vertexShader` × 2
- `three:WebGLProgramParametersWithUniforms.uniforms` × 2
- `dom:AbstractWorker.onerror` × 1
- `dom:CanvasPath.closePath` × 1
- `dom:CanvasState.restore` × 1
- `dom:CanvasState.save` × 1
- `dom:CanvasTransform.rotate` × 1
- `dom:CanvasTransform.scale` × 1
- `dom:CanvasTransform.translate` × 1
- `dom:Console.warn` × 1
- `dom:ErrorEvent.message` × 1
- `dom:Event.preventDefault` × 1
- `dom:Location.search` × 1
- `dom:MessageEventTarget.onmessage` × 1
- `dom:MessageEventTarget.onmessageerror` × 1
- `dom:navigator` × 1
- `dom:NavigatorConcurrentHardware.hardwareConcurrency` × 1
- `dom:requestAnimationFrame` × 1
- `dom:URLSearchParams` × 1
- `dom:URLSearchParams.get` × 1
- `dom:Worker.terminate` × 1
- `es:Array` × 1
- `es:Array.fill` × 1
- `es:Array.flat` × 1
- `es:Array.join` × 1
- `es:Array.reverse` × 1
- `es:ArrayConstructor.from` × 1
- `es:ArrayLike` × 1
- `es:Error.stack` × 1
- `es:Float64Array.fill` × 1
- `es:Int32ArrayConstructor.from` × 1
- `es:Map.entries` × 1
- `es:Map.has` × 1
- `es:Math.exp` × 1
- `es:Math.pow` × 1
- `es:Math.SQRT1_2` × 1
- `es:NumberConstructor.isNaN` × 1
- `es:NumberConstructor.NaN` × 1
- `es:ObjectConstructor.freeze` × 1
- `es:ObjectConstructor.keys` × 1
- `es:ObjectConstructor.values` × 1
- `es:Promise.catch` × 1
- `es:ReadonlyMap.has` × 1
- `es:ReadonlyMap.keys` × 1
- `es:ReadonlySet.size` × 1
- `es:String.indexOf` × 1
- `es:String.split` × 1
- `es:String.trim` × 1
- `es:StringConstructor.fromCharCode` × 1
- `es:Uint16Array.length` × 1
- `es:Uint32Array.set` × 1
- `es:Uint8Array.fill` × 1
- `es:WeakMap.delete` × 1
- `es:WeakSet` × 1
- `es:WeakSet.add` × 1
- `es:WeakSet.has` × 1
- `jolt-physics:ArrayFloat.data` × 1
- `jolt-physics:ArrayFloat.resize` × 1
- `jolt-physics:ArrayVec3.push_back` × 1
- `jolt-physics:Body.GetMotionProperties` × 1
- `jolt-physics:BodyCreationSettings.mAngularDamping` × 1
- `jolt-physics:BodyCreationSettings.mCollisionGroup` × 1
- `jolt-physics:BodyCreationSettings.mLinearDamping` × 1
- `jolt-physics:BodyCreationSettings.mMaxAngularVelocity` × 1
- `jolt-physics:BodyCreationSettings.mMotionQuality` × 1
- `jolt-physics:BodyCreationSettings.mRestitution` × 1
- `jolt-physics:BodyInterface.ActivateBody` × 1
- `jolt-physics:BodyInterface.AddImpulse` × 1
- `jolt-physics:BodyInterface.DeactivateBody` × 1
- `jolt-physics:BodyInterface.GetPointVelocity` × 1
- `jolt-physics:BodyInterface.GetPosition` × 1
- `jolt-physics:BodyInterface.GetRotation` × 1
- `jolt-physics:BodyInterface.IsActive` × 1
- `jolt-physics:ConvexHullShapeSettings.mMaxConvexRadius` × 1
- `jolt-physics:ConvexHullShapeSettings.mPoints` × 1
- `jolt-physics:GroupFilterTable.DisableCollision` × 1
- `jolt-physics:HeightFieldShapeSettings.mBlockSize` × 1
- `jolt-physics:HeightFieldShapeSettings.mOffset` × 1
- `jolt-physics:HeightFieldShapeSettings.mSampleCount` × 1
- `jolt-physics:HeightFieldShapeSettings.mScale` × 1
- `jolt-physics:HingeConstraint.SetMotorState` × 1
- `jolt-physics:HingeConstraint.SetTargetAngle` × 1
- `jolt-physics:HingeConstraintSettings.mHingeAxis1` × 1
- `jolt-physics:HingeConstraintSettings.mHingeAxis2` × 1
- `jolt-physics:HingeConstraintSettings.mLimitsMax` × 1
- `jolt-physics:HingeConstraintSettings.mLimitsMin` × 1
- `jolt-physics:HingeConstraintSettings.mMaxFrictionTorque` × 1
- `jolt-physics:HingeConstraintSettings.mMotorSettings` × 1
- `jolt-physics:HingeConstraintSettings.mNormalAxis1` × 1
- `jolt-physics:HingeConstraintSettings.mNormalAxis2` × 1
- `jolt-physics:HingeConstraintSettings.mPoint1` × 1
- `jolt-physics:HingeConstraintSettings.mPoint2` × 1
- `jolt-physics:HingeConstraintSettings.mSpace` × 1
- `jolt-physics:Jolt.BoxShapeSettings` × 1
- `jolt-physics:Jolt.BroadPhaseLayerInterfaceTable` × 1
- `jolt-physics:Jolt.CapsuleShapeSettings` × 1
- `jolt-physics:Jolt.castObject` × 1
- `jolt-physics:Jolt.CollisionGroup` × 1
- `jolt-physics:Jolt.ConvexHullShapeSettings` × 1
- `jolt-physics:Jolt.EActivation_Activate` × 1
- `jolt-physics:Jolt.EActivation_DontActivate` × 1
- `jolt-physics:Jolt.EMotionQuality_LinearCast` × 1
- `jolt-physics:Jolt.EMotionType_Dynamic` × 1
- `jolt-physics:Jolt.EMotionType_Static` × 1
- `jolt-physics:Jolt.EMotorState_Position` × 1
- `jolt-physics:Jolt.GroupFilterTable` × 1
- `jolt-physics:Jolt.HEAPF32` × 1
- `jolt-physics:Jolt.HeightFieldShapeSettings` × 1
- `jolt-physics:Jolt.HingeConstraint` × 1
- `jolt-physics:Jolt.HingeConstraintSettings` × 1
- `jolt-physics:Jolt.JoltInterface` × 1
- `jolt-physics:Jolt.JoltSettings` × 1
- `jolt-physics:Jolt.MotorSettings` × 1
- `jolt-physics:Jolt.ObjectLayerPairFilterTable` × 1
- `jolt-physics:Jolt.ObjectVsBroadPhaseLayerFilterTable` × 1
- `jolt-physics:Jolt.SpringSettings` × 1
- `jolt-physics:Jolt.SwingTwistConstraintSettings` × 1
- `jolt-physics:JoltInterface.GetPhysicsSystem` × 1
- `jolt-physics:JoltInterface.Step` × 1
- `jolt-physics:JoltSettings.mMaxWorkerThreads` × 1
- `jolt-physics:JoltSettings.mObjectVsBroadPhaseLayerFilter` × 1
- `jolt-physics:MotionProperties.GetInverseMass` × 1
- `jolt-physics:MotorSettings.mMaxTorqueLimit` × 1
- `jolt-physics:MotorSettings.mMinTorqueLimit` × 1
- `jolt-physics:MotorSettings.mSpringSettings` × 1
- `jolt-physics:PhysicsSystem.AddConstraint` × 1
- `jolt-physics:PhysicsSystem.GetBodyInterface` × 1
- `jolt-physics:PhysicsSystem.RemoveConstraint` × 1
- `jolt-physics:PhysicsSystem.SetGravity` × 1
- `jolt-physics:Quat.GetW` × 1
- `jolt-physics:Quat.GetX` × 1
- `jolt-physics:Quat.GetY` × 1
- `jolt-physics:Quat.GetZ` × 1
- `jolt-physics:RVec3.GetX` × 1
- `jolt-physics:RVec3.GetY` × 1
- `jolt-physics:RVec3.GetZ` × 1
- `jolt-physics:ShapeResult.HasError` × 1
- `jolt-physics:SpringSettings.mDamping` × 1
- `jolt-physics:SpringSettings.mFrequency` × 1
- `jolt-physics:SwingTwistConstraintSettings.mMaxFrictionTorque` × 1
- `jolt-physics:SwingTwistConstraintSettings.mNormalHalfConeAngle` × 1
- `jolt-physics:SwingTwistConstraintSettings.mPlaneAxis1` × 1
- `jolt-physics:SwingTwistConstraintSettings.mPlaneAxis2` × 1
- `jolt-physics:SwingTwistConstraintSettings.mPlaneHalfConeAngle` × 1
- `jolt-physics:SwingTwistConstraintSettings.mPosition1` × 1
- `jolt-physics:SwingTwistConstraintSettings.mPosition2` × 1
- `jolt-physics:SwingTwistConstraintSettings.mSpace` × 1
- `jolt-physics:SwingTwistConstraintSettings.mTwistAxis1` × 1
- `jolt-physics:SwingTwistConstraintSettings.mTwistAxis2` × 1
- `jolt-physics:SwingTwistConstraintSettings.mTwistMaxAngle` × 1
- `jolt-physics:SwingTwistConstraintSettings.mTwistMinAngle` × 1
- `jolt-physics:Vec3.GetX` × 1
- `jolt-physics:Vec3.GetY` × 1
- `jolt-physics:Vec3.GetZ` × 1
- `three:Box3.getCenter` × 1
- `three:Box3.max` × 1
- `three:Box3.min` × 1
- `three:BoxGeometry` × 1
- `three:BufferAttribute.count` × 1
- `three:BufferAttribute.getX` × 1
- `three:BufferAttribute.getY` × 1
- `three:BufferAttribute.getZ` × 1
- `three:BufferGeometry.clone` × 1
- `three:BufferGeometry.index` × 1
- `three:BufferGeometry.scale` × 1
- `three:BufferGeometry.setDrawRange` × 1
- `three:Camera` × 1
- `three:Color.setHex` × 1
- `three:Euler.set` × 1
- `three:InstancedMesh.boundingSphere` × 1
- `three:InstancedMesh.dispose` × 1
- `three:Line.material` × 1
- `three:Material.customProgramCacheKey` × 1
- `three:Material.onBeforeCompile` × 1
- `three:MaterialProperties.opacity` × 1
- `three:Matrix4.extractRotation` × 1
- `three:Matrix4.makeScale` × 1
- `three:Matrix4.set` × 1
- `three:Matrix4.setPosition` × 1
- `three:Matrix4.transpose` × 1
- `three:Mesh.material` × 1
- `three:MeshBasicMaterial` × 1
- `three:Object3D.clear` × 1
- `three:Object3D.getWorldDirection` × 1
- `three:Object3D.matrixAutoUpdate` × 1
- `three:Quaternion.multiply` × 1
- `three:Quaternion.multiplyQuaternions` × 1
- `three:Quaternion.normalize` × 1
- `three:Quaternion.premultiply` × 1
- `three:Quaternion.set` × 1
- `three:Quaternion.slerp` × 1
- `three:Quaternion.w` × 1
- `three:Quaternion.x` × 1
- `three:Quaternion.y` × 1
- `three:Quaternion.z` × 1
- `three:RingGeometry` × 1
- `three:ShaderMaterial` × 1
- `three:Sphere.copy` × 1
- `three:Sphere.isEmpty` × 1
- `three:SphereGeometry` × 1
- `three:Texture.anisotropy` × 1
- `three:Vector3.setFromMatrixScale` × 1
- `three:Vector3.setScalar` × 1
