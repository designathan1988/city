// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:191-191 slice of src/main.ts#NEW_RELIEF (shared): the feature's statements; "…" marks code left out
const NEW_RELIEF = RELIEF_FLAT;

// src/main.ts:192-192 slice of src/main.ts#<ExpressionStatement>@192 (shared): the feature's statements; "…" marks code left out
doc.terrainRelief = NEW_RELIEF;

// src/main.ts:194-194 slice of src/main.ts#newNature (shared): the feature's statements; "…" marks code left out
const newNature = (region = DEFAULT_REGION): NatureSettings => ({ region, seed: Math.floor(Math.random() * 1_000_000_000) });

// src/main.ts:195-195 slice of src/main.ts#<ExpressionStatement>@195 (shared): the feature's statements; "…" marks code left out
doc.nature = newNature();

// src/main.ts:248-276 slice of src/main.ts#restoreBootSave (shared): the feature's statements; "…" marks code left out
function restoreBootSave(loaded: Awaited<ReturnType<Persistence['loadSessionAsync']>>): { readonly settings: SavedSettings | null; readonly hadMap: boolean; readonly failed: boolean } {
  // … 6 lines
  try {
  // … 10 lines
  } catch (error) {
    // … 3 lines
    fresh.terrainRelief = NEW_RELIEF;
    fresh.nature = null;
  // … 3 lines
  }
}

// src/main.ts:374-374 slice of src/main.ts#BrushMode (shared): the feature's statements; "…" marks code left out
type BrushMode = TerrainMode | 'paint' | 'fog' | 'cloud' | 'elements' | 'gully' | 'trees' | 'weather' | Landform;

// src/main.ts:377-382 slice of src/main.ts#LANDFORMS (shared): the feature's statements; "…" marks code left out
const LANDFORMS: Readonly<Record<Landform, { readonly mode: 'raise' | 'lower'; readonly rock: GeologyKind; readonly hardness: number; readonly profile?: 'dome' }>> = {
  mesa: { mode: 'raise', rock: 'sandstone', hardness: 85 },
  canyon: { mode: 'lower', rock: 'sandstone', hardness: 80 },
  escarpment: { mode: 'raise', rock: 'basalt', hardness: 75 },
  sugarloaf: { mode: 'raise', rock: 'granite', hardness: 0, profile: 'dome' },
};

