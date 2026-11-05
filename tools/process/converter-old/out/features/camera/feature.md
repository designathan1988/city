# Feature: src/view/camera.ts, src/view/cameraGestures.ts, src/view/cameraMotion.ts, src/view/cameraProfile.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 112 declarations
- Slices of shared code: 39
- Code: 922 lines in 8 files (source/)
- Resources: 4 (4 entry, 0 style, 0 import; 0 only the feature uses)
- Uses: 19 shared declarations (interface/: 17 declarations in 7 files, the types they name included); hooks: 49; external APIs: 31

## Code

- src/main.ts — part, 159 lines
- src/render/isoViewport.ts — part, 56 lines
- src/ui/roads/keys.ts — part, 3 lines
- src/view/camera.ts — whole, 82 lines
- src/view/cameraGestures.ts — whole, 334 lines
- src/view/cameraMotion.ts — whole, 196 lines
- src/view/cameraProfile.ts — whole, 58 lines
- src/view/viewport.ts — part, 34 lines

## Slices of shared code

- `src/main.ts#<ExpressionStatement>@2564` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@2566` — 1 of 1 lines
- `src/main.ts#<IfStatement>@320` — 7 of 9 lines
- `src/main.ts#<IfStatement>@4036` — 15 of 196 lines
- `src/main.ts#CanvasInput` — 5 of 5 lines
- `src/main.ts#camera` — 1 of 1 lines
- `src/main.ts#cameraHand` — 7 of 7 lines
- `src/main.ts#cameraMotion` — 5 of 5 lines
- `src/main.ts#cancelGestures` — 3 of 13 lines
- `src/main.ts#captureLost` — 3 of 4 lines
- `src/main.ts#coalescedPath` — 2 of 4 lines
- `src/main.ts#currentGesture` — 4 of 18 lines
- `src/main.ts#endPointer` — 17 of 30 lines
- `src/main.ts#fitOpeningView` — 3 of 11 lines
- `src/main.ts#fitView` — 3 of 4 lines
- `src/main.ts#frame` — 7 of 102 lines
- `src/main.ts#gameKeys` — 6 of 77 lines
- `src/main.ts#mountInterface` — 9 of 88 lines
- `src/main.ts#pointerDown` — 22 of 180 lines
- `src/main.ts#pointerMove` — 5 of 72 lines
- `src/main.ts#releaseHeld` — 3 of 5 lines
- `src/main.ts#restoreSettings` — 5 of 20 lines
- `src/main.ts#roadTool` — 3 of 29 lines
- `src/main.ts#scene` — 1 of 1 lines
- `src/main.ts#syncFlatCameraFromView` — 5 of 7 lines
- `src/main.ts#syncViewFromFlatCamera` — 10 of 10 lines
- `src/main.ts#view` — 1 of 1 lines
- `src/main.ts#wheel` — 5 of 16 lines
- `src/render/isoViewport.ts#createIsoRig.apply` — 47 of 134 lines
- `src/render/isoViewport.ts#createIsoRig.askTilt` — 6 of 6 lines
- `src/render/isoViewport.ts#createIsoRig.tiltNow` — 3 of 3 lines
- `src/ui/roads/keys.ts#cameraKey` — 3 of 3 lines
- `src/view/viewport.ts#flatViewport` — 13 of 53 lines
- `src/view/viewport.ts#flatViewport.moveTo` — 4 of 4 lines
- `src/view/viewport.ts#flatViewport.panTo` — 5 of 5 lines
- `src/view/viewport.ts#flatViewport.toScreen` — 1 of 1 lines
- `src/view/viewport.ts#flatViewport.toWorld` — 1 of 1 lines
- `src/view/viewport.ts#flatViewport.toWorldAt` — 1 of 1 lines
- `src/view/viewport.ts#flatViewport.zoomAt` — 3 of 3 lines

## Resources

- shared entry `low` — src/render/quality.ts:123: `low: { pixelRatio: 1, shadows: false, shadowMapSize: 1024, postProcessing: false, ambientOcclusion: false, smaa: false, msaa: 0, aoFull: false, sharpen: 0, anis`
- shared entry `medium` — src/render/quality.ts:156: `medium: { pixelRatio: 1.25, shadows: true, shadowMapSize: 1024, postProcessing: false, ambientOcclusion: false, smaa: false, msaa: 0, aoFull: false, sharpen: 0,`
- shared entry `high` — src/render/quality.ts:188: `high: { pixelRatio: 1.5, shadows: true, // Three cascades of 2048: the nearest a few centimetres a texel round the street looked at shadowMapSize: 2048, postPro`
- shared entry `ultra` — src/render/quality.ts:223: `ultra: { pixelRatio: 2, shadows: true, // DO NOT raise it to 3072 with MSAA 4x and full-resolution GTAO (80074433): the picture came out washed out, 18 % of the`

