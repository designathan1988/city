// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:458-486 slice of src/main.ts#roadTool (shared): the feature's statements; "…" marks code left out
const roadTool = new RoadTool({
  // … 4 lines
  settings: () => ({
    // … 1 lines
    heightOffset: game.roadHeightOffset, parking: roadParking(), width: roadWidth(), grid: roadGridShown(),
  // … 4 lines
  }),
// … 15 lines
});

// src/main.ts:488-498 slice of src/main.ts#poleTool (shared): the feature's statements; "…" marks code left out
const poleTool = new PoleTool({
  // … 3 lines
  mode: () => poleToolMode(),
  lamps: () => poleLampMode(),
// … 4 lines
});

// src/main.ts:502-513 slice of src/main.ts#streetscapeTool (shared): the feature's statements; "…" marks code left out
const streetscapeTool = new StreetscapeTool({
  // … 3 lines
  kind: () => streetscapeKind(),
  extra: (kind) => kind === 'sign' ? { signType: signChoice.type, text: signChoice.text }
    : kind === 'streetname' ? { text: signChoice.streetName }
    : kind === 'tree' || kind === 'shrub' ? { planted: sim.minutes } : {},
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

// src/main.ts:1041-1053 slice of src/main.ts#cancelGestures (shared): the feature's statements; "…" marks code left out
function cancelGestures(): void {
  // … 5 lines
  bulldozer.cancel();
// … 5 lines
}

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  // … 11 lines
  const bulldozing = bulldozer.gesture();
  if (bulldozing) return bulldozing;
// … 3 lines
}

// src/main.ts:1105-1114 slice of src/main.ts#bulldozer (shared): the feature's statements; "…" marks code left out
const bulldozer = new Bulldozer({
  doc,
  net,
  bulldozeBuildingAt: (at) => buildings.bulldozeAt(at),
  poleReach: () => poleReach(),
  itemReach: () => streetscapeTool.reach(),
  mutate: (fn) => mutate(fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:1117-1119 slice of src/main.ts#gestureInProgress (shared): the feature's statements; "…" marks code left out
function gestureInProgress(): boolean {
  return roadTool.inProgress() || poleTool.inProgress() || terrainBrush.stroking || mover.dragging || lotTool.gesture() !== null || bulldozer.inProgress();
}

// src/main.ts:1137-1150 slice of src/main.ts#cloudTool (shared): the feature's statements; "…" marks code left out
const cloudTool = new CloudTool({
  // … 1 lines
  mode: () => cloudMode(),
  brush: () => cloudBrush(),
// … 9 lines
});

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 34 lines
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
      // … 1 lines
      break;
    // … 60 lines
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
  // … 43 lines
  }
// … 1 lines
}

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 13 lines
  if (bulldozer.move(e.pointerId, { x: e.clientX - r.left, y: e.clientY - r.top }, world)) return;
// … 56 lines
}

// src/main.ts:1484-1513 slice of src/main.ts#endPointer (shared): the feature's statements; "…" marks code left out
function endPointer(e: PointerEvent): void {
  // … 12 lines
  bulldozer.up(e.pointerId, !cancelled && !wasPinching);
// … 15 lines
}

// src/main.ts:1715-1771 slice of src/main.ts#SLIDERS (shared): the feature's statements; "…" marks code left out
const SLIDERS: Readonly<Record<string, ShellSlider>> = {
  // … 22 lines
  treeDensity: brushSlider(10, 600, 10, () => treeBrush().density, (v) => setTreeBrush({ density: v })),
  treeHeight: brushSlider(3, 30, 1, () => treeBrush().height, (v) => setTreeBrush({ height: v })),
  treeVariation: brushSlider(0, 80, 5, () => treeBrush().variation, (v) => setTreeBrush({ variation: v })),
  treeSpacing: brushSlider(2, 30, 1, () => treeBrush().spacing, (v) => setTreeBrush({ spacing: v })),
  // … 1 lines
  elDensity: brushSlider(0.1, 200, 0.1, () => elementBrush().density, (v) => setElementBrush({ density: v })),
  elSize: brushSlider(0.05, 10, 0.05, () => elementBrush().size, (v) => setElementBrush({ size: v })),
  elVariation: brushSlider(0, 100, 5, () => elementBrush().variation, (v) => setElementBrush({ variation: v })),
  elSpacing: brushSlider(0, 20, 0.1, () => elementBrush().spacing, (v) => setElementBrush({ spacing: v })),
  elStrength: brushSlider(5, 100, 5, () => elementBrush().strength, (v) => setElementBrush({ strength: v })),
  elIntensity: brushSlider(0, 100, 5, () => elementBrush().intensity, (v) => setElementBrush({ intensity: v })),
  // … 1 lines
  cloudSize: brushSlider(30, 600, 10, () => cloudBrush().size, (v) => setCloudBrush({ size: v })),
  cloudHeight: brushSlider(40, 900, 10, () => cloudBrush().height, (v) => setCloudBrush({ height: v })),
  cloudDensity: brushSlider(10, 100, 5, () => cloudBrush().density, (v) => setCloudBrush({ density: v })),
  // … 3 lines
  fogStrength: brushSlider(5, 100, 5, () => fogBrush().strength, (v) => setFogBrush({ strength: v })),
  fogHeight: brushSlider(2, 160, 2, () => fogBrush().height, (v) => setFogBrush({ height: v })),
  fogSpeed: brushSlider(0, 24, 0.5, () => fogBrush().speed, (v) => setFogBrush({ speed: v })),
// … 12 lines
};

// src/main.ts:1798-1817 slice of src/main.ts#MAP_ACTIONS (shared): the feature's statements; "…" marks code left out
const MAP_ACTIONS: Readonly<Record<string, () => void>> = {
  // … 11 lines
  scatterClouds: () => wholeMapEdit(null, () => {
    const brush = cloudBrush();
    const laid = scatterClouds(cloudScatter.count, {
      size: brush.size * UNITS_PER_METER, height: brush.height * UNITS_PER_METER, density: brush.density / 100,
    }, cloudScatter.variation / 100, CLOUD_SPAN / 2, doc.clouds, editRandom(doc, 0, 0));
  // … 1 lines
  }),
};

// src/main.ts:1985-1985 slice of src/main.ts#<ExpressionStatement>@1985 (shared): the feature's statements; "…" marks code left out
onRoadGridChange(requestDraw);

// src/main.ts:2479-2555 slice of src/main.ts#gameKeys (shared): the feature's statements; "…" marks code left out
const gameKeys = createGameKeys({
  // … 33 lines
  deleteRoad: () => {
    // … 3 lines
    mutate(() => removeRoads(doc, net, [id]));
  },
// … 36 lines
});

// src/main.ts:2597-2606 slice of src/main.ts#growZones (shared): the feature's statements; "…" marks code left out
function growZones(): void {
  // … 4 lines
  if (mover.dragging || (now >= zoneGrowthHold.at && now < zoneGrowthHold.until)) return;
// … 3 lines
}

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 26 lines
  if (!actionsWarm && game.tool === 'bulldoze' && strikeChoice.mode !== 'demolish') {
    actionsWarm = true;
    void scene.effects();
  }
  // … 11 lines
  actions.tick(simTime);
// … 58 lines
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
  // … 3 lines
  const bulldozeBox = bulldozer.box();
  if (bulldozeBox) {
    const { a, b } = bulldozeBox;
    const corners = [{ x: a.x, y: a.y }, { x: b.x, y: a.y }, { x: b.x, y: b.y }, { x: a.x, y: b.y }];
    ctx.save();
    ctx.beginPath();
    corners.forEach((p, i) => {
      const q = corners[(i + 1) % 4]!;
      const n = Math.max(1, Math.ceil(Math.hypot(q.x - p.x, q.y - p.y) / GRID_CELL));
      for (let k = 0; k < n; k++) {
        const on = { x: p.x + (q.x - p.x) * k / n, y: p.y + (q.y - p.y) * k / n };
        const s = view.toScreen(on, w, h, sceneHeightAt(on));
        if (i === 0 && k === 0) ctx.moveTo(s.x, s.y); else ctx.lineTo(s.x, s.y);
      }
    });
    ctx.closePath();
    ctx.fillStyle = 'rgba(227, 108, 96, 0.18)';
    ctx.fill();
    ctx.strokeStyle = '#ff6b5e';
    ctx.lineWidth = 2;
    ctx.setLineDash([6, 4]);
    ctx.stroke();
    ctx.restore();
  }
  // … 144 lines
  if (roadPreview) {
    // … 1 lines
    const chosenWidth = roadWidth();
    // … 3 lines
    const rt = picked ? profileRoad(picked.profile, picked.type) : roadProfile(game.roadTypeIndex, game.roadLanePreset,
      roadType(game.roadTypeIndex).lanes === 1 ? 'aToB' : 'both',
      chosenWidth === null ? undefined : sectionForWidth(plainRt, chosenWidth, Math.round(plainRt.speedLimit * 3.6 * METERS_PER_UNIT)),
      roadParking());
    // … 59 lines
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
    // … 20 lines
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
  // … 79 lines
  }
}

// src/main.ts:3573-3818 slice of src/main.ts#showInspector (shared): the feature's statements; "…" marks code left out
function showInspector(): void {
  openInspector(
    // … 4 lines
    {
      // … 220 lines
      onRemoveNode: (node) => {
        mutate(() => {
          // … 2 lines
          removeRoads(doc, net, [...source.incident]);
        // … 2 lines
        });
      // … 2 lines
      },
      onDelete: (id) => {
        mutate(() => removeRoads(doc, net, [id]));
      // … 2 lines
      },
    },
  );
}

// src/main.ts:4259-4274 slice of src/main.ts#actions (shared): the feature's statements; "…" marks code left out
const actions = createActions({
  doc,
  net,
  sim,
  scene,
  buildingAt: (screen) => buildings.tool.buildingAt(screen),
  view: () => view,
  size: () => ({ w: surface.cssW, h: surface.cssH }),
  heightAt: (p) => sceneHeightAt(p),
  strength: () => strikeChoice.strength,
  mutate: (fn) => mutate(fn),
  caused: (cause, fn) => caused(cause, fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
  holdGrowth: (seconds) => { zoneGrowthHold = { at: sim.clock.time, until: sim.clock.time + seconds }; },
});
