# Feature: src/editor/bulldozer.ts, src/actionsWiring.ts, src/render/playEffects.ts, src/render/blast.ts, src/render/destruction.ts, src/render/gore.ts, src/render/casualties.ts, src/render/ragdoll.ts, src/render/ragdollAxes.ts, src/render/ragdollGround.ts, src/render/ragdollJolt.ts, src/render/buildings/fracture.ts, src/render/buildings/fracture.worker.ts, src/sim/people/casualties.ts, src/render/exhaust.ts, src/sim/agents/walk.ts, src/ui/toolChoices.ts, src/render/riggedCitizens.ts, src/render/carBody.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 1044 declarations
- Slices of shared code: 72
- Code: 13231 lines in 59 files (source/)
- Resources: 413 (398 entry, 15 style, 0 import; 282 only the feature uses)
- Uses: 566 shared declarations (interface/: 1169 declarations in 248 files, the types they name included); hooks: 454; external APIs: 632

## Code

- src/actionsWiring.ts — whole, 476 lines
- src/core/intersect.ts — part, 16 lines
- src/core/vec2.ts — part, 1 lines
- src/editor/bulldozer.ts — whole, 151 lines
- src/editor/roads/streetKit.ts — part, 9 lines
- src/main.ts — part, 264 lines
- src/render/agents.ts — part, 240 lines
- src/render/bakePool.ts — part, 29 lines
- src/render/blast.ts — whole, 758 lines
- src/render/buildings/buildingMesh.ts — part, 86 lines
- src/render/buildings/cellArrays.ts — part, 7 lines
- src/render/buildings/fracture.ts — whole, 418 lines
- src/render/buildings/fracture.worker.ts — whole, 20 lines
- src/render/buildings/partPose.ts — part, 5 lines
- src/render/buildings/shellParts.ts — part, 12 lines
- src/render/carBody.ts — whole, 1019 lines
- src/render/casualties.ts — whole, 165 lines
- src/render/citizenAssets.ts — whole, 9 lines
- src/render/citizenBake.ts — part, 28 lines
- src/render/citizenCasting.ts — part, 246 lines
- src/render/citizenGait.ts — part, 422 lines
- src/render/citizenPalette.ts — part, 14 lines
- src/render/citizenPlays.ts — part, 124 lines
- src/render/destruction.ts — whole, 410 lines
- src/render/elements.ts — part, 22 lines
- src/render/exhaust.ts — whole, 240 lines
- src/render/gore.ts — whole, 259 lines
- src/render/groundShear.ts — part, 12 lines
- src/render/people/cookedLoad.ts — whole, 64 lines
- src/render/people/cookedPerson.ts — part, 3 lines
- src/render/people/crowdAnimation.ts — part, 12 lines
- src/render/people/faceExpression.ts — part, 18 lines
- src/render/people/heldProps.ts — part, 38 lines
- src/render/playEffects.ts — whole, 577 lines
- src/render/ragdoll.ts — whole, 1981 lines
- src/render/ragdollAxes.ts — whole, 61 lines
- src/render/ragdollGround.ts — whole, 73 lines
- src/render/ragdollJolt.ts — whole, 399 lines
- src/render/renderer.ts — part, 100 lines
- src/render/riggedCitizens.ts — whole, 1300 lines
- src/render/vehicleModels.ts — part, 21 lines
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
- src/ui/toolChoices.ts — whole, 234 lines
- src/ui/v2/shell.ts — part, 122 lines
- src/world/signalPosts.ts — part, 27 lines
- src/world/solids.ts — part, 6 lines
- src/world/walkways.ts — part, 9 lines

## Slices of shared code

- `src/main.ts#<ExpressionStatement>@1985` — 1 of 1 lines
- `src/main.ts#MAP_ACTIONS` — 8 of 20 lines
- `src/main.ts#SLIDERS` — 18 of 57 lines
- `src/main.ts#actions` — 16 of 16 lines
- `src/main.ts#bulldozer` — 10 of 10 lines
- `src/main.ts#cancelGestures` — 3 of 13 lines
- `src/main.ts#cloudTool` — 4 of 14 lines
- `src/main.ts#currentGesture` — 4 of 18 lines
- `src/main.ts#dabSettings` — 10 of 15 lines
- `src/main.ts#drawOverlayScreen` — 95 of 469 lines
- `src/main.ts#drawPolePlan` — 7 of 50 lines
- `src/main.ts#drawStreetscapeHover` — 3 of 40 lines
- `src/main.ts#endPointer` — 3 of 30 lines
- `src/main.ts#frame` — 7 of 102 lines
- `src/main.ts#gameKeys` — 5 of 77 lines
- `src/main.ts#gestureInProgress` — 3 of 3 lines
- `src/main.ts#growZones` — 3 of 10 lines
- `src/main.ts#pointerDown` — 32 of 180 lines
- `src/main.ts#pointerMove` — 3 of 72 lines
- `src/main.ts#poleTool` — 4 of 11 lines
- `src/main.ts#roadTool` — 5 of 29 lines
- `src/main.ts#showInspector` — 14 of 246 lines
- `src/main.ts#streetscapeTool` — 6 of 12 lines
- `src/render/agents.ts#AgentMeshes` — 3 of 31 lines
- `src/render/agents.ts#AgentRenderOptions` — 3 of 30 lines
- `src/render/agents.ts#createAgentMeshes` — 87 of 2045 lines
- `src/render/agents.ts#createAgentMeshes.bodyKey` — 3 of 6 lines
- `src/render/agents.ts#createAgentMeshes.census` — 1 of 1 lines
- `src/render/agents.ts#createAgentMeshes.dispose` — 3 of 21 lines
- `src/render/agents.ts#createAgentMeshes.driverBody` — 3 of 3 lines
- `src/render/agents.ts#createAgentMeshes.forgetGone` — 3 of 5 lines
- `src/render/agents.ts#createAgentMeshes.keepPeople` — 1 of 1 lines
- `src/render/agents.ts#createAgentMeshes.placeCarOccupants` — 13 of 36 lines
- `src/render/agents.ts#createAgentMeshes.placeKerbPerson` — 11 of 46 lines
- `src/render/agents.ts#createAgentMeshes.placeKerbPerson.walkTo` — 4 of 8 lines
- `src/render/agents.ts#createAgentMeshes.placeRiderFigure` — 8 of 31 lines
- `src/render/agents.ts#createAgentMeshes.procPose` — 17 of 124 lines
- `src/render/agents.ts#createAgentMeshes.renderPalettes` — 1 of 1 lines
- `src/render/agents.ts#createAgentMeshes.seatIdentity` — 7 of 7 lines
- `src/render/agents.ts#createAgentMeshes.sync` — 13 of 421 lines
- `src/render/elements.ts#EFFECTS` — 9 of 9 lines
- `src/render/elements.ts#ElementLayer` — 3 of 11 lines
- `src/render/elements.ts#createElementLayer.give` — 10 of 10 lines
- `src/render/renderer.ts#SceneHandle` — 4 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 62 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.loadEffects` — 17 of 18 lines
- `src/render/renderer.ts#createSceneRenderer.movingCasters` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/vehicleModels.ts#seatShell` — 21 of 34 lines
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
- `src/ui/v2/shell.ts#mountShell` — 6 of 1639 lines
- `src/ui/v2/shell.ts#mountShell.elementRows` — 6 of 10 lines
- `src/ui/v2/shell.ts#mountShell.landTabOf` — 3 of 9 lines
- `src/ui/v2/shell.ts#mountShell.renderLandscape` — 68 of 145 lines
- `src/ui/v2/shell.ts#mountShell.renderRoads` — 22 of 116 lines
- `src/ui/v2/shell.ts#mountShell.renderSimple` — 14 of 17 lines
- `src/ui/v2/shell.ts#mountShell.useElement` — 3 of 4 lines

## Resources

