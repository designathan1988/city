// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:159-159 slice of src/main.ts#<ExpressionStatement> (shared): the feature's statements; "…" marks code left out
setGlobalParams(roadKeyParams);

// src/main.ts:160-160 slice of src/main.ts#<ExpressionStatement>@160 (shared): the feature's statements; "…" marks code left out
initLanguage();

// src/main.ts:173-187 slice of src/main.ts#healthWatch (shared): the feature's statements; "…" marks code left out
const healthWatch = watchHealth({
  log: health,
  frames: frameTimer,
  canvas,
  context: () => {
    const last = doc.changes.latest(1)[0];
    // Shown to the player in the F9 monitor: every word from the dictionaries; the gestures are dictionary keys
    return [
      t('health.context.tool', { tool: game.tool }),
      game.gesture ? t(game.gesture) : '',
      last ? t('health.context.lastChange', { serial: last.serial, kind: last.kind, cause: last.cause }) : '',
      booted ? '' : t('health.context.opening'),
    ].filter(Boolean).join(' · ');
  },
});

// src/main.ts:197-197 slice of src/main.ts#camera (shared): the feature's statements; "…" marks code left out
const camera = new Camera();

// src/main.ts:201-201 slice of src/main.ts#history (shared): the feature's statements; "…" marks code left out
const history = new History();

// src/main.ts:204-204 slice of src/main.ts#desktopStores (shared): the feature's statements; "…" marks code left out
const desktopStores = desktopSaves();

// src/main.ts:205-205 slice of src/main.ts#persistence (shared): the feature's statements; "…" marks code left out
const persistence = desktopStores ? new Persistence(undefined, desktopStores.store, desktopStores.parts) : new Persistence();

// src/main.ts:210-218 slice of src/main.ts#<IfStatement>@210 (shared): the feature's statements; "…" marks code left out
if (desktopStores) {
  // … 1 lines
  const handPort = (worker: Worker): void => {
    desktopStores.connect().then((port) => worker.postMessage({ storeSetup: true, port }, [port]),
      (error: unknown) => console.error('[save] a job worker got no port to the saves', error));
  };
  // … 1 lines
  desktopStores.onLost(() => { for (const worker of workers) handPort(worker); });
}

// src/main.ts:241-241 slice of src/main.ts#bootSave (shared): the feature's statements; "…" marks code left out
const bootSave = restoreBootSave(streamedAtBoot ? null : await persistence.loadSessionAsync());

// src/main.ts:248-276 slice of src/main.ts#restoreBootSave (shared): the feature's statements; "…" marks code left out
function restoreBootSave(loaded: Awaited<ReturnType<Persistence['loadSessionAsync']>>): { readonly settings: SavedSettings | null; readonly hadMap: boolean; readonly failed: boolean } {
  // … 2 lines
  const map = loaded?.document && !isUntouchedStarter(loaded.document) ? loaded.document : null;
  const settings = loaded?.settings ?? null;
  if (!map) return { settings, hadMap: false, failed: false };
  // … 1 lines
  try {
    // … 3 lines
    const asTable = (map.buildings?.length ?? 0) >= TABLE_RESTORE_BUILDINGS ? (() => { const made = prepareCity(map); return preparedCity(made.data, made.buildings); })() : null;
    restoreInto(doc, asTable?.data ?? map, net, asTable?.table ?? null);
    // … 3 lines
    persistence.adoptLoaded(doc);
    return { settings, hadMap: true, failed: false };
  } catch (error) {
    // … 1 lines
    persistence.quarantineStored();
    // … 5 lines
    return { settings, hadMap: true, failed: true };
  }
}

// src/main.ts:319-319 slice of src/main.ts#bootSettings (shared): the feature's statements; "…" marks code left out
const bootSettings = savedSession && hadSavedMap ? savedSession.settings : streamedAtBoot ? persistence.readSettings() : null;

// src/main.ts:320-328 slice of src/main.ts#<IfStatement>@320 (shared): the feature's statements; "…" marks code left out
if (bootSettings) {
  camera.x = bootSettings.camera.x;
  camera.y = bootSettings.camera.y;
  // … 2 lines
  camera.zoom = clamp(bootSettings.camera.zoom, limits.min, limits.max);
} else {
  camera.fit(worldBounds(), surface.cssW, surface.cssH);
}

// src/main.ts:385-385 slice of src/main.ts#<ExpressionStatement>@385 (shared): the feature's statements; "…" marks code left out
sim.clock.paused = savedSession?.settings.paused === true;

// src/main.ts:386-386 slice of src/main.ts#<ExpressionStatement>@386 (shared): the feature's statements; "…" marks code left out
sim.clock.speed = savedSession?.settings.speed ?? 1;

// src/main.ts:387-387 slice of src/main.ts#<ExpressionStatement>@387 (shared): the feature's statements; "…" marks code left out
sim.trafficIntensity = savedSession?.settings.trafficIntensity ?? 1;

// src/main.ts:388-388 slice of src/main.ts#<ExpressionStatement>@388 (shared): the feature's statements; "…" marks code left out
sim.pedestrianIntensity = savedSession?.settings.pedestrianIntensity ?? 1;

// src/main.ts:389-389 slice of src/main.ts#<ExpressionStatement>@389 (shared): the feature's statements; "…" marks code left out
sim.demandMultiplier = savedSession?.settings.demandMultiplier ?? 1;

// src/main.ts:391-391 slice of src/main.ts#<IfStatement>@391 (shared): the feature's statements; "…" marks code left out
if (bootSettings?.minutes !== undefined) sim.setClock(bootSettings.minutes);

// src/main.ts:393-430 slice of src/main.ts#gameState (shared): the feature's statements; "…" marks code left out
const gameState = new GameState({
  // … 1 lines
  paused: sim.clock.paused,
  speed: sim.clock.speed,
  // … 24 lines
  congestionOverlay: savedSession?.settings.congestionOverlay ?? false,
// … 8 lines
}, () => { if (booted) requestDraw(); });

// src/main.ts:443-455 slice of src/main.ts#sessionSettings (shared): the feature's statements; "…" marks code left out
function sessionSettings(): SavedSettings {
  const centre = view.centre;
  return {
    camera: { x: centre.x, y: centre.y, zoom: view.zoom, azimuth: view.azimuth, elevation: view.elevation },
    paused: sim.clock.paused,
    speed: sim.clock.speed,
    trafficIntensity: sim.trafficIntensity,
    pedestrianIntensity: sim.pedestrianIntensity,
    demandMultiplier: sim.demandMultiplier,
  // … 2 lines
  };
}

// src/main.ts:458-486 slice of src/main.ts#roadTool (shared): the feature's statements; "…" marks code left out
const roadTool = new RoadTool({
  // … 2 lines
  zoom: () => view.zoom,
  px: (n) => camera.px(n),
  settings: () => ({
    // … 1 lines
    heightOffset: game.roadHeightOffset, parking: roadParking(), width: roadWidth(), grid: roadGridShown(),
  // … 4 lines
  }),
  // … 11 lines
  undoGroup: (group) => history.grouped(group),
// … 3 lines
});

// src/main.ts:488-498 slice of src/main.ts#poleTool (shared): the feature's statements; "…" marks code left out
const poleTool = new PoleTool({
  // … 2 lines
  zoom: () => view.zoom,
  mode: () => poleToolMode(),
  lamps: () => poleLampMode(),
// … 4 lines
});

// src/main.ts:502-513 slice of src/main.ts#streetscapeTool (shared): the feature's statements; "…" marks code left out
const streetscapeTool = new StreetscapeTool({
  // … 2 lines
  zoom: () => view.zoom,
  kind: () => streetscapeKind(),
  extra: (kind) => kind === 'sign' ? { signType: signChoice.type, text: signChoice.text }
    : kind === 'streetname' ? { text: signChoice.streetName }
    : kind === 'tree' || kind === 'shrub' ? { planted: sim.minutes } : {},
// … 3 lines
});

// src/main.ts:515-528 slice of src/main.ts#barrierTool (shared): the feature's statements; "…" marks code left out
const barrierTool = new BarrierTool({
  // … 2 lines
  removeAt: (at) => {
    // … 1 lines
    const hit = doc.barrierNear(at, BARRIER_PICK_PIXELS / view.zoom);
    if (!hit) return false;
    mutate(() => doc.removeBarrier(hit.id));
  // … 1 lines
  },
// … 3 lines
});