## Where the program plugs the feature in

### src/view/camera.ts

- `src/view/camera.ts#Camera` (class, line 11) ← construct from `src/main.ts#camera`
- `src/view/camera.ts#Camera` (class, line 11) ← typeRef from `src/view/viewport.ts#flatViewport`
- `src/view/camera.ts#Camera.fit` (method, line 69) ← call from `src/main.ts#<IfStatement>@320`, `src/main.ts#fitOpeningView`, `src/main.ts#fitView`
- `src/view/camera.ts#Camera.px` (method, line 78) ← call from `src/main.ts#roadTool`
- `src/view/camera.ts#Camera.screenToWorld` (method, line 32) ← call from `src/view/viewport.ts#flatViewport.panTo`, `src/view/viewport.ts#flatViewport.toWorld`, `src/view/viewport.ts#flatViewport.toWorldAt`
- `src/view/camera.ts#Camera.worldToScreen` (method, line 40) ← call from `src/view/viewport.ts#flatViewport.toScreen`
- `src/view/camera.ts#Camera.x` (field, line 13) ← read from `src/main.ts#scene`, `src/main.ts#syncViewFromFlatCamera`, `src/view/viewport.ts#flatViewport`, `src/view/viewport.ts#flatViewport.panTo`
- `src/view/camera.ts#Camera.x` (field, line 13) ← write from `src/main.ts#<IfStatement>@320`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#restoreSettings`, `src/main.ts#syncFlatCameraFromView`, …
- `src/view/camera.ts#Camera.y` (field, line 14) ← read from `src/main.ts#scene`, `src/main.ts#syncViewFromFlatCamera`, `src/view/viewport.ts#flatViewport`, `src/view/viewport.ts#flatViewport.panTo`
- `src/view/camera.ts#Camera.y` (field, line 14) ← write from `src/main.ts#<IfStatement>@320`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#restoreSettings`, `src/main.ts#syncFlatCameraFromView`, …
- `src/view/camera.ts#Camera.zoom` (field, line 16) ← read from `src/main.ts#<IfStatement>@4036`, `src/main.ts#scene`, `src/main.ts#syncViewFromFlatCamera`, `src/view/viewport.ts#flatViewport`
- `src/view/camera.ts#Camera.zoom` (field, line 16) ← write from `src/main.ts#<IfStatement>@320`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#restoreSettings`, `src/main.ts#syncFlatCameraFromView`
- `src/view/camera.ts#Camera.zoomAt` (method, line 48) ← call from `src/view/viewport.ts#flatViewport.zoomAt`
- `src/view/camera.ts#MAX_ZOOM` (variable, line 8) ← read from `src/view/viewport.ts#flatViewport`
- `src/view/camera.ts#MIN_ZOOM` (variable, line 7) ← read from `src/view/viewport.ts#flatViewport`

### src/view/cameraGestures.ts

- `src/view/cameraGestures.ts#CameraGestures` (class, line 52) ← construct from `src/main.ts#cameraHand`
- `src/view/cameraGestures.ts#CameraGestures.active` (getter, line 69) ← call from `src/main.ts#frame`, `src/main.ts#pointerMove`
- `src/view/cameraGestures.ts#CameraGestures.advance` (method, line 176) ← call from `src/main.ts#frame`
- `src/view/cameraGestures.ts#CameraGestures.cancel` (method, line 321) ← call from `src/main.ts#cancelGestures`
- `src/view/cameraGestures.ts#CameraGestures.coasting` (getter, line 102) ← call from `src/main.ts#frame`
- `src/view/cameraGestures.ts#CameraGestures.gesture` (method, line 87) ← call from `src/main.ts#currentGesture`
- `src/view/cameraGestures.ts#CameraGestures.has` (method, line 82) ← call from `src/main.ts#captureLost`
- `src/view/cameraGestures.ts#CameraGestures.move` (method, line 246) ← call from `src/main.ts#pointerMove`
- `src/view/cameraGestures.ts#CameraGestures.press` (method, line 95) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#CameraGestures.release` (method, line 303) ← call from `src/main.ts#endPointer`
- `src/view/cameraGestures.ts#CameraGestures.releaseAll` (method, line 328) ← call from `src/main.ts#releaseHeld`
- `src/view/cameraGestures.ts#CameraGestures.startOrbit` (method, line 230) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#CameraGestures.startPan` (method, line 236) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#CameraGestures.startPinch` (method, line 216) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#CameraGestures.touches` (getter, line 78) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#PointerSample` (interface, line 39) ← typeRef from `src/main.ts#CanvasInput`, `src/main.ts#coalescedPath`, `src/main.ts#pointerMove`

