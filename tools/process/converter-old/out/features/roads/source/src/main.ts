// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:154-154 slice of src/main.ts#clearBulbs (shared): the feature's statements; "…" marks code left out
const clearBulbs = bulbClearing();

// src/main.ts:159-159 slice of src/main.ts#<ExpressionStatement> (shared): the feature's statements; "…" marks code left out
setGlobalParams(roadKeyParams);

// src/main.ts:196-196 slice of src/main.ts#net (shared): the feature's statements; "…" marks code left out
const net = new Network(doc);

// src/main.ts:223-223 slice of src/main.ts#surfaceBake (shared): the feature's statements; "…" marks code left out
const surfaceBake = startSurfaceBake();

// src/main.ts:248-276 slice of src/main.ts#restoreBootSave (shared): the feature's statements; "…" marks code left out
function restoreBootSave(loaded: Awaited<ReturnType<Persistence['loadSessionAsync']>>): { readonly settings: SavedSettings | null; readonly hadMap: boolean; readonly failed: boolean } {
  // … 6 lines
  try {
    // … 4 lines
    restoreInto(doc, asTable?.data ?? map, net, asTable?.table ?? null);
  // … 5 lines
  } catch (error) {
    // … 6 lines
    net.rebuild();
  // … 1 lines
  }
}

// src/main.ts:295-295 slice of src/main.ts#sim (shared): the feature's statements; "…" marks code left out
const sim = new SimClient(doc, net);

