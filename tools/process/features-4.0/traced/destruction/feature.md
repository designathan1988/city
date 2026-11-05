# Feature: src/render/blast.ts, src/render/destruction.ts, src/render/casualties.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 86 declarations
- Slices of shared code: 23
- Code: 1687 lines in 8 files (source/)
- Resources: 1 (0 entry, 1 style, 0 import; 0 only the feature uses)
- Uses: 67 shared declarations (interface/: 117 declarations in 21 files, the types they name included); hooks: 36; external APIs: 236

## Code

- src/render/blast.ts — whole, 758 lines
- src/render/buildings/buildingMesh.ts — part, 86 lines
- src/render/buildings/fracture.ts — part, 85 lines
- src/render/buildings/partPose.ts — part, 5 lines
- src/render/casualties.ts — whole, 165 lines
- src/render/destruction.ts — whole, 410 lines
- src/render/playEffects.ts — part, 166 lines
- src/render/ragdoll.ts — part, 12 lines

## Slices of shared code

- `src/render/playEffects.ts#PlayEffectsContext` — 7 of 17 lines
- `src/render/playEffects.ts#createPlayEffects` — 24 of 417 lines
- `src/render/playEffects.ts#createPlayEffects.burn` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.busy` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.castersMoving` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.clearCasualties` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.dispose` — 5 of 8 lines
- `src/render/playEffects.ts#createPlayEffects.dropVehicle` — 5 of 14 lines
- `src/render/playEffects.ts#createPlayEffects.explode` — 87 of 143 lines
- `src/render/playEffects.ts#createPlayEffects.forgetRuin` — 3 of 4 lines
- `src/render/playEffects.ts#createPlayEffects.geyser` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.groundChanged` — 5 of 10 lines
- `src/render/playEffects.ts#createPlayEffects.leak` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.onBuildingDown` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.shake` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.soot` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.sparkAt` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.strikeBuilding` — 5 of 7 lines
- `src/render/playEffects.ts#createPlayEffects.update` — 5 of 8 lines
- `src/render/playEffects.ts#createPlayEffects.vehicleHit` — 10 of 13 lines
- `src/render/ragdoll.ts#Ragdolls` — 3 of 32 lines
- `src/render/ragdoll.ts#createRagdolls` — 6 of 1556 lines
- `src/render/ragdoll.ts#createRagdolls.update` — 3 of 51 lines

## Resources

- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `

## Where the program plugs the feature in

### src/render/blast.ts

- `src/render/blast.ts#Blast` (interface, line 68) ← call from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.burn`, `src/render/playEffects.ts#createPlayEffects.busy`, `src/render/playEffects.ts#createPlayEffects.castersMoving`, …
- `src/render/blast.ts#Blast` (interface, line 68) ← read from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.dispose`
- `src/render/blast.ts#createBlast` (function, line 208) ← call from `src/render/playEffects.ts#createPlayEffects`
- `src/render/blast.ts#createBlast` (function, line 208) ← implementation from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.dropVehicle`, `src/render/playEffects.ts#createPlayEffects.explode`, `src/render/playEffects.ts#createPlayEffects.vehicleHit`
- `src/render/blast.ts#createBlast.active` (method, line 605) ← implementation from `src/render/playEffects.ts#createPlayEffects.busy`
- `src/render/blast.ts#createBlast.arc` (method, line 577) ← implementation from `src/render/playEffects.ts#createPlayEffects.explode`, `src/render/playEffects.ts#createPlayEffects.sparkAt`
- `src/render/blast.ts#createBlast.burn` (method, line 594) ← implementation from `src/render/playEffects.ts#createPlayEffects.burn`
- `src/render/blast.ts#createBlast.casting` (method, line 604) ← implementation from `src/render/playEffects.ts#createPlayEffects.castersMoving`
- `src/render/blast.ts#createBlast.clear` (method, line 597) ← implementation from `src/render/playEffects.ts#createPlayEffects.clearCasualties`, `src/render/playEffects.ts#createPlayEffects.dispose`
- `src/render/blast.ts#createBlast.crater` (method, line 523) ← implementation from `src/render/playEffects.ts#createPlayEffects.explode`
- `src/render/blast.ts#createBlast.dispose` (method, line 742) ← implementation from `src/render/playEffects.ts#createPlayEffects.dispose`
- `src/render/blast.ts#createBlast.explode` (method, line 486) ← implementation from `src/render/playEffects.ts#createPlayEffects.explode`
- `src/render/blast.ts#createBlast.geyser` (method, line 589) ← implementation from `src/render/playEffects.ts#createPlayEffects.geyser`
- `src/render/blast.ts#createBlast.leak` (method, line 586) ← implementation from `src/render/playEffects.ts#createPlayEffects.leak`
- `src/render/blast.ts#createBlast.shake` (method, line 580) ← implementation from `src/render/playEffects.ts#createPlayEffects.shake`
- `src/render/blast.ts#createBlast.soot` (method, line 581) ← implementation from `src/render/playEffects.ts#createPlayEffects.soot`
- `src/render/blast.ts#createBlast.update` (method, line 606) ← implementation from `src/render/playEffects.ts#createPlayEffects.update`
- `src/render/blast.ts#createBlast.wire` (method, line 567) ← implementation from `src/render/playEffects.ts#createPlayEffects.explode`

