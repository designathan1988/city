// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:502-513 slice of src/main.ts#streetscapeTool (shared): the feature's statements; "…" marks code left out
const streetscapeTool = new StreetscapeTool({
  // … 4 lines
  extra: (kind) => kind === 'sign' ? { signType: signChoice.type, text: signChoice.text }
    : kind === 'streetname' ? { text: signChoice.streetName }
    : kind === 'tree' || kind === 'shrub' ? { planted: sim.minutes } : {},
// … 3 lines
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

// src/main.ts:876-911 slice of src/main.ts#mutateBuilt (shared): the feature's statements; "…" marks code left out
function mutateBuilt(fn: () => boolean): boolean {
  // … 9 lines
  if (changed) {
    // … 3 lines
    buildings.afterRoadEdit(roadsBefore);
  // … 5 lines
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
  // … 9 lines
  if (game.tool === 'building') buildings.pointerUp(true);
// … 1 lines
}

// src/main.ts:1105-1114 slice of src/main.ts#bulldozer (shared): the feature's statements; "…" marks code left out
const bulldozer = new Bulldozer({
  // … 2 lines
  bulldozeBuildingAt: (at) => buildings.bulldozeAt(at),
// … 5 lines
});

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
    // … 8 lines
    case 'building':
      buildings.pointerDown({ x: e.clientX - r.left, y: e.clientY - r.top }, world, e.shiftKey);
      break;
    // … 49 lines
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
      // … 2 lines
      break;
    // … 22 lines
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
  // … 25 lines
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
// … 16 lines
}

// src/main.ts:1558-1566 slice of src/main.ts#roadClasses (shared): the feature's statements; "…" marks code left out
function roadClasses(): ShellRoadClass[] {
// … 7 lines
}

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

// src/main.ts:1675-1676 slice of src/main.ts#weatherSlider (shared): the feature's statements; "…" marks code left out
const weatherSlider = (key: keyof typeof doc.weather, min: number, max: number, step: number, scale: number): ShellSlider =>
  mapSlider(min, max, step, () => Math.round(doc.weather[key] * scale * 10) / 10, (v) => doc.setWeather({ [key]: v / scale }));

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
// … 16 lines
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
  // … 50 lines
  pickRoadClass: (index) => {
    // … 2 lines
    shell?.toolKeyed('road');
  },
  // … 6 lines
  toolKey: (next) => {
    // … 1 lines
    shell?.toolKeyed(next);
  },
// … 8 lines
});

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 26 lines
  if (!actionsWarm && game.tool === 'bulldoze' && strikeChoice.mode !== 'demolish') {
    actionsWarm = true;
    void scene.effects();
  }
  // … 22 lines
  buildings.beforeDraw(game.tool === 'building');
// … 47 lines
}

// src/main.ts:2942-3410 slice of src/main.ts#drawOverlayScreen (shared): the feature's statements; "…" marks code left out
function drawOverlayScreen(): void {
  // … 48 lines
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
  // … 33 lines
  if (game.tool === 'building') buildings.drawOverlay(ctx);
// … 369 lines
}

// src/main.ts:3573-3818 slice of src/main.ts#showInspector (shared): the feature's statements; "…" marks code left out
function showInspector(): void {
  openInspector(
    // … 4 lines
    {
      // … 61 lines
      onDrawWithProfile: (held) => {
        // … 4 lines
        shell?.openTool('road');
      // … 1 lines
      },
    // … 167 lines
    },
  );
}

// src/main.ts:3835-3835 slice of src/main.ts#statusListeners (shared): the feature's statements; "…" marks code left out
const statusListeners: ((status: ShellStatus) => void)[] = [];

// src/main.ts:3837-3856 slice of src/main.ts#currentStatus (shared): the feature's statements; "…" marks code left out
function currentStatus(): ShellStatus {
// … 18 lines
}

// src/main.ts:3923-3930 slice of src/main.ts#<ExpressionStatement>@3923 (shared): the feature's statements; "…" marks code left out
onLanguageChange(() => {
  buildings.languageChanged();
// … 5 lines
});

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 193 lines
  buildings: buildings.tool,
};

// src/main.ts:4259-4274 slice of src/main.ts#actions (shared): the feature's statements; "…" marks code left out
const actions = createActions({
  // … 4 lines
  buildingAt: (screen) => buildings.tool.buildingAt(screen),
  // … 3 lines
  strength: () => strikeChoice.strength,
// … 5 lines
});
