// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:393-430 slice of src/main.ts#gameState (shared): the feature's statements; "…" marks code left out
const gameState = new GameState({
  // … 20 lines
  zoneUse: 'residential' as ZoneUse,
  zoneDensity: 'low' as ZoneDensity,
  // … 1 lines
  zoneMode: 'brush' as ZoneMode,
  lotSplitKind: 'vertical' as LotSplitKind,
// … 11 lines
}, () => { if (booted) requestDraw(); });

// src/main.ts:530-543 slice of src/main.ts#lotTool (shared): the feature's statements; "…" marks code left out
const lotTool = new LotTool({
  doc,
  net,
  zoom: () => view.zoom,
  settings: () => ({
    mode: game.zoneMode, use: game.zoneUse, density: game.zoneDensity, eraser: game.zoneEraser,
    splitKind: game.lotSplitKind, splitParts: game.lotSplitParts,
  }),
  mutate: (fn) => mutate(fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
  // Planned in the simulation's worker, on its copy of the region (`SimHost.lotPlan`).
  plan: () => sim.lotPlan(),
});

// src/main.ts:760-806 slice of src/main.ts#buildings (shared): the feature's statements; "…" marks code left out
const buildings = createBuildingWiring({
  doc,
  net,
  history,
  scene,
  view: () => view,
  size: () => ({ w: surface.cssW, h: surface.cssH }),
  undo: () => undo(),
  redo: () => redo(),
  focusBuilding(building) {
    const ground = building.volumes.filter((v) => v.base === 0);
    if (ground.length === 0) return;
    const cx = (Math.min(...ground.map((v) => v.x)) + Math.max(...ground.map((v) => v.x + v.w))) / 2;
    const cy = (Math.min(...ground.map((v) => v.y)) + Math.max(...ground.map((v) => v.y + v.d))) / 2;
    const c = Math.cos(building.rotation), s = Math.sin(building.rotation);
    const centre = { x: building.x + cx * c - cy * s, y: building.y + cx * s + cy * c };
    view.moveTo(centre);
    const span = Math.max(...ground.map((v) => Math.max(v.w, v.d)));
    const paletteWidth = document.getElementById('builder')?.getBoundingClientRect().width ?? 0;
    const available = Math.max(110, surface.cssW - paletteWidth - (surface.cssW < 600 ? 20 : 110));
      const corners = building.volumes.flatMap((v) => [
        [v.x, v.y], [v.x + v.w, v.y], [v.x + v.w, v.y + v.d], [v.x, v.y + v.d],
      ] as const).map(([x, y]) => view.toScreen({ x: building.x + x * c - y * s, y: building.y + x * s + y * c }, surface.cssW, surface.cssH));
      const projectedWidth = Math.max(...corners.map((p) => p.x)) - Math.min(...corners.map((p) => p.x));
      const top = Math.max(...building.volumes.map((v) => levelElevation(building, volumeTop(v)) + roofRise(building, v)
        + Math.max(0, ...(v.roofDetails ?? []).map((detail) => detail.h ?? 0))));
      const projectedRise = Math.abs(view.toScreen(centre, surface.cssW, surface.cssH, top).y
        - view.toScreen(centre, surface.cssW, surface.cssH).y);
      const projectedFootprint = Math.max(...corners.map((p) => p.y)) - Math.min(...corners.map((p) => p.y));
      const fit = Math.min(view.zoom * (available * .72) / Math.max(1, projectedWidth),
        view.zoom * (surface.cssH - 145) * .88 / Math.max(1, projectedRise + projectedFootprint));
    const ideal = Math.min(surface.cssW < 600 ? 3.2 : 7, Math.max(2.6, (surface.cssW < 600 ? 135 : 210) / span));
    const target = Math.max(view.zoomBounds.min, Math.min(view.zoomBounds.max, ideal, fit));
    view.zoomAt(surface.cssW / 2, surface.cssH / 2, target / view.zoom, surface.cssW, surface.cssH);
      const rise = Math.abs(view.toScreen(centre, surface.cssW, surface.cssH, top).y
        - view.toScreen(centre, surface.cssW, surface.cssH).y);
      view.panTo(centre, (surface.cssW - paletteWidth) / 2, surface.cssH / 2 + rise / 2 - 68, surface.cssW, surface.cssH);
  },
  afterEdit() {
    persistence.saveSessionSoon(doc, sessionSettings);
    updateHistoryButtons();
    requestDraw();
  },
  requestDraw: () => requestDraw(),
  flash: (key, params) => flashHint(key, params),
  hintChanged: () => updateHint(),
});

// src/main.ts:865-872 slice of src/main.ts#serializedDoc (shared): the feature's statements; "…" marks code left out
function serializedDoc(): string {
  // … 1 lines
  const key = `${doc.changes.serialOf(...DOC_CHANGE_KINDS)}:${doc.buildings.revision}:${doc.buildings.size}`;
  if (docText?.key === key) return docText.text;
  // … 1 lines
  docText = { key, text };
// … 1 lines
}

// src/main.ts:876-911 slice of src/main.ts#mutateBuilt (shared): the feature's statements; "…" marks code left out
function mutateBuilt(fn: () => boolean): boolean {
  // … 9 lines
  if (changed) {
    // … 3 lines
    buildings.afterRoadEdit(roadsBefore);
    // … 2 lines
    cutLotsByRoads(doc, net, roadDelta === null ? null : roadDelta.rects);
  // … 2 lines
  }
// … 14 lines
}

// src/main.ts:942-953 slice of src/main.ts#snapshotApplied (shared): the feature's statements; "…" marks code left out
function snapshotApplied(): void {
  buildings.restored();
// … 9 lines
}

// src/main.ts:1041-1053 slice of src/main.ts#cancelGestures (shared): the feature's statements; "…" marks code left out
function cancelGestures(): void {
  // … 4 lines
  lotTool.cancel();
  // … 4 lines
  if (game.tool === 'building') buildings.pointerUp(true);
// … 1 lines
}

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  // … 15 lines
  return lotTool.gesture();
}