### src/render/casualties.ts

- `src/render/casualties.ts#BloodDecal` (interface, line 15) ← read from `src/render/ragdoll.ts#createRagdolls.update`
- `src/render/casualties.ts#BloodDecal` (interface, line 15) ← typeRef from `src/render/ragdoll.ts#createRagdolls`, `src/render/ragdoll.ts#Ragdolls`
- `src/render/casualties.ts#BloodDecal` (interface, line 15) ← write from `src/render/ragdoll.ts#createRagdolls.update`
- `src/render/casualties.ts#CasualtyLayer` (interface, line 97) ← call from `src/render/playEffects.ts#createPlayEffects.dispose`, `src/render/playEffects.ts#createPlayEffects.update`
- `src/render/casualties.ts#CasualtyLayer` (interface, line 97) ← read from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.dispose`
- `src/render/casualties.ts#createCasualties` (function, line 103) ← call from `src/render/playEffects.ts#createPlayEffects`
- `src/render/casualties.ts#createCasualties.dispose` (method, line 159) ← implementation from `src/render/playEffects.ts#createPlayEffects.dispose`
- `src/render/casualties.ts#createCasualties.sync` (method, line 127) ← implementation from `src/render/playEffects.ts#createPlayEffects.update`

### src/render/destruction.ts

- `src/render/destruction.ts#createDestruction` (function, line 67) ← call from `src/render/playEffects.ts#createPlayEffects`
- `src/render/destruction.ts#createDestruction` (function, line 67) ← callbackAssign from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.onBuildingDown`
- `src/render/destruction.ts#createDestruction` (function, line 67) ← implementation from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.onBuildingDown`
- `src/render/destruction.ts#createDestruction` (function, line 67) ← typeRef from `src/render/playEffects.ts#PlayEffectsContext`
- `src/render/destruction.ts#createDestruction.dispose` (method, line 392) ← implementation from `src/render/playEffects.ts#createPlayEffects.dispose`
- `src/render/destruction.ts#createDestruction.hit` (method, line 335) ← implementation from `src/render/playEffects.ts#createPlayEffects.strikeBuilding`
- `src/render/destruction.ts#createDestruction.update` (method, line 352) ← implementation from `src/render/playEffects.ts#createPlayEffects.update`
- `src/render/destruction.ts#Destruction` (interface, line 53) ← call from `src/render/playEffects.ts#createPlayEffects.dispose`, `src/render/playEffects.ts#createPlayEffects.strikeBuilding`, `src/render/playEffects.ts#createPlayEffects.update`
- `src/render/destruction.ts#Destruction` (interface, line 53) ← read from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.dispose`, `src/render/playEffects.ts#createPlayEffects.forgetRuin`, `src/render/playEffects.ts#createPlayEffects.groundChanged`, …
- `src/render/destruction.ts#Destruction` (interface, line 53) ← write from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.onBuildingDown`

## Shared declarations the feature uses

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 16: `src/render/blast.ts#createBlast.burn`, `src/render/blast.ts#createBlast.carcassMaterial`, `src/render/blast.ts#createBlast.crater`, `src/render/blast.ts#createBlast.explode`, …
- `src/core/units.ts#TUNED_UNIT` (variable, line 12) — read by 1: `src/render/blast.ts#createBlast.update`

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 1: `src/render/destruction.ts#createDestruction.make`