// src/main.ts:502-513 slice of src/main.ts#streetscapeTool (shared): the feature's statements; "…" marks code left out
const streetscapeTool = new StreetscapeTool({
  doc,
  net,
  zoom: () => view.zoom,
  kind: () => streetscapeKind(),
  extra: (kind) => kind === 'sign' ? { signType: signChoice.type, text: signChoice.text }
    : kind === 'streetname' ? { text: signChoice.streetName }
    : kind === 'tree' || kind === 'shrub' ? { planted: sim.minutes } : {},
  mutate: (fn) => mutate(fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:579-589 slice of src/main.ts#terrainBrush (shared): the feature's statements; "…" marks code left out
const terrainBrush = new TerrainBrush({
  doc,
  settings: () => dabSettings(),
  heightAt: (at) => sceneHeightAt(at),
  // The cost-aware rate limit: every dab that moves the land re-solves the roads on it
  interval: (stroking) => Math.max(TERRAIN_MIN_MS, (stroking ? scene.stats.terrainMs : scene.stats.rebuildMs) * 1.4),
  record: () => { history.begin(doc); updateHistoryButtons(); },
  redraw: () => requestDraw(),
  // Decision 5.1: a full tile refuses the new item, nothing of the player's taken away.
  refused: () => flashHint('hint.tileFull'),
});

// src/main.ts:591-605 slice of src/main.ts#dabSettings (shared): the feature's statements; "…" marks code left out
function dabSettings(): DabSettings {
// … 13 lines
}

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  // … 6 lines
  if (terrainBrush.stroking) return 'gesture.terrain.stroke';
  // … 6 lines
  const cloud = cloudTool.gesture();
  if (cloud) return cloud;
// … 1 lines
}

// src/main.ts:1105-1114 slice of src/main.ts#bulldozer (shared): the feature's statements; "…" marks code left out
const bulldozer = new Bulldozer({
  // … 4 lines
  itemReach: () => streetscapeTool.reach(),
// … 3 lines
});

// src/main.ts:1117-1119 slice of src/main.ts#gestureInProgress (shared): the feature's statements; "…" marks code left out
function gestureInProgress(): boolean {
  return roadTool.inProgress() || poleTool.inProgress() || terrainBrush.stroking || mover.dragging || lotTool.gesture() !== null || bulldozer.inProgress();
}

// src/main.ts:1137-1150 slice of src/main.ts#cloudTool (shared): the feature's statements; "…" marks code left out
const cloudTool = new CloudTool({
  doc,
  mode: () => cloudMode(),
  brush: () => cloudBrush(),
  drift: () => scene.cloudDrift(),
  rayAt: (px, py, height) => view.toWorldAt(px, py, height, surface.cssW, surface.cssH),
  record: () => { history.begin(doc); cloudStepOpen = true; },
  changed: () => {
    updateHistoryButtons();
    persistence.saveSessionSoon(doc, sessionSettings);
    requestDraw();
  },
  hint: (key) => flashHint(key),
});

// src/main.ts:1152-1162 slice of src/main.ts#endTerrainStroke (shared): the feature's statements; "…" marks code left out
function endTerrainStroke(): void {
  const wasPainting = terrainBrush.end();
  // … 1 lines
  if (wasPainting) { history.end(doc); updateHistoryButtons(); }
  // … 1 lines
  if (wasPainting && doc.terrainStamps.length >= MAX_TERRAIN_STAMPS * 0.9) {
    flashHint(doc.terrainStamps.length >= MAX_TERRAIN_STAMPS ? 'hint.terrain.capReached' : 'hint.terrain.capNear');
  }
// … 2 lines
}

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 34 lines
  switch (game.tool) {
    // … 24 lines
    case 'terrain':
      if (game.terrainMode === 'cloud') cloudTool.down(e.pointerId, e.clientX - r.left, e.clientY - r.top);
      // The weather tool: a click calls a lightning bolt down there.
      else if (game.terrainMode === 'weather') scene.strikeAt(world.x, world.y);
      else terrainBrush.begin(e.pointerId, world);
      break;
    // … 18 lines
    case 'streetscape':
      streetscapeTool.down(world, e.shiftKey);
      break;
  // … 90 lines
  }
// … 1 lines
}

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 1 lines
  if (e.pointerType === 'mouse' && e.buttons === 0 && terrainBrush.stroking) endTerrainStroke();
  // … 39 lines
  if (game.tool === 'streetscape') {
    streetscapeTool.move(world, (e.buttons & 1) === 1);
  // … 1 lines
  }
  // … 4 lines
  if (terrainBrush.pointer === e.pointerId) {
    terrainBrush.paint(world);
    return;
  }
  if (cloudTool.pointer === e.pointerId) {
    const rect = canvas.getBoundingClientRect();
    cloudTool.move(e.clientX - rect.left, e.clientY - rect.top);
    return;
  }
// … 12 lines
}

// src/main.ts:1484-1513 slice of src/main.ts#endPointer (shared): the feature's statements; "…" marks code left out
function endPointer(e: PointerEvent): void {
  // … 7 lines
  if (terrainBrush.pointer === e.pointerId) endTerrainStroke();
  cloudTool.up(e.pointerId);
  // … 10 lines
  if (game.tool === 'streetscape') {
    if (cancelled || wasPinching) streetscapeTool.row = null;
    else streetscapeTool.up();
  }
// … 5 lines
}