- shared entry `walk` — src/render/citizenBake.ts:35: `walk: WALK`
- feature entry `walkElder` — src/render/citizenBake.ts:35: `walkElder: WALK_ELDER`
- shared entry `walkShuffle` — src/render/citizenBake.ts:35: `walkShuffle: WALK_SHUFFLE`
- feature entry `walkHandL` — src/render/citizenBake.ts:36: `walkHandL: WALK_SHUFFLE + 1 + LIBRARY.length + DIRECTIONAL.length + RIDER_CLIPS.length`
- feature entry `walkHandR` — src/render/citizenBake.ts:37: `walkHandR: WALK_SHUFFLE + 2 + LIBRARY.length + DIRECTIONAL.length + RIDER_CLIPS.length`
- feature entry `read` — src/render/citizenGait.ts:389: `read: 'phone'`
- feature entry `bag` — src/render/citizenGait.ts:389: `bag: 'bag'`
- feature entry `trolley` — src/render/citizenGait.ts:389: `trolley: 'trolley'`
- feature entry `umbrella` — src/render/citizenGait.ts:389: `umbrella: 'umbrella'`
- feature entry `cheer` — src/render/citizenGait.ts:389: `cheer: 'cheer'`
- feature entry `dance` — src/render/citizenGait.ts:389: `dance: 'dance'`
- feature entry `wave` — src/render/citizenGait.ts:390: `wave: 'wave'`
- feature entry `drink` — src/render/citizenGait.ts:390: `drink: 'drink'`
- feature entry `photo` — src/render/citizenGait.ts:390: `photo: 'photo'`
- feature entry `laugh` — src/render/citizenGait.ts:390: `laugh: 'laugh'`
- feature entry `argue` — src/render/citizenGait.ts:390: `argue: 'argue'`
- feature entry `knock` — src/render/citizenGait.ts:390: `knock: 'knock'`
- feature entry `headphones` — src/render/citizenGait.ts:391: `headphones: 'headphones'`
- feature entry `eat` — src/render/citizenGait.ts:391: `eat: 'eatIdle'`
- feature entry `work` — src/render/citizenGait.ts:391: `work: 'workTable'`
- shared entry `male` — src/render/citizenWalk.ts:76: `male: sourceOf(WALKS.male)`
- shared entry `female` — src/render/citizenWalk.ts:76: `female: sourceOf(WALKS.female)`
- shared entry `male` — src/render/citizenWalk.ts:81: `male: 1.495864`
- shared entry `female` — src/render/citizenWalk.ts:81: `female: 1.596625`
- shared entry `talk` — src/render/people/faceExpression.ts:45: `talk: { channel: 'faceJoy', amount: 0.25, talk: true }`
- shared entry `laugh` — src/render/people/faceExpression.ts:47: `laugh: { channel: 'faceJoy', amount: 1, laugh: true }`
- shared entry `cheer` — src/render/people/faceExpression.ts:48: `cheer: { channel: 'faceJoy', amount: 0.9, laugh: true }`
- shared entry `wave` — src/render/people/faceExpression.ts:49: `wave: { channel: 'faceJoy', amount: 0.6 }`
- shared entry `argue` — src/render/people/faceExpression.ts:50: `argue: { channel: 'faceAnger', amount: 0.8, talk: true }`
- shared entry `phone` — src/render/people/faceExpression.ts:52: `phone: { amount: 0, talk: true }`
- shared entry `dance` — src/render/people/faceExpression.ts:53: `dance: { channel: 'faceJoy', amount: 0.7 }`
- feature entry `read` — src/render/people/heldProps.ts:14: `read: { kind: 'newspaper' }`
- feature entry `drink` — src/render/people/heldProps.ts:15: `drink: { kind: 'cup' }`
- feature entry `phone` — src/render/people/heldProps.ts:16: `phone: { kind: 'phone' }`
- feature entry `photo` — src/render/people/heldProps.ts:17: `photo: { kind: 'camera' }`
- feature entry `bag` — src/render/people/heldProps.ts:18: `bag: { kind: 'bag' }`
- feature entry `umbrella` — src/render/people/heldProps.ts:19: `umbrella: { kind: 'umbrella' }`
- feature entry `cup` — src/render/people/heldProps.ts:82: `cup: [0.07, 0.0, -0.05]`
- feature entry `bag` — src/render/people/heldProps.ts:83: `bag: [0.08, 0.0, -0.02]`
- feature entry `umbrella` — src/render/people/heldProps.ts:84: `umbrella: [0.06, 0.0, -0.03]`
- feature entry `m` — src/sim/people/view.ts:45: `m: 2.9`
- feature entry `f` — src/sim/people/view.ts:45: `f: 4.9`
- feature entry `m` — src/sim/people/view.ts:46: `m: 2.5`
- feature entry `f` — src/sim/people/view.ts:46: `f: 3.33`
- feature entry `actions.pistol` — src/ui/i18n/en.ts:30: `'actions.pistol': 'Pistol'`
- feature entry `hint.shoot.miss` — src/ui/i18n/en.ts:31: `'hint.shoot.miss': 'Nobody there: click a person.'`
- feature entry `actions.bomb` — src/ui/i18n/en.ts:32: `'actions.bomb': 'Bomb'`
- shared entry `hint.lot.deleted` — src/ui/i18n/en.ts:40: `'hint.lot.deleted': 'Deleted.'`
- feature entry `zone.lot.size` — src/ui/i18n/en.ts:47: `'zone.lot.size': '{w} x {d} m'`
- shared entry `help.tool.pole` — src/ui/i18n/en.ts:134: `'help.tool.pole': 'Sets wire poles on the footways. Click a footway and keep clicking: the poles follow the footway and turn at the corners.'`
- shared entry `help.tool.barrier` — src/ui/i18n/en.ts:153: `'help.tool.barrier': 'Draws walls, fences and hedges along a path, as roads are drawn. Near a road a point goes onto the back of the pavement, flush with the ho`
- shared entry `tool.bulldoze` — src/ui/i18n/en.ts:326: `'tool.bulldoze': 'Bulldoze'`
- shared entry `help.tool.streetscape` — src/ui/i18n/en.ts:330: `'help.tool.streetscape': 'Places trees, shrubs, benches, bins, street lights, hydrants and post boxes on the footways. Click a footway to place the chosen item.`
- shared entry `hint.streetscape.removed` — src/ui/i18n/en.ts:335: `'hint.streetscape.removed': 'Item removed'`
- feature entry `streetscape.tree` — src/ui/i18n/en.ts:340: `'streetscape.tree': 'Tree'`
- feature entry `streetscape.shrub` — src/ui/i18n/en.ts:341: `'streetscape.shrub': 'Shrub'`
- feature entry `streetscape.bench` — src/ui/i18n/en.ts:342: `'streetscape.bench': 'Bench'`
- feature entry `streetscape.bin` — src/ui/i18n/en.ts:343: `'streetscape.bin': 'Bin'`
- feature entry `streetscape.lamp` — src/ui/i18n/en.ts:344: `'streetscape.lamp': 'Street light'`
- feature entry `streetscape.hydrant` — src/ui/i18n/en.ts:345: `'streetscape.hydrant': 'Hydrant'`
- feature entry `streetscape.postbox` — src/ui/i18n/en.ts:346: `'streetscape.postbox': 'Post box'`
- feature entry `streetscape.phone` — src/ui/i18n/en.ts:347: `'streetscape.phone': 'Public phone'`
- feature entry `streetscape.drain` — src/ui/i18n/en.ts:348: `'streetscape.drain': 'Kerb drain'`
- feature entry `streetscape.meadow` — src/ui/i18n/en.ts:349: `'streetscape.meadow': 'Long grass (on the lawn)'`
- feature entry `streetscape.sign` — src/ui/i18n/en.ts:350: `'streetscape.sign': 'Sign'`
- feature entry `streetscape.streetname` — src/ui/i18n/en.ts:351: `'streetscape.streetname': 'Street name'`
- feature entry `sign.type` — src/ui/i18n/en.ts:352: `'sign.type': 'Sign type'`
- feature entry `sign.type.stop` — src/ui/i18n/en.ts:353: `'sign.type.stop': 'Stop'`
- feature entry `sign.type.yield` — src/ui/i18n/en.ts:354: `'sign.type.yield': 'Give way'`
- feature entry `sign.type.speed` — src/ui/i18n/en.ts:355: `'sign.type.speed': 'Speed limit'`
- feature entry `sign.type.noParking` — src/ui/i18n/en.ts:356: `'sign.type.noParking': 'No parking'`
- feature entry `sign.type.noEntry` — src/ui/i18n/en.ts:357: `'sign.type.noEntry': 'No entry'`
- feature entry `sign.type.pedestrian` — src/ui/i18n/en.ts:358: `'sign.type.pedestrian': 'Pedestrians'`
- feature entry `sign.type.school` — src/ui/i18n/en.ts:359: `'sign.type.school': 'School'`
- feature entry `sign.type.direction` — src/ui/i18n/en.ts:360: `'sign.type.direction': 'Direction'`
- feature entry `sign.type.street` — src/ui/i18n/en.ts:361: `'sign.type.street': 'Street plate'`
- feature entry `sign.type.info` — src/ui/i18n/en.ts:362: `'sign.type.info': 'Information'`
- feature entry `sign.text` — src/ui/i18n/en.ts:363: `'sign.text': 'Sign text'`
- feature entry `sign.text.speed` — src/ui/i18n/en.ts:364: `'sign.text.speed': 'Speed (e.g. 40)'`
- feature entry `sign.street.placeholder` — src/ui/i18n/en.ts:365: `'sign.street.placeholder': 'Street name, then click the street'`
- feature entry `pole.lamps` — src/ui/i18n/en.ts:368: `'pole.lamps': 'Lights on the poles'`
- feature entry `pole.lamps.none` — src/ui/i18n/en.ts:369: `'pole.lamps.none': 'No lights'`
- feature entry `pole.lamps.alternate` — src/ui/i18n/en.ts:370: `'pole.lamps.alternate': 'A light on every other pole'`
- feature entry `pole.lamps.all` — src/ui/i18n/en.ts:371: `'pole.lamps.all': 'A light on every pole'`
- feature entry `pole.mode.build` — src/ui/i18n/en.ts:372: `'pole.mode.build': 'Build line'`
- feature entry `pole.mode.remove` — src/ui/i18n/en.ts:373: `'pole.mode.remove': 'Remove poles'`
- feature entry `help.tool.poleRemove` — src/ui/i18n/en.ts:374: `'help.tool.poleRemove': 'Click a pole to remove it, with its wires. Shift+click does the same while building.'`
- feature entry `palette.blocks` — src/ui/i18n/en.ts:396: `'palette.blocks': 'Blocks at once'`
- feature entry `palette.blocks.cols` — src/ui/i18n/en.ts:397: `'palette.blocks.cols': 'Columns'`
- feature entry `palette.blocks.rows` — src/ui/i18n/en.ts:398: `'palette.blocks.rows': 'Rows'`
- feature entry `palette.blocks.size` — src/ui/i18n/en.ts:399: `'palette.blocks.size': 'Block size'`
- feature entry `palette.blocks.angle` — src/ui/i18n/en.ts:400: `'palette.blocks.angle': 'Turn'`
- feature entry `hint.blocks.built` — src/ui/i18n/en.ts:401: `'hint.blocks.built': 'Blocks built'`
- feature entry `palette.width` — src/ui/i18n/en.ts:402: `'palette.width': 'Total width'`
- feature entry `palette.width.auto` — src/ui/i18n/en.ts:403: `'palette.width.auto': 'Default'`
- feature entry `el.kind.stones` — src/ui/i18n/en.ts:447: `'el.kind.stones': 'Stones'`
- feature entry `el.kind.pebbles` — src/ui/i18n/en.ts:448: `'el.kind.pebbles': 'Pebbles'`
- feature entry `el.kind.gravel` — src/ui/i18n/en.ts:449: `'el.kind.gravel': 'Gravel'`
- feature entry `el.kind.leaves` — src/ui/i18n/en.ts:450: `'el.kind.leaves': 'Fallen leaves'`
- feature entry `el.kind.grass` — src/ui/i18n/en.ts:451: `'el.kind.grass': 'Grass'`
- feature entry `el.kind.tallGrass` — src/ui/i18n/en.ts:452: `'el.kind.tallGrass': 'Tall grass'`
- feature entry `el.kind.scrub` — src/ui/i18n/en.ts:453: `'el.kind.scrub': 'Scrub'`
- feature entry `el.kind.fern` — src/ui/i18n/en.ts:454: `'el.kind.fern': 'Fern'`
- feature entry `el.kind.clover` — src/ui/i18n/en.ts:455: `'el.kind.clover': 'Clover'`
- feature entry `el.kind.flowers` — src/ui/i18n/en.ts:456: `'el.kind.flowers': 'Flowers'`
- feature entry `el.kind.mushrooms` — src/ui/i18n/en.ts:457: `'el.kind.mushrooms': 'Mushrooms'`
- feature entry `el.kind.smoke` — src/ui/i18n/en.ts:458: `'el.kind.smoke': 'Smoke'`
- feature entry `el.kind.fire` — src/ui/i18n/en.ts:459: `'el.kind.fire': 'Fire'`
- feature entry `el.kind.steam` — src/ui/i18n/en.ts:460: `'el.kind.steam': 'Steam'`
- feature entry `el.kind.dust` — src/ui/i18n/en.ts:461: `'el.kind.dust': 'Dust'`
- feature entry `el.kind.soot` — src/ui/i18n/en.ts:462: `'el.kind.soot': 'Soot'`
- feature entry `el.kind.sparks` — src/ui/i18n/en.ts:463: `'el.kind.sparks': 'Sparks'`
- feature entry `el.kind.spray` — src/ui/i18n/en.ts:464: `'el.kind.spray': 'Foam and spray'`
- feature entry `el.mode.lay` — src/ui/i18n/en.ts:465: `'el.mode.lay': 'Lay'`
- feature entry `el.mode.eraseKind` — src/ui/i18n/en.ts:466: `'el.mode.eraseKind': 'Erase this kind'`
- feature entry `el.mode.eraseAll` — src/ui/i18n/en.ts:467: `'el.mode.eraseAll': 'Erase all'`
- feature entry `el.intensity` — src/ui/i18n/en.ts:473: `'el.intensity': 'Effect intensity (%)'`
- feature entry `tree.kind.mixed` — src/ui/i18n/en.ts:490: `'tree.kind.mixed': 'Mixed'`
- feature entry `tree.kind.oak` — src/ui/i18n/en.ts:491: `'tree.kind.oak': 'Oak'`
- feature entry `tree.kind.cypress` — src/ui/i18n/en.ts:492: `'tree.kind.cypress': 'Cypress'`
- feature entry `tree.kind.palm` — src/ui/i18n/en.ts:493: `'tree.kind.palm': 'Palm'`
- feature entry `tree.mode.plant` — src/ui/i18n/en.ts:494: `'tree.mode.plant': 'Plant a stand'`
- feature entry `tree.mode.one` — src/ui/i18n/en.ts:495: `'tree.mode.one': 'One tree a click'`
- feature entry `tree.mode.cut` — src/ui/i18n/en.ts:496: `'tree.mode.cut': 'Cut trees'`
- feature entry `gully.cut` — src/ui/i18n/en.ts:505: `'gully.cut': 'Cut gullies'`
- feature entry `gully.erase` — src/ui/i18n/en.ts:506: `'gully.erase': 'Wipe away'`
- feature entry `cloud.add` — src/ui/i18n/en.ts:514: `'cloud.add': 'Add a cloud'`
- feature entry `cloud.move` — src/ui/i18n/en.ts:515: `'cloud.move': 'Move a cloud'`
- feature entry `cloud.edit` — src/ui/i18n/en.ts:516: `'cloud.edit': 'Adjust a cloud (applies the values below)'`
- feature entry `cloud.remove` — src/ui/i18n/en.ts:517: `'cloud.remove': 'Remove a cloud'`
- feature entry `fog.lay` — src/ui/i18n/en.ts:530: `'fog.lay': 'Lay fog'`
- feature entry `fog.erase` — src/ui/i18n/en.ts:531: `'fog.erase': 'Erase'`
- shared entry `paint.kind.sand` — src/ui/i18n/en.ts:540: `'paint.kind.sand': 'Sand'`
- shared entry `paint.kind.soil` — src/ui/i18n/en.ts:541: `'paint.kind.soil': 'Soil'`
- shared entry `paint.kind.meadow` — src/ui/i18n/en.ts:542: `'paint.kind.meadow': 'Meadow'`
- shared entry `paint.kind.snow` — src/ui/i18n/en.ts:543: `'paint.kind.snow': 'Snow'`
- shared entry `paint.kind.gravel` — src/ui/i18n/en.ts:544: `'paint.kind.gravel': 'Gravel'`
- shared entry `paint.kind.asphalt` — src/ui/i18n/en.ts:545: `'paint.kind.asphalt': 'Asphalt'`
- shared entry `paint.kind.concrete` — src/ui/i18n/en.ts:546: `'paint.kind.concrete': 'Concrete'`
- shared entry `paint.kind.grass` — src/ui/i18n/en.ts:547: `'paint.kind.grass': 'Grass (erases)'`
- shared entry `paint.kind.forest` — src/ui/i18n/en.ts:548: `'paint.kind.forest': 'Forest'`
- shared entry `paint.kind.granite` — src/ui/i18n/en.ts:555: `'paint.kind.granite': 'Rock: granite'`
- shared entry `paint.kind.sandstone` — src/ui/i18n/en.ts:556: `'paint.kind.sandstone': 'Rock: sandstone'`
- shared entry `paint.kind.basalt` — src/ui/i18n/en.ts:557: `'paint.kind.basalt': 'Rock: basalt'`
- shared entry `paint.kind.cerrado` — src/ui/i18n/en.ts:558: `'paint.kind.cerrado': 'Cerrado'`
- shared entry `paint.kind.atlantic` — src/ui/i18n/en.ts:559: `'paint.kind.atlantic': 'Atlantic forest'`
- shared entry `paint.kind.amazon` — src/ui/i18n/en.ts:560: `'paint.kind.amazon': 'Amazon'`
- shared entry `paint.kind.caatinga` — src/ui/i18n/en.ts:561: `'paint.kind.caatinga': 'Caatinga'`
- shared entry `paint.kind.pampa` — src/ui/i18n/en.ts:562: `'paint.kind.pampa': 'Pampa'`
- shared entry `paint.kind.pantanal` — src/ui/i18n/en.ts:563: `'paint.kind.pantanal': 'Pantanal'`
- shared entry `paint.kind.scrub` — src/ui/i18n/en.ts:574: `'paint.kind.scrub': 'Low scrub'`
- shared entry `paint.kind.flowers` — src/ui/i18n/en.ts:575: `'paint.kind.flowers': 'Flowers'`
- shared entry `paint.kind.rocks` — src/ui/i18n/en.ts:576: `'paint.kind.rocks': 'Rocks'`
- shared entry `hint.road.invalid` — src/ui/i18n/en.ts:721: `'hint.road.invalid': 'This connection needs more distance or a different endpoint.'`
- shared entry `hint.pole.removed` — src/ui/i18n/en.ts:906: `'hint.pole.removed': 'Pole removed'`
- feature entry `strike.force` — src/ui/i18n/en.ts:907: `'strike.force': 'Force'`
- feature entry `hint.strike.hit` — src/ui/i18n/en.ts:908: `'hint.strike.hit': 'Hit: keep striking to bring it down'`
- feature entry `hint.strike.down` — src/ui/i18n/en.ts:909: `'hint.strike.down': 'The building came down'`
- feature entry `hint.strike.deaths` — src/ui/i18n/en.ts:910: `'hint.strike.deaths': 'Hit: there were casualties'`
- feature entry `hint.strike.ground` — src/ui/i18n/en.ts:911: `'hint.strike.ground': 'Struck the ground'`
- feature entry `v2.blocks.count` — src/ui/i18n/en.ts:1332: `'v2.blocks.count': 'Blocks'`
- feature entry `v2.blocks.measure` — src/ui/i18n/en.ts:1333: `'v2.blocks.measure': 'Size'`
- feature entry `v2.row.width` — src/ui/i18n/en.ts:1335: `'v2.row.width': 'Width'`
- feature entry `v2.row.brush` — src/ui/i18n/en.ts:1339: `'v2.row.brush': 'Brush'`
- feature entry `v2.row.sign` — src/ui/i18n/en.ts:1343: `'v2.row.sign': 'Sign'`
- feature entry `v2.row.text` — src/ui/i18n/en.ts:1344: `'v2.row.text': 'Text'`
- feature entry `v2.row.lights` — src/ui/i18n/en.ts:1345: `'v2.row.lights': 'Lights'`
- shared entry `v2.s.effect` — src/ui/i18n/en.ts:1371: `'v2.s.effect': 'Effect'`
- feature entry `actions.pistol` — src/ui/i18n/pt-BR.ts:30: `'actions.pistol': 'Pistola'`
- feature entry `hint.shoot.miss` — src/ui/i18n/pt-BR.ts:31: `'hint.shoot.miss': 'Ninguém aí: clique numa pessoa.'`
- feature entry `actions.bomb` — src/ui/i18n/pt-BR.ts:32: `'actions.bomb': 'Bomba'`
- shared entry `hint.lot.deleted` — src/ui/i18n/pt-BR.ts:40: `'hint.lot.deleted': 'Apagado.'`
- feature entry `zone.lot.size` — src/ui/i18n/pt-BR.ts:47: `'zone.lot.size': '{w} x {d} m'`
- shared entry `help.tool.pole` — src/ui/i18n/pt-BR.ts:133: `'help.tool.pole': 'Coloca postes com fios nas calçadas. Clique numa calçada e siga clicando: os postes acompanham a calçada e viram nas esquinas.'`
- shared entry `help.tool.barrier` — src/ui/i18n/pt-BR.ts:152: `'help.tool.barrier': 'Desenha muros, cercas e sebes por um caminho, como as estradas. Perto de uma via o ponto vai para as costas da calçada, rente às casas.'`
- shared entry `tool.bulldoze` — src/ui/i18n/pt-BR.ts:324: `'tool.bulldoze': 'Demolir'`
- shared entry `help.tool.streetscape` — src/ui/i18n/pt-BR.ts:328: `'help.tool.streetscape': 'Coloca árvores, arbustos, bancos, lixeiras, iluminação, hidrantes e caixas de correio nas calçadas. Clique numa calçada para pôr o ite`
- shared entry `hint.streetscape.removed` — src/ui/i18n/pt-BR.ts:333: `'hint.streetscape.removed': 'Item removido'`
- feature entry `streetscape.tree` — src/ui/i18n/pt-BR.ts:338: `'streetscape.tree': 'Árvore'`
- feature entry `streetscape.shrub` — src/ui/i18n/pt-BR.ts:339: `'streetscape.shrub': 'Arbusto'`
- feature entry `streetscape.bench` — src/ui/i18n/pt-BR.ts:340: `'streetscape.bench': 'Banco'`
- feature entry `streetscape.bin` — src/ui/i18n/pt-BR.ts:341: `'streetscape.bin': 'Lixeira'`
- feature entry `streetscape.lamp` — src/ui/i18n/pt-BR.ts:342: `'streetscape.lamp': 'Iluminação'`
- feature entry `streetscape.hydrant` — src/ui/i18n/pt-BR.ts:343: `'streetscape.hydrant': 'Hidrante'`
- feature entry `streetscape.postbox` — src/ui/i18n/pt-BR.ts:344: `'streetscape.postbox': 'Caixa de correio'`
- feature entry `streetscape.phone` — src/ui/i18n/pt-BR.ts:345: `'streetscape.phone': 'Orelhão (telefone público)'`
- feature entry `streetscape.drain` — src/ui/i18n/pt-BR.ts:346: `'streetscape.drain': 'Boca de lobo'`
- feature entry `streetscape.meadow` — src/ui/i18n/pt-BR.ts:347: `'streetscape.meadow': 'Mato (no gramado)'`
- feature entry `streetscape.sign` — src/ui/i18n/pt-BR.ts:348: `'streetscape.sign': 'Placa'`
- feature entry `streetscape.streetname` — src/ui/i18n/pt-BR.ts:349: `'streetscape.streetname': 'Nome da rua'`
- feature entry `sign.type` — src/ui/i18n/pt-BR.ts:350: `'sign.type': 'Tipo de placa'`
- feature entry `sign.type.stop` — src/ui/i18n/pt-BR.ts:351: `'sign.type.stop': 'Pare'`
- feature entry `sign.type.yield` — src/ui/i18n/pt-BR.ts:352: `'sign.type.yield': 'Dê a preferência'`
- feature entry `sign.type.speed` — src/ui/i18n/pt-BR.ts:353: `'sign.type.speed': 'Velocidade máxima'`
- feature entry `sign.type.noParking` — src/ui/i18n/pt-BR.ts:354: `'sign.type.noParking': 'Proibido estacionar'`
- feature entry `sign.type.noEntry` — src/ui/i18n/pt-BR.ts:355: `'sign.type.noEntry': 'Sentido proibido'`
- feature entry `sign.type.pedestrian` — src/ui/i18n/pt-BR.ts:356: `'sign.type.pedestrian': 'Pedestres'`
- feature entry `sign.type.school` — src/ui/i18n/pt-BR.ts:357: `'sign.type.school': 'Escola'`
- feature entry `sign.type.direction` — src/ui/i18n/pt-BR.ts:358: `'sign.type.direction': 'Direção'`
- feature entry `sign.type.street` — src/ui/i18n/pt-BR.ts:359: `'sign.type.street': 'Placa de rua'`
- feature entry `sign.type.info` — src/ui/i18n/pt-BR.ts:360: `'sign.type.info': 'Informação'`
- feature entry `sign.text` — src/ui/i18n/pt-BR.ts:361: `'sign.text': 'Texto da placa'`
- feature entry `sign.text.speed` — src/ui/i18n/pt-BR.ts:362: `'sign.text.speed': 'Velocidade (ex.: 40)'`
- feature entry `sign.street.placeholder` — src/ui/i18n/pt-BR.ts:363: `'sign.street.placeholder': 'Nome da rua, depois clique nela'`
- feature entry `pole.lamps` — src/ui/i18n/pt-BR.ts:366: `'pole.lamps': 'Iluminação nos postes'`
- feature entry `pole.lamps.none` — src/ui/i18n/pt-BR.ts:367: `'pole.lamps.none': 'Sem luz'`
- feature entry `pole.lamps.alternate` — src/ui/i18n/pt-BR.ts:368: `'pole.lamps.alternate': 'Luz em postes alternados'`
- feature entry `pole.lamps.all` — src/ui/i18n/pt-BR.ts:369: `'pole.lamps.all': 'Luz em todos os postes'`
- feature entry `pole.mode.build` — src/ui/i18n/pt-BR.ts:370: `'pole.mode.build': 'Construir rede'`
- feature entry `pole.mode.remove` — src/ui/i18n/pt-BR.ts:371: `'pole.mode.remove': 'Remover postes'`
- feature entry `help.tool.poleRemove` — src/ui/i18n/pt-BR.ts:372: `'help.tool.poleRemove': 'Clique num poste para removê-lo, com os fios. Shift+clique faz o mesmo enquanto constrói.'`
- feature entry `palette.blocks` — src/ui/i18n/pt-BR.ts:392: `'palette.blocks': 'Quarteirões de uma vez'`
- feature entry `palette.blocks.cols` — src/ui/i18n/pt-BR.ts:393: `'palette.blocks.cols': 'Colunas'`
- feature entry `palette.blocks.rows` — src/ui/i18n/pt-BR.ts:394: `'palette.blocks.rows': 'Linhas'`
- feature entry `palette.blocks.size` — src/ui/i18n/pt-BR.ts:395: `'palette.blocks.size': 'Quadra'`
- feature entry `palette.blocks.angle` — src/ui/i18n/pt-BR.ts:396: `'palette.blocks.angle': 'Giro'`
- feature entry `hint.blocks.built` — src/ui/i18n/pt-BR.ts:397: `'hint.blocks.built': 'Quarteirões construídos'`
- feature entry `palette.width` — src/ui/i18n/pt-BR.ts:398: `'palette.width': 'Largura total'`
- feature entry `palette.width.auto` — src/ui/i18n/pt-BR.ts:399: `'palette.width.auto': 'Padrão'`
- feature entry `el.kind.stones` — src/ui/i18n/pt-BR.ts:442: `'el.kind.stones': 'Pedras'`
- feature entry `el.kind.pebbles` — src/ui/i18n/pt-BR.ts:443: `'el.kind.pebbles': 'Seixos'`
- feature entry `el.kind.gravel` — src/ui/i18n/pt-BR.ts:444: `'el.kind.gravel': 'Cascalho'`
- feature entry `el.kind.leaves` — src/ui/i18n/pt-BR.ts:445: `'el.kind.leaves': 'Folhas secas'`
- feature entry `el.kind.grass` — src/ui/i18n/pt-BR.ts:446: `'el.kind.grass': 'Grama'`
- feature entry `el.kind.tallGrass` — src/ui/i18n/pt-BR.ts:447: `'el.kind.tallGrass': 'Capim alto'`
- feature entry `el.kind.scrub` — src/ui/i18n/pt-BR.ts:448: `'el.kind.scrub': 'Mato'`
- feature entry `el.kind.fern` — src/ui/i18n/pt-BR.ts:449: `'el.kind.fern': 'Samambaia'`
- feature entry `el.kind.clover` — src/ui/i18n/pt-BR.ts:450: `'el.kind.clover': 'Trevo'`
- feature entry `el.kind.flowers` — src/ui/i18n/pt-BR.ts:451: `'el.kind.flowers': 'Flores'`
- feature entry `el.kind.mushrooms` — src/ui/i18n/pt-BR.ts:452: `'el.kind.mushrooms': 'Cogumelos'`
- feature entry `el.kind.smoke` — src/ui/i18n/pt-BR.ts:453: `'el.kind.smoke': 'Fumaça'`
- feature entry `el.kind.fire` — src/ui/i18n/pt-BR.ts:454: `'el.kind.fire': 'Fogo'`
- feature entry `el.kind.steam` — src/ui/i18n/pt-BR.ts:455: `'el.kind.steam': 'Vapor'`
- feature entry `el.kind.dust` — src/ui/i18n/pt-BR.ts:456: `'el.kind.dust': 'Poeira'`
- feature entry `el.kind.soot` — src/ui/i18n/pt-BR.ts:457: `'el.kind.soot': 'Fuligem'`
- feature entry `el.kind.sparks` — src/ui/i18n/pt-BR.ts:458: `'el.kind.sparks': 'Faíscas'`
- feature entry `el.kind.spray` — src/ui/i18n/pt-BR.ts:459: `'el.kind.spray': 'Espuma e borrifo'`
- feature entry `el.mode.lay` — src/ui/i18n/pt-BR.ts:460: `'el.mode.lay': 'Colocar'`
- feature entry `el.mode.eraseKind` — src/ui/i18n/pt-BR.ts:461: `'el.mode.eraseKind': 'Apagar este tipo'`
- feature entry `el.mode.eraseAll` — src/ui/i18n/pt-BR.ts:462: `'el.mode.eraseAll': 'Apagar tudo'`
- feature entry `el.intensity` — src/ui/i18n/pt-BR.ts:468: `'el.intensity': 'Intensidade do efeito (%)'`
- feature entry `tree.kind.mixed` — src/ui/i18n/pt-BR.ts:485: `'tree.kind.mixed': 'Mistas'`
- feature entry `tree.kind.oak` — src/ui/i18n/pt-BR.ts:486: `'tree.kind.oak': 'Carvalho'`
- feature entry `tree.kind.cypress` — src/ui/i18n/pt-BR.ts:487: `'tree.kind.cypress': 'Cipreste'`
- feature entry `tree.kind.palm` — src/ui/i18n/pt-BR.ts:488: `'tree.kind.palm': 'Palmeira'`
- feature entry `tree.mode.plant` — src/ui/i18n/pt-BR.ts:489: `'tree.mode.plant': 'Plantar bosque'`
- feature entry `tree.mode.one` — src/ui/i18n/pt-BR.ts:490: `'tree.mode.one': 'Uma árvore por clique'`
- feature entry `tree.mode.cut` — src/ui/i18n/pt-BR.ts:491: `'tree.mode.cut': 'Cortar árvores'`
- feature entry `gully.cut` — src/ui/i18n/pt-BR.ts:500: `'gully.cut': 'Abrir ravinas'`
- feature entry `gully.erase` — src/ui/i18n/pt-BR.ts:501: `'gully.erase': 'Apagar ravinas'`
- feature entry `cloud.add` — src/ui/i18n/pt-BR.ts:509: `'cloud.add': 'Criar nuvem'`
- feature entry `cloud.move` — src/ui/i18n/pt-BR.ts:510: `'cloud.move': 'Mover nuvem'`
- feature entry `cloud.edit` — src/ui/i18n/pt-BR.ts:511: `'cloud.edit': 'Ajustar nuvem (aplica os valores abaixo)'`
- feature entry `cloud.remove` — src/ui/i18n/pt-BR.ts:512: `'cloud.remove': 'Apagar nuvem'`
- feature entry `fog.lay` — src/ui/i18n/pt-BR.ts:525: `'fog.lay': 'Pôr neblina'`
- feature entry `fog.erase` — src/ui/i18n/pt-BR.ts:526: `'fog.erase': 'Apagar'`
- shared entry `paint.kind.sand` — src/ui/i18n/pt-BR.ts:535: `'paint.kind.sand': 'Areia'`
- shared entry `paint.kind.soil` — src/ui/i18n/pt-BR.ts:536: `'paint.kind.soil': 'Terra'`
- shared entry `paint.kind.meadow` — src/ui/i18n/pt-BR.ts:537: `'paint.kind.meadow': 'Mato'`
- shared entry `paint.kind.snow` — src/ui/i18n/pt-BR.ts:538: `'paint.kind.snow': 'Neve'`
- shared entry `paint.kind.gravel` — src/ui/i18n/pt-BR.ts:539: `'paint.kind.gravel': 'Cascalho'`
- shared entry `paint.kind.asphalt` — src/ui/i18n/pt-BR.ts:540: `'paint.kind.asphalt': 'Asfalto'`
- shared entry `paint.kind.concrete` — src/ui/i18n/pt-BR.ts:541: `'paint.kind.concrete': 'Concreto'`
- shared entry `paint.kind.grass` — src/ui/i18n/pt-BR.ts:542: `'paint.kind.grass': 'Grama (apaga)'`
- shared entry `paint.kind.forest` — src/ui/i18n/pt-BR.ts:543: `'paint.kind.forest': 'Floresta'`
- shared entry `paint.kind.granite` — src/ui/i18n/pt-BR.ts:550: `'paint.kind.granite': 'Rocha: granito'`
- shared entry `paint.kind.sandstone` — src/ui/i18n/pt-BR.ts:551: `'paint.kind.sandstone': 'Rocha: arenito'`
- shared entry `paint.kind.basalt` — src/ui/i18n/pt-BR.ts:552: `'paint.kind.basalt': 'Rocha: basalto'`
- shared entry `paint.kind.cerrado` — src/ui/i18n/pt-BR.ts:553: `'paint.kind.cerrado': 'Cerrado'`
- shared entry `paint.kind.atlantic` — src/ui/i18n/pt-BR.ts:554: `'paint.kind.atlantic': 'Mata Atlântica'`
- shared entry `paint.kind.amazon` — src/ui/i18n/pt-BR.ts:555: `'paint.kind.amazon': 'Amazônia'`
- shared entry `paint.kind.caatinga` — src/ui/i18n/pt-BR.ts:556: `'paint.kind.caatinga': 'Caatinga'`
- shared entry `paint.kind.pampa` — src/ui/i18n/pt-BR.ts:557: `'paint.kind.pampa': 'Pampa'`
- shared entry `paint.kind.pantanal` — src/ui/i18n/pt-BR.ts:558: `'paint.kind.pantanal': 'Pantanal'`
- shared entry `paint.kind.scrub` — src/ui/i18n/pt-BR.ts:569: `'paint.kind.scrub': 'Mata baixa'`
- shared entry `paint.kind.flowers` — src/ui/i18n/pt-BR.ts:570: `'paint.kind.flowers': 'Flores'`
- shared entry `paint.kind.rocks` — src/ui/i18n/pt-BR.ts:571: `'paint.kind.rocks': 'Pedras'`
- shared entry `hint.road.invalid` — src/ui/i18n/pt-BR.ts:714: `'hint.road.invalid': 'Esta conexão precisa de mais distância ou outro ponto final.'`
- shared entry `hint.pole.removed` — src/ui/i18n/pt-BR.ts:899: `'hint.pole.removed': 'Poste removido'`
- feature entry `strike.force` — src/ui/i18n/pt-BR.ts:900: `'strike.force': 'Força'`
- feature entry `hint.strike.hit` — src/ui/i18n/pt-BR.ts:901: `'hint.strike.hit': 'Atingido: continue batendo para derrubar'`
- feature entry `hint.strike.down` — src/ui/i18n/pt-BR.ts:902: `'hint.strike.down': 'Construção desabou'`
- feature entry `hint.strike.deaths` — src/ui/i18n/pt-BR.ts:903: `'hint.strike.deaths': 'Atingido: houve vítimas'`
- feature entry `hint.strike.ground` — src/ui/i18n/pt-BR.ts:904: `'hint.strike.ground': 'Impacto no chão'`
- feature entry `v2.blocks.count` — src/ui/i18n/pt-BR.ts:1322: `'v2.blocks.count': 'Quadras'`
- feature entry `v2.blocks.measure` — src/ui/i18n/pt-BR.ts:1323: `'v2.blocks.measure': 'Medidas'`
- feature entry `v2.row.width` — src/ui/i18n/pt-BR.ts:1325: `'v2.row.width': 'Largura'`
- feature entry `v2.row.brush` — src/ui/i18n/pt-BR.ts:1329: `'v2.row.brush': 'Pincel'`
- feature entry `v2.row.sign` — src/ui/i18n/pt-BR.ts:1333: `'v2.row.sign': 'Placa'`
- feature entry `v2.row.text` — src/ui/i18n/pt-BR.ts:1334: `'v2.row.text': 'Texto'`
- feature entry `v2.row.lights` — src/ui/i18n/pt-BR.ts:1335: `'v2.row.lights': 'Luzes'`
- shared entry `v2.s.effect` — src/ui/i18n/pt-BR.ts:1361: `'v2.s.effect': 'Efeito'`
- shared entry `sand` — src/ui/v2/shell.ts:28: `sand: '#ccb380'`
- shared entry `soil` — src/ui/v2/shell.ts:28: `soil: '#7a5a3c'`
- shared entry `meadow` — src/ui/v2/shell.ts:28: `meadow: '#7d8f3a'`
- shared entry `snow` — src/ui/v2/shell.ts:28: `snow: '#eef1f6'`
- shared entry `gravel` — src/ui/v2/shell.ts:28: `gravel: '#8b8a84'`
- shared entry `asphalt` — src/ui/v2/shell.ts:28: `asphalt: '#2d2e32'`
- shared entry `concrete` — src/ui/v2/shell.ts:28: `concrete: '#a8a69f'`
- shared entry `grass` — src/ui/v2/shell.ts:28: `grass: '#4f7a36'`
- shared entry `forest` — src/ui/v2/shell.ts:28: `forest: '#2c4f26'`
- shared entry `scrub` — src/ui/v2/shell.ts:29: `scrub: '#4c6a2c'`
- shared entry `flowers` — src/ui/v2/shell.ts:29: `flowers: '#c86a8e'`
- shared entry `rocks` — src/ui/v2/shell.ts:29: `rocks: '#7d776c'`
- shared entry `granite` — src/ui/v2/shell.ts:29: `granite: '#8d8a84'`
- shared entry `sandstone` — src/ui/v2/shell.ts:29: `sandstone: '#c27a44'`
- shared entry `basalt` — src/ui/v2/shell.ts:29: `basalt: '#3b3836'`
- shared entry `cerrado` — src/ui/v2/shell.ts:30: `cerrado: '#b8933e'`
- shared entry `atlantic` — src/ui/v2/shell.ts:30: `atlantic: '#2f6b34'`
- shared entry `amazon` — src/ui/v2/shell.ts:30: `amazon: '#1b4f27'`
- shared entry `caatinga` — src/ui/v2/shell.ts:30: `caatinga: '#a08b68'`
- shared entry `pampa` — src/ui/v2/shell.ts:30: `pampa: '#86ad4c'`
- shared entry `pantanal` — src/ui/v2/shell.ts:30: `pantanal: '#4e8f7c'`
- shared entry `zones` — src/ui/v2/shell.ts:65: `zones: '<rect x="3" y="3" width="8" height="8" rx="1"/><rect x="13" y="3" width="8" height="8" rx="1"/><rect x="3" y="13" width="8" height="8" rx="1"/><rect x="`
- feature entry `actions` — src/ui/v2/shell.ts:77: `actions: '<circle cx="12" cy="12" r="7"/><circle cx="12" cy="12" r="2.5"/><path d="M12 2v4M12 18v4M2 12h4M18 12h4"/>'`
- feature entry `bomb` — src/ui/v2/shell.ts:78: `bomb: '<circle cx="11" cy="14" r="7"/><path d="M15 8l2-2M17 6l1.5-1.5M19 3l1 1M20 6l1 0"/>'`
- shared entry `demolish` — src/ui/v2/shell.ts:79: `demolish: '<path d="m5 9 8-5 5 8-8 5Z"/><path d="m7 15 5 5m4-8 3 5"/>'`
- shared entry `undo` — src/ui/v2/shell.ts:85: `undo: '<path d="M9 7 4 12l5 5"/><path d="M5 12h8a6 6 0 0 1 6 6"/>'`
- shared entry `plus` — src/ui/v2/shell.ts:90: `plus: '<path d="M12 5v14M5 12h14"/>'`
- shared entry `brush` — src/ui/v2/shell.ts:103: `brush: '<path d="M14 4 20 10 10 20H4v-6Z"/>'`
- shared entry `eraser` — src/ui/v2/shell.ts:105: `eraser: '<path d="m8 20-5-5L14 4l7 7-9 9Z"/><path d="M8 20h12"/>'`
- shared entry `grid` — src/ui/v2/shell.ts:107: `grid: '<path d="M3 3h18v18H3zM3 9h18M3 15h18M9 3v18M15 3v18"/>'`
- feature entry `draw` — src/ui/v2/shell.ts:115: `draw: '<path d="M4 20 15 9l3 3L7 23H4Z" transform="translate(0 -3)"/><path d="m15 6 3-3 3 3-3 3"/>'`
- feature entry `move` — src/ui/v2/shell.ts:117: `move: '<path d="M12 3v18M3 12h18"/><path d="m9 6 3-3 3 3M9 18l3 3 3-3M6 9l-3 3 3 3M18 9l3 3-3 3"/>'`
- shared entry `ls_tree` — src/ui/v2/shell.ts:175: `ls_tree: '<path d="M12 21v-7"/><circle cx="12" cy="9" r="6"/><path d="M8 21h8"/>'`
- feature entry `ls_shrub` — src/ui/v2/shell.ts:176: `ls_shrub: '<path d="M4 19c0-4 3-7 8-7s8 3 8 7Z"/><path d="M8 13c0-3 2-5 4-5s4 2 4 5"/><path d="M3 21h18"/>'`
- shared entry `ls_bench` — src/ui/v2/shell.ts:177: `ls_bench: '<path d="M4 11h16"/><path d="M4 15h16"/><path d="M6 15v5M18 15v5M6 11V7h12v4"/>'`
- feature entry `ls_bin` — src/ui/v2/shell.ts:178: `ls_bin: '<path d="M6 7h12l-1 14H7Z"/><path d="M5 7h14M10 4h4"/><path d="M10 11v6M14 11v6"/>'`
- feature entry `ls_lamp` — src/ui/v2/shell.ts:179: `ls_lamp: '<path d="M7 21V5h7"/><path d="M12 5h5l1 2h-6Z"/><path d="M15 9l-1 3M17 9l1 3" stroke-dasharray="1.5 1.5"/><path d="M4 21h6"/>'`
- feature entry `ls_hydrant` — src/ui/v2/shell.ts:180: `ls_hydrant: '<path d="M8 21V10h8v11Z"/><path d="M8 10a4 4 0 0 1 8 0"/><path d="M5 14h3M16 14h3M12 4v2"/><path d="M6 21h12"/>'`
- feature entry `ls_postbox` — src/ui/v2/shell.ts:181: `ls_postbox: '<rect x="6" y="5" width="12" height="11" rx="5"/><path d="M9 9h6"/><path d="M12 16v5M8 21h8"/>'`
- feature entry `ls_phone` — src/ui/v2/shell.ts:182: `ls_phone: '<path d="M7 11a5 6 0 0 1 10 0v2H7Z"/><path d="M12 13v8M9 21h6"/><path d="M11 9h2v3h-2Z"/>'`
- feature entry `ls_meadow` — src/ui/v2/shell.ts:183: `ls_meadow: '<path d="M4 20c1-4 2-7 1-10M8 20c0-5 1-8 3-11M12 20c0-4-1-8-3-12M16 20c0-5 2-8 4-10M20 20c-1-3-1-6 0-8"/>'`
- feature entry `ls_sign` — src/ui/v2/shell.ts:184: `ls_sign: '<path d="M12 22V11"/><path d="M8 3h8l3 4-3 4H8L5 7Z"/>'`
- feature entry `ls_streetname` — src/ui/v2/shell.ts:185: `ls_streetname: '<path d="M12 22V8"/><rect x="3" y="3" width="18" height="5" rx="1"/><path d="M6 5.5h12" stroke-dasharray="2 1.5"/>'`
- feature entry `ls_drain` — src/ui/v2/shell.ts:186: `ls_drain: '<path d="M3 9h18"/><path d="M3 9v3h18V9"/><path d="M5 15h14v4H5Z"/><path d="M8 15v4M11 15v4M14 15v4M17 15v4"/>'`
- feature entry `pole_build` — src/ui/v2/shell.ts:190: `pole_build: '<path d="M6 21V4M18 21V4"/><path d="M3 7h6M15 7h6"/><path d="M6 8c4 3 8 3 12 0"/><path d="M12 14v6M9 17h6"/>'`
- feature entry `pole_remove` — src/ui/v2/shell.ts:191: `pole_remove: '<path d="M10 21V4"/><path d="M6 7h8"/><path d="M15 13l5 5M20 13l-5 5"/>'`
- feature entry `pl_none` — src/ui/v2/shell.ts:193: `pl_none: '<path d="M8 21V4"/><path d="M4 7h8"/><path d="M15 9l5 5M20 9l-5 5"/>'`
- feature entry `pl_alternate` — src/ui/v2/shell.ts:194: `pl_alternate: '<path d="M6 21V5h4M18 21V5"/><path d="M9 5l1 2H8Z"/><path d="M9 9l-.5 2" stroke-dasharray="1.5 1.5"/>'`
- feature entry `pl_all` — src/ui/v2/shell.ts:195: `pl_all: '<path d="M6 21V5h4M18 21V5h-4"/><path d="M9 5l1 2H8ZM15 5l-1 2h2Z"/><path d="M9 9l-.5 2M15 9l.5 2" stroke-dasharray="1.5 1.5"/>'`
- feature entry `blocks` — src/ui/v2/shell.ts:200: `blocks: '<path d="M2 8.5h20M2 15.5h20M8.5 2v20M15.5 2v20" stroke-width="2.6"/>'`
- feature entry `el_stones` — src/ui/v2/shell.ts:211: `el_stones: '<path d="M3 18c0-3 2.5-5.5 5.5-5.5S14 15 14 18Z"/><path d="M12.5 18c0-2.4 1.8-4.2 4.2-4.2S21 15.6 21 18Z"/><path d="M2 18h20"/>'`
- feature entry `el_pebbles` — src/ui/v2/shell.ts:212: `el_pebbles: '<ellipse cx="7" cy="16" rx="3" ry="2"/><ellipse cx="15" cy="17" rx="2.5" ry="1.6"/><ellipse cx="12" cy="11.5" rx="2" ry="1.4"/><ellipse cx="18" cy=`
- feature entry `el_gravel` — src/ui/v2/shell.ts:213: `el_gravel: '<circle cx="6" cy="16" r="1"/><circle cx="10" cy="18" r="1"/><circle cx="14" cy="15" r="1"/><circle cx="18" cy="18" r="1"/><circle cx="8" cy="12" r=`
- feature entry `el_leaves` — src/ui/v2/shell.ts:214: `el_leaves: '<path d="M5 19C5 10 11 5 19 5c0 8-5 14-14 14Z"/><path d="M5 19 14 10"/>'`
- feature entry `el_grass` — src/ui/v2/shell.ts:215: `el_grass: '<path d="M4 20c1-4 2-7 1-10M8 20c0-5 1-8 3-11M12 20c0-4-1-8-3-12M16 20c0-5 2-8 4-10M20 20c-1-3-1-6 0-8"/>'`
- feature entry `el_tallGrass` — src/ui/v2/shell.ts:216: `el_tallGrass: '<path d="M6 21c0-7-1-12-3-17M10 21c0-8 1-13 3-18M14 21c0-6 2-11 6-15M18 21c0-5-1-9-3-12"/>'`
- feature entry `el_scrub` — src/ui/v2/shell.ts:217: `el_scrub: '<path d="M4 19c0-4 3-7 8-7s8 3 8 7Z"/><path d="M8 13c0-3 2-5 4-5s4 2 4 5"/><path d="M3 21h18"/>'`
- feature entry `el_fern` — src/ui/v2/shell.ts:218: `el_fern: '<path d="M12 21V5"/><path d="M12 7 7 5M12 7l5-2M12 11 6 9M12 11l6-2M12 15l-5-2M12 15l5-2"/>'`
- feature entry `el_clover` — src/ui/v2/shell.ts:219: `el_clover: '<circle cx="9" cy="9" r="3"/><circle cx="15" cy="9" r="3"/><circle cx="12" cy="14" r="3"/><path d="M12 17v4"/>'`
- feature entry `el_flowers` — src/ui/v2/shell.ts:220: `el_flowers: '<circle cx="12" cy="7" r="3"/><circle cx="12" cy="7" r="1" fill="currentColor"/><path d="M12 10v11M12 15c-2-2-4-2-6-1M12 17c2-2 4-2 6-1"/>'`
- feature entry `el_mushrooms` — src/ui/v2/shell.ts:221: `el_mushrooms: '<path d="M4 12a8 6 0 0 1 16 0Z"/><path d="M10 12v7a2 2 0 0 0 4 0v-7"/>'`
- feature entry `el_smoke` — src/ui/v2/shell.ts:222: `el_smoke: '<path d="M7 19a4 4 0 0 1 1-7 5 5 0 0 1 9 1 3 3 0 0 1 0 6Z"/><path d="M11 9c0-2 1-3 3-4"/>'`
- feature entry `el_fire` — src/ui/v2/shell.ts:223: `el_fire: '<path d="M12 21c-4 0-6-3-6-6 0-4 4-6 4-10 3 2 4 5 4 7 1-1 2-2 2-4 2 2 2 4 2 7 0 3-2 6-6 6Z"/>'`
- feature entry `el_steam` — src/ui/v2/shell.ts:224: `el_steam: '<path d="M8 20c-2-2 0-4-1-6s-2-3 0-5M12 20c-2-2 0-4-1-6s-2-3 0-5M16 20c-2-2 0-4-1-6s-2-3 0-5"/>'`
- feature entry `el_dust` — src/ui/v2/shell.ts:225: `el_dust: '<path d="M3 17c4 0 5-4 9-4s5 4 9 4"/><circle cx="7" cy="9" r="1"/><circle cx="12" cy="7" r="1"/><circle cx="17" cy="9" r="1"/><circle cx="10" cy="4" r`
- feature entry `el_soot` — src/ui/v2/shell.ts:226: `el_soot: '<circle cx="8" cy="10" r="2.5" fill="currentColor" fill-opacity=".5"/><circle cx="15" cy="8" r="2" fill="currentColor" fill-opacity=".5"/><circle cx="`
- feature entry `el_sparks` — src/ui/v2/shell.ts:227: `el_sparks: '<path d="M12 3v4M12 17v4M3 12h4M17 12h4M5.6 5.6l2.8 2.8M15.6 15.6l2.8 2.8M5.6 18.4l2.8-2.8M15.6 8.4l2.8-2.8"/>'`
- feature entry `el_spray` — src/ui/v2/shell.ts:228: `el_spray: '<path d="M12 21v-4"/><path d="M12 17c-3 0-5-2-5-5M12 17c3 0 5-2 5-5"/><circle cx="7" cy="7" r="1"/><circle cx="12" cy="5" r="1"/><circle cx="17" cy="`
- feature entry `tk_mixed` — src/ui/v2/shell.ts:230: `tk_mixed: '<circle cx="8" cy="10" r="4"/><path d="M8 14v6"/><path d="M16 4l4 10h-8Z"/><path d="M16 14v6"/><path d="M3 21h18"/>'`
- feature entry `tk_oak` — src/ui/v2/shell.ts:231: `tk_oak: '<circle cx="12" cy="10" r="6"/><path d="M12 16v5M9 13l3 3 3-3"/>'`
- feature entry `tk_cypress` — src/ui/v2/shell.ts:232: `tk_cypress: '<path d="M12 2c3 4 4 9 3 15H9C8 11 9 6 12 2Z"/><path d="M12 17v4"/>'`
- feature entry `tk_palm` — src/ui/v2/shell.ts:233: `tk_palm: '<path d="M12 21c0-5 1-9 0-12"/><path d="M12 9C9 6 5 6 3 8M12 9c3-3 7-3 9-1M12 9c-1-3-4-5-7-5M12 9c1-3 4-5 7-5"/>'`
- shared entry `trash` — src/ui/v2/shell.ts:234: `trash: '<path d="M3 6h18M8 6V4h8v2M6 6l1 15h10l1-15"/><path d="M10 10v7M14 10v7"/>'`
- feature entry `lay` — src/ui/v2/shell.ts:1580: `lay: 'brush'`
- feature entry `eraseKind` — src/ui/v2/shell.ts:1580: `eraseKind: 'eraser'`
- feature entry `eraseAll` — src/ui/v2/shell.ts:1580: `eraseAll: 'trash'`
- feature entry `stones` — src/world/elements.ts:46: `stones: { density: 2, size: 1.2, variation: 60, spacing: 2, strength: 60, intensity: 50 }`
- feature entry `pebbles` — src/world/elements.ts:47: `pebbles: { density: 30, size: 0.3, variation: 50, spacing: 0.4, strength: 60, intensity: 50 }`
- feature entry `gravel` — src/world/elements.ts:48: `gravel: { density: 120, size: 0.12, variation: 40, spacing: 0.15, strength: 60, intensity: 50 }`
- feature entry `leaves` — src/world/elements.ts:49: `leaves: { density: 6, size: 1.6, variation: 40, spacing: 0.8, strength: 60, intensity: 50 }`
- feature entry `grass` — src/world/elements.ts:50: `grass: { density: 40, size: 0.5, variation: 40, spacing: 0.3, strength: 60, intensity: 50 }`
- feature entry `tallGrass` — src/world/elements.ts:51: `tallGrass: { density: 20, size: 0.9, variation: 40, spacing: 0.5, strength: 60, intensity: 50 }`
- feature entry `scrub` — src/world/elements.ts:52: `scrub: { density: 6, size: 1.2, variation: 50, spacing: 1, strength: 60, intensity: 50 }`
- feature entry `fern` — src/world/elements.ts:53: `fern: { density: 4, size: 1.1, variation: 40, spacing: 1, strength: 60, intensity: 50 }`
- feature entry `clover` — src/world/elements.ts:54: `clover: { density: 30, size: 0.25, variation: 40, spacing: 0.2, strength: 60, intensity: 50 }`
- feature entry `flowers` — src/world/elements.ts:55: `flowers: { density: 12, size: 0.5, variation: 40, spacing: 0.4, strength: 60, intensity: 50 }`
- feature entry `mushrooms` — src/world/elements.ts:56: `mushrooms: { density: 6, size: 0.25, variation: 50, spacing: 0.3, strength: 60, intensity: 50 }`
- feature entry `smoke` — src/world/elements.ts:57: `smoke: { density: 0.3, size: 3, variation: 30, spacing: 8, strength: 100, intensity: 60 }`
- feature entry `fire` — src/world/elements.ts:58: `fire: { density: 0.3, size: 2, variation: 30, spacing: 4, strength: 100, intensity: 60 }`
- feature entry `steam` — src/world/elements.ts:59: `steam: { density: 0.3, size: 2, variation: 30, spacing: 6, strength: 100, intensity: 50 }`
- feature entry `dust` — src/world/elements.ts:60: `dust: { density: 0.3, size: 4, variation: 30, spacing: 8, strength: 100, intensity: 40 }`
- feature entry `soot` — src/world/elements.ts:61: `soot: { density: 0.3, size: 3, variation: 30, spacing: 8, strength: 100, intensity: 50 }`
- feature entry `sparks` — src/world/elements.ts:62: `sparks: { density: 0.3, size: 1, variation: 30, spacing: 4, strength: 100, intensity: 50 }`
- feature entry `spray` — src/world/elements.ts:63: `spray: { density: 0.5, size: 2, variation: 30, spacing: 4, strength: 100, intensity: 50 }`
- shared entry `tree` — src/world/landscape.ts:58: `tree: TREE_PIT / 2`
- shared entry `shrub` — src/world/landscape.ts:59: `shrub: m(0.35)`
- shared entry `bench` — src/world/landscape.ts:60: `bench: Math.hypot(m(0.9), m(0.26))`
- shared entry `bin` — src/world/landscape.ts:61: `bin: m(0.33)`
- shared entry `lamp` — src/world/landscape.ts:62: `lamp: m(0.13)`
- shared entry `hydrant` — src/world/landscape.ts:63: `hydrant: m(0.16)`
- shared entry `postbox` — src/world/landscape.ts:64: `postbox: m(0.33)`
- shared entry `phone` — src/world/landscape.ts:65: `phone: m(0.45)`
- shared entry `drain` — src/world/landscape.ts:66: `drain: m(0.5)`
- shared entry `meadow` — src/world/landscape.ts:67: `meadow: m(2)`
- shared entry `sign` — src/world/landscape.ts:68: `sign: m(0.15)`
- shared entry `streetname` — src/world/landscape.ts:69: `streetname: m(1)`
- shared style `grid` — src/peopleLab/peopleLab.css:61: `.grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; }`
- shared style `side` — src/peopleLab/peopleLab.css:142: `.side { background: var(--panel); border-left: 1px solid var(--line); display: flex; flex-direction: column; padding: 14px; gap: 8px; }`
- shared style `side` — src/peopleLab/peopleLab.css:143: `.side h2 { margin: 0 0 2px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `card` — src/peopleLab/peopleLab.css:146: `.card { position: relative; border-radius: var(--radius); background: var(--panel-2); border: 1px solid var(--line); cursor: pointer; padding: 3px; text-align: `
- shared style `card` — src/peopleLab/peopleLab.css:147: `.card img { width: 100%; aspect-ratio: 1; object-fit: cover; border-radius: 6px; background: #2a2f37; display: block; margin-bottom: 3px; }`
- shared style `card` — src/peopleLab/peopleLab.css:148: `.card.on { border-color: var(--accent); color: var(--ink); }`
- shared style `card` — src/peopleLab/peopleLab.css:149: `.card .x { position: absolute; top: 5px; right: 5px; width: 18px; height: 18px; border-radius: 4px; border: 0; background: rgba(0,0,0,.55); color: #fff; cursor:`
- shared style `card` — src/peopleLab/peopleLab.css:150: `.card:hover .x { opacity: 1; }`
- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `
- shared style `xs-label` — src/ui/roads/roads.css:61: `.xs-label { fill: #e8f0ee; font: 600 12.5px Inter, ui-sans-serif, sans-serif; font-variant-numeric: tabular-nums; }`
- shared style `xs-glyph` — src/ui/roads/roads.css:63: `.xs-glyph { fill: #f2efe6; font: 800 13px Inter, ui-sans-serif, sans-serif; }`
- shared style `v2-icon` — src/ui/v2/shell.css:41: `.v2-icon > span, .v2-pill > span, .v2-seg-b > span, .v2-tabs.compact .v2-tab.icon:not(.on) > span, .v2-tabs.tight .v2-tab.icon > span, .v2-choice.icon > span, .`
- shared style `v2-icon` — src/ui/v2/shell.css:69: `.v2-pill, .v2-icon { position: relative; display: inline-flex; align-items: center; justify-content: center; gap: 6px; width: 32px; height: 32px; padding: 0; fl`
- shared style `v2-icon` — src/ui/v2/shell.css:74: `.v2-pill:hover, .v2-icon:hover:not(:disabled) { background: var(--v-raise-2); border-color: var(--v-accent-line); }`
- shared style `v2-icon` — src/ui/v2/shell.css:75: `.v2-icon.on, .v2-pill.on { color: var(--v-accent); border-color: var(--v-accent); background: var(--v-accent-soft); }`

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

### src/render/carBody.ts

- `src/render/carBody.ts#buildCarModel` (function, line 382) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/carBody.ts#carStyleOf` (function, line 297) ← call from `src/render/agents.ts#createAgentMeshes.bodyKey`
- `src/render/carBody.ts#carStylesFor` (function, line 306) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/carBody.ts#UPRIGHT_CUSHION_REACH` (variable, line 61) ← read from `src/render/vehicleModels.ts#seatShell`

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

### src/render/riggedCitizens.ts

- `src/render/riggedCitizens.ts#ClipIdentity` (interface, line 141) ← read from `src/render/agents.ts#createAgentMeshes.placeCarOccupants`
- `src/render/riggedCitizens.ts#ClipIdentity` (interface, line 141) ← typeRef from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.seatIdentity`
- `src/render/riggedCitizens.ts#ClipIdentity` (interface, line 141) ← write from `src/render/agents.ts#createAgentMeshes.seatIdentity`
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) ← call from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) ← read from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) ← typeRef from `src/render/agents.ts#AgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.begin` (method, line 944) ← call from `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/riggedCitizens.ts#createRiggedCitizens.capturedPose` (method, line 1194) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.census` (method, line 1137) ← call from `src/render/agents.ts#createAgentMeshes.census`
- `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose` (method, line 1224) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.dispose` (method, line 1275) ← call from `src/render/agents.ts#createAgentMeshes.dispose`
- `src/render/riggedCitizens.ts#createRiggedCitizens.draw` (method, line 967) ← call from `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip` (method, line 1060) ← call from `src/render/agents.ts#createAgentMeshes.placeCarOccupants`, `src/render/agents.ts#createAgentMeshes.placeKerbPerson`, `src/render/agents.ts#createAgentMeshes.placeKerbPerson.walkTo`, `src/render/agents.ts#createAgentMeshes.placeRiderFigure`
- `src/render/riggedCitizens.ts#createRiggedCitizens.drawPalette` (method, line 1261) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.finish` (method, line 1143) ← call from `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/riggedCitizens.ts#createRiggedCitizens.forgetCompany` (method, line 1254) ← call from `src/render/agents.ts#createAgentMeshes.forgetGone`
- `src/render/riggedCitizens.ts#createRiggedCitizens.indexFor` (method, line 1242) ← call from `src/render/agents.ts#createAgentMeshes.driverBody`
- `src/render/riggedCitizens.ts#createRiggedCitizens.keepAll` (method, line 1246) ← call from `src/render/agents.ts#createAgentMeshes.keepPeople`
- `src/render/riggedCitizens.ts#createRiggedCitizens.load` (function, line 514) ← indirectCall from `src/render/citizenBake.ts#bake`
- `src/render/riggedCitizens.ts#createRiggedCitizens.loadedIndices` (method, line 1221) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.renderPalettes` (method, line 1185) ← call from `src/render/agents.ts#createAgentMeshes.renderPalettes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.skeletonOf` (method, line 1204) ← call from `src/render/agents.ts#createAgentMeshes`

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