### src/jobs/jobPool.ts

- `src/jobs/jobPool.ts#jobPort` (function, line 669) — call by 1: `src/render/destruction.ts#createDestruction.workerOf`

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

### src/render/buildings/cellArrays.ts

- `src/render/buildings/cellArrays.ts#FINISH_ATTRIBUTE` (variable, line 80) — read by 2: `src/render/destruction.ts#createDestruction.place`, `src/render/buildings/fracture.ts#fragmentGeometry`
- `src/render/buildings/cellArrays.ts#finishId` (function, line 87) — call by 1: `src/render/buildings/fracture.ts#prepareFracture`

### src/render/buildings/chunk.ts

- `src/render/buildings/chunk.ts#BuildingChunk` (interface, line 108) — read by 2: `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/fracture.ts#prepareFracture`
- `src/render/buildings/chunk.ts#BuildingChunk` (interface, line 108) — typeRef by 3: `src/render/destruction.ts#createDestruction`, `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/fracture.ts#prepareFracture`
- `src/render/buildings/chunk.ts#PartBatch` (interface, line 75) — read by 2: `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/chunk.ts#PartBatch` (interface, line 75) — typeRef by 3: `src/render/buildings/buildingMesh.ts#interiorFurniture`, `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/chunk.ts#PartKind` (type, line 8) — typeRef by 3: `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/render/buildings/chunk.ts#ShellChunk` (interface, line 89) — read by 2: `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/fracture.ts#prepareFracture`
- `src/render/buildings/chunk.ts#ShellChunk` (interface, line 89) — typeRef by 2: `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/buildingMesh.ts#shellPartFaces`

### src/render/buildings/fracture.ts

- `src/render/buildings/fracture.ts#FractureInput` (interface, line 79) — typeRef by 1: `src/render/buildings/fracture.ts#prepareFracture`
- `src/render/buildings/fracture.ts#Fragment` (interface, line 31) — read by 7: `src/render/destruction.ts#createDestruction.land`, `src/render/destruction.ts#createDestruction.loosen`, `src/render/destruction.ts#createDestruction.place`, `src/render/destruction.ts#createDestruction.strike`, …
- `src/render/buildings/fracture.ts#Fragment` (interface, line 31) — typeRef by 1: `src/render/destruction.ts#Piece`
- `src/render/buildings/fracture.ts#MODEL_ARRAYS` (variable, line 104) — read by 1: `src/render/buildings/fracture.ts#modelArrays`
- `src/render/buildings/fracture.ts#MODEL_ARRAYS` (variable, line 104) — write by 1: `src/render/buildings/fracture.ts#modelArrays`
- `src/render/buildings/fracture.ts#StillData` (interface, line 164) — read by 2: `src/render/destruction.ts#createDestruction.place`, `src/render/buildings/fracture.ts#fragmentGeometry`
- `src/render/buildings/fracture.ts#StillData` (interface, line 164) — typeRef by 4: `src/render/destruction.ts#createDestruction.place`, `src/render/destruction.ts#createDestruction.workerOf`, `src/render/destruction.ts#Ruin`, `src/render/buildings/fracture.ts#fragmentGeometry`

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

### src/render/depthConvention.ts

- `src/render/depthConvention.ts#pullTowardEye` (function, line 28) — call by 2: `src/render/blast.ts#createBlast`, `src/render/casualties.ts#createCasualties`

### src/render/exhaust.ts

- `src/render/exhaust.ts#createExhaust.burst` (method, line 219) — implementation by 7: `src/render/blast.ts#createBlast.explode`, `src/render/blast.ts#createBlast.stepRope`, `src/render/blast.ts#createBlast.update`, `src/render/destruction.ts#createDestruction.hit`, …
- `src/render/exhaust.ts#Exhaust` (interface, line 22) — call by 7: `src/render/blast.ts#createBlast.explode`, `src/render/blast.ts#createBlast.stepRope`, `src/render/blast.ts#createBlast.update`, `src/render/destruction.ts#createDestruction.hit`, …
- `src/render/exhaust.ts#Exhaust` (interface, line 22) — typeRef by 2: `src/render/blast.ts#createBlast`, `src/render/destruction.ts#createDestruction`