// src/main.ts:1105-1114 slice of src/main.ts#bulldozer (shared): the feature's statements; "…" marks code left out
const bulldozer = new Bulldozer({
  // … 2 lines
  bulldozeBuildingAt: (at) => buildings.bulldozeAt(at),
// … 5 lines
});

// src/main.ts:1117-1119 slice of src/main.ts#gestureInProgress (shared): the feature's statements; "…" marks code left out
function gestureInProgress(): boolean {
  return roadTool.inProgress() || poleTool.inProgress() || terrainBrush.stroking || mover.dragging || lotTool.gesture() !== null || bulldozer.inProgress();
}

// src/main.ts:1207-1212 slice of src/main.ts#doubleClick (shared): the feature's statements; "…" marks code left out
function doubleClick(e: MouseEvent, r: DOMRect): void {
  // … 3 lines
  buildings.insideClick({ x: e.clientX - r.left, y: e.clientY - r.top }, true);
}

// src/main.ts:1213-1216 slice of src/main.ts#singleClick (shared): the feature's statements; "…" marks code left out
function singleClick(e: MouseEvent, r: DOMRect): void {
  // … 1 lines
  buildings.insideClick({ x: e.clientX - r.left, y: e.clientY - r.top }, false);
}

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 11 lines
  if (e.pointerType === 'mouse' && e.button === 2 && !e.shiftKey) {
    if (game.tool === 'building' && buildings.cancelOperation()) {
      requestDraw();
      return;
    }
  // … 4 lines
  }
  // … 13 lines
  switch (game.tool) {
    // … 7 lines
    case 'road':
      if (blockGridChoice.armed) {
        // … 3 lines
        mutate(() => {
          laid = commitBlockGrid(doc, net, world, blockGridChoice, game.roadTypeIndex, game.roadLanePreset, roadParking());
          return laid > 0;
        });
        flashHint(laid > 0 ? 'hint.blocks.built' : 'hint.road.invalid');
      // … 3 lines
      }
      // … 1 lines
      break;
    // … 8 lines
    case 'building':
      buildings.pointerDown({ x: e.clientX - r.left, y: e.clientY - r.top }, world, e.shiftKey);
      break;
    // … 1 lines
    case 'zone':
      lotTool.down(e.pointerId, world, e.shiftKey, e.detail);
      break;
    // … 82 lines
    case 'inspect': {
      // … 2 lines
      const hitBuilding = buildings.tool.buildingAt({ x: e.clientX - box.left, y: e.clientY - box.top });
      if (hitBuilding !== null) {
        // Shift+click: maintenance - the building is renovated, as new.
        if (e.shiftKey) {
          const b = doc.buildings.get(hitBuilding as BuildingId);
          if (b) {
            mutate(() => { doc.buildings.put({ ...b, builtAt: sim.minutes, decay: 0 }); return true; });
            flashHint('hint.building.renovated');
          }
        }
        break;
      }
    }
      // … 4 lines
      break;
  }