// src/main.ts:530-543 slice of src/main.ts#lotTool (shared): the feature's statements; "…" marks code left out
const lotTool = new LotTool({
  // … 2 lines
  zoom: () => view.zoom,
// … 9 lines
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

// src/main.ts:579-589 slice of src/main.ts#terrainBrush (shared): the feature's statements; "…" marks code left out
const terrainBrush = new TerrainBrush({
  // … 5 lines
  record: () => { history.begin(doc); updateHistoryButtons(); },
// … 3 lines
});

// src/main.ts:591-605 slice of src/main.ts#dabSettings (shared): the feature's statements; "…" marks code left out
function dabSettings(): DabSettings {
  const fog = fogBrush();
  return {
    // … 5 lines
    element: { mode: elementMode(), kind: elementKind(), brush: elementBrush(elementKind()) },
    tree: { mode: treeMode(), kind: treeKind(), brush: treeBrush() },
    gully: { strength: gullyStrength, erase: gullyErase() },
    fog: { strength: fog.strength, height: fog.height, speed: fog.speed, erase: fogErase() },
    paint: paintKind(),
  };
}

// src/main.ts:608-608 slice of src/main.ts#view (shared): the feature's statements; "…" marks code left out
let view: Viewport = flatViewport(camera);

// src/main.ts:671-671 slice of src/main.ts#scene (shared): the feature's statements; "…" marks code left out
const scene: SceneHandle = createSceneRenderer(canvas3d, { x: camera.x, y: camera.y }, camera.zoom, graphics.level, requestDraw, engine, graphics.settings);

// src/main.ts:674-691 slice of src/main.ts#comesBackFromLostContext (shared): the feature's statements; "…" marks code left out
function comesBackFromLostContext(canvas: HTMLCanvasElement): void {
  // … 2 lines
  canvas.addEventListener('webglcontextlost', (event) => {
    // … 2 lines
    reload = setTimeout(() => {
      try {
        persistence.saveSessionSoon(doc, sessionSettings);
        persistence.flush();
      } catch (error) {
      // … 1 lines
      }
    // … 1 lines
    }, RELOAD_MS);
  });
// … 1 lines
}

// src/main.ts:699-741 slice of src/main.ts#wireProfiler (shared): the feature's statements; "…" marks code left out
function wireProfiler(): () => void {
  // … 4 lines
  mountProfilerHud(profiler, { visible: new URLSearchParams(location.search).has('profiler') });
// … 36 lines
}

// src/main.ts:744-744 slice of src/main.ts#<ExpressionStatement>@744 (shared): the feature's statements; "…" marks code left out
view = scene.viewport;

// src/main.ts:745-745 slice of src/main.ts#<ExpressionStatement>@745 (shared): the feature's statements; "…" marks code left out
restoreOrbit(savedSession?.settings.camera);

// src/main.ts:760-806 slice of src/main.ts#buildings (shared): the feature's statements; "…" marks code left out
const buildings = createBuildingWiring({
  // … 2 lines
  history,
  // … 1 lines
  view: () => view,
  // … 3 lines
  focusBuilding(building) {
    // … 6 lines
    view.moveTo(centre);
      // … 3 lines
      const corners = building.volumes.flatMap((v) => [
        [v.x, v.y], [v.x + v.w, v.y], [v.x + v.w, v.y + v.d], [v.x, v.y + v.d],
      ] as const).map(([x, y]) => view.toScreen({ x: building.x + x * c - y * s, y: building.y + x * s + y * c }, surface.cssW, surface.cssH));
      const projectedWidth = Math.max(...corners.map((p) => p.x)) - Math.min(...corners.map((p) => p.x));
      // … 2 lines
      const projectedRise = Math.abs(view.toScreen(centre, surface.cssW, surface.cssH, top).y
        - view.toScreen(centre, surface.cssW, surface.cssH).y);
      const projectedFootprint = Math.max(...corners.map((p) => p.y)) - Math.min(...corners.map((p) => p.y));
      const fit = Math.min(view.zoom * (available * .72) / Math.max(1, projectedWidth),
        view.zoom * (surface.cssH - 145) * .88 / Math.max(1, projectedRise + projectedFootprint));
    // … 1 lines
    const target = Math.max(view.zoomBounds.min, Math.min(view.zoomBounds.max, ideal, fit));
    view.zoomAt(surface.cssW / 2, surface.cssH / 2, target / view.zoom, surface.cssW, surface.cssH);
      const rise = Math.abs(view.toScreen(centre, surface.cssW, surface.cssH, top).y
        - view.toScreen(centre, surface.cssW, surface.cssH).y);
      view.panTo(centre, (surface.cssW - paletteWidth) / 2, surface.cssH / 2 + rise / 2 - 68, surface.cssW, surface.cssH);
  },
  afterEdit() {
    persistence.saveSessionSoon(doc, sessionSettings);
  // … 2 lines
  },
// … 3 lines
});

// src/main.ts:808-810 slice of src/main.ts#restoreOrbit (shared): the feature's statements; "…" marks code left out
function restoreOrbit(saved: SavedSettings['camera'] | undefined): void {
  view.setOrbit(saved?.azimuth ?? DEFAULT_AZIMUTH, saved?.elevation ?? DEFAULT_ELEVATION);
}

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
  if (view.kind === '2d') return;
  const centre = view.centre;
  camera.x = centre.x;
  camera.y = centre.y;
  camera.zoom = view.zoom;
}

// src/main.ts:865-872 slice of src/main.ts#serializedDoc (shared): the feature's statements; "…" marks code left out
function serializedDoc(): string {
  // … 3 lines
  const text = serialize(doc);
  docText = { key, text };
  return text;
}

// src/main.ts:874-874 slice of src/main.ts#<ExpressionStatement>@874 (shared): the feature's statements; "…" marks code left out
persistence.documentText = (d) => (d === doc ? serializedDoc() : null);

// src/main.ts:876-911 slice of src/main.ts#mutateBuilt (shared): the feature's statements; "…" marks code left out
function mutateBuilt(fn: () => boolean): boolean {
  // … 2 lines
  followNetwork(doc, net);
  // … 1 lines
  history.begin(doc);
  // … 17 lines
  const recorded = history.end(doc);
  if (!changed || !recorded) return false;
  // … 3 lines
  persistence.saveSessionSoon(doc, sessionSettings);
  // … 2 lines
  refreshInspector();
// … 3 lines
}

// src/main.ts:915-928 slice of src/main.ts#applySnapshot (shared): the feature's statements; "…" marks code left out
function applySnapshot(data: ReturnType<RoadDoc['toJSON']> | null, source: 'snapshot' | 'import' = 'snapshot', table: ArrayBufferLike | null = null): void {
  // … 2 lines
  if (source === 'import') {
    caused('map opened', () => restoreInto(doc, data, net, table));
  // … 2 lines
  } else {
    restoreSnapshot(doc, data, net);
  }
// … 3 lines
}

// src/main.ts:935-941 slice of src/main.ts#saveOnceShown (shared): the feature's statements; "…" marks code left out
async function saveOnceShown(): Promise<void> {
  // … 4 lines
  persistence.saveSessionSoon(doc, sessionSettings);
}

// src/main.ts:942-953 slice of src/main.ts#snapshotApplied (shared): the feature's statements; "…" marks code left out
function snapshotApplied(): void {
  // … 3 lines
  if (deferSessionSave) {
    deferSessionSave = false;
    void saveOnceShown();
  } else persistence.saveSessionSoon(doc, sessionSettings);
// … 3 lines
}

// src/main.ts:959-971 slice of src/main.ts#worldAtScreen (shared): the feature's statements; "…" marks code left out
function worldAtScreen(px: number, py: number, heightOffset?: number): Vec2 {
  let point = view.toWorld(px, py, surface.cssW, surface.cssH);
  if (view.kind === '2d') return point;
  if (heightOffset === undefined) return firstSurfaceAt(px, py) ?? point;
  // … 1 lines
  for (let pass = 0; pass < 3; pass++) {
    const next = scene.terrainHeightAt(point.x, point.y) + heightOffset;
    if (Math.abs(next - height) < 0.05) break;
    height = next;
    point = view.toWorldAt(px, py, height, surface.cssW, surface.cssH);
  }
  return point;
}

// src/main.ts:975-975 slice of src/main.ts#groundHeightUnder.point (shared): the feature's statements; "…" marks code left out
  const point = (h: number): Vec2 => view.toWorldAt(at.x, at.y, h, surface.cssW, surface.cssH);

// src/main.ts:1006-1006 slice of src/main.ts#firstSurfaceAt.at (shared): the feature's statements; "…" marks code left out
  const at = (h: number): Vec2 => view.toWorldAt(px, py, h, surface.cssW, surface.cssH);

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

// src/main.ts:1137-1150 slice of src/main.ts#cloudTool (shared): the feature's statements; "…" marks code left out
const cloudTool = new CloudTool({
  // … 1 lines
  mode: () => cloudMode(),
  brush: () => cloudBrush(),
  // … 1 lines
  rayAt: (px, py, height) => view.toWorldAt(px, py, height, surface.cssW, surface.cssH),
  record: () => { history.begin(doc); cloudStepOpen = true; },
  changed: () => {
    // … 1 lines
    persistence.saveSessionSoon(doc, sessionSettings);
  // … 1 lines
  },
// … 1 lines
});

// src/main.ts:1152-1162 slice of src/main.ts#endTerrainStroke (shared): the feature's statements; "…" marks code left out
function endTerrainStroke(): void {
  // … 2 lines
  if (wasPainting) { history.end(doc); updateHistoryButtons(); }
// … 6 lines
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
  // … 5 lines
  const anchor = findAnchor(doc, net, world, view.zoom);
  // … 1 lines
  switch (game.tool) {
    // … 7 lines
    case 'road':
      if (blockGridChoice.armed) {
        // Roads > Blocks at once: the whole grid, centred where the click lands.
        blockGridChoice.armed = false;
        let laid = 0;
        mutate(() => {
          laid = commitBlockGrid(doc, net, world, blockGridChoice, game.roadTypeIndex, game.roadLanePreset, roadParking());
          return laid > 0;
        });
        flashHint(laid > 0 ? 'hint.blocks.built' : 'hint.road.invalid');
        refreshShell();
        requestDraw();
        break;
      }
      roadTool.down(world, anchor);
      break;
    // … 34 lines
    case 'move':
      if (anchor.kind === 'node' && anchor.node !== undefined) {
        mover.grab(anchor.node);
      } else {
        cameraHand.startPan(e.pointerId, { x: e.clientX - r.left, y: e.clientY - r.top });
      }
      break;
    // … 1 lines
    case 'split':
      if (
        anchor.kind === 'segment' &&
        anchor.segment !== undefined &&
        anchor.s !== undefined
      ) {
        let node: NodeId | null = null;
        if (!mutateRoads((doc, net) => {
          node = splitSegment(doc, net, anchor.segment as SegmentId, anchor.s as number, anchor.at);
          return node !== null;
        })) node = null;
        if (node !== null) {
          select(null, game.selectedSegmentS, node, 'road split');
          showInspector();
        }
      }
      break;
    // … 1 lines
    case 'bulldoze':
      // … 1 lines
      if (strikeChoice.mode === 'strike') {
        const sx = e.clientX - r.left, sy = e.clientY - r.top, at = { ...world };
        void scene.effects().then(() => actions.strikeAt(sx, sy, at));
        break;
      }
      if (strikeChoice.mode === 'shoot') {
        const sx = e.clientX - r.left, sy = e.clientY - r.top;
        void scene.effects().then(() => actions.shootAt(sx, sy));
        break;
      }
      // … 1 lines
      bulldozer.down(e.pointerId, { x: e.clientX - r.left, y: e.clientY - r.top }, world, anchor);
      break;
    // … 1 lines
    case 'upgrade':
      if (anchor.kind === 'segment' && anchor.segment !== undefined) {
        const id = anchor.segment;
        const seg = doc.segment(id);
        if (seg && seg.type < LAST_UPGRADE_CLASS) {
          mutateRoads((doc, net) => carryStreetKit(doc, net, [id], () => {
            doc.setSegmentType(id, seg.type + 1);
            return true;
          }));
        }
      }
      break;
    // … 1 lines
    case 'control':
      if (anchor.kind === 'node' && anchor.node !== undefined) {
        cycleNodeControl(anchor.node, e.shiftKey ? -1 : 1);
      } else {
        flashHint('hint.control.miss');
      }
      break;
    // … 1 lines
    case 'inspect': {
      // … 15 lines
      select(anchor.kind === 'segment' ? (anchor.segment ?? null) : null,
        anchor.kind === 'segment' ? (anchor.s ?? null) : null,
        anchor.kind === 'node' ? (anchor.node ?? null) : null, 'inspection click');
      // … 1 lines
      break;
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
  // … 51 lines
  const hovered = findAnchor(doc, net, world, view.zoom, undefined,
    game.tool === 'road' ? game.roadHeightOffset : undefined);
  // … 1 lines
  hoverAnchor = game.tool === 'terrain'
    ? { kind: 'free', at: world }
    : game.tool === 'road'
      ? anchorForHeight(doc, net, hovered, game.roadHeightOffset)
      : hovered;
// … 2 lines
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
  // … 3 lines
  if (cloudStepOpen) { cloudStepOpen = false; history.end(doc); updateHistoryButtons(); }
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
  persistence.saveSettingsSoon(sessionSettings);
// … 1 lines
}

// src/main.ts:1535-1550 slice of src/main.ts#wheel (shared): the feature's statements; "…" marks code left out
function wheel(e: WheelEvent, r: DOMRect): void {
  {
    // … 10 lines
    cameraMotion.wheel(-e.deltaY * lines * 0.0013, at);
  // … 1 lines
  }
}

// src/main.ts:1558-1566 slice of src/main.ts#roadClasses (shared): the feature's statements; "…" marks code left out
function roadClasses(): ShellRoadClass[] {
  return ROAD_TYPES.map((rt, index) => ({
    // … 1 lines
    name: roadTypeName(rt),
    title: `${roadTypeName(rt)} — ${roadTypeDescription(rt)}`,
  // … 2 lines
  }));
}

// src/main.ts:1577-1585 slice of src/main.ts#laneChoices (shared): the feature's statements; "…" marks code left out
function laneChoices(): { id: string; label: string; on: boolean }[] {
  return LANE_CHOICES.map((choice) => ({
    // … 1 lines
    label: choice.lanes === null ? t('palette.lanes.median') : plural('palette.lanes.count', choice.lanes),
  // … 3 lines
  }));
}

// src/main.ts:1611-1611 slice of src/main.ts#<ExpressionStatement>@1611 (shared): the feature's statements; "…" marks code left out
onRoadKeysChange(() => { applyTranslations(document); updateHint(); });

// src/main.ts:1617-1630 slice of src/main.ts#setZone (shared): the feature's statements; "…" marks code left out
function setZone(patch: ShellZonePatch): void {
  if (patch.eraser !== undefined) gameState.set('zoneEraser', patch.eraser, 'zone eraser');
  if (patch.use === 'residential' || patch.use === 'commercial' || patch.use === 'industrial') gameState.set('zoneUse', patch.use, 'zone use');
  if (patch.density === 'low' || patch.density === 'medium' || patch.density === 'high') gameState.set('zoneDensity', patch.density, 'zone density');
  if (patch.mode !== undefined) {
    const wanted = patch.mode;
    gameState.set('zoneMode', wanted === 'fill' || wanted === 'edit' || wanted === 'front' || wanted === 'split' || wanted === 'join' || wanted === 'add' ||
      wanted === 'polygon' || wanted === 'curve' || wanted === 'delete' ? wanted : 'brush', 'zone mode');
    lotTool.modeChanged();
  }
  if (patch.splitKind !== undefined) gameState.set('lotSplitKind', patch.splitKind === 'horizontal' ? 'horizontal' : patch.splitKind === 'line' ? 'line' : 'vertical', 'lot cut');
  if (patch.splitParts !== undefined) gameState.set('lotSplitParts', clamp(Math.round(patch.splitParts), 2, 6), 'lot parts');
// … 1 lines
}

// src/main.ts:1655-1668 slice of src/main.ts#mapSlider (shared): the feature's statements; "…" marks code left out
function mapSlider(min: number, max: number, step: number, get: () => number, set: (v: number) => void): ShellSlider {
// … 12 lines
}

// src/main.ts:1659-1664 method mapSlider.set: the feature's own
    set: (v) => {
      if (!recorded) { history.begin(doc); recorded = true; updateHistoryButtons(); }
      set(v);
      persistence.saveSessionSoon(doc, sessionSettings);
      requestDraw();
    },

// src/main.ts:1666-1666 method mapSlider.end: the feature's own
    end: () => { if (recorded) { history.end(doc); updateHistoryButtons(); } recorded = false; },

// src/main.ts:1670-1671 slice of src/main.ts#brushSlider (shared): the feature's statements; "…" marks code left out
const brushSlider = (min: number, max: number, step: number, get: () => number, set: (v: number) => void): ShellSlider =>
  ({ min, max, step, get, set: (v) => { set(v); requestDraw(); } });

// src/main.ts:1674-1674 slice of src/main.ts#<ExpressionStatement>@1674 (shared): the feature's statements; "…" marks code left out
scene.onStrike((_x, _y, distance) => playThunder(distance / UNITS_PER_METER, doc.weather.thunder));

// src/main.ts:1675-1676 slice of src/main.ts#weatherSlider (shared): the feature's statements; "…" marks code left out
const weatherSlider = (key: keyof typeof doc.weather, min: number, max: number, step: number, scale: number): ShellSlider =>
  mapSlider(min, max, step, () => Math.round(doc.weather[key] * scale * 10) / 10, (v) => doc.setWeather({ [key]: v / scale }));

// src/main.ts:1681-1689 slice of src/main.ts#<TryStatement>@1681 (shared): the feature's statements; "…" marks code left out
try {
  // … 1 lines
  if (kept) {
    for (const key of Object.keys(atmosphere) as (keyof typeof atmosphere)[]) {
      const value = kept[key];
      if (value !== undefined && Number.isFinite(value)) atmosphere[key] = value;
    }
  }
} catch { /* storage blocked: the defaults */ }

// src/main.ts:1690-1701 slice of src/main.ts#applyAtmosphere (shared): the feature's statements; "…" marks code left out
function applyAtmosphere(redraw = true): void {
  scene.setAtmosphere({
    clouds: atmosphere.atmoClouds / 100,
    cloudBase: atmosphere.atmoCloudBase * UNITS_PER_METER,
    cloudThickness: atmosphere.atmoCloudThickness * UNITS_PER_METER,
    fog: atmosphere.atmoFog / 100,
    fogHeight: atmosphere.atmoFogHeight * UNITS_PER_METER,
  });
  try { localStorage.setItem(ATMOSPHERE_KEY, JSON.stringify(atmosphere)); } catch { /* not kept */ }
// … 2 lines
}

// src/main.ts:1703-1711 slice of src/main.ts#<IfStatement>@1703 (shared): the feature's statements; "…" marks code left out
if (atmosphere.atmoClouds > 0) {
  if (doc.clouds.length === 0) {
    doc.addClouds(scatterClouds(Math.round((atmosphere.atmoClouds / 100) * 12), {
      size: Math.max(40, atmosphere.atmoCloudThickness * UNITS_PER_METER), height: atmosphere.atmoCloudBase * UNITS_PER_METER, density: 0.8,
    }, 0.3, CLOUD_SPAN / 2, [], editRandom(doc, 0, 0)));
    persistence.saveSessionSoon(doc, sessionSettings);
  }
  atmosphere.atmoClouds = 0;
}

// src/main.ts:1715-1771 slice of src/main.ts#SLIDERS (shared): the feature's statements; "…" marks code left out
const SLIDERS: Readonly<Record<string, ShellSlider>> = {
  // The terrain brush (Paisagem > Relevo).
  terrainRadius: { min: TERRAIN_RADIUS_RANGE[0], max: TERRAIN_RADIUS_RANGE[1], step: 10, get: () => game.terrainRadius, set: setTerrainRadius },
  terrainStrength: { min: TERRAIN_STRENGTH_RANGE[0], max: TERRAIN_STRENGTH_RANGE[1], step: 1, get: () => game.terrainStrength, set: setTerrainStrength },
  // The brush's hardness: a mesa's cliff or a canyon's wall instead of a dome; each mode keeps its own.
  terrainHardness: { min: 0, max: 95, step: 5, get: () => game.terrainHardness, set: (v) => {
    gameState.set('terrainHardness', clamp(Math.round(v), 0, 95), 'brush hardness');
    if (hardnessByMode[game.terrainMode] !== undefined) hardnessByMode[game.terrainMode] = game.terrainHardness;
  } },
  // The gully brush, and how much of the steep land carries gullies of itself (the map's, `world/gullies.ts`).
  gullyStrength: { min: 5, max: 100, step: 5, get: () => gullyStrength, set: (v) => { gullyStrength = v; } },
  gullyAuto: mapSlider(0, 100, 5, () => Math.round(doc.gullyAuto * 100), (v) => doc.setGullyAuto(v / 100)),
  // The water of every river and lake.
  waterWaves: weatherSlider('waves', 0, 100, 5, 100),
  waterFoam: weatherSlider('foam', 0, 100, 5, 100),
  waterCurrent: weatherSlider('current', 0, 4, 0.1, 1),
  // Rain, wind, lightning and thunder over the whole map.
  weatherRain: weatherSlider('rain', 0, 100, 5, 100),
  weatherWind: weatherSlider('wind', 0, 30, 1, 1),
  weatherWindDir: weatherSlider('windDirection', 0, 355, 5, 1),
  weatherLightning: weatherSlider('lightning', 0, 30, 1, 1),
  weatherThunder: weatherSlider('thunder', 0, 100, 5, 100),
  // The tree brush (`world/trees.ts`).
  treeDensity: brushSlider(10, 600, 10, () => treeBrush().density, (v) => setTreeBrush({ density: v })),
  treeHeight: brushSlider(3, 30, 1, () => treeBrush().height, (v) => setTreeBrush({ height: v })),
  treeVariation: brushSlider(0, 80, 5, () => treeBrush().variation, (v) => setTreeBrush({ variation: v })),
  treeSpacing: brushSlider(2, 30, 1, () => treeBrush().spacing, (v) => setTreeBrush({ spacing: v })),
  // The element brush, each kind its own settings (`world/elements.ts`).
  elDensity: brushSlider(0.1, 200, 0.1, () => elementBrush().density, (v) => setElementBrush({ density: v })),
  elSize: brushSlider(0.05, 10, 0.05, () => elementBrush().size, (v) => setElementBrush({ size: v })),
  elVariation: brushSlider(0, 100, 5, () => elementBrush().variation, (v) => setElementBrush({ variation: v })),
  elSpacing: brushSlider(0, 20, 0.1, () => elementBrush().spacing, (v) => setElementBrush({ spacing: v })),
  elStrength: brushSlider(5, 100, 5, () => elementBrush().strength, (v) => setElementBrush({ strength: v })),
  elIntensity: brushSlider(0, 100, 5, () => elementBrush().intensity, (v) => setElementBrush({ intensity: v })),
  // The cloud tool: the next cloud's size, base and density, and how many "spread over the sky" lays.
  cloudSize: brushSlider(30, 600, 10, () => cloudBrush().size, (v) => setCloudBrush({ size: v })),
  cloudHeight: brushSlider(40, 900, 10, () => cloudBrush().height, (v) => setCloudBrush({ height: v })),
  cloudDensity: brushSlider(10, 100, 5, () => cloudBrush().density, (v) => setCloudBrush({ density: v })),
  cloudCount: { min: 1, max: 24, step: 1, get: () => cloudScatter.count, set: (v) => { cloudScatter.count = v; } },
  cloudVariation: { min: 0, max: 80, step: 5, get: () => cloudScatter.variation, set: (v) => { cloudScatter.variation = v; } },
  // The fog brush: each bank keeps the thickness, height and drift it was laid with (`world/fogPaint.ts`).
  fogStrength: brushSlider(5, 100, 5, () => fogBrush().strength, (v) => setFogBrush({ strength: v })),
  fogHeight: brushSlider(2, 160, 2, () => fogBrush().height, (v) => setFogBrush({ height: v })),
  fogSpeed: brushSlider(0, 24, 0.5, () => fogBrush().speed, (v) => setFogBrush({ speed: v })),
  // The map's: every bank's thickness at once, and the mist over everything.
  fogMapDensity: mapSlider(0, 200, 10, () => Math.round(doc.fogSettings.density * 100), (v) => doc.setFogSettings({ density: v / 100 })),
  atmoFog: { min: 0, max: 100, step: 5, get: () => atmosphere.atmoFog, set: (v) => { atmosphere.atmoFog = v; applyAtmosphere(); } },
  atmoFogHeight: { min: 5, max: 200, step: 5, get: () => atmosphere.atmoFogHeight, set: (v) => { atmosphere.atmoFogHeight = v; applyAtmosphere(); } },
  // The simulation: cars and people on foot as shares (%) of what the city's own density asks (`SimWorld.followCity`).
  trafficIntensity: { min: 0, max: MAX_INTENSITY * 100, step: 10, get: () => Math.round(sim.trafficIntensity * 100),
    set: (v) => { sim.trafficIntensity = v / 100; persistence.saveSettingsSoon(sessionSettings); } },
  pedIntensity: { min: 0, max: MAX_INTENSITY * 100, step: 10, get: () => Math.round(sim.pedestrianIntensity * 100),
    set: (v) => { sim.pedestrianIntensity = v / 100; persistence.saveSettingsSoon(sessionSettings); } },
  // The roundabout tool's radius, metres.
  roundaboutRadius: { min: ROUNDABOUT_RADIUS_M[0], max: ROUNDABOUT_RADIUS_M[1], step: 4, get: () => Math.round(game.roundaboutRadius / UNITS_PER_METER),
    set: (v) => { gameState.set('roundaboutRadius', clamp(v, ROUNDABOUT_RADIUS_M[0], ROUNDABOUT_RADIUS_M[1]) * UNITS_PER_METER, 'roundabout size'); requestDraw(); } },
};

// src/main.ts:1777-1786 slice of src/main.ts#setMapBiome (shared): the feature's statements; "…" marks code left out
function setMapBiome(key: string): void {
  // … 2 lines
  history.begin(doc);
  // … 2 lines
  history.end(doc);
// … 2 lines
}

// src/main.ts:1789-1797 slice of src/main.ts#wholeMapEdit (shared): the feature's statements; "…" marks code left out
function wholeMapEdit(confirmKey: string | null, edit: () => void): void {
  if (confirmKey && !window.confirm(t(confirmKey))) return;
  history.begin(doc);
  // … 1 lines
  history.end(doc);
  // … 1 lines
  persistence.saveSessionSoon(doc, sessionSettings);
// … 1 lines
}

// src/main.ts:1798-1817 slice of src/main.ts#MAP_ACTIONS (shared): the feature's statements; "…" marks code left out
const MAP_ACTIONS: Readonly<Record<string, () => void>> = {
  // … 11 lines
  scatterClouds: () => wholeMapEdit(null, () => {
    const brush = cloudBrush();
    const laid = scatterClouds(cloudScatter.count, {
      size: brush.size * UNITS_PER_METER, height: brush.height * UNITS_PER_METER, density: brush.density / 100,
    }, cloudScatter.variation / 100, CLOUD_SPAN / 2, doc.clouds, editRandom(doc, 0, 0));
    if (doc.addClouds(laid) < cloudScatter.count) flashHint('hint.cloud.full');
  }),
};

// src/main.ts:1857-1857 slice of src/main.ts#shell (shared): the feature's statements; "…" marks code left out
let shell: ShellHandle | null = null;

// src/main.ts:1859-1946 slice of src/main.ts#mountInterface (shared): the feature's statements; "…" marks code left out
function mountInterface(): void {
  // … 1 lines
  shell = mountShell({
    workspace: buildings.workspace,
    game: gameState,
    inspectorPanel: document.getElementById('inspector'),
    pickTool: (next) => { if (isTool(next)) pickTool(next); },
    setTool: (next) => { if (isTool(next)) setTool(next); },
    putDown: freeSelection,
    setSpeed,
    undo,
    redo,
    newMap,
    newWorld,
    newLargeWorld,
    openMap: () => void openMap(),
    saveMap,
    about: openAbout,
    qualityLevels: ['auto', ...QUALITY_LEVELS],
    setQuality,
    graphicsOptions: () => graphics.openPanel(),
    effectsPanel: () => lab.open('switches'),
    markingStyle: () => doc.markingStyle,
    setMarkingStyle,
    setSky,
    toggleCongestion,
    camera: cameraCommand,
    cameraTurns: view.kind !== '2d',
    roundabout: true,
    roadClasses,
    selectRoadType,
    setAlignment: (next) => { if (next === 'straight' || next === 'curve' || next === 'free') setAlignment(next); },
    stepRoadHeight: (d) => roadTool.stepHeight(d),
    laneChoices,
    setLanes,
    setZone,
    setTerrainMode: (mode) => { if ((BRUSH_MODES as readonly string[]).includes(mode)) setTerrainMode(mode as BrushMode); },
    setBarrierKind: (kind) => { if ((BARRIER_KINDS as readonly string[]).includes(kind)) gameState.set('barrierKind', kind as BarrierKind, 'fence type'); },
    mapBiome,
    setMapBiome,
    mapAction: (name) => MAP_ACTIONS[name]?.(),
    demand: () => sim.demandMultiplier,
    setDemand,
    sliders: SLIDERS,
    onStatus: (listen) => { statusListeners.push(listen); listen(currentStatus()); },
    onHint: listenHint,
    roadSnap,
    setRoadSnap,
    parkingPresets: ROAD_PARKING_PRESETS,
    roadParkingPreset,
    setRoadParkingPreset: (preset) => {
      if ((ROAD_PARKING_PRESETS as readonly string[]).includes(preset)) setRoadParkingPreset(preset as RoadParkingPreset);
    },
    transitTool,
    // Junctions that cannot be built (`Network.impossible`)
    showImpossible: () => {
      const id = net.impossible.keys().next().value;
      const node = id === undefined ? undefined : doc.node(id);
      if (id === undefined || !node) return;
      setTool('inspect');
      select(null, null, id, 'impossible node shown');
      showInspector();
      view.moveTo(node);
      requestDraw();
    },
    // The old status bar's numbers (`ShellReadouts`): read while the Simulation or the Camera menu is open.
    readouts: () => {
      const centre = view.centre;
      return { roads: doc.segments.size, nodes: doc.nodes.size, zoom: view.zoom, x: centre.x, y: centre.y };
    },
  });
  // … 1 lines
  mountStart(document.querySelector<HTMLElement>('.v2') ?? document.body, { saved: hadSavedMap, newMap, openMap: () => void openMap(), graphicsOptions: () => graphics.openPanel(), about: openAbout });
  // … 1 lines
  if (view.kind !== '2d') {
    compass = mountCompass(document.querySelector<HTMLElement>('.v2') ?? document.body, {
      north: () => {
        cameraMotion.aim(() => northTurn(view, surface.cssW, surface.cssH));
        requestDraw();
      },
      drag: (turn, tilt) => {
        view.orbit(turn, tilt);
        persistence.saveSettingsSoon(sessionSettings);
        requestDraw();
      },
    });
  }
}

// src/main.ts:1948-1948 slice of src/main.ts#compass (shared): the feature's statements; "…" marks code left out
let compass: Compass | null = null;

// src/main.ts:1950-1952 slice of src/main.ts#updateCompass (shared): the feature's statements; "…" marks code left out
function updateCompass(): void {
  compass?.point(northOnScreen(view, surface.cssW, surface.cssH));
}

// src/main.ts:1956-1962 slice of src/main.ts#setPaused (shared): the feature's statements; "…" marks code left out
function setPaused(paused: boolean): void {
  // … 1 lines
  sim.clock.paused = paused;
  // … 1 lines
  persistence.saveSettingsSoon(sessionSettings);
// … 1 lines
}

// src/main.ts:1964-1971 slice of src/main.ts#setSpeed (shared): the feature's statements; "…" marks code left out
function setSpeed(speed: number): void {
  if (speed <= 0) setPaused(true);
  else {
    // … 1 lines
    sim.clock.speed = speed;
  // … 1 lines
  }
}

// src/main.ts:1974-1978 slice of src/main.ts#setDemand (shared): the feature's statements; "…" marks code left out
function setDemand(value: number): void {
  // … 1 lines
  sim.demandMultiplier = value;
  persistence.saveSettingsSoon(sessionSettings);
}

// src/main.ts:1980-1983 slice of src/main.ts#toggleCongestion (shared): the feature's statements; "…" marks code left out
function toggleCongestion(): void {
  // … 1 lines
  persistence.saveSettingsSoon(sessionSettings);
}

// src/main.ts:1984-1984 slice of src/main.ts#<ExpressionStatement>@1984 (shared): the feature's statements; "…" marks code left out
trackFocusModality();

// src/main.ts:1985-1985 slice of src/main.ts#<ExpressionStatement>@1985 (shared): the feature's statements; "…" marks code left out
onRoadGridChange(requestDraw);

// src/main.ts:1986-1986 slice of src/main.ts#<ExpressionStatement>@1986 (shared): the feature's statements; "…" marks code left out
mountAbout();

// src/main.ts:1998-2012 slice of src/main.ts#growthAnswered (shared): the feature's statements; "…" marks code left out
function growthAnswered(answer: GrowAnswer): boolean {
  // … 7 lines
  if (outcome.kind === 'answered' && outcome.grown.length) {
    persistence.saveSessionSoon(doc, sessionSettings);
  // … 2 lines
  }
// … 1 lines
}

// src/main.ts:2015-2027 slice of src/main.ts#newMap (shared): the feature's statements; "…" marks code left out
function newMap(): void {
  if (!window.confirm(t('confirm.newMap'))) return;
  {
    // … 1 lines
    history.begin(doc);
    // … 2 lines
    history.end(doc);
  // … 3 lines
  }
}

// src/main.ts:2029-2033 slice of src/main.ts#newWorld (shared): the feature's statements; "…" marks code left out
function newWorld(): void {
  if (!window.confirm(t('confirm.newWorld'))) return;
// … 2 lines
}

// src/main.ts:2035-2039 slice of src/main.ts#newLargeWorld (shared): the feature's statements; "…" marks code left out
function newLargeWorld(): void {
  if (!window.confirm(t('confirm.newLargeWorld'))) return;
// … 2 lines
}

// src/main.ts:2090-2106 slice of src/main.ts#networkBakeOf (shared): the feature's statements; "…" marks code left out
function networkBakeOf(data: SerializedDoc, table: ArrayBufferLike | null): Promise<NetworkBake | null> {
  // … 1 lines
  return new Promise((done) => {
    submitJob({
      // … 3 lines
      write: () => ({ data: { id: cityAskId++, data, table, generated: openedGenerated(data, table) } satisfies NetworkBakeAsk, transfer: [] }),
    // … 6 lines
    });
  });
}

// src/main.ts:2213-2227 slice of src/main.ts#openCity (shared): the feature's statements; "…" marks code left out
function openCity(data: ReturnType<typeof buildSmallCityMap>, large = false, table: ArrayBufferLike | null = null, bake: NetworkBake | null = null): void {
  history.begin(doc);
  // … 9 lines
  history.end(doc);
// … 2 lines
}

// src/main.ts:2230-2240 slice of src/main.ts#fitOpeningView (shared): the feature's statements; "…" marks code left out
function fitOpeningView(): void {
  // … 5 lines
  camera.fit({ minX: cx - half, minY: cy - half, maxX: cx + half, maxY: cy + half }, surface.cssW, surface.cssH);
  // … 2 lines
  view.setOrbit(view.azimuth, Math.max(view.elevation, 1.38));
}

// src/main.ts:2263-2271 slice of src/main.ts#openImported (shared): the feature's statements; "…" marks code left out
function openImported(result: ImportResult): boolean | Promise<boolean> {
  if (result.status === 'cancelled') return false;
  if (result.status === 'invalid') {
    flashHint('hint.openFailed');
    return false;
  }
  const { session } = result;
  return openSession(session);
}

// src/main.ts:2273-2293 slice of src/main.ts#openSession (shared): the feature's statements; "…" marks code left out
function openSession(session: Extract<ImportResult, { status: 'ok' }>['session']): boolean {
  // … 1 lines
  history.begin(doc);
  try {
    applySnapshot(session.document, 'import');
  } catch (error) {
    // … 3 lines
    history.cancel(doc);
    followNetwork(doc, net);
  // … 3 lines
  }
  history.end(doc);
  // … 1 lines
  restoreSettings(session.settings);
// … 2 lines
}

// src/main.ts:2294-2296 slice of src/main.ts#openMap (shared): the feature's statements; "…" marks code left out
async function openMap(): Promise<void> {
  await openImported(await importFromFile());
}

// src/main.ts:2319-2332 slice of src/main.ts#cameraCommand (shared): the feature's statements; "…" marks code left out
function cameraCommand(command: string): void {
  switch (command) {
    case 'turnLeft': view.orbit(-KEY_TURN, 0); break;
    case 'turnRight': view.orbit(KEY_TURN, 0); break;
    case 'tiltUp': view.orbit(0, TILT_STEP); break;
    case 'tiltDown': view.orbit(0, -TILT_STEP); break;
    case 'north': view.setOrbit(DEFAULT_AZIMUTH, DEFAULT_ELEVATION); break;
    // … 1 lines
    case 'reset': view.setOrbit(DEFAULT_AZIMUTH, DEFAULT_ELEVATION); fitView(); break;
  // … 1 lines
  }
  persistence.saveSettingsSoon(sessionSettings);
// … 1 lines
}

// src/main.ts:2335-2343 slice of src/main.ts#undo (shared): the feature's statements; "…" marks code left out
function undo(): void {
  // … 3 lines
  const done = history.undo(doc);
  if (done) afterRestore();
  // … 1 lines
  if (done) flashHint('hint.undone');
}

// src/main.ts:2344-2351 slice of src/main.ts#redo (shared): the feature's statements; "…" marks code left out
function redo(): void {
  // … 2 lines
  const done = history.redo(doc);
  if (done) afterRestore();
  // … 1 lines
  if (done) flashHint('hint.redone');
}

// src/main.ts:2353-2363 slice of src/main.ts#afterRestore (shared): the feature's statements; "…" marks code left out
function afterRestore(): void {
  // … 1 lines
  followNetwork(doc, net);
  // … 3 lines
  persistence.saveSessionSoon(doc, sessionSettings);
// … 3 lines
}

// src/main.ts:2366-2369 slice of src/main.ts#updateHistoryButtons (shared): the feature's statements; "…" marks code left out
function updateHistoryButtons(): void {
  gameState.set('canUndo', history.canUndo, 'history');
  gameState.set('canRedo', history.canRedo, 'history');
}

// src/main.ts:2372-2391 slice of src/main.ts#restoreSettings (shared): the feature's statements; "…" marks code left out
function restoreSettings(settings: SavedSettings): void {
  camera.x = settings.camera.x;
  camera.y = settings.camera.y;
  // … 1 lines
  const limits = view.zoomBounds;
  camera.zoom = clamp(settings.camera.zoom, limits.min, limits.max);
  // … 1 lines
  restoreOrbit(settings.camera);
  // … 1 lines
  gameState.set('speed', settings.speed, 'map loaded');
  sim.clock.speed = settings.speed;
  setPaused(settings.paused);
  sim.trafficIntensity = settings.trafficIntensity;
  sim.pedestrianIntensity = settings.pedestrianIntensity;
  sim.demandMultiplier = settings.demandMultiplier ?? 1;
  if (settings.minutes !== undefined) sim.setClock(settings.minutes);
  gameState.set('congestionOverlay', settings.congestionOverlay, 'map loaded');
  persistence.saveSettingsSoon(sessionSettings);
// … 1 lines
}

// src/main.ts:2402-2404 slice of src/main.ts#closeInspector (shared): the feature's statements; "…" marks code left out
function closeInspector(): void {
  hideInspector();
}

// src/main.ts:2406-2406 slice of src/main.ts#<ExpressionStatement>@2406 (shared): the feature's statements; "…" marks code left out
onInspectorChange((open) => gameState.set('inspectorOpen', open, open ? 'inspector opened' : 'inspector closed'));

// src/main.ts:2444-2448 slice of src/main.ts#flashHint (shared): the feature's statements; "…" marks code left out
function flashHint(key: string, params?: Readonly<Record<string, string | number>>): void {
  const text = t(key, params);
  if (hintListeners.length === 0) earlyFlashes.push(text);
  for (const listen of hintListeners) listen(text, true);
}

// src/main.ts:2461-2466 slice of src/main.ts#updateHint (shared): the feature's statements; "…" marks code left out
function updateHint(): void {
  const text = t(hintKey('hint'));
  if (text === hintShown) return;
  hintShown = text;
  for (const listen of hintListeners) listen(text, false);
}

// src/main.ts:2468-2473 slice of src/main.ts#listenHint (shared): the feature's statements; "…" marks code left out
function listenHint(listen: (text: string, flash: boolean) => void): void {
  // … 1 lines
  hintShown = t(hintKey('hint'));
  listen(hintShown, false);
// … 1 lines
}

// src/main.ts:2479-2555 slice of src/main.ts#gameKeys (shared): the feature's statements; "…" marks code left out
const gameKeys = createGameKeys({
  tool: () => game.tool,
  buildingKey: (e) => buildings.key(e),
  // A run being traced: Enter ends it, Backspace takes the last point back, Esc drops it.
  transitKey: (key) => transitEditor.key(key),
  barrierKey: (key) => barrierTool.key(key),
  // The road tool's keys, as the player bound them (`ui/roads/keys.ts`).
  roadKey: (key) => {
    const action = roadKeyAction(key);
    if (action === 'alignment') {
      const order: readonly Alignment[] = ['straight', 'curve', 'free'];
      setAlignment(order[(order.indexOf(game.alignment) + 1) % order.length]!);
    } else if (action) roadTool.stepHeight(action === 'heightUp' ? 1 : -1);
    return action !== null;
  },
  smoothNodePicked: () => game.selectedNode !== null && (doc.node(game.selectedNode)?.smooth ?? false),
  stepNodeHeight: (metres) => {
    const id = game.selectedNode;
    if (id === null) return;
    setNodeHeightMetres(id, doc.requireNode(id).heightOffset / UNITS_PER_METER + metres);
  },
  camera: {
    turn: (radians) => cameraMotion.turn(radians),
    press: (code, shift) => cameraMotion.press(code, shift),
    home: () => { view.setOrbit(DEFAULT_AZIMUTH, DEFAULT_ELEVATION); fitView(); },
  },
  requestDraw,
  saveMap,
  openMap: () => void openMap(),
  duplicateRoad: () => duplicateSelectedSegment(),
  undo,
  redo,
  roadPicked: () => game.selectedSegment !== null,
  // Delete removes what Inspect has picked: the road it shows.
  deleteRoad: () => {
    const id = game.selectedSegment;
    if (id === null) return;
    closeInspectorPanel();
    mutate(() => removeRoads(doc, net, [id]));
  },
  // A button the player reached with the keyboard keeps Space for itself; one merely clicked with the mouse
  keyboardOnButton: (e) => e.target instanceof HTMLButtonElement && focusCameFromKeyboard(),
  // The focused control is let go by the listener below.
  togglePause: () => setSpeed(game.paused ? game.speed : 0),
  terrainMode: (index) => {
    const chosen = TERRAIN_KEY_MODES[index - 1];
    if (chosen) setTerrainMode(chosen);
    return chosen !== undefined;
  },
  terrainRadius: (step) => setTerrainRadius(game.terrainRadius + step * 10),
  terrainStrength: (step) => setTerrainStrength(game.terrainStrength + step),
  roadClassCount: ROAD_TYPES.length,
  // The class palette is shown only with the road tool
  pickRoadClass: (index) => {
    if (game.tool !== 'road') setTool('road');
    selectRoadType(index);
    shell?.toolKeyed('road');
  },
  // F8: what changed, where (`drawChanges`).
  toggleChanges: () => { showChanges = !showChanges; requestDraw(); },
  // F9: the monitor (`healthPanel`, mounted at the end of this file).
  health: { toggle: () => healthPanel?.toggle() ?? false, close: () => healthPanel?.close() ?? false },
  // F7: the debugging lab (`lab/lab.ts`).
  effects: { toggle: () => lab.toggle(), close: () => lab.close() },
  toolKey: (next) => {
    pickTool(next);
    shell?.toolKeyed(next);
  },
  // Escape ends whatever is being drawn
  escape: () => {
    if (gestureInProgress()) cancelGestures();
    else if (game.selectedSegment !== null || game.selectedNode !== null) closeInspectorPanel();
    else if (game.tool !== 'inspect') freeSelection();
    else return false;
    return true;
  },
});

// src/main.ts:2556-2563 slice of src/main.ts#<ExpressionStatement>@2556 (shared): the feature's statements; "…" marks code left out
window.addEventListener('keydown', (e) => {
  // … 1 lines
  const rule = gameKeys.press({ code: e.code, key: e.key, ctrl: e.ctrlKey || e.metaKey, shift: e.shiftKey, alt: e.altKey }, e);
  if (!rule) return;
  if (rule.prevent !== false) e.preventDefault();
  // … 1 lines
  if (e.target instanceof HTMLElement && e.target !== document.body && !focusCameFromKeyboard()) e.target.blur();
});

// src/main.ts:2564-2564 slice of src/main.ts#<ExpressionStatement>@2564 (shared): the feature's statements; "…" marks code left out
window.addEventListener('keyup', (e) => { cameraMotion.release(e.code, e.shiftKey); });

// src/main.ts:2566-2566 slice of src/main.ts#<ExpressionStatement>@2566 (shared): the feature's statements; "…" marks code left out
window.addEventListener('blur', () => cameraMotion.releaseAll());

// src/main.ts:2574-2574 slice of src/main.ts#lab (shared): the feature's statements; "…" marks code left out
const lab = createLab({ scene, sim, engine, frameTimer, effects, requestDraw, setPreset: (level) => { if (isQualityLevel(level)) graphics.setPreset(level); } });

// src/main.ts:2597-2606 slice of src/main.ts#growZones (shared): the feature's statements; "…" marks code left out
function growZones(): void {
  // … 2 lines
  const now = sim.clock.time;
  // … 1 lines
  if (mover.dragging || (now >= zoneGrowthHold.at && now < zoneGrowthHold.until)) return;
// … 3 lines
}

// src/main.ts:2617-2617 slice of src/main.ts#<ExpressionStatement>@2617 (shared): the feature's statements; "…" marks code left out
loadingCurtain.textContent = t('map.loading');

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 12 lines
  if (cameraMotion.step(now)) persistence.saveSettingsSoon(sessionSettings);
  // … 1 lines
  cameraHand.advance(now);
  // … 11 lines
  if (!actionsWarm && game.tool === 'bulldoze' && strikeChoice.mode !== 'demolish') {
    actionsWarm = true;
    void scene.effects();
  }
  // … 6 lines
  const simTime = sim.clock.time;
  // … 2 lines
  if (effects.growth && growthDue.due(simTime)) growZones();
  if (effects.growth && decayDue.due(simTime)) decayWatch();
  actions.tick(simTime);
  // … 34 lines
  if (panelsDue.due(wall)) {
    // … 2 lines
    refreshInspector();
  // … 2 lines
  }
  // … 6 lines
  healthWatch.frameEnded(timed.start, timed.end);
  // … 7 lines
  if (!pageHidden() && (measure.on || !effects.onDemand || simMoving || roadTool.draft || mover.dragging || cameraHand.active || cameraHand.coasting || cameraMotion.moving || scene.busy())) requestDraw();
  // Only the clouds moving (they drift, form and fade), or the clock alone: a slower frame.
  else if (!pageHidden() && (!game.paused || scene.drifting())) frameClock.drift();
}

// src/main.ts:2757-2760 slice of src/main.ts#screenReach (shared): the feature's statements; "…" marks code left out
function screenReach(): ChangeRect {
  const c = view.centre;
  return [c.x - SCREEN_REACH, c.y - SCREEN_REACH, c.x + SCREEN_REACH, c.y + SCREEN_REACH];
}

// src/main.ts:2762-2780 slice of src/main.ts#groundOnScreen (shared): the feature's statements; "…" marks code left out
function groundOnScreen(w: number, h: number): ChangeRect {
  // … 2 lines
  for (const [px, py] of [[0, 0], [w / 2, 0], [w, 0], [w, h / 2], [w, h], [w / 2, h], [0, h], [0, h / 2]] as const) {
    // … 1 lines
    const ray = view.rayAt?.(px, py, w, h);
    for (const z of [0, top]) {
      // … 1 lines
      if (ray) {
        const t = (z - ray.oz) / ray.dz;
        if (!(ray.dz < 0) || !(t >= 0) || !Number.isFinite(t)) return screenReach();
        p = { x: ray.ox + ray.dx * t, y: ray.oy + ray.dy * t };
      } else p = view.toWorldAt(px, py, z, w, h);
      x0 = Math.min(x0, p.x); y0 = Math.min(y0, p.y); x1 = Math.max(x1, p.x); y1 = Math.max(y1, p.y);
    }
  }
  // … 1 lines
  return [Math.max(rx0, x0), Math.max(ry0, y0), Math.min(rx1, x1), Math.min(ry1, y1)];
}

// src/main.ts:2808-2847 slice of src/main.ts#drawStreetscapeHover (shared): the feature's statements; "…" marks code left out
function drawStreetscapeHover(ctx: CanvasRenderingContext2D, at: (p: Vec2) => Vec2): void {
  // … 18 lines
  const edge = at({ x: hover.at.x + LANDSCAPE_RADIUS[streetscapeKind()] + m(0.3), y: hover.at.y });
// … 19 lines
}

// src/main.ts:2849-2898 slice of src/main.ts#drawPolePlan (shared): the feature's statements; "…" marks code left out
function drawPolePlan(
// … 3 lines
): void {
  // … 24 lines
  if (game.tool === 'pole' && poleToolMode() === 'remove') {
    if (marks.removing) ring(at(marks.removing), '#e5534b', 10);
    return;
  }
// … 16 lines
}

// src/main.ts:2942-3410 slice of src/main.ts#drawOverlayScreen (shared): the feature's statements; "…" marks code left out
function drawOverlayScreen(): void {
  // … 47 lines
  scene.setGrid(roadGridShown());
  if (game.tool === 'road' && blockGridChoice.armed && hoverAnchor) {
    // The grid the next click lays, on the ground.
    for (const [a, b] of blockGridLines(hoverAnchor.at, blockGridChoice)) {
      const steps = Math.max(2, Math.ceil(stepsIn(dist(a, b), 4)));
      ctx.save();
      ctx.strokeStyle = SELECTION;
      ctx.lineWidth = 3;
      ctx.beginPath();
      for (let k = 0; k <= steps; k++) {
        const p = at({ x: a.x + (b.x - a.x) * (k / steps), y: a.y + (b.y - a.y) * (k / steps) });
        if (k === 0) ctx.moveTo(p.x, p.y); else ctx.lineTo(p.x, p.y);
      }
      ctx.stroke();
      ctx.restore();
    }
  }
  // … 7 lines
  const showLots = game.tool === 'zone' || (lotTool.anyZoned() && zoneColoursShown());
  if (showLots) {
    // What the Zoning tool draws (`editor/lotTool.ts`), in the scene; its labels on the 2D layer.
    const overlay = lotTool.overlay(game.tool === 'zone');
    const labels = overlay.labels;
    // All of them from the next frame: the ground's shaders draw them (`lotOverlay.ts`); the diary for the built lots' levels.
    scene.setLotOverlay(overlay, doc.changes);
    // Labels stay on the 2D layer, projected at the ground's real height.
    const ground = (p: Vec2): Vec2 => view.toScreen(p, w, h, scene.surfaceHeightAt(p.x, p.y));
    ctx.save();
    for (const label of labels) {
      const c = ground(label.at);
      ctx.textAlign = 'center'; ctx.lineWidth = 3; ctx.strokeStyle = '#0b1416cc'; ctx.fillStyle = '#ffffff';
      if (label.kind === 'size') {
        const text = t('zone.lot.size', { w: Math.round(label.width * METERS_PER_UNIT), d: Math.round(label.depth * METERS_PER_UNIT) });
        ctx.font = '600 13px system-ui, sans-serif'; ctx.textBaseline = 'middle';
        ctx.strokeText(text, c.x, c.y); ctx.fillText(text, c.x, c.y);
      } else {
        const text = `${Math.round(label.length * METERS_PER_UNIT)} m`;
        ctx.font = '600 12px system-ui, sans-serif';
        ctx.strokeText(text, c.x, c.y - 10); ctx.fillText(text, c.x, c.y - 10);
      }
    }
    ctx.restore();
  } else scene.setLotOverlay(null);
  // … 4 lines
  if (bulldozeBox) {
    // … 4 lines
    corners.forEach((p, i) => {
      // … 2 lines
      for (let k = 0; k < n; k++) {
        // … 1 lines
        const s = view.toScreen(on, w, h, sceneHeightAt(on));
        if (i === 0 && k === 0) ctx.moveTo(s.x, s.y); else ctx.lineTo(s.x, s.y);
      }
    });
  // … 8 lines
  }
  // … 62 lines
  if (game.tool === 'terrain' && hoverAnchor) {
    // … 36 lines
    const label = game.terrainMode === 'flatten'
      ? `${t('terrain.level')} ${formatDecimal(height, 1)}`
      : formatDecimal(height, 1);
    // … 3 lines
    const width = ctx.measureText(label).width + 12;
    // … 2 lines
    ctx.roundRect(centre.x - width / 2, centre.y - ry - 24, width, 17, 8);
    // … 2 lines
    ctx.fillText(label, centre.x, centre.y - ry - 10);
  // … 1 lines
  }
  // … 30 lines
  if (roadPreview) {
    // … 1 lines
    const chosenWidth = roadWidth();
    // … 3 lines
    const rt = picked ? profileRoad(picked.profile, picked.type) : roadProfile(game.roadTypeIndex, game.roadLanePreset,
      roadType(game.roadTypeIndex).lanes === 1 ? 'aToB' : 'both',
      chosenWidth === null ? undefined : sectionForWidth(plainRt, chosenWidth, Math.round(plainRt.speedLimit * 3.6 * METERS_PER_UNIT)),
      roadParking());
    // … 8 lines
    for (const [pieceIndex, piece] of pieces.entries()) {
      // … 8 lines
      for (let i = pieceIndex === 0 ? 0 : 1; i < flattened.length; i++) {
        // … 7 lines
        groundProjected.push(view.toScreen(p, w, h, ground));
        projected.push(view.toScreen(p, w, h, ground + offset));
      }
    // … 1 lines
    }
    // … 3 lines
    if (exact && exact.length > 1) {
      // … 1 lines
      for (const st of exact) {
        // … 3 lines
        projected.push(view.toScreen(p, w, h, st.deck));
        groundProjected.push(view.toScreen(p, w, h, st.ground));
      // … 1 lines
      }
    }
    // … 15 lines
    const asphaltWidth = Math.max(3, rt.width * pixelsPerUnit);
    const kerbWidth = Math.max(asphaltWidth + 2, (rt.width + 1.8) * pixelsPerUnit);
    const footwayWidth = Math.max(kerbWidth + 2, (rt.width + 1.8 + rt.sidewalk * 2) * pixelsPerUnit);
    const casingWidth = Math.max(footwayWidth + 2, footwayWidth + 3 * pixelsPerUnit);
    // … 3 lines
    if (modes.length > 1) drawBuildModes(ctx, projected, groundProjected, modes);
    else if (points.length > 1 && offsets.some((offset) => Math.abs(offset) > UNITS_PER_METER * 0.5)) {
      strokeScreen(points, 'rgba(6, 19, 21, 0.52)', casingWidth + 5, [7, 7], groundProjected);
    // … 7 lines
    }
    // … 1 lines
    const band = (half: number, fill: string | CanvasPattern): void => {
      // … 2 lines
      for (let i = 0; i < points.length; i++) {
        // … 5 lines
        left.push(view.toScreen(l, w, h, groundUnder(l) + z));
        right.push(view.toScreen(r, w, h, groundUnder(r) + z));
      }
    // … 6 lines
    };
    const asphaltHalf = rt.width / 2, kerbHalf = (rt.width + 1.8) / 2;
    const footwayHalf = (rt.width + 1.8 + rt.sidewalk * 2) / 2, casingHalf = footwayHalf + 1.5;
    if (!settling) band(casingHalf + 2, ok ? SELECTION : INVALID);
    band(casingHalf, '#536b47');
    band(footwayHalf, '#a7a498');
    band(kerbHalf, '#87877f');
    band(asphaltHalf, asphaltPreviewPattern(ctx));
    // … 1 lines
    if (modes.includes('tunnel')) drawTunnelStretches(ctx, projected, modes, asphaltWidth);
    if (rt.markings !== 'none') {
      const dash = [Math.max(4, 10 * pixelsPerUnit), Math.max(3, 8 * pixelsPerUnit)];
      strokeScreen(points, rt.line, Math.max(1, 1.1 * pixelsPerUnit), dash, projected);
    }
    // … 1 lines
    if (points.length && !settling) {
      // … 5 lines
      {
        // … 5 lines
        const text = refusal ? `${priced} · ${t(`rule.short.${refusal}`)}` : priced;
        // … 2 lines
        const tw = ctx.measureText(text).width;
        const bx = clamp(end.x + 16, 8, Math.max(8, w - tw - 28)), by = clamp(end.y - 40, 60, h - 40);
        // … 2 lines
        ctx.roundRect(bx, by, tw + 18, 24, 6);
        // … 3 lines
        ctx.fillText(text, bx + 9, by + 12.5);
      // … 1 lines
      }
      // … 19 lines
      const label = `${metres >= 0 ? '+' : ''}${formatDecimal(metres, 1)} m · ${grade}%`;
      if (Math.abs(previewHeight) > 0.25 ||
        Math.abs(previewHeight - roadPreview.startHeightOffset) > 0.25) {
        // … 26 lines
        ctx.fillText(label, x + 11, y + 16);
      // … 1 lines
      }
    }
  }
}

// src/main.ts:2960-2960 slice of src/main.ts#drawOverlayScreen.at (shared): the feature's statements; "…" marks code left out
  const at = (p: Vec2): Vec2 => view.toScreen(p, w, h, sceneHeightAt(p));

// src/main.ts:3478-3486 slice of src/main.ts#buildSummary (shared): the feature's statements; "…" marks code left out
function buildSummary(stations: Parameters<typeof buildRuns>[0]): string {
  // … 2 lines
  return [...totals]
    .filter(([mode, length]) => mode !== 'ground' && length * METERS_PER_UNIT >= 5)
    .sort((x, y) => y[1] - x[1])
    .map(([mode, length]) => `${t(mode === 'cutting' && cutWallsChosen() ? 'buildMode.cuttingWalls' : `buildMode.${mode}`)} ${Math.max(10, Math.round((length * METERS_PER_UNIT) / 10) * 10)} m`)
    .join(' · ');
}

// src/main.ts:3573-3818 slice of src/main.ts#showInspector (shared): the feature's statements; "…" marks code left out
function showInspector(): void {
  openInspector(
    doc,
    net,
    sim,
    { segment: game.selectedSegment, node: game.selectedNode },
    {
      onUpgrade: (id) => {
        const seg = doc.segment(id);
        if (!seg || seg.type >= LAST_UPGRADE_CLASS) return;
        mutateRoads((doc, net) => carryStreetKit(doc, net, [id], () => {
          doc.setSegmentType(id, seg.type + 1);
          return true;
        }));
      },
      onSetType: (id, type) => {
        const seg = doc.segment(id);
        if (!seg || seg.type === type) return;
        mutateRoads((doc, net) => carryStreetKit(doc, net, [id], () => {
          doc.setSegmentType(id, type);
          return true;
        }));
      },
      onSetLanes: (id, lanes) => {
        if (!doc.segment(id)) return;
        mutateRoads((doc, net) => carryStreetKit(doc, net, [id], () => {
          doc.setSegmentLanes(id, lanes);
          return true;
        }));
      },
      onSetParking: (id, parking) => {
        if (!doc.segment(id)) return;
        mutateRoads((doc, net) => carryStreetKit(doc, net, [id], () => {
          doc.setSegmentParking(id, parking);
          return true;
        }));
      },
      // A profile or template applied to the road in place: judged and paid for as every road edit
      onApplyProfile: (id, profile, type) => {
        if (!doc.segment(id)) return;
        let problem: string | undefined;
        mutateRoads((doc, net) => carryStreetKit(doc, net, [id], () => {
          const result = applyProfileTo(doc, [id], profile, type);
          problem = result.problems[0];
          return result.changed;
        }));
        if (problem) flashHint(`profile.problem.${problem}`);
      },
      // V8: the road's profile on the whole street it runs in, one edit, paid for.
      streetLength: (id) => streetChain(doc, id).length,
      onProfileToStreet: (id) => {
        const seg = doc.segment(id);
        if (!seg) return;
        const profile = profileOf(seg);
        let problem: string | undefined;
        mutateRoads((doc, net) => carryStreetKit(doc, net, streetChain(doc, id).map((piece) => piece.id).filter((piece) => piece !== id), () => {
          let changed = false;
          for (const piece of streetChain(doc, id)) {
            if (piece.id === id) continue;
            const result = applyProfileTo(doc, [piece.id], piece.flipped ? flipProfile(profile) : profile, seg.type);
            problem ??= result.problems[0];
            changed = result.changed || changed;
          }
          return changed;
        }));
        flashHint(problem ? `profile.problem.${problem}` : 'hint.profile.toStreet');
      },
      // The eyedropper: the road tool in hand, drawing with the profile just picked.
      onDrawWithProfile: (held) => {
        // The class first, so the tool's panel shows the profile's own class, then the profile in hand.
        selectRoadType(held.type);
        setDrawProfile(held);
        // The road tool's panel opened, showing the profile picked.
        shell?.openTool('road');
        flashHint('hint.profile.picked');
      },
      onSetSection: (id, section) => {
        if (!freeRoadsEnabled() || !doc.segment(id)) return;
        mutateRoads((doc, net) => carryStreetKit(doc, net, [id], () => {
          doc.setSegmentSection(id, section);
          return true;
        }));
      },
      onSetDirection: (id, direction) => {
        if (!doc.segment(id)) return;
        mutateRoads((doc) => {
          doc.setSegmentDirection(id, direction);
          return true;
        });
      },
      onSetNodeHeight: setNodeHeightMetres,
      onSetNodeEnd: (node, end) => mutateRoads((doc) => {
        doc.setNodeEnd(node, end);
        return true;
      }),
      onReverseDirection: (id) => {
        const seg = doc.segment(id);
        if (!seg) return;
        const direction = seg.direction === 'aToB' ? 'bToA' : 'aToB';
        mutateRoads((doc) => {
          doc.setSegmentDirection(id, direction);
          return true;
        });
      },
      onSplit: (id) => {
        const seg = doc.segment(id);
        if (!seg) return;
        const polyline = net.polylines.get(doc, id);
        const at = polyline.sampleAt(polyline.length / 2).p;
        mutateRoads((doc, net) => splitSegment(doc, net, id, polyline.length / 2, at) !== null);
      },
      onAddCrossing: (id, kind) => {
        if (!doc.segment(id)) return;
        // Where the player clicked on the road, or its middle.
        const chosen = game.selectedSegment === id && game.selectedSegmentS !== null ? game.selectedSegmentS : undefined;
        let placed: NodeId | null = null;
        mutate(() => {
          const result = commitPedestrianCrossing(doc, net, id, kind, chosen);
          if (!result.committed) {
            flashHint(`hint.crossing.${result.reason}`);
            return false;
          }
          placed = result.node;
          return true;
        });
        if (placed !== null) {
          select(null, null, placed, 'point inserted on the road');
          showInspector();
        }
      },
      onAddUturn: (id, toward) => {
        if (!doc.segment(id)) return;
        const chosen = game.selectedSegment === id && game.selectedSegmentS !== null ? game.selectedSegmentS : undefined;
        let placed: NodeId | null = null;
        mutateRoads((doc, net) => {
          const result = commitUturn(doc, net, id, toward, chosen);
          if (!result.committed) {
            flashHint(`hint.uturn.${result.reason}`);
            return false;
          }
          placed = result.node;
          return true;
        });
        if (placed !== null) flashHint('hint.uturn.placed');
      },
      onRemoveCrossing: (node) => {
        mutate(() => {
          if (!doc.node(node)?.crossing) return false;
          doc.clearNodeCrossing(node);
          return true;
        });
      },
      onAddHeightPoint: (id) => {
        if (!doc.segment(id)) return;
        const polyline = net.polylines.get(doc, id);
        if (polyline.length < 20) { flashHint('hint.road.invalid'); return; }
        const chosen = game.selectedSegment === id && game.selectedSegmentS !== null
          ? game.selectedSegmentS : polyline.length / 2;
        const s = chosen < 5 || chosen > polyline.length - 5
          ? polyline.length / 2 : chosen;
        let node: NodeId | null = null;
        if (!mutateRoads((doc, net) => {
          node = splitSegment(doc, net, id, s, polyline.sampleAt(s).p);
          if (node === null) return false;
          doc.requireNode(node).smooth = true;
          return true;
        })) node = null;
        if (node !== null) {
          select(null, null, node, 'smooth point inserted');
          showInspector();
        }
      },
      onDuplicate: (id) => {
        duplicateSelectedSegment(id);
      },
      onSetControl: (id, control) => {
        if (!doc.node(id)) return;
        mutate(() => {
          doc.setNodeControl(id, control);
          return true;
        });
      },
      onSetMovementBlocked: (node, from, to, blocked) => {
        if (!doc.node(node)) return;
        mutate(() => {
          doc.setMovementBlocked(node, from, to, blocked);
          return true;
        });
      },
      project: (x, y) => view.toScreen({ x, y }, surface.cssW, surface.cssH),
      // Each leg's rule and the signal's settings.
      onSetApproachRules: (node, rules) => {
        if (!doc.node(node)) return;
        mutate(() => { doc.setNodeApproachRules(node, rules); return true; });
      },
      onSetSignal: (node, settings) => {
        if (!doc.node(node)) return;
        mutate(() => { doc.setNodeSignal(node, settings); return true; });
      },
      onSetSignals: (settings) => {
        mutate(() => { for (const [node, s] of settings) doc.setNodeSignal(node, s); return true; });
      },
      // The player's lane connections at a junction.
      onSetLaneLinks: (node, links) => {
        if (!doc.node(node)) return;
        mutate(() => {
          doc.setNodeLaneLinks(node, links);
          return true;
        });
      },
      onSetCurve: (id, curve) => {
        const seg = doc.segment(id);
        if (!seg) return;
        const unchanged =
          seg.curve === curve ||
          (seg.curve !== null && curve !== null && seg.curve.t === curve.t && seg.curve.h === curve.h);
        if (unchanged) return;
        mutateRoads((doc) => {
          doc.setSegmentCurve(id, curve);
          return true;
        });
      },
      onJoin: (node) => {
        mutateRoads((doc) => joinSegments(doc, node));
        select(game.selectedSegment, game.selectedSegmentS, null, 'roads joined');
        closeInspector();
      },
      onRemoveNode: (node) => {
        mutate(() => {
          const source = doc.node(node);
          if (!source) return false;
          removeRoads(doc, net, [...source.incident]);
          if (doc.node(node)) doc.removeNode(node);
          return true;
        });
        select(game.selectedSegment, game.selectedSegmentS, null, 'point removed');
        closeInspector();
      },
      onDelete: (id) => {
        mutate(() => removeRoads(doc, net, [id]));
        select(null, game.selectedSegmentS, game.selectedNode, 'road deleted');
        closeInspector();
      },
    },
  );
}

// src/main.ts:3835-3835 slice of src/main.ts#statusListeners (shared): the feature's statements; "…" marks code left out
const statusListeners: ((status: ShellStatus) => void)[] = [];

// src/main.ts:3837-3856 slice of src/main.ts#currentStatus (shared): the feature's statements; "…" marks code left out
function currentStatus(): ShellStatus {
  // … 5 lines
  return {
    // … 2 lines
    vehicles: vehicleCountLabel(sim.vehicles.size + (sim.stats?.lotsMoving ?? 0)),
    people: peopleCountLabel(sim.pedViews.length),
  // … 7 lines
  };
}

// src/main.ts:3865-3869 slice of src/main.ts#desktopShown (shared): the feature's statements; "…" marks code left out
const desktopShown = watchDesktopShown((shown) => {
  if (!shown) { persistence.flush(); return; }
  frameClock.restart();
  requestDraw();
});

// src/main.ts:3871-3873 slice of src/main.ts#pageHidden (shared): the feature's statements; "…" marks code left out
function pageHidden(): boolean {
  return document.hidden || desktopShown.hidden;
}

// src/main.ts:3885-3898 slice of src/main.ts#saveOnClose (shared): the feature's statements; "…" marks code left out
function saveOnClose(): (event: BeforeUnloadEvent) => void {
  // … 1 lines
  return (event) => {
    if (saved) return;
    persistence.saveSession(doc, sessionSettings());
    // … 3 lines
    void Promise.race([persistence.written(), new Promise((done) => setTimeout(done, CLOSE_WAIT_MS))]).then(() => {
      saved = true;
      window.close();
    });
  };
}

// src/main.ts:3901-3903 slice of src/main.ts#<ExpressionStatement>@3901 (shared): the feature's statements; "…" marks code left out
document.addEventListener('visibilitychange', () => {
  if (document.visibilityState === 'hidden') persistence.flush();
});

// src/main.ts:3904-3904 slice of src/main.ts#<ExpressionStatement>@3904 (shared): the feature's statements; "…" marks code left out
window.addEventListener('pagehide', () => persistence.saveSession(doc, sessionSettings()));

// src/main.ts:3908-3913 slice of src/main.ts#<ExpressionStatement>@3908 (shared): the feature's statements; "…" marks code left out
persistence.onSaveFailed = () => {
  const now = performance.now();
  if (now - saveWarnedAt < 60_000) return;
  saveWarnedAt = now;
  flashHint('hint.saveFailed');
};

// src/main.ts:3923-3930 slice of src/main.ts#<ExpressionStatement>@3923 (shared): the feature's statements; "…" marks code left out
onLanguageChange(() => {
  buildings.languageChanged();
  hintShown = '';
  updateHint();
  updateStatus();
  refreshInspector();
  requestDraw();
});

// src/main.ts:3933-3941 slice of src/main.ts#setMarkingStyle (shared): the feature's statements; "…" marks code left out
function setMarkingStyle(value: string): void {
  // … 1 lines
  history.begin(doc);
  // … 1 lines
  history.end(doc);
  persistence.saveSessionSoon(doc, sessionSettings);
// … 2 lines
}

// src/main.ts:3976-3984 slice of src/main.ts#noteSimulationIssues (shared): the feature's statements; "…" marks code left out
function noteSimulationIssues(): void {
  for (const issue of sim.issues) {
    // … 3 lines
    health.record('invariant', 'warning', t('health.msg.invariant', { code: issue.code }),
      { detail: t('health.msg.invariantDetail', { subject: issue.subject, detail: issue.detail, tick: issue.tick }) });
  }
}

// src/main.ts:3987-4002 slice of src/main.ts#healthPanel (shared): the feature's statements; "…" marks code left out
const healthPanel = !import.meta.env.DEV ? null : mountHealthPanel({
  log: health,
  world: (n) => doc.changes.latest(n).map((c) => {
    const where = c.rects === null ? t('health.world.whole') : t('health.world.rects', { count: c.rects.length });
    return `#${c.serial} ${c.kind} · ${c.cause}${c.parent ? ` ← #${c.parent}` : ''} · ${where}${c.ms !== undefined ? ` · ${c.ms.toFixed(1)} ms` : ''}${c.detail ? ` · ${c.detail}` : ''}`;
  }),
  state: (n) => ({
    now: [
      ...Object.entries(gameState.snapshot()).map(([key, value]) => `${key}: ${valueText(value)}`),
      ...tools.latest(3).map((r) => `tool machine #${r.serial}: ${r.from} → ${r.to} (${r.event})`),
    ],
    wide: [...gpuLines(),
      t('health.state.simulation', { ms: (sim.stats?.stepMs ?? 0).toFixed(1), share: Math.round((sim.stats?.populationShare ?? 1) * 100), vehicles: sim.vehicles.size, people: sim.pedViews.length })],
    changes: gameState.latest(n).map((c) => `${c.key}: ${valueText(c.from)} → ${valueText(c.to)} (${c.cause})`),
  }),
});

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 17 lines
  camera,
  // … 5 lines
  saved: () => { persistence.saveSessionSoon(doc, sessionSettings); persistence.flush(); return persistence.written(); },
  importMap: async () => openImported(await importFromFile()),
  // … 3 lines
  measure: (on: boolean | 'running') => {
    // … 3 lines
    sim.clock.paused = on === true || game.paused;
  // … 2 lines
  },
  // … 19 lines
  loadDoc: (data: ReturnType<RoadDoc['toJSON']>) => {
    history.begin(doc);
  // … 1 lines
  },
  // … 3 lines
  lookAt: (x: number, y: number, zoom = camera.zoom) => {
    camera.x = x;
    camera.y = y;
    camera.zoom = zoom;
    syncViewFromFlatCamera();
    requestDraw();
  },
  // … 4 lines
  variant: (fx?: Partial<EffectSwitches> | null, b?: Partial<BuildingChoices> | null, opt?: Partial<Optimizations> | null) => {
    scene.setEffects(fx ? { ...ALL_EFFECTS_ON, ...fx } : ALL_EFFECTS_ON, b ? { ...DEFAULT_BUILDING_CHOICES, ...b } : DEFAULT_BUILDING_CHOICES);
    scene.setOptimizations(opt ? { ...DEFAULT_OPTIMIZATIONS, ...opt } as Optimizations : DEFAULT_OPTIMIZATIONS);
    requestDraw();
  },
  setView: (at: { x: number; y: number; zoom: number; azimuth: number; elevation: number }) => {
    camera.x = at.x;
    camera.y = at.y;
    camera.zoom = at.zoom;
    // … 1 lines
    view.setOrbit(at.azimuth, at.elevation);
    view.moveTo({ x: at.x, y: at.y });
  // … 1 lines
  },
  // … 84 lines
  pickAtScreen: (x: number, y: number) =>
    findAnchor(
      doc,
      net,
      worldAtScreen(x, y),
      view.zoom,
    ),
  // … 1 lines
  utilities: {
    plan: (from: Vec2, to: Vec2, reach = POLE_PICK_PIXELS / view.zoom) =>
      planPoleRun(doc, net, from, to, reach),
    run: (from: Vec2, to: Vec2, reach = POLE_PICK_PIXELS / view.zoom) => {
      const plan = planPoleRun(doc, net, from, to, reach);
      return mutateBuilt(() => commitPoleRun(doc, plan));
    },
  },
  // … 4 lines
  view: () => view,
// … 2 lines
};

// src/main.ts:4259-4274 slice of src/main.ts#actions (shared): the feature's statements; "…" marks code left out
const actions = createActions({
  // … 5 lines
  view: () => view,
  // … 2 lines
  strength: () => strikeChoice.strength,
  // … 4 lines
  holdGrowth: (seconds) => { zoneGrowthHold = { at: sim.clock.time, until: sim.clock.time + seconds }; },
});
