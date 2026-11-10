// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:197-197 slice of src/main.ts#camera (shared): the feature's statements; "…" marks code left out
const camera = new Camera();

// src/main.ts:320-328 slice of src/main.ts#<IfStatement>@320 (shared): the feature's statements; "…" marks code left out
if (bootSettings) {
  camera.x = bootSettings.camera.x;
  camera.y = bootSettings.camera.y;
  // … 2 lines
  camera.zoom = clamp(bootSettings.camera.zoom, limits.min, limits.max);
} else {
  camera.fit(worldBounds(), surface.cssW, surface.cssH);
}

// src/main.ts:458-486 slice of src/main.ts#roadTool (shared): the feature's statements; "…" marks code left out
const roadTool = new RoadTool({
  // … 3 lines
  px: (n) => camera.px(n),
// … 23 lines
});

// src/main.ts:553-559 slice of src/main.ts#cameraHand (shared): the feature's statements; "…" marks code left out
const cameraHand = new CameraGestures({
  view: () => view,
  size: () => ({ w: surface.cssW, h: surface.cssH }),
  orbited: () => persistence.saveSettingsSoon(sessionSettings),
  redraw: () => requestDraw(),
  heightUnder: (at) => groundHeightUnder(at),
});

// src/main.ts:563-567 slice of src/main.ts#cameraMotion (shared): the feature's statements; "…" marks code left out
const cameraMotion = new CameraMotion({
  view: () => view,
  size: () => ({ w: surface.cssW, h: surface.cssH }),
  heightUnder: (at) => groundHeightUnder(at),
});

// src/main.ts:608-608 slice of src/main.ts#view (shared): the feature's statements; "…" marks code left out
let view: Viewport = flatViewport(camera);

// src/main.ts:671-671 slice of src/main.ts#scene (shared): the feature's statements; "…" marks code left out
const scene: SceneHandle = createSceneRenderer(canvas3d, { x: camera.x, y: camera.y }, camera.zoom, graphics.level, requestDraw, engine, graphics.settings);

// src/main.ts:812-821 slice of src/main.ts#syncViewFromFlatCamera (shared): the feature's statements; "…" marks code left out
function syncViewFromFlatCamera(): void {
  view.moveTo({ x: camera.x, y: camera.y });
  view.zoomAt(
    surface.cssW / 2,
    surface.cssH / 2,
    camera.zoom / Math.max(0.001, view.zoom),
    surface.cssW,
    surface.cssH,
  );
}

// src/main.ts:823-826 slice of src/main.ts#fitView (shared): the feature's statements; "…" marks code left out
function fitView(): void {
  camera.fit(worldBounds(), surface.cssW, surface.cssH);
// … 1 lines
}

// src/main.ts:828-834 slice of src/main.ts#syncFlatCameraFromView (shared): the feature's statements; "…" marks code left out
function syncFlatCameraFromView(): void {
  // … 2 lines
  camera.x = centre.x;
  camera.y = centre.y;
  camera.zoom = view.zoom;
}

// src/main.ts:1034-1037 slice of src/main.ts#coalescedPath (shared): the feature's statements; "…" marks code left out
function coalescedPath(e: PointerEvent, r: DOMRect): PointerSample[] | undefined {
// … 2 lines
}

// src/main.ts:1041-1053 slice of src/main.ts#cancelGestures (shared): the feature's statements; "…" marks code left out
function cancelGestures(): void {
  // … 8 lines
  cameraHand.cancel();
// … 2 lines
}

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  const camera = cameraHand.gesture();
  if (camera) return camera;
// … 14 lines
}

// src/main.ts:1122-1126 slice of src/main.ts#releaseHeld (shared): the feature's statements; "…" marks code left out
function releaseHeld(): void {
  // … 2 lines
  cameraHand.releaseAll();
}

// src/main.ts:1166-1170 slice of src/main.ts#CanvasInput (shared): the feature's statements; "…" marks code left out
type CanvasInput =
  | { readonly type: 'dblclick' | 'click'; readonly e: MouseEvent; readonly r: DOMRect }
  | { readonly type: 'down' | 'up' | 'cancel' | 'leave' | 'lost'; readonly e: PointerEvent; readonly r: DOMRect }
  | { readonly type: 'move'; readonly e: PointerEvent; readonly r: DOMRect; readonly path: PointerSample[] | undefined }
  | { readonly type: 'wheel'; readonly e: WheelEvent; readonly r: DOMRect };

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 1 lines
  cameraHand.press(e.pointerId, { x: e.clientX - r.left, y: e.clientY - r.top }, e.timeStamp);
  // … 2 lines
  if (cameraHand.touches >= 2) {
    cancelGestures();
    cameraHand.startPinch();
    return;
  }
  // … 2 lines
  if (e.pointerType === 'mouse' && e.button === 2 && !e.shiftKey) {
    // … 6 lines
    cameraHand.startOrbit(e.pointerId, at, gestureInProgress(), groundHeightUnder(at));
  // … 1 lines
  }
  // … 1 lines
  if (e.pointerType === 'mouse' && (e.button === 1 || e.button === 2)) {
    // … 1 lines
    cameraHand.startPan(e.pointerId, at);
  // … 1 lines
  }
  // … 7 lines
  switch (game.tool) {
    // … 57 lines
    case 'move':
      if (anchor.kind === 'node' && anchor.node !== undefined) {
      // … 1 lines
      } else {
        cameraHand.startPan(e.pointerId, { x: e.clientX - r.left, y: e.clientY - r.top });
      }
      break;
  // … 77 lines
  }