### src/ui/toolChoices.ts

- `src/ui/toolChoices.ts#blockGridChoice` (variable, line 208) ← read from `src/main.ts#drawOverlayScreen`, `src/main.ts#pointerDown`, `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/toolChoices.ts#blockGridChoice` (variable, line 208) ← write from `src/main.ts#pointerDown`, `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/toolChoices.ts#CLOUD_MODES` (variable, line 125) ← read from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#CLOUD_MODES` (variable, line 125) ← typeRef from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#cloudBrush` (function, line 150) ← call from `src/main.ts#cloudTool`, `src/main.ts#MAP_ACTIONS`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#CloudBrush` (interface, line 135) ← read from `src/main.ts#MAP_ACTIONS`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#cloudMode` (function, line 128) ← call from `src/main.ts#cloudTool`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#elementBrush` (function, line 113) ← call from `src/main.ts#dabSettings`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#elementKind` (function, line 110) ← call from `src/main.ts#dabSettings`, `src/ui/v2/shell.ts#mountShell.elementRows`, `src/ui/v2/shell.ts#mountShell.landTabOf`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#elementMode` (function, line 111) ← call from `src/main.ts#dabSettings`, `src/ui/v2/shell.ts#mountShell.elementRows`
- `src/ui/toolChoices.ts#fogBrush` (function, line 174) ← call from `src/main.ts#dabSettings`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#FogBrush` (interface, line 159) ← read from `src/main.ts#dabSettings`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#fogErase` (function, line 67) ← call from `src/main.ts#dabSettings`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#gullyErase` (function, line 95) ← call from `src/main.ts#dabSettings`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#onRoadGridChange` (function, line 233) ← call from `src/main.ts#<ExpressionStatement>@1985`
- `src/ui/toolChoices.ts#paintKind` (function, line 186) ← call from `src/main.ts#dabSettings`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#POLE_TOOL_MODES` (variable, line 52) ← read from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#poleLampMode` (function, line 42) ← call from `src/main.ts#poleTool`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#poleToolMode` (function, line 56) ← call from `src/main.ts#drawPolePlan`, `src/main.ts#poleTool`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#roadGridShown` (function, line 226) ← call from `src/main.ts#drawOverlayScreen`, `src/main.ts#roadTool`, `src/ui/v2/shell.ts#mountShell`
- `src/ui/toolChoices.ts#roadGridShown` (function, line 226) ← reference from `src/ui/v2/shell.ts#mountShell`
- `src/ui/toolChoices.ts#roadWidth` (function, line 199) ← call from `src/main.ts#drawOverlayScreen`, `src/main.ts#roadTool`, `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/toolChoices.ts#setCloudBrush` (function, line 153) ← call from `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#setCloudMode` (function, line 131) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setElementBrush` (function, line 116) ← call from `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#setElementKind` (function, line 120) ← call from `src/ui/v2/shell.ts#mountShell.useElement`
- `src/ui/toolChoices.ts#setElementMode` (function, line 112) ← call from `src/ui/v2/shell.ts#mountShell.elementRows`
- `src/ui/toolChoices.ts#setFogBrush` (function, line 177) ← call from `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#setFogErase` (function, line 70) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setGullyErase` (function, line 98) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setPaintKind` (function, line 190) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setPoleLampMode` (function, line 46) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setPoleToolMode` (function, line 61) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setRoadGridShown` (function, line 227) ← call from `src/ui/v2/shell.ts#mountShell`
- `src/ui/toolChoices.ts#setRoadWidth` (function, line 203) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/toolChoices.ts#setStreetscapeKind` (function, line 34) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setTreeBrush` (function, line 88) ← call from `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#setTreeKind` (function, line 86) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setTreeMode` (function, line 84) ← call from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#setZoneColoursShown` (function, line 220) ← call from `src/ui/v2/shell.ts#mountShell`
- `src/ui/toolChoices.ts#signChoice` (variable, line 211) ← read from `src/main.ts#streetscapeTool`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#signChoice` (variable, line 211) ← typeRef from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#signChoice` (variable, line 211) ← write from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#streetscapeKind` (function, line 30) ← call from `src/main.ts#drawStreetscapeHover`, `src/main.ts#streetscapeTool`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#strikeChoice` (variable, line 214) ← read from `src/main.ts#actions`, `src/main.ts#frame`, `src/main.ts#pointerDown`, `src/ui/v2/shell.ts#mountShell.renderSimple`
- `src/ui/toolChoices.ts#strikeChoice` (variable, line 214) ← write from `src/ui/v2/shell.ts#mountShell.renderSimple`
- `src/ui/toolChoices.ts#TREE_MODES` (variable, line 75) ← read from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#TREE_MODES` (variable, line 75) ← typeRef from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#treeBrush` (function, line 87) ← call from `src/main.ts#dabSettings`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#treeKind` (function, line 85) ← call from `src/main.ts#dabSettings`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#treeMode` (function, line 83) ← call from `src/main.ts#dabSettings`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/ui/toolChoices.ts#zoneColoursShown` (function, line 219) ← call from `src/main.ts#drawOverlayScreen`, `src/ui/v2/shell.ts#mountShell`
- `src/ui/toolChoices.ts#zoneColoursShown` (function, line 219) ← reference from `src/ui/v2/shell.ts#mountShell`

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