### src/view/cameraMotion.ts

- `src/view/cameraMotion.ts#CameraMotion` (class, line 50) ← construct from `src/main.ts#cameraMotion`
- `src/view/cameraMotion.ts#CameraMotion` (class, line 50) ← reference from `src/ui/roads/keys.ts#cameraKey`
- `src/view/cameraMotion.ts#CameraMotion.aim` (method, line 119) ← call from `src/main.ts#mountInterface`
- `src/view/cameraMotion.ts#CameraMotion.moving` (getter, line 136) ← call from `src/main.ts#frame`
- `src/view/cameraMotion.ts#CameraMotion.owns` (method, line 96) ← call from `src/ui/roads/keys.ts#cameraKey`
- `src/view/cameraMotion.ts#CameraMotion.press` (method, line 74) ← call from `src/main.ts#gameKeys`
- `src/view/cameraMotion.ts#CameraMotion.release` (method, line 84) ← call from `src/main.ts#<ExpressionStatement>@2564`
- `src/view/cameraMotion.ts#CameraMotion.releaseAll` (method, line 90) ← call from `src/main.ts#<ExpressionStatement>@2566`
- `src/view/cameraMotion.ts#CameraMotion.step` (method, line 142) ← call from `src/main.ts#frame`
- `src/view/cameraMotion.ts#CameraMotion.turn` (method, line 113) ← call from `src/main.ts#gameKeys`
- `src/view/cameraMotion.ts#CameraMotion.wheel` (method, line 101) ← call from `src/main.ts#wheel`

### src/view/cameraProfile.ts

- `src/view/cameraProfile.ts#eyeLift` (function, line 40) ← call from `src/render/isoViewport.ts#createIsoRig.apply`
- `src/view/cameraProfile.ts#FAR_TILT` (variable, line 9) ← read from `src/render/isoViewport.ts#createIsoRig.askTilt`, `src/render/isoViewport.ts#createIsoRig.tiltNow`
- `src/view/cameraProfile.ts#fieldOfView` (function, line 45) ← call from `src/render/isoViewport.ts#createIsoRig.apply`
- `src/view/cameraProfile.ts#minTilt` (function, line 35) ← call from `src/render/isoViewport.ts#createIsoRig.askTilt`, `src/render/isoViewport.ts#createIsoRig.tiltNow`
- `src/view/cameraProfile.ts#profileTilt` (function, line 30) ← call from `src/render/isoViewport.ts#createIsoRig.askTilt`, `src/render/isoViewport.ts#createIsoRig.tiltNow`
- `src/view/cameraProfile.ts#pullWeight` (function, line 50) ← call from `src/render/isoViewport.ts#createIsoRig.apply`
- `src/view/cameraProfile.ts#viewDistance` (function, line 55) ← call from `src/render/isoViewport.ts#createIsoRig.apply`

## Shared declarations the feature uses

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 1: `src/view/camera.ts#Camera.fit`
- `src/core/aabb.ts#Aabb` (interface, line 3) — typeRef by 2: `src/view/camera.ts#Camera.fit`, `src/view/camera.ts#Camera.viewBounds`

### src/core/scalar.ts