// … 1 lines
}

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 12 lines
  if (lotTool.move(e.pointerId, world)) return;
  // … 1 lines
  if (game.tool === 'zone') {
    lotTool.hover = world;
  // … 1 lines
  }
  // … 7 lines
  if (game.tool === 'building') {
    buildings.pointerMove(screen, world, e.shiftKey);
  // … 1 lines
  }
// … 41 lines
}

// src/main.ts:1484-1513 slice of src/main.ts#endPointer (shared): the feature's statements; "…" marks code left out
function endPointer(e: PointerEvent): void {
  // … 11 lines
  if (game.tool === 'building') buildings.pointerUp(cancelled || wasPinching);
  // … 1 lines
  lotTool.up(e.pointerId, !cancelled && !wasPinching);
// … 14 lines
}

// src/main.ts:1617-1630 slice of src/main.ts#setZone (shared): the feature's statements; "…" marks code left out
function setZone(patch: ShellZonePatch): void {
  // … 3 lines
  if (patch.mode !== undefined) {
    // … 3 lines
    lotTool.modeChanged();
  }
// … 3 lines
}

// src/main.ts:1820-1831 slice of src/main.ts#takeTool (shared): the feature's statements; "…" marks code left out
function takeTool(next: Tool): void {
  // … 7 lines
  if (buildingActive) buildings.activate();
  else buildings.deactivate();
// … 1 lines
}

// src/main.ts:1834-1838 slice of src/main.ts#showTool (shared): the feature's statements; "…" marks code left out
function showTool(next: Tool): void {
  // … 1 lines
  buildings.workspace.setMode(next === 'building' ? 'builder' : 'road');
// … 1 lines
}

// src/main.ts:1859-1946 slice of src/main.ts#mountInterface (shared): the feature's statements; "…" marks code left out
function mountInterface(): void {
  // … 1 lines
  shell = mountShell({
    workspace: buildings.workspace,
  // … 66 lines
  });
// … 16 lines
}

// src/main.ts:1993-1993 slice of src/main.ts#growthPool (shared): the feature's statements; "…" marks code left out
const growthPool = new GrowthPool((answer) => growthAnswered(answer), () => growth.restart());

// src/main.ts:1994-1996 slice of src/main.ts#growth (shared): the feature's statements; "…" marks code left out
const growth = new GrowthSession(growthPool, () => ({
  doc, net, groundAt: (x, y) => scene.terrainHeightAt(x, y), pavedAt: scene.pavedHeightAt, cell: TERRAIN_CELL,
}));

// src/main.ts:1998-2012 slice of src/main.ts#growthAnswered (shared): the feature's statements; "…" marks code left out
function growthAnswered(answer: GrowAnswer): boolean {
  // … 2 lines
  const outcome = caused('zone growth', () => growth.receive(answer, (id) => {
    const fresh = doc.buildings.get(id as BuildingId);
    if (fresh) doc.buildings.put({ ...fresh, builtAt: sim.minutes, decay: 0, lotPlan: LOT_PLAN_VERSION });
  }));
  for (const told of measure.grown.splice(0)) told(outcome.kind === 'answered' ? outcome.grown : []);
  if (outcome.kind === 'answered' && outcome.grown.length) {
    persistence.saveSessionSoon(doc, sessionSettings);
    updateStatus();
    requestDraw();
  }
// … 1 lines
}

// src/main.ts:2056-2076 slice of src/main.ts#cityData (shared): the feature's statements; "…" marks code left out
async function cityData(ask: CityGenAsk): Promise<OpenedCity> {
  // … 9 lines
  if (kept?.data && kept.buildings instanceof ArrayBuffer) { performance.mark('city:kept'); return preparedCity(kept.data, tableIn(kept.buildings)); }
  // … 2 lines
  if (cooked) {
    // … 2 lines
    return preparedCity(cooked.data, tableIn(cooked.buildings));
  }
// … 2 lines
}