### src/world/buildings/blocks.ts

- `src/world/buildings/blocks.ts#resolveBlocks` (variable, line 11) — call by 1: `src/render/destruction.ts#createDestruction.make`

### src/world/buildings/geometry.ts

- `src/world/buildings/geometry.ts#levelElevation` (function, line 97) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/geometry.ts#levelHeight` (function, line 90) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/geometry.ts#liftAt` (function, line 120) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/geometry.ts#localToWorld` (function, line 32) — call by 1: `src/render/destruction.ts#createDestruction.make`
- `src/world/buildings/geometry.ts#volumeElevation` (function, line 117) — call by 1: `src/render/destruction.ts#createDestruction.make`

### src/world/buildings/interior.ts

- `src/world/buildings/interior.ts#FloorInterior` (interface, line 42) — read by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/interior.ts#Furniture` (interface, line 21) — read by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/interior.ts#FURNITURE_KINDS` (variable, line 13) — read by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/interior.ts#FurnitureKind` (type, line 19) — typeRef by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/interior.ts#interiorAt` (function, line 194) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`

### src/world/buildings/materials.ts

- `src/world/buildings/materials.ts#Finish` (type, line 7) — typeRef by 3: `src/render/buildings/buildingMesh.ts#withShellParts`, `src/render/buildings/fracture.ts#prepareFracture`, `src/render/buildings/buildingMesh.ts#shellPartFaces`
- `src/world/buildings/materials.ts#FINISHES` (variable, line 6) — read by 1: `src/render/buildings/buildingMesh.ts#shellPartFaces`

### src/world/buildings/types.ts

- `src/world/buildings/types.ts#Building` (interface, line 284) — read by 5: `src/render/destruction.ts#createDestruction.fail`, `src/render/destruction.ts#createDestruction.hit`, `src/render/destruction.ts#createDestruction.make`, `src/render/destruction.ts#createDestruction.workerOf`, …
- `src/world/buildings/types.ts#Building` (interface, line 284) — typeRef by 4: `src/render/destruction.ts#createDestruction`, `src/render/destruction.ts#createDestruction.make`, `src/render/destruction.ts#Destruction`, `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/types.ts#Volume` (interface, line 150) — read by 2: `src/render/destruction.ts#createDestruction.make`, `src/render/buildings/buildingMesh.ts#interiorFurniture`
- `src/world/buildings/types.ts#volumeTop` (function, line 447) — call by 1: `src/render/buildings/buildingMesh.ts#interiorFurniture`

## External APIs