- `src/core/scalar.ts#clamp` (function, line 58) — call by 4: `src/view/camera.ts#Camera.fit`, `src/view/camera.ts#Camera.zoomAt`, `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraProfile.ts#blend`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 6: `src/view/cameraProfile.ts#EYE_HEIGHT`, `src/view/cameraProfile.ts#eyeLift`, `src/view/cameraProfile.ts#fieldOfView`, `src/view/cameraProfile.ts#minTilt`, …
- `src/core/units.ts#perM` (function, line 9) — call by 4: `src/view/camera.ts#Camera.fit`, `src/view/camera.ts#Camera.zoom`, `src/view/camera.ts#MAX_ZOOM`, `src/view/camera.ts#MIN_ZOOM`

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 8: `src/view/camera.ts#Camera.worldToScreen`, `src/view/camera.ts#Camera.zoomAt`, `src/view/cameraGestures.ts#CameraGestures.advance`, `src/view/cameraGestures.ts#CameraGestures.move`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 15: `src/view/camera.ts#Camera.screenToWorld`, `src/view/camera.ts#Camera.worldToScreen`, `src/view/cameraGestures.ts#CameraGestures.anchor`, `src/view/cameraGestures.ts#CameraGestures.move`, …

### src/main.ts

- `src/main.ts#cameraHand` (variable, line 553) — implementation by 6: `src/view/cameraGestures.ts#CameraGestures.advance`, `src/view/cameraGestures.ts#CameraGestures.anchor`, `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraGestures.ts#CameraGestures.startPan`, …
- `src/main.ts#cameraMotion` (variable, line 563) — implementation by 2: `src/view/cameraMotion.ts#CameraMotion.step`, `src/view/cameraMotion.ts#CameraMotion.wheel`

### src/render/isoViewport.ts

- `src/render/isoViewport.ts#createIsoRig` (function, line 78) — implementation by 7: `src/view/cameraGestures.ts#CameraGestures.advance`, `src/view/cameraGestures.ts#CameraGestures.anchor`, `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraGestures.ts#CameraGestures.startPan`, …

### src/view/viewport.ts

- `src/view/viewport.ts#flatViewport` (function, line 52) — read by 1: `src/view/viewport.ts#flatViewport.slide`
- `src/view/viewport.ts#flatViewport.moveTo` (method, line 90) — implementation by 1: `src/view/cameraGestures.ts#CameraGestures.advance`
- `src/view/viewport.ts#flatViewport.orbit` (method, line 79) — implementation by 3: `src/view/cameraGestures.ts#CameraGestures.advance`, `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraMotion.ts#CameraMotion.step`
- `src/view/viewport.ts#flatViewport.panTo` (method, line 60) — implementation by 1: `src/view/cameraGestures.ts#CameraGestures.move`
- `src/view/viewport.ts#flatViewport.toWorldAt` (method, line 57) — implementation by 2: `src/view/cameraGestures.ts#CameraGestures.anchor`, `src/view/cameraGestures.ts#CameraGestures.throwCoast`
- `src/view/viewport.ts#flatViewport.zoomAt` (method, line 65) — implementation by 2: `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraMotion.ts#CameraMotion.step`
- `src/view/viewport.ts#Viewport` (interface, line 8) — call by 7: `src/view/cameraGestures.ts#CameraGestures.advance`, `src/view/cameraGestures.ts#CameraGestures.anchor`, `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraGestures.ts#CameraGestures.startPan`, …
- `src/view/viewport.ts#Viewport` (interface, line 8) — read by 5: `src/view/cameraGestures.ts#CameraGestures.advance`, `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraGestures.ts#CameraGestures.startPinch`, `src/view/cameraMotion.ts#CameraMotion.step`, …
- `src/view/viewport.ts#Viewport` (interface, line 8) — typeRef by 2: `src/view/cameraGestures.ts#CameraGesturesHost`, `src/view/cameraMotion.ts#MotionHost`

## External APIs

- `es:Math` × 52
- `es:Math.max` × 14
- `es:Math.exp` × 8
- `dom:performance` × 7
- `dom:Performance.now` × 7
- `es:Array.length` × 7
- `es:Math.abs` × 6
- `es:Math.hypot` × 5
- `es:Math.min` × 5
- `es:Math.PI` × 5
- `es:Map.size` × 4
- `es:Record` × 4
- `es:Map.clear` × 3
- `es:Map.set` × 3
- `es:Map.values` × 3
- `es:Math.atan2` × 3
- `es:Math.log` × 3
- `es:Number` × 3
- `es:NumberConstructor.isFinite` × 3
- `es:Array.push` × 2
- `es:Map` × 2
- `es:Map.delete` × 2
- `es:Map.has` × 2
- `dom:DOMMatrix2DInit` × 1
- `es:Array.shift` × 1
- `es:Math.cos` × 1
- `es:Math.sin` × 1
- `es:Math.tan` × 1
- `es:Readonly` × 1
- `es:ReadonlyArray.length` × 1
- `es:ReadonlyArray.some` × 1