### src/core/hitch.ts

- `src/core/hitch.ts#measureHitch` (function, line 2) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`

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

- `src/core/units.ts#m` (function, line 6) — call by 114: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.explodeAt.throwAboard`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`, …
- `src/core/units.ts#perM` (function, line 9) — call by 1: `src/render/riggedCitizens.ts#citizenBandAt`
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

### src/people/spec.ts

- `src/people/spec.ts#PersonSpec` (interface, line 7) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/people/spec.ts#PersonSpec` (interface, line 7) — typeRef by 1: `src/render/bakePool.ts#buildPersonInWorker`

### src/render/agents.ts

- `src/render/agents.ts#AgentMeshes` (interface, line 168) — call by 4: `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.dispose`, `src/render/playEffects.ts#createPlayEffects.dropVehicle`, `src/render/playEffects.ts#createPlayEffects.explode`
- `src/render/agents.ts#AgentMeshes` (interface, line 168) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#AgentMeshes` (interface, line 168) — typeRef by 1: `src/render/playEffects.ts#PlayEffectsContext`
- `src/render/agents.ts#BodyPlan` (interface, line 379) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#CarParts` (interface, line 621) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#createAgentMeshes` (function, line 552) — implementation by 12: `src/render/playEffects.ts#createPlayEffects.frame`, `src/render/ragdoll.ts#createRagdolls.build`, `src/render/ragdoll.ts#createRagdolls.captured`, `src/render/ragdoll.ts#createRagdolls.detach`, …
- `src/render/agents.ts#createAgentMeshes` (function, line 552) — indirectCall by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/agents.ts#createAgentMeshes` (function, line 552) — read by 2: `src/render/agents.ts#createAgentMeshes.carcass`, `src/render/agents.ts#createAgentMeshes.setBleed`
- `src/render/agents.ts#createAgentMeshes` (function, line 552) — write by 1: `src/render/agents.ts#createAgentMeshes.setBleed`
- `src/render/agents.ts#createAgentMeshes.bodyKey` (function, line 651) — call by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#planOf` (function, line 438) — call by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#SIDES` (variable, line 112) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/agents.ts#TwoWheelerParts` (interface, line 817) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`