- `es:Math` × 157
- `three:Vector3` × 70
- `three:Vector3.y` × 48
- `es:Math.random` × 47
- `es:Array.length` × 46
- `three:Vector3.x` × 45
- `three:Vector3.z` × 43
- `es:Float32Array` × 27
- `three:Object3D.position` × 27
- `es:Array.push` × 25
- `es:Math.PI` × 25
- `es:Math.sin` × 22
- `es:Math.cos` × 21
- `three:Vector3.clone` × 19
- `three:Vector3.set` × 18
- `es:Math.max` × 17
- `three:BufferAttribute` × 17
- `three:InstancedMesh.count` × 15
- `es:Math.min` × 14
- `three:BufferGeometry.dispose` × 13
- `three:BufferGeometry.setAttribute` × 13
- `three:Vector3.copy` × 13
- `three:Mesh.geometry` × 12
- `three:Object3D.visible` × 11
- `dom:CanvasGradient.addColorStop` × 10
- `es:Record` × 9
- `three:BufferGeometry` × 9
- `three:Mesh` × 9
- `three:Object3D.add` × 9
- `three:Object3D.matrix` × 9
- `three:Object3D.scale` × 9
- `three:Vector3.addScaledVector` × 9
- `three:InstancedMesh` × 8
- `three:MeshStandardMaterial` × 8
- `dom:MessageEvent.data` × 7
- `es:Float32Array.set` × 7
- `es:Map.get` × 7
- `three:BufferGeometry.getAttribute` × 7
- `three:InstancedMesh.setMatrixAt` × 7
- `three:Quaternion` × 7
- `dom:CanvasFillStrokeStyles.fillStyle` × 6
- `es:Array.forEach` × 6
- `es:Array.map` × 6
- `es:Array.splice` × 6
- `three:BufferAttribute.addUpdateRange` × 6
- `three:BufferAttribute.array` × 6
- `three:BufferAttribute.needsUpdate` × 6
- `three:Color` × 6
- `three:DoubleSide` × 6
- `three:Group` × 6
- `three:InstancedMesh.instanceMatrix` × 6
- `three:Material` × 6
- `three:Material.dispose` × 6
- `three:Object3D.receiveShadow` × 6
- `three:Quaternion.setFromAxisAngle` × 6
- `three:Vector3.length` × 6
- `dom:CanvasDrawPath.beginPath` × 5
- `dom:document` × 5
- `es:Array.filter` × 5
- `es:Float32Array.subarray` × 5
- `es:Infinity` × 5
- `es:Object` × 5
- `es:Partial` × 5
- `es:ReadonlyArray.map` × 5
- `three:CanvasTexture` × 5
- `three:InstancedMesh.setColorAt` × 5
- `three:Object3D.castShadow` × 5
- `three:Object3D.frustumCulled` × 5
- `three:Object3D.rotation` × 5
- `three:Object3D.updateMatrix` × 5
- `three:Vector3.multiplyScalar` × 5
- `dom:CanvasDrawPath.fill` × 4
- `dom:CanvasFillStrokeStyles.createRadialGradient` × 4
- `dom:CanvasPath.moveTo` × 4
- `es:Array.shift` × 4
- `es:Array.some` × 4
- `es:Float32Array.length` × 4
- `es:Int32Array.length` × 4
- `es:Map` × 4
- `es:Map.delete` × 4
- `es:Map.set` × 4
- `es:Number.toFixed` × 4
- `es:ObjectConstructor.entries` × 4
- `es:ReadonlyArray.length` × 4
- `es:Uint32Array.length` × 4
- `three:BufferGeometry.rotateX` × 4
- `three:Object3D.quaternion` × 4
- `three:Object3D.renderOrder` × 4
- `dom:CanvasPath.arc` × 3
- `dom:CanvasPath.lineTo` × 3
- `dom:CanvasPathDrawingStyles.lineWidth` × 3
- `dom:Document.createElement` × 3
- `dom:HTMLCanvasElement.getContext` × 3
- `dom:HTMLCanvasElement.height` × 3
- `dom:HTMLCanvasElement.width` × 3
- `dom:Worker` × 3
- `es:Array.flatMap` × 3
- `es:Map.clear` × 3
- `es:Math.round` × 3
- `three:BufferAttribute.clearUpdateRanges` × 3
- `three:BufferGeometry.boundingSphere` × 3
- `three:Object3D.name` × 3
- `three:Sphere` × 3
- `three:SRGBColorSpace` × 3
- `three:Texture.colorSpace` × 3
- `three:Vector3.add` × 3
- `three:Vector3.distanceTo` × 3
- `three:Vector3.normalize` × 3
- `three:Vector3.setY` × 3
- `dom:CanvasDrawPath.stroke` × 2
- `dom:CanvasFillStrokeStyles.strokeStyle` × 2
- `dom:CanvasRect.fillRect` × 2
- `dom:performance` × 2
- `dom:Performance.now` × 2
- `es:Array.reduce` × 2
- `es:Int16Array` × 2
- `es:Int32Array` × 2
- `es:Math.hypot` × 2
- `es:Math.sign` × 2
- `es:Math.sqrt` × 2
- `es:Number` × 2
- `es:Readonly` × 2
- `es:String.replace` × 2
- `es:Uint16Array` × 2
- `es:WeakMap.get` × 2
- `es:WeakMap.set` × 2
- `three:AdditiveBlending` × 2
- `three:BufferAttribute.setUsage` × 2
- `three:BufferGeometry.addGroup` × 2
- `three:BufferGeometry.translate` × 2
- `three:CircleGeometry` × 2
- `three:Color.getHex` × 2
- `three:Color.multiplyScalar` × 2
- `three:CurvePath.closePath` × 2
- `three:DynamicDrawUsage` × 2
- `three:Euler.x` × 2
- `three:Euler.y` × 2
- `three:Euler.z` × 2
- `three:ExtrudeGeometry` × 2
- `three:Light.color` × 2
- `three:LineBasicMaterial` × 2
- `three:MeshStandardMaterialProperties.map` × 2
- `three:Object3D` × 2
- `three:Object3D.remove` × 2
- `three:Path.lineTo` × 2
- `three:Path.moveTo` × 2
- `three:Quaternion.copy` × 2
- `three:Shape` × 2
- `three:Sphere.set` × 2
- `three:Sphere.union` × 2
- `three:Texture.dispose` × 2
- `three:Vector3.sub` × 2
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
- `dom:console` × 1
- `dom:Console.error` × 1
- `dom:ErrorEvent.message` × 1
- `dom:Event.preventDefault` × 1
- `dom:MessageEvent` × 1
- `dom:MessageEventTarget.onmessage` × 1
- `dom:MessageEventTarget.onmessageerror` × 1
- `dom:navigator` × 1
- `dom:NavigatorConcurrentHardware.hardwareConcurrency` × 1
- `dom:Worker.postMessage` × 1
- `dom:Worker.terminate` × 1
- `es:Array.fill` × 1
- `es:Array.findIndex` × 1
- `es:Array.indexOf` × 1
- `es:Array.pop` × 1
- `es:ArrayLike` × 1
- `es:Float32ArrayConstructor.from` × 1
- `es:Map.values` × 1
- `es:Math.abs` × 1
- `es:Math.exp` × 1
- `es:NonNullable` × 1
- `es:NumberConstructor.isFinite` × 1
- `es:NumberConstructor.NaN` × 1
- `es:ObjectConstructor.values` × 1
- `es:ReadonlySet` × 1
- `es:Set` × 1
- `es:Set.add` × 1
- `es:Set.clear` × 1
- `es:Uint32Array` × 1
- `es:Uint32Array.set` × 1
- `es:Uint8Array` × 1
- `es:WeakMap` × 1
- `three:BoxGeometry` × 1
- `three:BufferAttribute.itemSize` × 1
- `three:BufferGeometry.index` × 1
- `three:BufferGeometry.setDrawRange` × 1
- `three:BufferGeometry.toNonIndexed` × 1
- `three:Color.lerp` × 1
- `three:Color.setHex` × 1
- `three:CylinderGeometry` × 1
- `three:Euler.set` × 1
- `three:InstancedMesh.dispose` × 1
- `three:InstancedMesh.instanceColor` × 1
- `three:Light.intensity` × 1
- `three:Line.material` × 1
- `three:LineSegments` × 1
- `three:Material.customProgramCacheKey` × 1
- `three:Material.onBeforeCompile` × 1
- `three:MaterialProperties.opacity` × 1
- `three:Matrix3` × 1
- `three:Matrix3.getNormalMatrix` × 1
- `three:Mesh.material` × 1
- `three:MeshBasicMaterial` × 1
- `three:Object3D.clear` × 1
- `three:Object3D.matrixAutoUpdate` × 1
- `three:PointLight` × 1
- `three:PointLight.decay` × 1
- `three:Quaternion.clone` × 1
- `three:Quaternion.multiply` × 1
- `three:Quaternion.normalize` × 1
- `three:Quaternion.premultiply` × 1
- `three:RingGeometry` × 1
- `three:Sphere.copy` × 1
- `three:Sphere.isEmpty` × 1
- `three:Texture.anisotropy` × 1
- `three:Vector3.applyMatrix3` × 1
- `three:Vector3.applyMatrix4` × 1
- `three:Vector3.applyQuaternion` × 1
- `three:Vector3.cross` × 1
- `three:Vector3.divideScalar` × 1
- `three:Vector3.dot` × 1
- `three:Vector3.lengthSq` × 1
- `three:Vector3.lerp` × 1
- `three:Vector3.subVectors` × 1