// … 1 lines
}

// src/main.ts:1399-1402 slice of src/main.ts#captureLost (shared): the feature's statements; "…" marks code left out
function captureLost(e: PointerEvent): void {
  // … 1 lines
  if (cameraHand.has(e.pointerId)) releaseHeld();
}

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 5 lines
  const held = cameraHand.active ? 'held' : '';
  if (canvas.dataset['camera'] !== held) canvas.dataset['camera'] = held;
  // … 1 lines
  if (cameraHand.move(e.pointerId, screen, e.timeStamp, cameraHand.active ? path : undefined)) return;
// … 61 lines
}

// src/main.ts:1484-1513 slice of src/main.ts#endPointer (shared): the feature's statements; "…" marks code left out
function endPointer(e: PointerEvent): void {
  // … 1 lines
  const { wasPinching, clickCancels } = cameraHand.release(e.pointerId, e.timeStamp);
  // … 1 lines
  if (clickCancels && !cancelled) {
    cancelGestures();
    return;
  }
  // … 4 lines
  if (game.tool === 'building') buildings.pointerUp(cancelled || wasPinching);
  bulldozer.up(e.pointerId, !cancelled && !wasPinching);
  lotTool.up(e.pointerId, !cancelled && !wasPinching);
  // … 1 lines
  roadTool.up(() => pointerWorld(e), !cancelled && !wasPinching);
  // … 1 lines
  poleTool.up(!cancelled && !wasPinching);
  // … 1 lines
  if (game.tool === 'streetscape') {
    if (cancelled || wasPinching) streetscapeTool.row = null;
    else streetscapeTool.up();
  }
  // … 2 lines
  mover.drop(!cancelled && !wasPinching);
// … 2 lines
}

// src/main.ts:1535-1550 slice of src/main.ts#wheel (shared): the feature's statements; "…" marks code left out
function wheel(e: WheelEvent, r: DOMRect): void {
  {
    // … 10 lines
    cameraMotion.wheel(-e.deltaY * lines * 0.0013, at);
  // … 1 lines
  }
}

// src/main.ts:1859-1946 slice of src/main.ts#mountInterface (shared): the feature's statements; "…" marks code left out
function mountInterface(): void {
  // … 73 lines
  if (view.kind !== '2d') {
    compass = mountCompass(document.querySelector<HTMLElement>('.v2') ?? document.body, {
      north: () => {
        cameraMotion.aim(() => northTurn(view, surface.cssW, surface.cssH));
      // … 1 lines
      },
    // … 5 lines
    });
  }
}

// src/main.ts:2230-2240 slice of src/main.ts#fitOpeningView (shared): the feature's statements; "…" marks code left out
function fitOpeningView(): void {
  // … 5 lines
  camera.fit({ minX: cx - half, minY: cy - half, maxX: cx + half, maxY: cy + half }, surface.cssW, surface.cssH);
// … 3 lines
}

// src/main.ts:2372-2391 slice of src/main.ts#restoreSettings (shared): the feature's statements; "…" marks code left out
function restoreSettings(settings: SavedSettings): void {
  camera.x = settings.camera.x;
  camera.y = settings.camera.y;
  // … 2 lines
  camera.zoom = clamp(settings.camera.zoom, limits.min, limits.max);
// … 13 lines
}

// src/main.ts:2479-2555 slice of src/main.ts#gameKeys (shared): the feature's statements; "…" marks code left out
const gameKeys = createGameKeys({
  // … 20 lines
  camera: {
    turn: (radians) => cameraMotion.turn(radians),
    press: (code, shift) => cameraMotion.press(code, shift),
  // … 1 lines
  },
// … 50 lines
});

// src/main.ts:2564-2564 slice of src/main.ts#<ExpressionStatement>@2564 (shared): the feature's statements; "…" marks code left out
window.addEventListener('keyup', (e) => { cameraMotion.release(e.code, e.shiftKey); });

// src/main.ts:2566-2566 slice of src/main.ts#<ExpressionStatement>@2566 (shared): the feature's statements; "…" marks code left out
window.addEventListener('blur', () => cameraMotion.releaseAll());

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 12 lines
  if (cameraMotion.step(now)) persistence.saveSettingsSoon(sessionSettings);
  // … 1 lines
  cameraHand.advance(now);
  // … 82 lines
  if (!pageHidden() && (measure.on || !effects.onDemand || simMoving || roadTool.draft || mover.dragging || cameraHand.active || cameraHand.coasting || cameraMotion.moving || scene.busy())) requestDraw();
  // Only the clouds moving (they drift, form and fade), or the clock alone: a slower frame.
  else if (!pageHidden() && (!game.paused || scene.drifting())) frameClock.drift();
}

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 17 lines
  camera,
  // … 44 lines
  lookAt: (x: number, y: number, zoom = camera.zoom) => {
    camera.x = x;
    camera.y = y;
    camera.zoom = zoom;
    syncViewFromFlatCamera();
    requestDraw();
  },
  // … 9 lines
  setView: (at: { x: number; y: number; zoom: number; azimuth: number; elevation: number }) => {
    camera.x = at.x;
    camera.y = at.y;
    camera.zoom = at.zoom;
  // … 4 lines
  },
// … 107 lines
};