// src/main.ts:1674-1674 slice of src/main.ts#<ExpressionStatement>@1674 (shared): the feature's statements; "…" marks code left out
scene.onStrike((_x, _y, distance) => playThunder(distance / UNITS_PER_METER, doc.weather.thunder));

// src/main.ts:1675-1676 slice of src/main.ts#weatherSlider (shared): the feature's statements; "…" marks code left out
const weatherSlider = (key: keyof typeof doc.weather, min: number, max: number, step: number, scale: number): ShellSlider =>
  mapSlider(min, max, step, () => Math.round(doc.weather[key] * scale * 10) / 10, (v) => doc.setWeather({ [key]: v / scale }));

// src/main.ts:1703-1711 slice of src/main.ts#<IfStatement>@1703 (shared): the feature's statements; "…" marks code left out
if (atmosphere.atmoClouds > 0) {
  if (doc.clouds.length === 0) {
    doc.addClouds(scatterClouds(Math.round((atmosphere.atmoClouds / 100) * 12), {
      size: Math.max(40, atmosphere.atmoCloudThickness * UNITS_PER_METER), height: atmosphere.atmoCloudBase * UNITS_PER_METER, density: 0.8,
    }, 0.3, CLOUD_SPAN / 2, [], editRandom(doc, 0, 0)));
  // … 1 lines
  }
// … 1 lines
}

// src/main.ts:1715-1771 slice of src/main.ts#SLIDERS (shared): the feature's statements; "…" marks code left out
const SLIDERS: Readonly<Record<string, ShellSlider>> = {
  // … 10 lines
  gullyAuto: mapSlider(0, 100, 5, () => Math.round(doc.gullyAuto * 100), (v) => doc.setGullyAuto(v / 100)),
  // … 11 lines
  treeDensity: brushSlider(10, 600, 10, () => treeBrush().density, (v) => setTreeBrush({ density: v })),
  treeHeight: brushSlider(3, 30, 1, () => treeBrush().height, (v) => setTreeBrush({ height: v })),
  treeVariation: brushSlider(0, 80, 5, () => treeBrush().variation, (v) => setTreeBrush({ variation: v })),
  treeSpacing: brushSlider(2, 30, 1, () => treeBrush().spacing, (v) => setTreeBrush({ spacing: v })),
  // … 18 lines
  fogMapDensity: mapSlider(0, 200, 10, () => Math.round(doc.fogSettings.density * 100), (v) => doc.setFogSettings({ density: v / 100 })),
// … 10 lines
};

// src/main.ts:1774-1776 slice of src/main.ts#mapBiome (shared): the feature's statements; "…" marks code left out
function mapBiome(): string {
  return doc.nature?.region ?? 'none';
}

// src/main.ts:1777-1786 slice of src/main.ts#setMapBiome (shared): the feature's statements; "…" marks code left out
function setMapBiome(key: string): void {
  const next = isRegionId(key) ? { region: key, seed: doc.nature?.seed ?? newNature().seed } : null;
  if (JSON.stringify(doc.nature) === JSON.stringify(next)) return;
  // … 1 lines
  doc.setNature(next);
// … 4 lines
}

// src/main.ts:1798-1817 slice of src/main.ts#MAP_ACTIONS (shared): the feature's statements; "…" marks code left out
const MAP_ACTIONS: Readonly<Record<string, () => void>> = {
  // … 7 lines
  clearClouds: () => {
    if (doc.clouds.length > 0) wholeMapEdit('confirm.clearClouds', () => { for (const cloud of [...doc.clouds]) doc.removeCloud(cloud.id); });
  },
  // … 1 lines
  scatterClouds: () => wholeMapEdit(null, () => {
    // … 1 lines
    const laid = scatterClouds(cloudScatter.count, {
      size: brush.size * UNITS_PER_METER, height: brush.height * UNITS_PER_METER, density: brush.density / 100,
    }, cloudScatter.variation / 100, CLOUD_SPAN / 2, doc.clouds, editRandom(doc, 0, 0));
    if (doc.addClouds(laid) < cloudScatter.count) flashHint('hint.cloud.full');
  }),
};