// src/main.ts:2079-2083 slice of src/main.ts#openedOf (shared): the feature's statements; "…" marks code left out
function openedOf(reply: CityGenReply): OpenedCity {
  // … 2 lines
  return reply.buildings ? preparedCity(reply.data, reply.buildings instanceof ArrayBuffer ? tableIn(reply.buildings) : reply.buildings) : { data: reply.data, table: null };
}

// src/main.ts:2109-2127 slice of src/main.ts#cityJob (shared): the feature's statements; "…" marks code left out
function cityJob(ask: CityGenAsk, key: string | null, priority: number): Promise<CityGenReply> {
  return new Promise((done, fail) => {
    submitJob({
      // … 4 lines
      done: (answer) => {
        // … 4 lines
        if (key && reply.buildings) void keepDerivedDurably(key, { data: reply.data, buildings: tableBytes(reply.buildings) } satisfies KeptCity);
      // … 1 lines
      },
    // … 1 lines
    });
  });
}

// src/main.ts:2353-2363 slice of src/main.ts#afterRestore (shared): the feature's statements; "…" marks code left out
function afterRestore(): void {
  // … 2 lines
  buildings.restored();
// … 6 lines
}

// src/main.ts:2451-2458 slice of src/main.ts#hintKey (shared): the feature's statements; "…" marks code left out
function hintKey(prefix: string): string {
  // … 4 lines
  if (game.tool === 'building') return buildings.hintKey(prefix);
// … 1 lines
}

// src/main.ts:2479-2555 slice of src/main.ts#gameKeys (shared): the feature's statements; "…" marks code left out
const gameKeys = createGameKeys({
  // … 1 lines
  buildingKey: (e) => buildings.key(e),
// … 73 lines
});

// src/main.ts:2597-2606 slice of src/main.ts#growZones (shared): the feature's statements; "…" marks code left out
function growZones(): void {
  // … 1 lines
  const lotRefused = lotTool.refusedNow();
  // … 3 lines
  if (growth.busy) return;
  // … 1 lines
  caused('zone growth', () => growth.ask(lotRefused, 0x5eed));
}

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 38 lines
  growthPool.pump();
  // … 13 lines
  buildings.beforeDraw(game.tool === 'building');
// … 47 lines
}

// src/main.ts:2942-3410 slice of src/main.ts#drawOverlayScreen (shared): the feature's statements; "…" marks code left out
function drawOverlayScreen(): void {
  // … 48 lines
  if (game.tool === 'road' && blockGridChoice.armed && hoverAnchor) {
    // … 1 lines
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
  // … 1 lines
  if (game.tool === 'building') buildings.drawOverlay(ctx);
// … 369 lines
}

// src/main.ts:3923-3930 slice of src/main.ts#<ExpressionStatement>@3923 (shared): the feature's statements; "…" marks code left out
onLanguageChange(() => {
  buildings.languageChanged();
// … 5 lines
});

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 48 lines
  growTimed: (): Promise<{ grown: readonly number[]; ms: number } | { grown: null; why: string }> => {
    if (growth.busy) return Promise.resolve({ grown: null, why: 'a building is being made already' });
    // … 1 lines
    if (!growth.ask(lotTool.refusedNow(), 0x5eed)) return Promise.resolve({ grown: null, why: 'no open lot' });
  // … 1 lines
  },
  // … 37 lines
  inventory: (options: { text?: boolean; top?: number } = {}) => {
    // … 2 lines
    const counts = {
      doc: {
        nodes: doc.nodes.size, segments: doc.segments.size, buildings: doc.buildings.size, lots: doc.lots.length, poles: doc.poles.size,
      // … 1 lines
      },
    // … 3 lines
    };
  // … 4 lines
  },
  // … 85 lines
  buildings: buildings.tool,
};

// src/main.ts:4253-4253 slice of src/main.ts#decayLook (shared): the feature's statements; "…" marks code left out
const decayLook = createDecayWatch(doc, (id, decay) => caused('building wear', () => doc.buildings.wear(id, decay)));

// src/main.ts:4254-4254 slice of src/main.ts#decayWatch (shared): the feature's statements; "…" marks code left out
const decayWatch = (): void => { if (decayLook(sim.minutes)) requestDraw(); };

// src/main.ts:4259-4274 slice of src/main.ts#actions (shared): the feature's statements; "…" marks code left out
const actions = createActions({
  // … 4 lines
  buildingAt: (screen) => buildings.tool.buildingAt(screen),
// … 9 lines
});