### src/render/bakePool.ts

- `src/render/bakePool.ts#releaseIfIdle` (function, line 37) — call by 1: `src/render/bakePool.ts#releaseIdleBakeWorkers`
- `src/render/bakePool.ts#releaseRequested` (variable, line 21) — write by 1: `src/render/bakePool.ts#releaseIdleBakeWorkers`
- `src/render/bakePool.ts#run` (function, line 127) — call by 4: `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/bakePool.ts#buildPersonInWorker`, `src/render/bakePool.ts#uncookInWorker`, `src/render/bakePool.ts#bakeInWorker`
- `src/render/bakePool.ts#usable` (function, line 20) — call by 1: `src/render/bakePool.ts#bakeInWorker`

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

### src/render/citizenBake.ts

- `src/render/citizenBake.ts#afterFrame` (function, line 94) — reference by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.personAsset`
- `src/render/citizenBake.ts#bake` (function, line 400) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/citizenBake.ts#CARRY_AT` (variable, line 47) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/citizenBake.ts#CitizenClipKey` (type, line 51) — typeRef by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#ClipFrames` (interface, line 55) — read by 9: `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`, …
- `src/render/citizenBake.ts#ClipFrames` (interface, line 55) — typeRef by 16: `src/render/riggedCitizens.ts#CitizenBatch`, `src/render/riggedCitizens.ts#createRiggedCitizens`, `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`, …
- `src/render/citizenBake.ts#Deferred` (type, line 347) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#Deferred` (type, line 347) — typeRef by 1: `src/render/riggedCitizens.ts#CitizenBatch`
- `src/render/citizenBake.ts#deferredClips` (function, line 367) — callbackAssign by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#deferredClips` (function, line 367) — implementation by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#deferredClips.carried` (function, line 381) — callbackAssign by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#deferredClips.carried` (function, line 381) — implementation by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#GAIT_AT` (variable, line 34) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/citizenBake.ts#gaitClips`
- `src/render/citizenBake.ts#LIBRARY_AT` (variable, line 28) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#Played` (type, line 27) — typeRef by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#RIDER_AT` (variable, line 40) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#RigNode` (interface, line 455) — typeRef by 4: `src/render/riggedCitizens.ts#CitizenBatch`, `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/bakePool.ts#bakeRig`, `src/render/citizenBake.ts#rigData`
- `src/render/citizenBake.ts#ShuffleFacts` (interface, line 353) — read by 1: `src/render/bakePool.ts#bakeDeferredInWorker`
- `src/render/citizenBake.ts#ShuffleFacts` (interface, line 353) — typeRef by 1: `src/render/bakePool.ts#bakeDeferredInWorker`
- `src/render/citizenBake.ts#WALK` (variable, line 15) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#WALK_ELDER` (variable, line 17) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#WALK_SHUFFLE` (variable, line 19) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`