// src/main.ts:1994-1996 slice of src/main.ts#growth (shared): the feature's statements; "…" marks code left out
const growth = new GrowthSession(growthPool, () => ({
  doc, net, groundAt: (x, y) => scene.terrainHeightAt(x, y), pavedAt: scene.pavedHeightAt, cell: TERRAIN_CELL,
}));

// src/main.ts:2015-2027 slice of src/main.ts#newMap (shared): the feature's statements; "…" marks code left out
function newMap(): void {
  // … 1 lines
  {
    // … 3 lines
    applySnapshot({ ...new RoadDoc().toJSON(), relief: NEW_RELIEF, markingStyle: 'br' }, 'import');
  // … 4 lines
  }
}

// src/main.ts:2175-2200 slice of src/main.ts#openCityNow (shared): the feature's statements; "…" marks code left out
async function openCityNow(ask: CityGenAsk): Promise<void> {
  // … 4 lines
  try {
    // … 3 lines
    const bake = await networkBakeOf({ ...city.data, relief: NEW_RELIEF, markingStyle: 'br' }, city.table);
    // … 3 lines
    openCity({ ...city.data, nature: city.data.nature ?? { region: DEFAULT_REGION, seed: ask.seed ?? 7 } }, openedBuildings(city) > 5_000, city.table, bake);
  // … 1 lines
  } catch (error) {
  // … 8 lines
  }
}

// src/main.ts:2213-2227 slice of src/main.ts#openCity (shared): the feature's statements; "…" marks code left out
function openCity(data: ReturnType<typeof buildSmallCityMap>, large = false, table: ArrayBufferLike | null = null, bake: NetworkBake | null = null): void {
  // … 8 lines
  applySnapshot({ ...data, relief: NEW_RELIEF, markingStyle: 'br', nature: data.nature ?? newNature() }, 'import', table);
// … 4 lines
}

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 16 lines
  if (terrainBrush.tick(performance.now())) requestDraw();
  // … 43 lines
  scene.draw(net, sim, alpha, wall, { holdRoads: terrainBrush.stroking });
// … 39 lines
}

// src/main.ts:2808-2847 slice of src/main.ts#drawStreetscapeHover (shared): the feature's statements; "…" marks code left out
function drawStreetscapeHover(ctx: CanvasRenderingContext2D, at: (p: Vec2) => Vec2): void {
  // … 1 lines
  const row = streetscapeTool.row;
  for (const piece of row?.pieces ?? []) {
    const c = at(piece.at);
    const e = at({ x: piece.at.x + LANDSCAPE_RADIUS[row!.kind] + m(0.3), y: piece.at.y });
    ctx.save();
    ctx.lineWidth = 2;
    ctx.strokeStyle = SELECTION;
    ctx.fillStyle = 'rgba(120, 200, 255, 0.18)';
    ctx.beginPath();
    ctx.arc(c.x, c.y, Math.max(6, Math.hypot(e.x - c.x, e.y - c.y)), 0, Math.PI * 2);
    ctx.fill();
    ctx.stroke();
    ctx.restore();
  }
  const hover = streetscapeTool.hover;
  if (!hover) return;
  const centre = at(hover.at);
  const edge = at({ x: hover.at.x + LANDSCAPE_RADIUS[streetscapeKind()] + m(0.3), y: hover.at.y });
  const radius = Math.max(6, Math.hypot(edge.x - centre.x, edge.y - centre.y));
  // … 2 lines
  ctx.strokeStyle = hover.ok ? SELECTION : '#e5534b';
  ctx.fillStyle = hover.ok ? 'rgba(120, 200, 255, 0.18)' : 'rgba(229, 83, 75, 0.18)';
  // … 1 lines
  ctx.arc(centre.x, centre.y, radius, 0, Math.PI * 2);
  // … 2 lines
  if (!hover.ok) {
    const k = radius * 0.6;
    ctx.beginPath();
    ctx.moveTo(centre.x - k, centre.y - k);
    ctx.lineTo(centre.x + k, centre.y + k);
    ctx.moveTo(centre.x + k, centre.y - k);
    ctx.lineTo(centre.x - k, centre.y + k);
    ctx.stroke();
  }
// … 1 lines
}