// src/main.ts:458-486 slice of src/main.ts#roadTool (shared): the feature's statements; "…" marks code left out
const roadTool = new RoadTool({
  doc,
  net,
  zoom: () => view.zoom,
  px: (n) => camera.px(n),
  settings: () => ({
    typeIndex: game.roadTypeIndex, lanes: game.roadLanePreset, alignment: game.alignment,
    heightOffset: game.roadHeightOffset, parking: roadParking(), width: roadWidth(), grid: roadGridShown(),
    // The profile chosen in the profile editor for new roads.
    profile: drawProfile(),
    cutWalls: cutWallsChosen(),
    furniture: furnitureChosen(),
  }),
  setHeight: (value, cause) => gameState.set('roadHeightOffset', value, cause),
  worldAtScreen: (px, py, height) => worldAtScreen(px, py, height),
  naturalHeightAt: (x, y) => scene.builtGroundAt(x, y),
  offerElevation: (solution, revision) => scene.offerElevation(solution, revision),
  // The renderer's last solve while the land is the one it was solved on: the tunnel test starts from it.
  groundSolve: () => {
    const solved = scene.roadsSolve();
    return solved && solved.terrainRevision === doc.terrainRevision ? solved.elevation : null;
  },
  flash: (ids) => scene.flashRoads(ids),
  // Its parts one undo step (`History.grouped`).
  undoGroup: (group) => history.grouped(group),
  mutate: (fn) => mutate(fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:488-498 slice of src/main.ts#poleTool (shared): the feature's statements; "…" marks code left out
const poleTool = new PoleTool({
  // … 1 lines
  net,
// … 7 lines
});

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

// src/main.ts:515-528 slice of src/main.ts#barrierTool (shared): the feature's statements; "…" marks code left out
const barrierTool = new BarrierTool({
  net: () => net,
// … 11 lines
});

// src/main.ts:530-543 slice of src/main.ts#lotTool (shared): the feature's statements; "…" marks code left out
const lotTool = new LotTool({
  // … 1 lines
  net,
// … 10 lines
});

// src/main.ts:544-544 slice of src/main.ts#hoverAnchor (shared): the feature's statements; "…" marks code left out
let hoverAnchor: Anchor | null = null;

// src/main.ts:569-577 slice of src/main.ts#mover (shared): the feature's statements; "…" marks code left out
const mover = new NodeMover({
  doc,
  net,
  // The worker's simulation catches its topology up with the map it is sent (`SimClient.frame`).
  settleTopology: () => {},
  mutate: (fn) => mutate(fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:657-657 slice of src/main.ts#<ExpressionStatement>@657 (shared): the feature's statements; "…" marks code left out
primeSurfaceBake(await surfaceBake);

// src/main.ts:760-806 slice of src/main.ts#buildings (shared): the feature's statements; "…" marks code left out
const buildings = createBuildingWiring({
  // … 1 lines
  net,
// … 43 lines
});

// src/main.ts:842-851 slice of src/main.ts#mutateRoads (shared): the feature's statements; "…" marks code left out
function mutateRoads(fn: (doc: RoadDoc, net: Network) => boolean): boolean {
  let refused = null as RoadEditRefusal | null;
  const changed = mutateBuilt(() => {
    const result = guardRoadEdit(doc, net, fn);
    refused = result.refused;
    return result.changed;
  });
  if (refused) flashHint(`hint.rule.${refused}`);
// … 1 lines
}

// src/main.ts:854-860 slice of src/main.ts#transitEditor (shared): the feature's statements; "…" marks code left out
const transitEditor = new TransitTool({
  // … 1 lines
  net: () => net,
// … 3 lines
});

// src/main.ts:876-911 slice of src/main.ts#mutateBuilt (shared): the feature's statements; "…" marks code left out
function mutateBuilt(fn: () => boolean): boolean {
  // … 2 lines
  followNetwork(doc, net);
  const roadsBefore = net.state;
  // … 5 lines
  if (changed) {
    // … 1 lines
    if (net.revision !== doc.revision) net.rebuild();
    // … 1 lines
    buildings.afterRoadEdit(roadsBefore);
    // … 1 lines
    const roadDelta = net.changesSince(roadsBefore);
    cutLotsByRoads(doc, net, roadDelta === null ? null : roadDelta.rects);
    // … 1 lines
    clearBulbs(doc, net);
  }
  // … 6 lines
  topologyAfterDraw = topologyAfterDraw || sim.topologyRevision !== net.trafficRevision;
// … 7 lines
}

// src/main.ts:915-928 slice of src/main.ts#applySnapshot (shared): the feature's statements; "…" marks code left out
function applySnapshot(data: ReturnType<RoadDoc['toJSON']> | null, source: 'snapshot' | 'import' = 'snapshot', table: ArrayBufferLike | null = null): void {
  // … 1 lines
  roadTool.cancel();
  if (source === 'import') {
    caused('map opened', () => restoreInto(doc, data, net, table));
  // … 2 lines
  } else {
    restoreSnapshot(doc, data, net);
  }
// … 3 lines
}

// src/main.ts:997-997 slice of src/main.ts#roadTop (shared): the feature's statements; "…" marks code left out
const roadTop = new RoadTop(doc, net, (x, y) => scene.elevationAt(x, y));

// src/main.ts:998-1002 slice of src/main.ts#surfaceTop (shared): the feature's statements; "…" marks code left out
function surfaceTop(): number {
  // … 1 lines
  const top = Math.max(scene.landTop(), roadTop.height());
  return Number.isFinite(top) ? top + 12 : PICK_TOP;
}

// src/main.ts:1025-1031 slice of src/main.ts#pointerWorld (shared): the feature's statements; "…" marks code left out
function pointerWorld(e: PointerEvent, rect?: DOMRect): Vec2 {
  // … 1 lines
  const authoredHeight = game.tool === 'road' && roadTool.inProgress()
    ? game.roadHeightOffset
    : undefined;
  return worldAtScreen(e.clientX - r.left, e.clientY - r.top, authoredHeight);
}

// src/main.ts:1041-1053 slice of src/main.ts#cancelGestures (shared): the feature's statements; "…" marks code left out
function cancelGestures(): void {
  roadTool.cancel();
// … 10 lines
}

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  // … 2 lines
  const movingNode = mover.gesture();
  if (movingNode) return movingNode;
  const road = roadTool.gesture();
  if (road) return road;
// … 10 lines
}

// src/main.ts:1105-1114 slice of src/main.ts#bulldozer (shared): the feature's statements; "…" marks code left out
const bulldozer = new Bulldozer({
  // … 1 lines
  net,
  // … 2 lines
  itemReach: () => streetscapeTool.reach(),
// … 3 lines
});

// src/main.ts:1117-1119 slice of src/main.ts#gestureInProgress (shared): the feature's statements; "…" marks code left out
function gestureInProgress(): boolean {
  return roadTool.inProgress() || poleTool.inProgress() || terrainBrush.stroking || mover.dragging || lotTool.gesture() !== null || bulldozer.inProgress();
}

// src/main.ts:1129-1131 slice of src/main.ts#cancelMove (shared): the feature's statements; "…" marks code left out
function cancelMove(): void {
  mover.cancel();
}

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 29 lines
  if (game.tool === 'road') roadTool.pointerAt({ x: e.clientX - r.left, y: e.clientY - r.top });
  // … 1 lines
  if (game.tool === 'road' && roadTool.bend(world)) return;
  const anchor = findAnchor(doc, net, world, view.zoom);
  // … 1 lines
  switch (game.tool) {
    case 'roundabout':
      mutate(() => {
        const result = commitRoundabout(doc, net, world, game.roundaboutRadius, 0);
        if (!result.committed) flashHint(`hint.roundabout.${result.reason}`);
        return result.committed;
      });
      break;
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
      roadTool.down(world, anchor);
      break;
    // … 25 lines
    case 'streetscape':
      streetscapeTool.down(world, e.shiftKey);
      break;
    // … 6 lines
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
      // … 12 lines
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

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 3 lines
  if (game.tool === 'road') roadTool.pointerAt(screen);
  // … 16 lines
  if (game.tool === 'road' && roadTool.bending()) {
    roadTool.move(world);
    return;
  }
  // … 7 lines
  if (roadTool.move(world)) return;
  // … 9 lines
  if (game.tool === 'streetscape') {
    streetscapeTool.move(world, (e.buttons & 1) === 1);
  // … 1 lines
  }
  // … 1 lines
  if (mover.move(world)) return;
  // … 13 lines
  const hovered = findAnchor(doc, net, world, view.zoom, undefined,
    game.tool === 'road' ? game.roadHeightOffset : undefined);
  if (game.tool === 'road') roadTool.hover(world);
  hoverAnchor = game.tool === 'terrain'
    ? { kind: 'free', at: world }
    : game.tool === 'road'
      ? anchorForHeight(doc, net, hovered, game.roadHeightOffset)
      : hovered;
// … 2 lines
}

// src/main.ts:1484-1513 slice of src/main.ts#endPointer (shared): the feature's statements; "…" marks code left out
function endPointer(e: PointerEvent): void {
  // … 15 lines
  roadTool.up(() => pointerWorld(e), !cancelled && !wasPinching);
  // … 3 lines
  if (game.tool === 'streetscape') {
    if (cancelled || wasPinching) streetscapeTool.row = null;
    else streetscapeTool.up();
  }
  // … 2 lines
  mover.drop(!cancelled && !wasPinching);
// … 2 lines
}

// src/main.ts:1518-1523 slice of src/main.ts#pointerLeft (shared): the feature's statements; "…" marks code left out
function pointerLeft(): void {
  if (hoverAnchor) {
    hoverAnchor = null;
    requestDraw();
  }
}

// src/main.ts:1558-1566 slice of src/main.ts#roadClasses (shared): the feature's statements; "…" marks code left out
function roadClasses(): ShellRoadClass[] {
  return ROAD_TYPES.map((rt, index) => ({
    index,
    name: roadTypeName(rt),
    title: `${roadTypeName(rt)} — ${roadTypeDescription(rt)}`,
    picture: (roadClassPictures[index] ??= roadSwatch(rt)),
    widthM: Math.round((rt.width + rt.sidewalk * 2) * METERS_PER_UNIT),
  }));
}

// src/main.ts:1569-1569 slice of src/main.ts#MEDIAN_CLASS (shared): the feature's statements; "…" marks code left out
const MEDIAN_CLASS = ROAD_TYPES.findIndex((rt) => rt.median > 0);

// src/main.ts:1577-1585 slice of src/main.ts#laneChoices (shared): the feature's statements; "…" marks code left out
function laneChoices(): { id: string; label: string; on: boolean }[] {
  return LANE_CHOICES.map((choice) => ({
    // … 2 lines
    on: choice.lanes === null
      ? game.roadLanePreset === null && game.roadTypeIndex === MEDIAN_CLASS
      : game.roadTypeIndex !== MEDIAN_CLASS && choice.lanes === (game.roadLanePreset ?? roadType(game.roadTypeIndex).lanes),
  }));
}

// src/main.ts:1586-1594 slice of src/main.ts#setLanes (shared): the feature's statements; "…" marks code left out
function setLanes(id: string): void {
  // … 3 lines
  setDrawProfile(null);
  // … 1 lines
  if (choice.lanes === null) selectRoadType(MEDIAN_CLASS);
// … 1 lines
}

// src/main.ts:1596-1602 slice of src/main.ts#selectRoadType (shared): the feature's statements; "…" marks code left out
function selectRoadType(i: number): void {
  if (!ROAD_TYPES[i]) return;
  // … 1 lines
  setDrawProfile(null);
// … 2 lines
}

// src/main.ts:1604-1609 slice of src/main.ts#setAlignment (shared): the feature's statements; "…" marks code left out
function setAlignment(next: Alignment): void {
  // … 1 lines
  if (next !== 'curve') roadTool.dropCurve();
// … 2 lines
}

// src/main.ts:1611-1611 slice of src/main.ts#<ExpressionStatement>@1611 (shared): the feature's statements; "…" marks code left out
onRoadKeysChange(() => { applyTranslations(document); updateHint(); });

// src/main.ts:1715-1771 slice of src/main.ts#SLIDERS (shared): the feature's statements; "…" marks code left out
const SLIDERS: Readonly<Record<string, ShellSlider>> = {
  // … 10 lines
  gullyAuto: mapSlider(0, 100, 5, () => Math.round(doc.gullyAuto * 100), (v) => doc.setGullyAuto(v / 100)),
// … 44 lines
};

// src/main.ts:1859-1946 slice of src/main.ts#mountInterface (shared): the feature's statements; "…" marks code left out
function mountInterface(): void {
  // … 1 lines
  shell = mountShell({
    // … 29 lines
    stepRoadHeight: (d) => roadTool.stepHeight(d),
    // … 13 lines
    roadSnap,
    setRoadSnap,
    parkingPresets: ROAD_PARKING_PRESETS,
    roadParkingPreset,
    setRoadParkingPreset: (preset) => {
      if ((ROAD_PARKING_PRESETS as readonly string[]).includes(preset)) setRoadParkingPreset(preset as RoadParkingPreset);
    },
    // … 2 lines
    showImpossible: () => {
      const id = net.impossible.keys().next().value;
      const node = id === undefined ? undefined : doc.node(id);
      if (id === undefined || !node) return;
      // … 1 lines
      select(null, null, id, 'impossible node shown');
      // … 1 lines
      view.moveTo(node);
    // … 1 lines
    },
  // … 5 lines
  });
// … 16 lines
}

// src/main.ts:1994-1996 slice of src/main.ts#growth (shared): the feature's statements; "…" marks code left out
const growth = new GrowthSession(growthPool, () => ({
  doc, net, groundAt: (x, y) => scene.terrainHeightAt(x, y), pavedAt: scene.pavedHeightAt, cell: TERRAIN_CELL,
}));

// src/main.ts:1998-2012 slice of src/main.ts#growthAnswered (shared): the feature's statements; "…" marks code left out
function growthAnswered(answer: GrowAnswer): boolean {
  if (mover.dragging) return false;
// … 12 lines
}

// src/main.ts:2015-2027 slice of src/main.ts#newMap (shared): the feature's statements; "…" marks code left out
function newMap(): void {
  // … 1 lines
  {
    // … 5 lines
    roadTool.reset();
  // … 2 lines
  }
}

// src/main.ts:2090-2106 slice of src/main.ts#networkBakeOf (shared): the feature's statements; "…" marks code left out
function networkBakeOf(data: SerializedDoc, table: ArrayBufferLike | null): Promise<NetworkBake | null> {
// … 15 lines
}

// src/main.ts:2213-2227 slice of src/main.ts#openCity (shared): the feature's statements; "…" marks code left out
function openCity(data: ReturnType<typeof buildSmallCityMap>, large = false, table: ArrayBufferLike | null = null, bake: NetworkBake | null = null): void {
  // … 5 lines
  if (bake) net.offerBake(bake);
  // … 5 lines
  roadTool.reset();
// … 1 lines
}

// src/main.ts:2273-2293 slice of src/main.ts#openSession (shared): the feature's statements; "…" marks code left out
function openSession(session: Extract<ImportResult, { status: 'ok' }>['session']): boolean {
  // … 2 lines
  try {
  // … 1 lines
  } catch (error) {
    // … 2 lines
    roadTool.cancel();
    // … 1 lines
    followNetwork(doc, net);
  // … 3 lines
  }
// … 5 lines
}

// src/main.ts:2353-2363 slice of src/main.ts#afterRestore (shared): the feature's statements; "…" marks code left out
function afterRestore(): void {
  roadTool.cancel();
  followNetwork(doc, net);
// … 7 lines
}

// src/main.ts:2479-2555 slice of src/main.ts#gameKeys (shared): the feature's statements; "…" marks code left out
const gameKeys = createGameKeys({
  // … 6 lines
  roadKey: (key) => {
    const action = roadKeyAction(key);
    if (action === 'alignment') {
      const order: readonly Alignment[] = ['straight', 'curve', 'free'];
      setAlignment(order[(order.indexOf(game.alignment) + 1) % order.length]!);
    } else if (action) roadTool.stepHeight(action === 'heightUp' ? 1 : -1);
    return action !== null;
  },
  // … 19 lines
  deleteRoad: () => {
    // … 3 lines
    mutate(() => removeRoads(doc, net, [id]));
  },
  // … 11 lines
  roadClassCount: ROAD_TYPES.length,
// … 24 lines
});

// src/main.ts:2597-2606 slice of src/main.ts#growZones (shared): the feature's statements; "…" marks code left out
function growZones(): void {
  // … 4 lines
  if (mover.dragging || (now >= zoneGrowthHold.at && now < zoneGrowthHold.until)) return;
  if (growth.busy) return;
// … 2 lines
}

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 24 lines
  let holdSim = mover.dragging || topologyAfterDraw || !worldShown || simOpening || !effects.simulation;
  // … 8 lines
  sim.frame(wall, holdSim, { traffic: !game.paused && effects.traffic, pedestrians: !game.paused && effects.pedestrians, maxShare: maxPopulationShare * effects.populationShare, startShare: startPopulationShare, opening: simOpening });
  const alpha = holdSim ? 1 : sim.alpha;
  // … 9 lines
  if (net.revision !== doc.revision) {
    // Geometry is still refreshed during a drag
    if (!mover.dragging || mover.previewDue(now, scene.stats.rebuildMs)) {
      // DO NOT rebuild it here, whole and on the page
      net.rebuild();
    }
  }
  // … 5 lines
  scene.setPolePreview(net, framePolePlan && !framePolePlan.refused && framePolePlan.poles.length >= 2
    ? { poles: framePolePlan.poles.map((pole) => ({ x: pole.at.x, y: pole.at.y, lamp: pole.lamp, standing: pole.existing !== null })) }
    : null);
  // … 1 lines
  scene.draw(net, sim, alpha, wall, { holdRoads: terrainBrush.stroking });
  // … 7 lines
  if (topologyAfterDraw) {
    topologyAfterDraw = false;
    requestDraw();
  }
  // … 25 lines
  if (!pageHidden() && (measure.on || !effects.onDemand || simMoving || roadTool.draft || mover.dragging || cameraHand.active || cameraHand.coasting || cameraMotion.moving || scene.busy())) requestDraw();
  // Only the clouds moving (they drift, form and fade), or the clock alone: a slower frame.
  else if (!pageHidden() && (!game.paused || scene.drifting())) frameClock.drift();
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
  // … 21 lines
  if (game.tool === 'roundabout' && hoverAnchor) {
    ctx.save();
    // Red where the click would be refused (`roundaboutRefusal`), the reason said as the hint.
    ctx.strokeStyle = roundaboutRefusal(doc, net, hoverAnchor.at, game.roundaboutRadius) ? INVALID : HOVER;
    ctx.lineWidth = 2;
    ctx.setLineDash([6, 4]);
    ctx.beginPath();
    for (let i = 0; i <= 64; i++) {
      const angle = i * Math.PI / 32;
      const p = at({ x: hoverAnchor.at.x + Math.cos(angle) * game.roundaboutRadius,
        y: hoverAnchor.at.y + Math.sin(angle) * game.roundaboutRadius });
      if (i === 0) ctx.moveTo(p.x, p.y); else ctx.lineTo(p.x, p.y);
    }
    ctx.stroke();
    ctx.restore();
  }
  // … 2 lines
  if (game.congestionOverlay) {
    // … 2 lines
    drawCongestion(ctx, net, sim.vehicles.values(), [Math.min(...xs) - 50, Math.min(...ys) - 50, Math.max(...xs) + 50, Math.max(...ys) + 50], at);
  }
  // … 4 lines
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
  // … 36 lines
  if (bulldozeBox) {
    // … 4 lines
    corners.forEach((p, i) => {
      // … 1 lines
      const n = Math.max(1, Math.ceil(Math.hypot(q.x - p.x, q.y - p.y) / GRID_CELL));
      for (let k = 0; k < n; k++) {
        const on = { x: p.x + (q.x - p.x) * k / n, y: p.y + (q.y - p.y) * k / n };
        const s = view.toScreen(on, w, h, sceneHeightAt(on));
        if (i === 0 && k === 0) ctx.moveTo(s.x, s.y); else ctx.lineTo(s.x, s.y);
      }
    });
  // … 8 lines
  }
  // … 47 lines
  if (infoTool && hoverAnchor?.kind === 'segment' && hoverAnchor.segment !== undefined && hoverAnchor.segment !== game.selectedSegment) {
    roadBand(hoverAnchor.segment, 'rgba(255, 255, 255, 0.14)');
  }
  // … 7 lines
  if (game.tool === 'road' && !roadTool.draft && hoverAnchor?.kind === 'node') {
    ring(hoverAnchor.at, 9, HOVER, 2);
  }
  // … 2 lines
  if (game.tool === 'terrain' && hoverAnchor) {
    const brush = terrainBrush.at ?? hoverAnchor.at;
    const centre = at(brush);
    const xEdge = at({ x: brush.x + game.terrainRadius, y: brush.y });
    const yEdge = at({ x: brush.x, y: brush.y + game.terrainRadius });
    const rx = Math.max(4, Math.hypot(xEdge.x - centre.x, xEdge.y - centre.y));
    const ry = Math.max(4, Math.hypot(yEdge.x - centre.x, yEdge.y - centre.y));
    const colour = TERRAIN_BRUSH_COLOUR[game.terrainMode];
    ctx.save();
    ctx.strokeStyle = colour;
    ctx.fillStyle = TERRAIN_BRUSH_FILL[game.terrainMode];
    ctx.lineWidth = 2;
    ctx.setLineDash([6, 5]);
    ctx.beginPath();
    ctx.ellipse(centre.x, centre.y, rx, ry, 0, 0, Math.PI * 2);
    ctx.fill();
    ctx.stroke();

    // Inner ring: the half-strength contour, scaled by the strength setting so a heavier brush visibly bites deeper.
    const strengthMax = TERRAIN_STRENGTH_RANGE[1];
    const bite = 0.3 + 0.35 * (game.terrainStrength / strengthMax);
    ctx.setLineDash([]);
    ctx.globalAlpha = 0.65;
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.ellipse(centre.x, centre.y, rx * bite, ry * bite, 0, 0, Math.PI * 2);
    ctx.stroke();

    ctx.globalAlpha = 1;
    ctx.beginPath();
    ctx.moveTo(centre.x - 5, centre.y);
    ctx.lineTo(centre.x + 5, centre.y);
    ctx.moveTo(centre.x, centre.y - 5);
    ctx.lineTo(centre.x, centre.y + 5);
    ctx.stroke();

    const height = terrainBrush.level ?? sceneHeightAt(brush);
    const label = game.terrainMode === 'flatten'
      ? `${t('terrain.level')} ${formatDecimal(height, 1)}`
      : formatDecimal(height, 1);
    ctx.font = '600 11px system-ui, sans-serif';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'bottom';
    const width = ctx.measureText(label).width + 12;
    ctx.fillStyle = 'rgba(12,18,16,0.76)';
    ctx.beginPath();
    ctx.roundRect(centre.x - width / 2, centre.y - ry - 24, width, 17, 8);
    ctx.fill();
    ctx.fillStyle = colour;
    ctx.fillText(label, centre.x, centre.y - ry - 10);
    ctx.restore();
  }
  // … 2 lines
  if (game.tool === 'control') {
    // … 4 lines
    for (const id of net.junctionCells.idsMeeting(groundOnScreen(w, h))) {
      const node = doc.node(id);
      if (!node || node.incident.length < 3) continue;
      const centre = at({ x: node.x, y: node.y });
      const colour = CONTROL_COLOUR[node.control] ?? HOVER;
      const hot = hoverAnchor?.kind === 'node' && hoverAnchor.node === node.id;
      ctx.beginPath();
      ctx.arc(centre.x, centre.y, hot ? 13 : 10, 0, Math.PI * 2);
      ctx.fillStyle = 'rgba(12,18,16,0.72)';
      ctx.fill();
      ctx.strokeStyle = colour;
      ctx.lineWidth = hot ? 3 : 2;
      ctx.stroke();
      ctx.fillStyle = colour;
      ctx.fillText(CONTROL_GLYPH[node.control] ?? '?', centre.x, centre.y + 0.5);
    }
  }
  // … 3 lines
  const shownRoad = roadTool.preview(!scene.worldBusy && !topologyAfterDraw);
  const settling = shownRoad?.settling ?? false;
  const roadPreview: RoadDraft | null = shownRoad?.road ?? null;
  if (roadPreview) {
    // The profile the road will be laid with: its lanes and its parking.
    const chosenWidth = roadWidth();
    const plainRt = roadProfile(game.roadTypeIndex, game.roadLanePreset);
    // A road picked from the catalogue (or customised) is previewed as it will be laid.
    const picked = drawProfile();
    const rt = picked ? profileRoad(picked.profile, picked.type) : roadProfile(game.roadTypeIndex, game.roadLanePreset,
      roadType(game.roadTypeIndex).lanes === 1 ? 'aToB' : 'both',
      chosenWidth === null ? undefined : sectionForWidth(plainRt, chosenWidth, Math.round(plainRt.speedLimit * 3.6 * METERS_PER_UNIT)),
      roadParking());
    const pieces = roadTool.pieces(roadPreview);
    const groundUnder = (p: Vec2): number => scene.terrainHeightAt(p.x, p.y);
    const points: Vec2[] = [];
    const projected: Vec2[] = [];
    const groundProjected: Vec2[] = [];
    const offsets: number[] = [];
    let previewHeight = roadPreview.startHeightOffset;
    let limited = false;
    for (const [pieceIndex, piece] of pieces.entries()) {
      // Sampled every couple of metres, so the preview lies on the ground as the road will
      const flattened = densify(flattenSegment(piece.start.at, piece.end.at, piece.curve), m(2));
      const pieceLength = flattened.reduce((sum, p, i) =>
        i === 0 ? 0 : sum + dist(p, flattened[i - 1] as Vec2), 0);
      const reach = pieceLength * MAX_AUTHORED_GRADE;
      const nextHeight = clamp(piece.end.heightOffset,
        previewHeight - reach, previewHeight + reach);
      limited ||= Math.abs(nextHeight - piece.end.heightOffset) > 1e-6;
      for (let i = pieceIndex === 0 ? 0 : 1; i < flattened.length; i++) {
        const p = flattened[i] as Vec2;
        const t = i / Math.max(1, flattened.length - 1);
        const eased = t * t * (3 - 2 * t);
        const offset = previewHeight + (nextHeight - previewHeight) * eased;
        const ground = groundUnder(p);
        points.push(p);
        offsets.push(offset);
        groundProjected.push(view.toScreen(p, w, h, ground));
        projected.push(view.toScreen(p, w, h, ground + offset));
      }
      previewHeight = nextHeight;
    }
    // Once the draft is judged, the road as it will be built
    const exact = settling ? null : roadTool.stations();
    const modes: BuildMode[] = [];
    if (exact && exact.length > 1) {
      points.length = projected.length = groundProjected.length = offsets.length = 0;
      for (const st of exact) {
        const p = { x: st.x, y: st.y };
        points.push(p);
        offsets.push(st.deck - groundUnder(p));
        projected.push(view.toScreen(p, w, h, st.deck));
        groundProjected.push(view.toScreen(p, w, h, st.ground));
        modes.push(st.mode);
      }
    }
    const pathLength = points.reduce((sum, point, i) =>
      i === 0 ? 0 : sum + Math.hypot(point.x - (points[i - 1] as Vec2).x, point.y - (points[i - 1] as Vec2).y), 0);
    // A draft the editing rules would refuse, named before it is let go (`RoadTool.verdict`, judged once the pointer rests).
    const refusal = settling ? null : roadTool.verdict();
    const ok = pathLength >= MIN_LINK_LENGTH * 0.25 &&
      !(limited && roadPreview.snap.guide === 'network') && refusal === null;
    // Use the length of both projected world axes.
    const here = points[0] ?? { x: 0, y: 0 };
    const origin = at(here);
    const xAxis = at({ x: here.x + 100, y: here.y });
    const yAxis = at({ x: here.x, y: here.y + 100 });
    const pixelsPerUnit = (
      Math.hypot(xAxis.x - origin.x, xAxis.y - origin.y) +
      Math.hypot(yAxis.x - origin.x, yAxis.y - origin.y)
    ) / 200;
    const asphaltWidth = Math.max(3, rt.width * pixelsPerUnit);
    const kerbWidth = Math.max(asphaltWidth + 2, (rt.width + 1.8) * pixelsPerUnit);
    const footwayWidth = Math.max(kerbWidth + 2, (rt.width + 1.8 + rt.sidewalk * 2) * pixelsPerUnit);
    const casingWidth = Math.max(footwayWidth + 2, footwayWidth + 3 * pixelsPerUnit);

    // The editor used to paint one translucent class-colour stroke here.
    ctx.save();
    if (modes.length > 1) drawBuildModes(ctx, projected, groundProjected, modes);
    else if (points.length > 1 && offsets.some((offset) => Math.abs(offset) > UNITS_PER_METER * 0.5)) {
      strokeScreen(points, 'rgba(6, 19, 21, 0.52)', casingWidth + 5, [7, 7], groundProjected);
      ctx.beginPath();
      ctx.moveTo(projected[0]!.x, projected[0]!.y);
      for (const p of projected.slice(1)) ctx.lineTo(p.x, p.y);
      for (const p of [...groundProjected].reverse()) ctx.lineTo(p.x, p.y);
      ctx.closePath();
      ctx.fillStyle = previewHeight >= 0 ? 'rgba(101, 229, 195, 0.16)' : 'rgba(244, 184, 103, 0.20)';
      ctx.fill();
    }
    // Each band of the road as it will be laid
    const band = (half: number, fill: string | CanvasPattern): void => {
      if (points.length < 2) return;
      const left: Vec2[] = [], right: Vec2[] = [];
      for (let i = 0; i < points.length; i++) {
        const a = points[Math.max(0, i - 1)]!, b = points[Math.min(points.length - 1, i + 1)]!;
        const len = Math.hypot(b.x - a.x, b.y - a.y) || 1;
        const nx = -(b.y - a.y) / len, ny = (b.x - a.x) / len;
        const p = points[i]!, z = (projected[i] && groundProjected[i]) ? offsets[i]! : 0;
        const l = { x: p.x + nx * half, y: p.y + ny * half }, r = { x: p.x - nx * half, y: p.y - ny * half };
        left.push(view.toScreen(l, w, h, groundUnder(l) + z));
        right.push(view.toScreen(r, w, h, groundUnder(r) + z));
      }
      ctx.beginPath();
      left.forEach((q, i) => (i === 0 ? ctx.moveTo(q.x, q.y) : ctx.lineTo(q.x, q.y)));
      for (let i = right.length - 1; i >= 0; i--) ctx.lineTo(right[i]!.x, right[i]!.y);
      ctx.closePath();
      ctx.fillStyle = fill;
      ctx.fill();
    };
    const asphaltHalf = rt.width / 2, kerbHalf = (rt.width + 1.8) / 2;
    const footwayHalf = (rt.width + 1.8 + rt.sidewalk * 2) / 2, casingHalf = footwayHalf + 1.5;
    if (!settling) band(casingHalf + 2, ok ? SELECTION : INVALID);
    band(casingHalf, '#536b47');
    band(footwayHalf, '#a7a498');
    band(kerbHalf, '#87877f');
    band(asphaltHalf, asphaltPreviewPattern(ctx));
    // A tunnel's stretch is under the land: the road drawn through it, dimmed and dashed.
    if (modes.includes('tunnel')) drawTunnelStretches(ctx, projected, modes, asphaltWidth);
    if (rt.markings !== 'none') {
      const dash = [Math.max(4, 10 * pixelsPerUnit), Math.max(3, 8 * pixelsPerUnit)];
      strokeScreen(points, rt.line, Math.max(1, 1.1 * pixelsPerUnit), dash, projected);
    }
    ctx.restore();
    if (points.length && !settling) {
      ring(roadPreview.start.at, 7, ok ? SELECTION : INVALID, 2, projected[0]);
      ring(roadPreview.snap.at, 7, ok ? SELECTION : INVALID, 2, projected[projected.length - 1]);
      const end = projected[projected.length - 1]!;
      const groundEnd = groundProjected[groundProjected.length - 1]!;
      // The road's length beside the pointer, in steps of 10 m, as SimCity shows it.
      {
        const tens = Math.round((pathLength * METERS_PER_UNIT) / 10) * 10;
        // With the reason a refused draft would be refused, beside its length.
        const cost = settling ? null : roadTool.cost();
        const built = exact && exact.length > 1 ? buildSummary(exact) : '';
        const priced = (cost === null ? `${tens} m` : `${tens} m · ${formatCost(cost)}`) + (built ? ` · ${built}` : '');
        const text = refusal ? `${priced} · ${t(`rule.short.${refusal}`)}` : priced;
        ctx.save();
        ctx.font = '700 13px system-ui, sans-serif';
        const tw = ctx.measureText(text).width;
        const bx = clamp(end.x + 16, 8, Math.max(8, w - tw - 28)), by = clamp(end.y - 40, 60, h - 40);
        ctx.fillStyle = 'rgba(9, 27, 28, 0.92)';
        ctx.beginPath();
        ctx.roundRect(bx, by, tw + 18, 24, 6);
        ctx.fill();
        ctx.fillStyle = ok ? '#e7fff7' : '#ffb4ab';
        ctx.textBaseline = 'middle';
        ctx.fillText(text, bx + 9, by + 12.5);
        ctx.restore();
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

// src/main.ts:3104-3111 slice of src/main.ts#drawOverlayScreen.roadBand (shared): the feature's statements; "…" marks code left out
  const roadBand = (id: SegmentId, colour: string): void => {
    const ribbon = net.ribbons.get(id);
    if (!ribbon) return;
    const mid = ribbon.full.sampleAt(ribbon.full.length / 2);
    const a = at(mid.p);
    const b = at({ x: mid.p.x + mid.n.x * ribbon.road.width / 2, y: mid.p.y + mid.n.y * ribbon.road.width / 2 });
    strokeScreen(ribbon.full.toPoints(), colour, Math.max(3, 2 * Math.hypot(b.x - a.x, b.y - a.y)));
  };

// src/main.ts:3413-3419 slice of src/main.ts#BUILD_FILL (shared): the feature's statements; "…" marks code left out
const BUILD_FILL: Readonly<Record<BuildMode, string | null>> = {
  ground: null,
  embankment: 'rgba(164, 120, 74, 0.62)',
  cutting: 'rgba(132, 128, 120, 0.62)',
  bridge: null,
  tunnel: 'rgba(40, 34, 30, 0.55)',
};

// src/main.ts:3421-3459 slice of src/main.ts#drawBuildModes (shared): the feature's statements; "…" marks code left out
function drawBuildModes(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], ground: readonly Vec2[], modes: readonly BuildMode[]): void {
  // … 1 lines
  while (i < modes.length) {
    const mode = modes[i]!;
    let j = i;
    while (j + 1 < modes.length && modes[j + 1] === mode) j++;
    const a = Math.max(0, i - 1), b = Math.min(modes.length - 1, j + 1);
    const fill = BUILD_FILL[mode];
    if (fill) {
      ctx.beginPath();
      ctx.moveTo(deck[a]!.x, deck[a]!.y);
      for (let k = a + 1; k <= b; k++) ctx.lineTo(deck[k]!.x, deck[k]!.y);
      for (let k = b; k >= a; k--) ctx.lineTo(ground[k]!.x, ground[k]!.y);
      ctx.closePath();
      ctx.fillStyle = fill;
      ctx.fill();
      if (mode === 'cutting') {
        // The held side of the cut: its top line on the ground.
        ctx.strokeStyle = 'rgba(214, 208, 196, 0.9)';
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(ground[a]!.x, ground[a]!.y);
        for (let k = a + 1; k <= b; k++) ctx.lineTo(ground[k]!.x, ground[k]!.y);
        ctx.stroke();
      }
    }
    if (mode === 'bridge') {
      ctx.strokeStyle = 'rgba(206, 200, 188, 0.95)';
      ctx.lineWidth = 3;
      ctx.beginPath();
      for (let k = i; k <= j; k += PIER_EVERY) {
        ctx.moveTo(deck[k]!.x, deck[k]!.y);
        ctx.lineTo(ground[k]!.x, ground[k]!.y);
      }
      ctx.stroke();
    }
    i = j + 1;
  }
}

// src/main.ts:3461-3475 slice of src/main.ts#drawTunnelStretches (shared): the feature's statements; "…" marks code left out
function drawTunnelStretches(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], modes: readonly BuildMode[], width: number): void {
  // … 5 lines
  for (let k = 0; k < modes.length; k++) {
    if (modes[k] !== 'tunnel') continue;
    const start = modes[k - 1] !== 'tunnel';
    if (start) ctx.moveTo(deck[k]!.x, deck[k]!.y);
    else ctx.lineTo(deck[k]!.x, deck[k]!.y);
  }
// … 2 lines
}

// src/main.ts:3478-3486 slice of src/main.ts#buildSummary (shared): the feature's statements; "…" marks code left out
function buildSummary(stations: Parameters<typeof buildRuns>[0]): string {
  const totals = new Map<BuildMode, number>();
  for (const run of buildRuns(stations)) totals.set(run.mode, (totals.get(run.mode) ?? 0) + run.to - run.from);
  return [...totals]
    .filter(([mode, length]) => mode !== 'ground' && length * METERS_PER_UNIT >= 5)
    .sort((x, y) => y[1] - x[1])
    .map(([mode, length]) => `${t(mode === 'cutting' && cutWallsChosen() ? 'buildMode.cuttingWalls' : `buildMode.${mode}`)} ${Math.max(10, Math.round((length * METERS_PER_UNIT) / 10) * 10)} m`)
    .join(' · ');
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

// src/main.ts:3821-3832 slice of src/main.ts#duplicateSelectedSegment (shared): the feature's statements; "…" marks code left out
function duplicateSelectedSegment(id = game.selectedSegment): void {
  // … 2 lines
  if (!mutateRoads((doc, net) => {
    copy = duplicateSegment(doc, net, id);
    return copy !== null;
  })) copy = null;
  if (copy !== null) {
    select(copy, game.selectedSegmentS, null, 'road duplicated');
    showInspector();
  }
}

// src/main.ts:3837-3856 slice of src/main.ts#currentStatus (shared): the feature's statements; "…" marks code left out
function currentStatus(): ShellStatus {
  // … 5 lines
  return {
    // … 9 lines
    impossible: net.impossible.size,
    balance: doc.economy.balance,
  };
}

// src/main.ts:3933-3941 slice of src/main.ts#setMarkingStyle (shared): the feature's statements; "…" marks code left out
function setMarkingStyle(value: string): void {
  if (!isMarkingStyle(value) || value === doc.markingStyle) return;
// … 6 lines
}

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 15 lines
  net,
  // … 23 lines
  setRoadHeight: (metres: number) => {
    roadTool.heightChosen(metres * UNITS_PER_METER);
  // … 1 lines
  },
  // … 5 lines
  growTimed: (): Promise<{ grown: readonly number[]; ms: number } | { grown: null; why: string }> => {
    if (growth.busy) return Promise.resolve({ grown: null, why: 'a building is being made already' });
  // … 3 lines
  },
  // … 117 lines
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
// … 7 lines
};

// src/main.ts:4259-4274 slice of src/main.ts#actions (shared): the feature's statements; "…" marks code left out
const actions = createActions({
  // … 1 lines
  net,
// … 12 lines
});