### src/render/citizenBake.worker.ts

- `src/render/citizenBake.worker.ts#BakedClip` (type, line 54) — read by 1: `src/render/bakePool.ts#clipsOf`
- `src/render/citizenBake.worker.ts#BakedClip` (type, line 54) — typeRef by 1: `src/render/bakePool.ts#clipsOf`
- `src/render/citizenBake.worker.ts#BakeReply` (interface, line 56) — read by 4: `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/bakePool.ts#buildPersonInWorker`, `src/render/bakePool.ts#uncookInWorker`, `src/render/bakePool.ts#bakeInWorker`
- `src/render/citizenBake.worker.ts#serve` (function, line 89) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/citizenCasting.ts

- `src/render/citizenCasting.ts#AgeBand` (type, line 10) — typeRef by 1: `src/render/citizenCasting.ts#CastingRegistry.candidates.of`
- `src/render/citizenCasting.ts#CAST_NEAR` (variable, line 145) — read by 1: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`
- `src/render/citizenCasting.ts#CitizenModel` (interface, line 13) — read by 10: `src/render/riggedCitizens.ts#createRiggedCitizens`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`, …
- `src/render/citizenCasting.ts#CitizenModel` (interface, line 13) — typeRef by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.partArraysAgain`, `src/render/riggedCitizens.ts#createRiggedCitizens.personAsset`, `src/render/citizenCasting.ts#CastingRegistry.constructor`
- `src/render/citizenCasting.ts#CODES` (variable, line 71) — read by 2: `src/render/citizenCasting.ts#codesFor`, `src/render/citizenCasting.ts#codesFor.withTradition`
- `src/render/citizenCasting.ts#Company` (type, line 83) — typeRef by 9: `src/render/riggedCitizens.ts#ClipIdentity`, `src/render/riggedCitizens.ts#companyOf`, `src/render/riggedCitizens.ts#createRiggedCitizens.forgetCompany`, `src/render/citizenCasting.ts#CastingRegistry.forgetCompany`, …
- `src/render/citizenCasting.ts#CROWD` (variable, line 29) — read by 5: `src/render/riggedCitizens.ts#createRiggedCitizens`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`, …
- `src/render/citizenCasting.ts#CROWD_IDS` (variable, line 59) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens`, `src/render/riggedCitizens.ts#createRiggedCitizens.request`
- `src/render/citizenCasting.ts#DressCode` (type, line 69) — typeRef by 12: `src/render/citizenCasting.ts#CastingRegistry.codes`, `src/render/citizenCasting.ts#Entry`, `src/render/citizenCasting.ts#CastingRegistry.census`, `src/render/citizenCasting.ts#CastingRegistry.drawn`, …
- `src/render/citizenCasting.ts#Sex` (type, line 11) — typeRef by 1: `src/render/citizenCasting.ts#CastingContext`
- `src/render/citizenCasting.ts#Wardrobe` (type, line 9) — typeRef by 1: `src/render/citizenCasting.ts#CastingRegistry.census`

### src/render/citizenGait.ts

- `src/render/citizenGait.ts#ACCEL_TIME` (variable, line 302) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#ACTIVITY` (variable, line 387) — read by 1: `src/render/citizenGait.ts#standingKey`
- `src/render/citizenGait.ts#ADULT_WALKS` (variable, line 201) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#CARRY_WALKS` (variable, line 209) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#CLIP_FLOOR` (variable, line 290) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#Cycle` (type, line 28) — typeRef by 2: `src/render/citizenGait.ts#Gait`, `src/render/citizenGait.ts#walksOf`
- `src/render/citizenGait.ts#DRUNK_WALKS` (variable, line 204) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#ELDER_WALKS` (variable, line 202) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#FADE` (variable, line 298) — read by 4: `src/render/citizenGait.ts#createGait`, `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#play`, `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#FEET_LEAD_RATE` (variable, line 273) — read by 1: `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#GAIT_FADE` (variable, line 296) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#GaitClip` (interface, line 8) — read by 13: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`, `src/render/citizenGait.ts#standing`, `src/render/citizenGait.ts#fractionAtYaw`, …
- `src/render/citizenGait.ts#GaitClip` (interface, line 8) — typeRef by 8: `src/render/citizenGait.ts#fractionAtYaw`, `src/render/citizenGait.ts#fractionAtTravel`, `src/render/citizenGait.ts#moving.share`, `src/render/citizenGait.ts#naturalPace`, …
- `src/render/citizenGait.ts#GaitClipName` (type, line 21) — typeRef by 2: `src/render/citizenGait.ts#GaitPlay`, `src/render/citizenGait.ts#put`
- `src/render/citizenGait.ts#GaitClips` (type, line 26) — typeRef by 10: `src/render/riggedCitizens.ts#CitizenBatch`, `src/render/citizenBake.ts#gaitClips`, `src/render/citizenGait.ts#gaitPlays`, `src/render/citizenGait.ts#stepGait`, …
- `src/render/citizenGait.ts#HAND_L_WALKS` (variable, line 206) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#HAND_R_WALKS` (variable, line 207) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#JUMP` (variable, line 304) — read by 1: `src/render/citizenGait.ts#drawnVelocity`
- `src/render/citizenGait.ts#LAG_STEPS` (variable, line 275) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#RUN_TIME` (variable, line 299) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#Single` (type, line 30) — typeRef by 3: `src/render/citizenGait.ts#standing`, `src/render/citizenGait.ts#standingKey`, `src/render/citizenGait.ts#PlayKey`
- `src/render/citizenGait.ts#START_BELOW` (variable, line 288) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#STEPPED_TURN_RATE` (variable, line 271) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#STILL_BELOW` (variable, line 466) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#STOP_BELOW` (variable, line 285) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#STOP_DECEL` (variable, line 286) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#STOP_SETTLE` (variable, line 292) — read by 2: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#STRIDE_EXPONENT` (variable, line 294) — read by 2: `src/render/citizenGait.ts#moving.share`, `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#styleWalks` (variable, line 217) — read by 1: `src/render/citizenGait.ts#walksOf`
- `src/render/citizenGait.ts#styleWalks` (variable, line 217) — write by 1: `src/render/citizenGait.ts#walksOf`
- `src/render/citizenGait.ts#TURN_FADE` (variable, line 297) — read by 1: `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#TURN_FOOT` (variable, line 283) — read by 2: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#TURN_IN_PLACE` (variable, line 277) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#TURN_SETTLE` (variable, line 279) — read by 1: `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#TURN_STEPS` (variable, line 267) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#TURN_STEPS_STANDING` (variable, line 269) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#TURN_WINDUP` (variable, line 281) — read by 1: `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#VELOCITY_TIME` (variable, line 301) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#WALK_STYLES` (variable, line 212) — read by 1: `src/render/citizenGait.ts#walkStyle`
- `src/render/citizenGait.ts#WalkStyle` (type, line 213) — typeRef by 3: `src/render/citizenGait.ts#pace`, `src/render/citizenGait.ts#walkStyle`, `src/render/citizenGait.ts#walksOf`

### src/render/citizenPalette.ts

- `src/render/citizenPalette.ts#blendPackedFrames` (function, line 41) — call by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose`, `src/render/citizenPlays.ts#cpuPose`
- `src/render/citizenPalette.ts#PACKED_BONE_FLOATS` (variable, line 2) — read by 5: `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose`, `src/render/citizenPlays.ts#cpuPose`, `src/render/citizenPalette.ts#copyPackedFrame`, `src/render/citizenPlays.ts#citizenAtlas`, …
- `src/render/citizenPalette.ts#SKIN_BONE_FLOATS` (variable, line 3) — read by 5: `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`, `src/render/riggedCitizens.ts#createRiggedCitizens.maim`, `src/render/citizenPlays.ts#cpuPose`, …

### src/render/citizenPlays.ts

- `src/render/citizenPlays.ts#ATLAS_SLACK` (variable, line 26) — read by 1: `src/render/citizenPlays.ts#citizenAtlas`
- `src/render/citizenPlays.ts#ATLAS_TEXELS` (variable, line 24) — read by 2: `src/render/citizenPlays.ts#citizenAtlas`, `src/render/citizenPlays.ts#appendClip`
- `src/render/citizenPlays.ts#GPU_PLAYS` (variable, line 9) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens`, `src/render/citizenPlays.ts#gpuPlays`

### src/render/citizenWalk.ts

- `src/render/citizenWalk.ts#loadRocketboxClips` (function, line 371) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/citizenWalk.ts#loadRocketboxClips` (function, line 371) — typeRef by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/citizenWalk.ts#WalkSex` (type, line 77) — typeRef by 3: `src/render/riggedCitizens.ts#CitizenBatch`, `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/bakePool.ts#bakeInWorker`

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

### src/render/groundShear.ts

- `src/render/groundShear.ts#Gradient` (interface, line 4) — read by 1: `src/render/groundShear.ts#shearMatrix`
- `src/render/groundShear.ts#Gradient` (interface, line 4) — typeRef by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, `src/render/groundShear.ts#shearMatrix`

### src/render/instanceBounds.ts

- `src/render/instanceBounds.ts#fitInstances` (function, line 5) — call by 3: `src/render/gore.ts#createGore.update`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/people/heldProps.ts#createHeldProps.put`

### src/render/interiorSolids.ts

- `src/render/interiorSolids.ts#CabinParts` (class, line 44) — construct by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/interiorSolids.ts#CabinParts.add` (method, line 49) — call by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/interiorSolids.ts#CabinParts.geometries` (field, line 45) — read by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/interiorSolids.ts#CabinParts.solids` (method, line 59) — call by 1: `src/render/carBody.ts#buildCarModel`

### src/render/isoViewport.ts

- `src/render/isoViewport.ts#createIsoRig` (function, line 78) — implementation by 3: `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`

### src/render/landStroke.worker.ts

- `src/render/landStroke.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/mesh/gpuOnly.ts

- `src/render/mesh/gpuOnly.ts#fillArrays` (function, line 37) — call by 1: `src/render/riggedCitizens.ts#keepPartOnGpu`
- `src/render/mesh/gpuOnly.ts#GeometryArrays` (type, line 6) — typeRef by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.partArraysAgain`, `src/render/riggedCitizens.ts#keepPartOnGpu`
- `src/render/mesh/gpuOnly.ts#gpuOnlyAttributes` (function, line 22) — call by 1: `src/render/riggedCitizens.ts#keepPartOnGpu`

### src/render/mesh/weld.ts

- `src/render/mesh/weld.ts#weldGeometry` (function, line 5) — call by 1: `src/render/carBody.ts#merge`

### src/render/networkFarRoads.worker.ts

- `src/render/networkFarRoads.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/people/cartoon.ts

- `src/render/people/cartoon.ts#CartoonStyle` (interface, line 5) — typeRef by 2: `src/render/riggedCitizens.ts#peopleStyle`, `src/render/bakePool.ts#buildPersonInWorker`
- `src/render/people/cartoon.ts#PIXAR` (variable, line 20) — read by 1: `src/render/riggedCitizens.ts#peopleStyle`
- `src/render/people/cartoon.ts#REALISTIC` (variable, line 17) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.personAsset`, `src/render/riggedCitizens.ts#peopleStyle`

### src/render/people/cookedPerson.ts

- `src/render/people/cookedPerson.ts#assemblePerson` (function, line 205) — call by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.personAsset`, `src/render/people/cookedLoad.ts#loadCookedPerson`, `src/render/people/cookedPerson.ts#uncookPerson`
- `src/render/people/cookedPerson.ts#cookPerson` (function, line 51) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens`
- `src/render/people/cookedPerson.ts#decodeCookedPerson` (function, line 149) — call by 1: `src/render/people/cookedPerson.ts#uncookPerson`
- `src/render/people/cookedPerson.ts#decodeCookedPerson` (function, line 149) — read by 1: `src/render/people/cookedPerson.ts#uncookPerson`
- `src/render/people/cookedPerson.ts#DecodedPerson` (interface, line 146) — typeRef by 2: `src/render/bakePool.ts#buildPersonInWorker`, `src/render/bakePool.ts#uncookInWorker`

### src/render/people/crowdAnimation.ts

- `src/render/people/crowdAnimation.ts#ClassAnimation` (interface, line 36) — read by 4: `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`, `src/render/riggedCitizens.ts#paletteBytes`, `src/render/citizenPlays.ts#appendClip`
- `src/render/people/crowdAnimation.ts#ClassAnimation` (interface, line 36) — typeRef by 2: `src/render/citizenPlays.ts#CitizenAtlas`, `src/render/citizenPlays.ts#animationOf`
- `src/render/people/crowdAnimation.ts#createPalettePass` (function, line 117) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/people/crowdAnimation.ts#createPalettePass` (function, line 117) — read by 2: `src/render/people/crowdAnimation.ts#createPalettePass.dispose`, `src/render/people/crowdAnimation.ts#createPalettePass.setAtlas`
- `src/render/people/crowdAnimation.ts#createPalettePass` (function, line 117) — write by 1: `src/render/people/crowdAnimation.ts#createPalettePass.setAtlas`
- `src/render/people/crowdAnimation.ts#createPalettePass.grow` (method, line 312) — implementation by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.grow`
- `src/render/people/crowdAnimation.ts#createPalettePass.render` (method, line 276) — implementation by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.renderPalettes`
- `src/render/people/crowdAnimation.ts#createPalettePass.setOverride` (method, line 272) — implementation by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.drawPalette`, `src/render/riggedCitizens.ts#createRiggedCitizens.pose`
- `src/render/people/crowdAnimation.ts#createPalettePass.setPlays` (method, line 252) — implementation by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.drawPalette`, `src/render/riggedCitizens.ts#createRiggedCitizens.pose`
- `src/render/people/crowdAnimation.ts#MOST_ROW_RANGES` (variable, line 28) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.finish`
- `src/render/people/crowdAnimation.ts#PalettePass` (interface, line 93) — call by 6: `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawPalette`, `src/render/riggedCitizens.ts#createRiggedCitizens.grow`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`, …
- `src/render/people/crowdAnimation.ts#PalettePass` (interface, line 93) — read by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.grow`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`, `src/render/riggedCitizens.ts#createRiggedCitizens.renderPalettes`
- `src/render/people/crowdAnimation.ts#PalettePass` (interface, line 93) — typeRef by 1: `src/render/riggedCitizens.ts#CitizenBatch`

### src/render/people/faceExpression.ts

- `src/render/people/faceExpression.ts#CHANNELS` (variable, line 7) — read by 1: `src/render/people/faceExpression.ts#applyFace`
- `src/render/people/faceExpression.ts#faceAt` (function, line 57) — call by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/people/faceExpression.ts#FaceWeights` (type, line 4) — typeRef by 5: `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, `src/render/riggedCitizens.ts#createRiggedCitizens.place`, `src/render/riggedCitizens.ts#NEUTRAL_FACE`, `src/render/riggedCitizens.ts#setFacialExpression`, …
- `src/render/people/faceExpression.ts#GAZE` (variable, line 22) — read by 1: `src/render/people/faceExpression.ts#applyFace`

### src/render/people/heldProps.ts