// src/main.ts:2942-3410 slice of src/main.ts#drawOverlayScreen (shared): the feature's statements; "…" marks code left out
function drawOverlayScreen(): void {
  // … 185 lines
  if (game.tool === 'terrain' && hoverAnchor) {
    const brush = terrainBrush.at ?? hoverAnchor.at;
    const centre = at(brush);
    const xEdge = at({ x: brush.x + game.terrainRadius, y: brush.y });
    const yEdge = at({ x: brush.x, y: brush.y + game.terrainRadius });
    const rx = Math.max(4, Math.hypot(xEdge.x - centre.x, xEdge.y - centre.y));
    const ry = Math.max(4, Math.hypot(yEdge.x - centre.x, yEdge.y - centre.y));
    // … 7 lines
    ctx.ellipse(centre.x, centre.y, rx, ry, 0, 0, Math.PI * 2);
    // … 10 lines
    ctx.ellipse(centre.x, centre.y, rx * bite, ry * bite, 0, 0, Math.PI * 2);
    // … 4 lines
    ctx.moveTo(centre.x - 5, centre.y);
    ctx.lineTo(centre.x + 5, centre.y);
    ctx.moveTo(centre.x, centre.y - 5);
    ctx.lineTo(centre.x, centre.y + 5);
    // … 2 lines
    const height = terrainBrush.level ?? sceneHeightAt(brush);
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
    // … 17 lines
    for (const [pieceIndex, piece] of pieces.entries()) {
      // … 4 lines
      const reach = pieceLength * MAX_AUTHORED_GRADE;
      const nextHeight = clamp(piece.end.heightOffset,
        previewHeight - reach, previewHeight + reach);
      limited ||= Math.abs(nextHeight - piece.end.heightOffset) > 1e-6;
      for (let i = pieceIndex === 0 ? 0 : 1; i < flattened.length; i++) {
        // … 3 lines
        const offset = previewHeight + (nextHeight - previewHeight) * eased;
        // … 2 lines
        offsets.push(offset);
        // … 1 lines
        projected.push(view.toScreen(p, w, h, ground + offset));
      }
      previewHeight = nextHeight;
    }
    // … 18 lines
    const ok = pathLength >= MIN_LINK_LENGTH * 0.25 &&
      !(limited && roadPreview.snap.guide === 'network') && refusal === null;
    // … 16 lines
    if (modes.length > 1) drawBuildModes(ctx, projected, groundProjected, modes);
    else if (points.length > 1 && offsets.some((offset) => Math.abs(offset) > UNITS_PER_METER * 0.5)) {
      // … 6 lines
      ctx.fillStyle = previewHeight >= 0 ? 'rgba(101, 229, 195, 0.16)' : 'rgba(244, 184, 103, 0.20)';
    // … 1 lines
    }
    // … 22 lines
    if (!settling) band(casingHalf + 2, ok ? SELECTION : INVALID);
    // … 11 lines
    if (points.length && !settling) {
      ring(roadPreview.start.at, 7, ok ? SELECTION : INVALID, 2, projected[0]);
      ring(roadPreview.snap.at, 7, ok ? SELECTION : INVALID, 2, projected[projected.length - 1]);
      // … 3 lines
      {
        // … 14 lines
        ctx.fillStyle = ok ? '#e7fff7' : '#ffb4ab';
      // … 3 lines
      }
      if (Math.abs(previewHeight) > UNITS_PER_METER * 0.5) {
        ctx.save();
        ctx.strokeStyle = previewHeight >= 0 ? '#7df7d3' : '#ffc864';
        ctx.lineWidth = 3;
        ctx.setLineDash([4, 3]);
        ctx.beginPath();
        ctx.moveTo(groundEnd.x, groundEnd.y);
        ctx.lineTo(end.x, end.y);
        ctx.stroke();
        ctx.setLineDash([]);
        ctx.beginPath();
        ctx.arc(groundEnd.x, groundEnd.y, 5, 0, Math.PI * 2);
        ctx.stroke();
        ctx.restore();
      }
      const metres = previewHeight / UNITS_PER_METER;
      const grade = pathLength > 1
        ? Math.round(Math.abs(previewHeight - roadPreview.startHeightOffset) / pathLength * 100)
        : 0;
      const label = `${metres >= 0 ? '+' : ''}${formatDecimal(metres, 1)} m · ${grade}%`;
      if (Math.abs(previewHeight) > 0.25 ||
        Math.abs(previewHeight - roadPreview.startHeightOffset) > 0.25) {
        const x = clamp(end.x + 16, 12, Math.max(12, w - 430));
        const y = clamp(end.y + 24, 80, h - 80);
        const baseline = y + 51;
        const levelY = (offset: number): number => baseline -
          clamp(offset / UNITS_PER_METER * 3, -22, 22);
        const startY = levelY(roadPreview.startHeightOffset);
        const endY = levelY(previewHeight);
        const colour = previewHeight >= 0 ? '#7df7d3' : '#ffc864';
        ctx.save();
        ctx.fillStyle = 'rgba(9, 27, 28, 0.94)';
        ctx.fillRect(x, y, 170, 68);
        ctx.strokeStyle = 'rgba(210, 226, 218, 0.55)';
        ctx.setLineDash([3, 3]);
        ctx.beginPath();
        ctx.moveTo(x + 12, baseline);
        ctx.lineTo(x + 158, baseline);
        ctx.stroke();
        ctx.setLineDash([]);
        ctx.strokeStyle = colour;
        ctx.lineWidth = 3;
        ctx.beginPath();
        ctx.moveTo(x + 12, startY);
        ctx.bezierCurveTo(x + 63, startY, x + 107, endY, x + 158, endY);
        ctx.stroke();
        ctx.font = '600 11px system-ui, sans-serif';
        ctx.fillStyle = limited ? '#ffc864' : '#e7fff7';
        ctx.fillText(label, x + 11, y + 16);
        ctx.restore();
      }
    }
  }
}