- `src/render/people/heldProps.ts#CAPACITY` (variable, line 89) — read by 2: `src/render/people/heldProps.ts#createHeldProps.place`, `src/render/people/heldProps.ts#createHeldProps.placeMatrix`
- `src/render/people/heldProps.ts#createHeldProps` (function, line 101) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens`
- `src/render/people/heldProps.ts#createHeldProps` (function, line 101) — read by 3: `src/render/people/heldProps.ts#createHeldProps.place`, `src/render/people/heldProps.ts#createHeldProps.placeMatrix`, `src/render/people/heldProps.ts#createHeldProps.begin`
- `src/render/people/heldProps.ts#createHeldProps` (function, line 101) — write by 3: `src/render/people/heldProps.ts#createHeldProps.place`, `src/render/people/heldProps.ts#createHeldProps.placeMatrix`, `src/render/people/heldProps.ts#createHeldProps.begin`
- `src/render/people/heldProps.ts#HELD` (variable, line 13) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/people/heldProps.ts#HeldProps` (interface, line 91) — call by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.begin`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/people/heldProps.ts#HeldProps` (interface, line 91) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens`
- `src/render/people/heldProps.ts#UP` (variable, line 87) — read by 1: `src/render/people/heldProps.ts#createHeldProps.place`
- `src/render/people/heldProps.ts#UPRIGHT` (variable, line 81) — read by 1: `src/render/people/heldProps.ts#createHeldProps.place`

### src/render/people/lodWorker.ts

- `src/render/people/lodWorker.ts#serve` (function, line 82) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/people/paletteTargets.ts

- `src/render/people/paletteTargets.ts#freeSlab` (function, line 75) — call by 1: `src/render/people/crowdAnimation.ts#createPalettePass.dispose`

### src/render/people/personBuild.ts

- `src/render/people/personBuild.ts#buildPersonScene` (function, line 20) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.personAsset`

### src/render/people/skinAppearance.ts

- `src/render/people/skinAppearance.ts#applySkinAppearance` (function, line 377) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/people/skinAppearance.ts#createPersonLayers` (function, line 41) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens`
- `src/render/people/skinAppearance.ts#loadSkinAppearance` (function, line 292) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.personAsset.dressed`
- `src/render/people/skinAppearance.ts#releaseSkinAppearance` (function, line 369) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.personAsset.dressed`
- `src/render/people/skinAppearance.ts#SkinAppearance` (interface, line 12) — typeRef by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`

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

### src/render/riderPoses.ts

- `src/render/riderPoses.ts#CAB_WHEEL` (variable, line 63) — read by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/riderPoses.ts#DRIVER_WHEEL` (variable, line 60) — read by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/riderPoses.ts#NO_HELMET` (variable, line 319) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.helmetFits`
- `src/render/riderPoses.ts#RiderClipKey` (type, line 5) — typeRef by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/riderPoses.ts#WheelSpec` (interface, line 52) — read by 1: `src/render/carBody.ts#buildCarModel`

### src/render/roadStations.worker.ts

- `src/render/roadStations.worker.ts#serve` (function, line 15) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/roadTile.worker.ts

- `src/render/roadTile.worker.ts#serve` (function, line 102) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/terrainLight.worker.ts

- `src/render/terrainLight.worker.ts#serve` (function, line 5) — callbackAssign by 1: `src/render/buildings/fracture.worker.ts#serve`

### src/render/uploads.ts

- `src/render/uploads.ts#cancelUploads` (function, line 17) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.disposeOwned`
- `src/render/uploads.ts#compileAhead` (function, line 242) — call by 2: `src/render/playEffects.ts#createPlayEffects`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/uploads.ts#shadowStandIn` (function, line 75) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/uploads.ts#warmAhead` (function, line 440) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`

### src/render/vehicleModels.ts

- `src/render/vehicleModels.ts#DoorModel` (interface, line 31) — read by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/vehicleModels.ts#DoorModel` (interface, line 31) — typeRef by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/vehicleModels.ts#DoorSide` (type, line 29) — typeRef by 5: `src/render/carBody.ts#buildCarModel`, `src/render/carBody.ts#buildCarModel.rowOf`, `src/render/carBody.ts#buildCarModel.seatAt`, `src/render/carBody.ts#seatIndex`, …
- `src/render/vehicleModels.ts#Lamp` (interface, line 153) — typeRef by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/vehicleModels.ts#merge` (function, line 221) — call by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/vehicleModels.ts#SeatModel` (interface, line 53) — read by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/vehicleModels.ts#SeatModel` (interface, line 53) — typeRef by 2: `src/render/carBody.ts#buildCarModel`, `src/render/carBody.ts#buildCarModel.seatAt`
- `src/render/vehicleModels.ts#TwoWheelerModel` (interface, line 535) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/vehicleModels.ts#VehicleModel` (interface, line 119) — read by 1: `src/render/agents.ts#createAgentMeshes.carcass`
- `src/render/vehicleModels.ts#VehicleModel` (interface, line 119) — typeRef by 1: `src/render/carBody.ts#buildCarModel`

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

- `src/sim/dt.ts#DT` (variable, line 2) — read by 4: `src/sim/agents/walk.ts#movePlayerWalker`, `src/sim/agents/walk.ts#stepWalkers`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/citizenGait.ts#drawnVelocity`

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
- `src/sim/people/view.ts#GestureView` (interface, line 32) — read by 4: `src/render/playEffects.ts#createPlayEffects.frame`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#standingKey`
- `src/sim/people/view.ts#GestureView` (interface, line 32) — write by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/people/view.ts#PartyView` (interface, line 11) — read by 9: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#finish`, `src/sim/agents/walk.ts#joinParty`, `src/sim/agents/walk.ts#mourn`, …
- `src/sim/people/view.ts#PartyView` (interface, line 11) — typeRef by 2: `src/sim/people/casualties.ts#Casualty`, `src/render/riggedCitizens.ts#companyOf`
- `src/sim/people/view.ts#PedView` (interface, line 55) — read by 15: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootAt`, `src/render/playEffects.ts#createPlayEffects.frame`, `src/sim/agents/walk.ts#createAgentWalkEngine`, …
- `src/sim/people/view.ts#PedView` (interface, line 55) — typeRef by 11: `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`, `src/sim/agents/walk.ts#Walker`, `src/render/riggedCitizens.ts#createRiggedCitizens`, …
- `src/sim/people/view.ts#PedView` (interface, line 55) — write by 2: `src/sim/agents/walk.ts#publish`, `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/view.ts#PersonAgeClass` (type, line 6) — typeRef by 3: `src/sim/people/casualties.ts#Casualty`, `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/view.ts#PersonGender` (type, line 7) — typeRef by 3: `src/sim/people/casualties.ts#Casualty`, `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/view.ts#personHash` (function, line 105) — call by 8: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#frighten`, `src/sim/agents/walk.ts#mourn`, `src/sim/agents/walk.ts#startWalk`, …
- `src/sim/people/view.ts#Severable` (type, line 51) — typeRef by 15: `src/render/playEffects.ts#PlayEffects`, `src/render/ragdoll.ts#createRagdolls`, `src/render/ragdoll.ts#createRagdolls.detach`, `src/render/ragdoll.ts#createRagdolls.fling`, …
- `src/sim/people/view.ts#SIT_DOWN_SECONDS` (variable, line 45) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/sim/people/view.ts#STAND_UP_SECONDS` (variable, line 46) — read by 1: `src/render/citizenGait.ts#stepGait`

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

- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) — read by 14: `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootVehicle`, `src/actionsWiring.ts#VEHICLE_HALF_SPAN`, `src/render/playEffects.ts#createPlayEffects`, …
- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) — typeRef by 7: `src/render/playEffects.ts#PlayEffects`, `src/render/carBody.ts#axlesOf`, `src/render/carBody.ts#buildCarModel`, `src/render/carBody.ts#carShape`, …
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

### src/world/elements.ts

- `src/world/elements.ts#DEFAULT_BRUSH` (variable, line 45) — read by 1: `src/ui/toolChoices.ts#elementBrush`
- `src/world/elements.ts#ElementBrush` (interface, line 29) — typeRef by 3: `src/ui/toolChoices.ts#elementBrush`, `src/ui/toolChoices.ts#elementBrushes`, `src/ui/toolChoices.ts#setElementBrush`
- `src/world/elements.ts#ElementKind` (type, line 6) — typeRef by 5: `src/ui/toolChoices.ts#elementBrush`, `src/ui/toolChoices.ts#elementBrushes`, `src/ui/toolChoices.ts#elementKind`, `src/ui/toolChoices.ts#elementKindNow`, …

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 9: `src/sim/crossings/permission.ts#indexReservations`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`, `src/sim/peds/signalPostIndex.ts#SignalPostWorld`, …
- `src/world/ids.ts#PoleId` (type, line 10) — typeRef by 1: `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 4: `src/editor/bulldozer.ts#removeRoads`, `src/sim/agents/walk.ts#publish`, `src/world/signalPosts.ts#SignalJunctions`, `src/editor/roads/streetKit.ts#removeWithStreetKit`

### src/world/landscape.ts

- `src/world/landscape.ts#LANDSCAPE_KINDS` (variable, line 13) — read by 1: `src/ui/toolChoices.ts#kind`
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — read by 3: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/actionsWiring.ts#createActions.explodeAt`
- `src/world/landscape.ts#LandscapeKind` (type, line 14) — typeRef by 3: `src/ui/toolChoices.ts#kind`, `src/ui/toolChoices.ts#setStreetscapeKind`, `src/ui/toolChoices.ts#streetscapeKind`
- `src/world/landscape.ts#landscapeNearIn` (function, line 398) — call by 1: `src/editor/bulldozer.ts#Bulldozer.click`
- `src/world/landscape.ts#SignType` (type, line 43) — typeRef by 1: `src/ui/toolChoices.ts#signChoice`

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

### src/world/terrainPaint.ts

- `src/world/terrainPaint.ts#PAINT_KINDS` (variable, line 4) — read by 1: `src/ui/toolChoices.ts#paint`
- `src/world/terrainPaint.ts#PaintKind` (type, line 6) — typeRef by 3: `src/ui/toolChoices.ts#paint`, `src/ui/toolChoices.ts#paintKind`, `src/ui/toolChoices.ts#setPaintKind`

### src/world/transit.ts

- `src/world/transit.ts#TransitLine` (interface, line 35) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`

### src/world/trees.ts

- `src/world/trees.ts#DEFAULT_TREE_BRUSH` (variable, line 38) — read by 1: `src/ui/toolChoices.ts#treeBrushNow`
- `src/world/trees.ts#TreeBrush` (interface, line 27) — typeRef by 3: `src/ui/toolChoices.ts#setTreeBrush`, `src/ui/toolChoices.ts#treeBrush`, `src/ui/toolChoices.ts#treeBrushNow`
- `src/world/trees.ts#TreeKind` (type, line 5) — typeRef by 3: `src/ui/toolChoices.ts#setTreeKind`, `src/ui/toolChoices.ts#treeKind`, `src/ui/toolChoices.ts#treeKindNow`

### src/world/utilities.ts

- `src/world/utilities.ts#POLE_ARM_DROP` (variable, line 16) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_ARM_HALF` (variable, line 14) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_HEIGHT` (variable, line 9) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_LAMP_MODES` (variable, line 72) — read by 1: `src/ui/toolChoices.ts#lamps`
- `src/world/utilities.ts#POLE_LAMP_REACH` (variable, line 24) — read by 1: `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#PoleLampMode` (type, line 73) — typeRef by 3: `src/ui/toolChoices.ts#lamps`, `src/ui/toolChoices.ts#poleLampMode`, `src/ui/toolChoices.ts#setPoleLampMode`
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

- `es:Math` × 1034
- `three:Vector3` × 411
- `es:Array.length` × 234
- `es:Math.random` × 215
- `es:Math.max` × 203
- `es:Array.push` × 201
- `three:Vector3.x` × 188
- `three:Vector3.z` × 185
- `three:Vector3.y` × 176
- `es:Math.min` × 153
- `es:Float32Array` × 123
- `es:Map.get` × 117
- `three:Vector3.clone` × 112
- `three:Matrix4` × 81
- `es:Math.sin` × 80
- `es:Math.cos` × 76
- `es:Math.floor` × 75
- `es:Array.map` × 73
- `three:Vector3.addScaledVector` × 64
- `es:Infinity` × 59
- `es:Math.abs` × 59
- `three:Vector3.multiplyScalar` × 56
- `es:Map` × 53
- `es:Math.PI` × 53
- `es:Map.set` × 52
- `three:Vector3.subVectors` × 51
- `es:Set.add` × 50
- `three:Vector3.copy` × 49
- `es:Math.hypot` × 46
- `three:Vector3.set` × 45
- `es:ReadonlyArray.length` × 44
- `es:Set` × 44
- `three:Mesh.geometry` × 44
- `three:Vector3.add` × 43
- `es:Array.filter` × 42
- `es:Map.delete` × 42
- `three:BufferGeometry` × 42
- `three:Vector3.normalize` × 42
- `es:Record` × 41
- `three:InstancedMesh.count` × 38
- `three:BufferAttribute` × 37
- `three:BufferGeometry.setAttribute` × 34
- `three:Object3D.position` × 32
- `three:Quaternion` × 32
- `es:Promise` × 29
- `es:Set.has` × 27
- `three:InstancedMesh` × 26
- `jolt-physics:Jolt.destroy` × 25
- `three:Object3D.userData` × 25
- `es:Math.atan2` × 24
- `three:Vector3.lengthSq` × 24
- `three:BufferGeometry.dispose` × 23
- `three:BufferGeometry.getAttribute` × 23
- `three:Vector3.dot` × 23
- `three:Vector3.length` × 23
- `three:InstancedMesh.instanceMatrix` × 22
- `es:Number` × 21
- `es:WeakMap.get` × 21
- `three:Color` × 21
- `three:Object3D.visible` × 21
- `es:ReadonlyArray.map` × 19
- `es:ReadonlySet.has` × 19
- `three:Object3D.add` × 19
- `es:Map.clear` × 18
- `three:BufferAttribute.array` × 18
- `three:InstancedMesh.instanceColor` × 18
- `es:Int32Array` × 17
- `es:Math.round` × 17
- `es:Object` × 17
- `es:WeakMap.set` × 17
- `three:Object3D.name` × 17
- `three:Vector3.crossVectors` × 17
- `dom:performance` × 16
- `es:Array.forEach` × 16
- `es:Map.size` × 16
- `es:Partial` × 16
- `three:BufferAttribute.needsUpdate` × 16
- `three:Vector3.lerp` × 16
- `dom:Performance.now` × 15
- `es:Array.slice` × 15
- `es:Readonly` × 15
- `es:ReadonlyArray.includes` × 15
- `jolt-physics:Body.GetID` × 15
- `three:BufferGeometry.translate` × 15
- `three:MeshStandardMaterial` × 15
- `three:Object3D.matrix` × 15
- `three:Quaternion.setFromAxisAngle` × 15
- `three:SkinnedMesh` × 15
- `three:Vector3.distanceTo` × 15
- `es:Float32Array.length` × 14
- `es:Number.toFixed` × 14
- `three:Vector3.setY` × 14
- `three:Vector3.sub` × 14
- `dom:localStorage` × 13
- `es:Array.some` × 13
- `es:WeakMap` × 13
- `three:BufferAttribute.addUpdateRange` × 13
- `three:BufferGeometry.index` × 13
- `three:Group` × 13
- `three:Material.dispose` × 13
- `three:Matrix4.multiply` × 13
- `three:Mesh.material` × 13
- `es:Array` × 12
- `es:Array.sort` × 12
- `es:Array.splice` × 12
- `es:Map.values` × 12
- `es:NumberConstructor.isFinite` × 12
- `three:Object3D.castShadow` × 12
- `three:Object3D.scale` × 12
- `es:Array.indexOf` × 11
- `es:Float32Array.set` × 11
- `es:InstanceType` × 11
- `es:Math.sqrt` × 11
- `three:InstancedMesh.setMatrixAt` × 11
- `three:Matrix4.copy` × 11
- `dom:CanvasGradient.addColorStop` × 10
- `dom:console` × 10
- `dom:MessageEvent.data` × 10
- `es:Array.includes` × 10
- `es:Array.shift` × 10
- `es:Float32Array.subarray` × 10
- `jolt-physics:Jolt.Vec3` × 10
- `three:BufferAttribute.getX` × 10
- `three:BufferAttribute.setUsage` × 10
- `three:DynamicDrawUsage` × 10
- `three:GLTF.scene` × 10
- `three:Matrix4.compose` × 10
- `three:Object3D` × 10
- `three:Object3D.frustumCulled` × 10
- `three:Object3D.receiveShadow` × 10
- `three:SkinnedMesh.skeleton` × 10
- `three:Vector3.applyQuaternion` × 10
- `three:Vector3.divideScalar` × 10
- `es:ArrayConstructor.isArray` × 9
- `es:ReadonlyMap.get` × 9
- `es:String.replace` × 9
- `three:BufferAttribute.clearUpdateRanges` × 9
- `three:BufferGeometry.userData` × 9
- `three:InstancedMesh.setColorAt` × 9
- `three:Matrix4.multiplyMatrices` × 9
- `three:Mesh` × 9
- `es:Float32Array.fill` × 8
- `es:Float32ArrayConstructor.from` × 8
- `es:JSON` × 8
- `three:BufferGeometry.boundingBox` × 8
- `three:Texture.dispose` × 8
- `dom:Console.error` × 7
- `dom:location` × 7
- `es:ArrayBuffer` × 7
- `es:ArrayLike` × 7
- `es:Int32Array.length` × 7
- `es:Map.has` × 7
- `es:ObjectConstructor.entries` × 7
- `es:Promise.then` × 7
- `three:Box3.min` × 7
- `three:BufferAttribute.count` × 7
- `three:BufferGeometry.boundingSphere` × 7
- `three:Float32BufferAttribute` × 7
- `three:Matrix4.fromArray` × 7
- `three:Matrix4.invert` × 7
- `three:Mesh.morphTargetInfluences` × 7
- `three:Object3D.rotation` × 7
- `three:Object3D.traverse` × 7
- `three:WebGLProgramParametersWithUniforms.uniforms` × 7
- `dom:CanvasFillStrokeStyles.fillStyle` × 6
- `dom:Storage.getItem` × 6
- `es:Array.reduce` × 6
- `es:Error` × 6
- `es:Math.sign` × 6
- `es:NumberConstructor.EPSILON` × 6
- `es:ReadonlyArray.forEach` × 6
- `es:ReadonlySet` × 6
- `es:Set.delete` × 6
- `es:String` × 6
- `es:Uint32Array` × 6
- `es:Uint32Array.length` × 6
- `three:Box3.max` × 6
- `three:BufferGeometry.clone` × 6
- `three:BufferGeometry.computeVertexNormals` × 6
- `three:BufferGeometry.rotateX` × 6
- `three:Color.b` × 6
- `three:Color.g` × 6
- `three:Color.r` × 6
- `three:DataTexture` × 6
- `three:DoubleSide` × 6
- `three:InstancedBufferAttribute` × 6
- `three:Material` × 6
- `three:Matrix4.clone` × 6
- `three:Matrix4.elements` × 6
- `three:Object3D.remove` × 6
- `three:Object3D.updateMatrix` × 6
- `three:Path.lineTo` × 6
- `three:Texture` × 6
- `three:WebGLProgramParameters.fragmentShader` × 6
- `three:WebGLProgramParameters.vertexShader` × 6
- `dom:CanvasDrawPath.beginPath` × 5
- `dom:document` × 5
- `dom:Storage.setItem` × 5
- `dom:Worker` × 5
- `es:Array.find` × 5
- `es:Array.findIndex` × 5
- `es:Int16Array` × 5
- `es:Math.imul` × 5
- `es:NaN` × 5
- `es:Promise.catch` × 5
- `es:ReadonlyArray.some` × 5
- `es:Set.clear` × 5
- `es:Set.size` × 5
- `es:Uint8Array` × 5
- `three:BufferAttribute.setXYZ` × 5
- `three:CanvasTexture` × 5
- `three:CylinderGeometry` × 5
- `three:Object3D.parent` × 5
- `three:Object3D.quaternion` × 5
- `three:Object3D.renderOrder` × 5
- `three:Skeleton.bones` × 5
- `three:Sphere` × 5
- `three:Texture.image` × 5
- `three:uniforms.__index` × 5
- `three:Vector3.fromArray` × 5
- `dom:CanvasDrawPath.fill` × 4
- `dom:CanvasFillStrokeStyles.createRadialGradient` × 4
- `dom:CanvasPath.moveTo` × 4
- `dom:fetch` × 4
- `dom:Location.search` × 4
- `dom:URLSearchParams` × 4
- `es:Array.flatMap` × 4
- `es:Float32Array.buffer` × 4
- `es:Float32Array.byteLength` × 4
- `es:Float64Array` × 4
- `es:JSON.parse` × 4
- `es:JSON.stringify` × 4
- `es:NonNullable` × 4
- `es:ObjectConstructor.keys` × 4
- `es:Parameters` × 4
- `es:ReadonlyMap` × 4
- `jolt-physics:Jolt.RVec3` × 4
- `jolt-physics:ShapeResult.Get` × 4
- `jolt-physics:ShapeSettings.Create` × 4
- `three:BoxGeometry` × 4
- `three:BufferAttribute.getY` × 4
- `three:BufferAttribute.getZ` × 4
- `three:BufferGeometry.attributes` × 4
- `three:BufferGeometry.computeBoundingBox` × 4
- `three:BufferGeometry.morphAttributes` × 4
- `three:BufferGeometry.position` × 4
- `three:Color.setHex` × 4
- `three:DataTextureImageData.data` × 4
- `three:ExtrudeGeometry` × 4
- `three:InstancedMesh.morphTexture` × 4
- `three:Light.intensity` × 4
- `three:Matrix4.decompose` × 4
- `three:Object3D.clear` × 4
- `three:Object3D.customDepthMaterial` × 4
- `three:Path.quadraticCurveTo` × 4
- `three:Quaternion.clone` × 4
- `three:Shape` × 4
- `three:Skeleton.boneInverses` × 4
- `three:Texture.needsUpdate` × 4
- `three:Vector3.addVectors` × 4
- `three:Vector3.applyAxisAngle` × 4
- `three:Vector3.applyMatrix4` × 4
- `three:Vector3.negate` × 4
- `three:Vector3.setFromMatrixPosition` × 4
- `three:Vector3.toArray` × 4
- `vite:src` × 4
- `dom:CanvasPath.arc` × 3
- `dom:CanvasPath.lineTo` × 3
- `dom:CanvasPathDrawingStyles.lineWidth` × 3
- `dom:Console.warn` × 3
- `dom:Document.createElement` × 3
- `dom:HTMLCanvasElement.getContext` × 3
- `dom:HTMLCanvasElement.height` × 3
- `dom:HTMLCanvasElement.width` × 3
- `dom:Response.ok` × 3
- `dom:URLSearchParams.get` × 3
- `dom:window` × 3
- `dom:Worker.postMessage` × 3
- `es:Array.pop` × 3
- `es:ArrayConstructor.from` × 3
- `es:Map.entries` × 3
- `es:Math.ceil` × 3
- `es:Math.pow` × 3
- `es:ObjectConstructor.fromEntries` × 3
- `es:PromiseConstructor.all` × 3
- `es:ReadonlyArray.every` × 3
- `es:ReadonlyArray.filter` × 3
- `es:ReadonlyArray.slice` × 3
- `es:String.slice` × 3
- `es:Uint16Array` × 3
- `jolt-physics:Jolt.getPointer` × 3
- `jolt-physics:Vec3.Set` × 3
- `three:BufferAttribute.itemSize` × 3
- `three:BufferGeometry.addGroup` × 3
- `three:BufferGeometry.groups` × 3
- `three:BufferGeometry.setIndex` × 3
- `three:BufferGeometry.toNonIndexed` × 3
- `three:Color.getHex` × 3
- `three:GLTF` × 3
- `three:LineBasicMaterial` × 3
- `three:MeshStandardMaterialProperties.map` × 3
- `three:Object3D.onAfterRender` × 3
- `three:Path.moveTo` × 3
- `three:PointLight` × 3
- `three:Quaternion.invert` × 3
- `three:Quaternion.setFromRotationMatrix` × 3
- `three:Quaternion.setFromUnitVectors` × 3
- `three:SkinnedMesh.bindMatrix` × 3
- `three:SRGBColorSpace` × 3
- `three:Texture.colorSpace` × 3
- `three:Texture.userData` × 3
- `three:Vector3.cross` × 3
- `three:Vector3.lerpVectors` × 3
- `vite:ImportMeta.env` × 3
- `dom:CanvasDrawPath.stroke` × 2
- `dom:CanvasFillStrokeStyles.strokeStyle` × 2
- `dom:CanvasRect.fillRect` × 2
- `dom:MessageEvent` × 2
- `dom:setTimeout` × 2
- `es:Array.every` × 2
- `es:Array.fill` × 2
- `es:Float32Array.slice` × 2
- `es:Int32Array.fill` × 2
- `es:Int8Array.set` × 2
- `es:Map.keys` × 2
- `es:Math.asin` × 2
- `es:Math.exp` × 2
- `es:ObjectConstructor.values` × 2
- `es:parseInt` × 2
- `es:PromiseConstructor.resolve` × 2
- `es:ReadonlyArray.indexOf` × 2
- `es:ReadonlyMap.has` × 2
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
- `three:BufferAttribute.getComponent` × 2
- `three:BufferAttribute.setW` × 2
- `three:BufferAttribute.setXYZW` × 2
- `three:CircleGeometry` × 2
- `three:Color.lerp` × 2
- `three:Color.multiplyScalar` × 2
- `three:CurvePath.closePath` × 2
- `three:Euler.set` × 2
- `three:Euler.x` × 2
- `three:Euler.y` × 2
- `three:Euler.z` × 2
- `three:FloatType` × 2
- `three:InstancedMesh.boundingSphere` × 2
- `three:InstancedMesh.setMorphAt` × 2
- `three:IUniform.value` × 2
- `three:Light.color` × 2
- `three:LineSegments` × 2
- `three:Material.customProgramCacheKey` × 2
- `three:Material.defines` × 2
- `three:Material.onBeforeCompile` × 2
- `three:MaterialProperties.alphaTest` × 2
- `three:Matrix3` × 2
- `three:Matrix3.getNormalMatrix` × 2
- `three:Matrix4.makeBasis` × 2
- `three:Matrix4.premultiply` × 2
- `three:Matrix4.toArray` × 2
- `three:MeshDepthMaterial` × 2
- `three:NormalBlending` × 2
- `three:PointLight.decay` × 2
- `three:Points` × 2
- `three:Quaternion.copy` × 2
- `three:RGBADepthPacking` × 2
- `three:RGBAFormat` × 2
- `three:Scene` × 2
- `three:ShaderMaterialProperties.uniforms` × 2
- `three:Sphere.set` × 2
- `three:Sphere.union` × 2
- `three:Texture.addUpdateRange` × 2
- `three:Vector3.applyMatrix3` × 2
- `three:Vector3.equals` × 2
- `vite:ImportMetaEnv.BASE_URL` × 2
- `dom:AbstractWorker.onerror` × 1
- `dom:Body.arrayBuffer` × 1
- `dom:Body.json` × 1
- `dom:CanvasPath.closePath` × 1
- `dom:CanvasState.restore` × 1
- `dom:CanvasState.save` × 1
- `dom:CanvasTransform.rotate` × 1
- `dom:CanvasTransform.scale` × 1
- `dom:CanvasTransform.translate` × 1
- `dom:clearInterval` × 1
- `dom:ErrorEvent.message` × 1
- `dom:Event.preventDefault` × 1
- `dom:Location.href` × 1
- `dom:MessageEventTarget.onmessage` × 1
- `dom:MessageEventTarget.onmessageerror` × 1
- `dom:navigator` × 1
- `dom:NavigatorConcurrentHardware.hardwareConcurrency` × 1
- `dom:requestAnimationFrame` × 1
- `dom:Response.status` × 1
- `dom:URL` × 1
- `dom:URL.href` × 1
- `dom:URLSearchParams.has` × 1
- `dom:WindowOrWorkerGlobalScope.setInterval` × 1
- `dom:Worker.terminate` × 1
- `es:Array.entries` × 1
- `es:Array.flat` × 1
- `es:Array.join` × 1
- `es:Array.reverse` × 1
- `es:ArrayBuffer.byteLength` × 1
- `es:CallableFunction.apply` × 1
- `es:Error.stack` × 1
- `es:Float64Array.fill` × 1
- `es:Int16Array.fill` × 1
- `es:Int32ArrayConstructor.from` × 1
- `es:Iterator.next` × 1
- `es:IteratorYieldResult.value` × 1
- `es:Math.SQRT1_2` × 1
- `es:NumberConstructor.isNaN` × 1
- `es:NumberConstructor.NaN` × 1
- `es:ObjectConstructor.freeze` × 1
- `es:Pick` × 1
- `es:Promise.finally` × 1
- `es:ReadonlyMap.keys` × 1
- `es:ReadonlySet.size` × 1
- `es:RegExp.test` × 1
- `es:ReturnType` × 1
- `es:String.includes` × 1
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
- `three:Bone` × 1
- `three:Bone.isBone` × 1
- `three:Box3.getCenter` × 1
- `three:BufferGeometry.applyQuaternion` × 1
- `three:BufferGeometry.clearGroups` × 1
- `three:BufferGeometry.deleteAttribute` × 1
- `three:BufferGeometry.rotateY` × 1
- `three:BufferGeometry.rotateZ` × 1
- `three:BufferGeometry.scale` × 1
- `three:BufferGeometry.setDrawRange` × 1
- `three:Camera` × 1
- `three:DataTexture.magFilter` × 1
- `three:DataTexture.minFilter` × 1
- `three:DataTextureImageData.height` × 1
- `three:DataTextureImageData.width` × 1
- `three:GeometryGroup.count` × 1
- `three:GeometryGroup.materialIndex` × 1
- `three:GeometryGroup.start` × 1
- `three:GLTF.parser` × 1
- `three:GLTFLoader` × 1
- `three:GLTFParser.getDependency` × 1
- `three:InstancedMesh.dispose` × 1
- `three:Line.material` × 1
- `three:Loader.loadAsync` × 1
- `three:Material.clone` × 1
- `three:MaterialProperties.opacity` × 1
- `three:MaterialProperties.userData` × 1
- `three:Matrix4.extractRotation` × 1
- `three:Matrix4.getMaxScaleOnAxis` × 1
- `three:Matrix4.makeScale` × 1
- `three:Matrix4.set` × 1
- `three:Matrix4.setPosition` × 1
- `three:Matrix4.transpose` × 1
- `three:mergeGeometries` × 1
- `three:Mesh.morphTargetDictionary` × 1
- `three:MeshBasicMaterial` × 1
- `three:MeshDepthMaterialProperties.map` × 1
- `three:MeshStandardMaterialProperties.color` × 1
- `three:MeshStandardMaterialProperties.metalness` × 1
- `three:MeshStandardMaterialProperties.roughness` × 1
- `three:NearestFilter` × 1
- `three:Object3D.getWorldDirection` × 1
- `three:Object3D.matrixAutoUpdate` × 1
- `three:Object3D.matrixWorld` × 1
- `three:Object3D.onAfterShadow` × 1
- `three:Object3D.onBeforeShadow` × 1
- `three:Object3D.updateMatrixWorld` × 1
- `three:OrthographicCamera` × 1
- `three:OrthographicCamera.isOrthographicCamera` × 1
- `three:OrthographicCamera.left` × 1
- `three:OrthographicCamera.right` × 1
- `three:OrthographicCamera.zoom` × 1
- `three:Quaternion.multiply` × 1
- `three:Quaternion.multiplyQuaternions` × 1
- `three:Quaternion.normalize` × 1
- `three:Quaternion.premultiply` × 1
- `three:Quaternion.set` × 1
- `three:Quaternion.slerp` × 1
- `three:Quaternion.toArray` × 1
- `three:Quaternion.w` × 1
- `three:Quaternion.x` × 1
- `three:Quaternion.y` × 1
- `three:Quaternion.z` × 1
- `three:RenderTarget.dispose` × 1
- `three:RenderTarget.width` × 1
- `three:RingGeometry` × 1
- `three:ShaderMaterial` × 1
- `three:SkinnedMesh.bindMatrixInverse` × 1
- `three:SkinnedMesh.bindMode` × 1
- `three:Sphere.copy` × 1
- `three:Sphere.isEmpty` × 1
- `three:SphereGeometry` × 1
- `three:Texture.anisotropy` × 1
- `three:Texture.clearUpdateRanges` × 1
- `three:Texture.onUpdate` × 1
- `three:TorusGeometry` × 1
- `three:Vector2` × 1
- `three:Vector3.setFromMatrixScale` × 1
- `three:Vector3.setScalar` × 1
- `three:WebGLRenderer` × 1
- `three:WebGLRenderer.getRenderTarget` × 1
- `vite:ImportMetaEnv.DEV` × 1