// src/main.ts:3546-3571 slice of src/main.ts#setNodeHeightMetres (shared): the feature's statements; "…" marks code left out
function setNodeHeightMetres(id: NodeId, metres: number): void {
  // … 4 lines
  for (const segmentId of node.incident) {
    // … 4 lines
    const rise = net.polylines.get(doc, segmentId).length * MAX_AUTHORED_GRADE;
    lower = Math.max(lower, other.heightOffset - rise);
    upper = Math.min(upper, other.heightOffset + rise);
  }
  // … 1 lines
  const height = lower <= upper ? clamp(requested, lower, upper) : node.heightOffset;
  if (height === node.heightOffset) {
    if (Math.abs(height - requested) > 1e-6) flashHint('hint.road.gradeLimited');
    return;
  }
  mutateRoads((doc) => {
    doc.setNodeHeightOffset(id, height);
  // … 1 lines
  });
  if (Math.abs(height - requested) > 1e-6) flashHint('hint.road.gradeLimited');
}

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 91 lines
  inventory: (options: { text?: boolean; top?: number } = {}) => {
    // … 2 lines
    const counts = {
      doc: {
        // … 1 lines
        landscape: doc.landscape.size, barriers: doc.barriers.size, trees: doc.trees.length, zones: doc.zones.length, people: doc.people.length,
      },
    // … 3 lines
    };
  // … 4 lines
  },
// … 86 lines
};
