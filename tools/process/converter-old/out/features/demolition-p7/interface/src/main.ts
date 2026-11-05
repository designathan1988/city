// What the feature uses of src/main.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/main.ts:156-156 type Alignment
type Alignment = 'straight' | 'curve' | 'free';

// src/main.ts:168-168 variable canvas
const canvas = document.getElementById('game') as HTMLCanvasElement;

// src/main.ts:171-171 variable health
/** The game's health (`core/health.ts`): what broke and what was slow, with what the player was doing. */
const health = new HealthLog();

// src/main.ts:189-189 variable doc
const doc = new RoadDoc();

// src/main.ts:196-196 variable net
const net = new Network(doc);

// src/main.ts:197-197 variable camera
const camera = new Camera();

// src/main.ts:199-199 variable booted
/** Whether the whole module has run. */
let booted = false;

// src/main.ts:200-200 variable surface
const surface = new CanvasSurface(canvas, () => { if (booted) requestDraw(); });

// src/main.ts:201-201 variable history
const history = new History();

// src/main.ts:204-204 variable desktopStores
const desktopStores = desktopSaves();

// src/main.ts:205-205 variable persistence
const persistence = desktopStores ? new Persistence(undefined, desktopStores.store, desktopStores.parts) : new Persistence();

// src/main.ts:233-233 variable streamedAtBoot
const streamedAtBoot: string | null = null;

// src/main.ts:241-241 variable bootSave
/** The autosave read and restored into the document; what the boot reads on - the save's settings, whether the player had a map, whether it failed to load.
 *  DO NOT keep the loaded save in the module's scope: it held the save's whole map (every building decoded, ~8 KB of heap each) for the whole session,
 *  beside the table the document reads (`npm run diagnose`, 2026-10-22). Restored here, it is let go with this call. */
const bootSave = restoreBootSave(streamedAtBoot ? null : await persistence.loadSessionAsync());

// src/main.ts:243-243 variable savedSession
/** The boot's save without its map: its settings, read on through the boot. */
const savedSession = bootSave.settings && { settings: bootSave.settings };

// src/main.ts:248-276 function restoreBootSave
function restoreBootSave(loaded: Awaited<ReturnType<Persistence['loadSessionAsync']>>): { readonly settings: SavedSettings | null; readonly hadMap: boolean; readonly failed: boolean };

// src/main.ts:295-295 variable sim
/** The simulation, in a worker of its own (`sim/host/simHost.ts`): the page sends it the map's changes and reads its frames. */
const sim = new SimClient(doc, net);

// src/main.ts:374-374 type BrushMode
/** The terrain brush: a land stamp (`TerrainMode`), or painting the ground (`terrainPaint.ts`). */
type BrushMode = TerrainMode | 'paint' | 'fog' | 'cloud' | 'elements' | 'gully' | 'trees' | 'weather' | Landform;

// src/main.ts:376-376 type Landform
/** The landforms: a shape and its ROCK in one tool */
type Landform = 'mesa' | 'canyon' | 'escarpment' | 'sugarloaf';

// src/main.ts:384-384 variable hardnessByMode
const hardnessByMode: Partial<Record<BrushMode, number>> = { raise: 0, lower: 0, mesa: 85, canyon: 80, escarpment: 75 };

// src/main.ts:393-430 variable gameState
/** The game's state - the tool in hand, the pause and speed, the quality, the selection */
const gameState = new GameState({
  tool: 'inspect' as Tool,
  paused: sim.clock.paused,
  speed: sim.clock.speed,
  quality: 'high' as string,
  selectedSegment: null as SegmentId | null,
  selectedSegmentS: null as number | null,
  selectedNode: null as NodeId | null,
  // The road tool: class, plan, height above the ground, lanes.
  roadTypeIndex: 1,
  alignment: 'straight' as Alignment,
  roadHeightOffset: 0,
  roadLanePreset: null as number | null,
  roundaboutRadius: 40,
  // The terrain brush.
  terrainMode: 'raise' as BrushMode,
  terrainRadius: 80,
  terrainStrength: 24,
  terrainHardness: 0,
  // Walls, zones and lots.
  barrierKind: 'fence' as BarrierKind,
  zoneUse: 'residential' as ZoneUse,
  zoneDensity: 'low' as ZoneDensity,
  zoneEraser: false,
  zoneMode: 'brush' as ZoneMode,
  lotSplitKind: 'vertical' as LotSplitKind,
  lotSplitParts: 2,
  // The view.
  congestionOverlay: savedSession?.settings.congestionOverlay ?? false,
  perspective: false,
  /** What the player is in the middle of doing (`currentGesture`), null between gestures. */
  gesture: null as string | null,
  // The history, the sky and the road inspector, as the interface shows them.
  canUndo: false,
  canRedo: false,
  sky: 'day' as SkyMode,
  inspectorOpen: false,
}, () => { if (booted) requestDraw(); });

// src/main.ts:432-432 variable game
/** The game's state, read-only: `game.tool`, `game.paused`, `game.selectedSegment`... */
const game = gameState.values;

// src/main.ts:434-437 variable tools
/** The tool in hand (`editor/toolMachine.ts`): the only way it changes. */
const tools = createToolMachine(game.tool, {
  take: takeTool,
  letGo: () => { if (game.selectedSegment !== null || game.selectedNode !== null) closeInspectorPanel(); },
});

// src/main.ts:439-439 variable gullyStrength
/** The gully brush's strength, % (Paisagem > Relevo > Ravinas). */
let gullyStrength = 60;

// src/main.ts:441-441 variable cloudScatter
/** How many clouds "spread over the sky" lays, and how much they vary, %. */
const cloudScatter = { count: 8, variation: 40 };

// src/main.ts:443-455 function sessionSettings
function sessionSettings(): SavedSettings;

// src/main.ts:458-486 variable roadTool
/** The road tool (`editor/roadTool.ts`): the stroke, the chain, the curve waiting for its bend, the road settling. */
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

// src/main.ts:488-498 variable poleTool
/** The pole tool (`editor/poles.ts`): the stretch being dragged and the line's last pole. */
const poleTool = new PoleTool({
  doc,
  net,
  zoom: () => view.zoom,
  mode: () => poleToolMode(),
  lamps: () => poleLampMode(),
  inHand: () => game.tool === 'pole',
  mutate: (fn) => mutateBuilt(fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:502-513 variable streetscapeTool
/** The landscaping tool (`editor/streetscapeTool.ts`): items on the footways. */
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

// src/main.ts:515-528 variable barrierTool
/** The walls tool (`editor/barriers.ts`): the wall, fence or hedge being traced. */
const barrierTool = new BarrierTool({
  net: () => net,
  kind: () => game.barrierKind,
  removeAt: (at) => {
    // The runs round the point only (`RoadDoc.barrierNear`), not every wall of the map.
    const hit = doc.barrierNear(at, BARRIER_PICK_PIXELS / view.zoom);
    if (!hit) return false;
    mutate(() => doc.removeBarrier(hit.id));
    return true;
  },
  build: (kind, path) => mutate(() => doc.addBarrier(kind, path) !== null),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:530-543 variable lotTool
/** The Zoning tool (`editor/lotTool.ts`): the lots the player draws, edits and zones, with its gestures' own state. */
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

// src/main.ts:544-544 variable hoverAnchor
let hoverAnchor: Anchor | null = null;

// src/main.ts:563-567 variable cameraMotion
/** The keys held and the wheel's notches, spent as a glide frame by frame (`view/cameraMotion.ts`). */
const cameraMotion = new CameraMotion({
  view: () => view,
  size: () => ({ w: surface.cssW, h: surface.cssH }),
  heightUnder: (at) => groundHeightUnder(at),
});

// src/main.ts:569-577 variable mover
/** The Move tool's node drag (`editor/nodeMover.ts`), previewed live and dropped as one undo step. */
const mover = new NodeMover({
  doc,
  net,
  // The worker's simulation catches its topology up with the map it is sent (`SimClient.frame`).
  settleTopology: () => {},
  mutate: (fn) => mutate(fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:579-589 variable terrainBrush
/** The terrain brush (`editor/terrainBrush.ts`): the stroke in progress and what each dab does. */
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

// src/main.ts:591-605 function dabSettings
/** The brush in hand, as one dab reads it (`editor/terrainBrush.ts` `DabSettings`). */
function dabSettings(): DabSettings;

// src/main.ts:608-608 variable view
let view: Viewport = flatViewport(camera);

// src/main.ts:617-638 function asphaltPreviewPattern
/** Asphalt grain used by the live 3D-editor preview. */
function asphaltPreviewPattern(ctx: CanvasRenderingContext2D): CanvasPattern | string;

// src/main.ts:640-640 variable canvas3d
const canvas3d = document.createElement('canvas');

// src/main.ts:647-647 variable fieldSession
/** The graphics options (`graphicsWiring.ts`): the tier the player chose, or the one this GPU holds at the first start, and their settings on it. */
/** The session's summary told to the field log as the page closes (`fieldReport.ts`): the GPU, the tiers, the frames' times, the heap's peak. */
const fieldSession = new SessionSummary(() => performance.now(), () => frameTimes, () => scene.gpu.label);

// src/main.ts:648-648 variable graphics
const graphics = createGraphicsWiring((preset) => gameState.set('quality', preset, 'quality menu'), (level, by) => fieldSession.level(level, by));

// src/main.ts:669-669 variable engine
/** The engine that brings every derived product up to date, its ledger the work each did (`core/engine.ts`). */
const engine = new Engine();

// src/main.ts:671-671 variable scene
const scene: SceneHandle = createSceneRenderer(canvas3d, { x: camera.x, y: camera.y }, camera.zoom, graphics.level, requestDraw, engine, graphics.settings);

// src/main.ts:760-806 variable buildings
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

// src/main.ts:823-826 function fitView
function fitView(): void;

// src/main.ts:837-839 function mutate
function mutate(fn: () => boolean): void;

// src/main.ts:854-860 variable transitEditor
/** The public transport tool (`editor/transitTools.ts`): each edit one undo step. */
const transitEditor = new TransitTool({
  doc: () => doc,
  net: () => net,
  edit: (next) => mutate(() => { doc.setTransit(next); return true; }),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:876-911 function mutateBuilt
function mutateBuilt(fn: () => boolean): boolean;

// src/main.ts:959-971 function worldAtScreen
/** The one conversion that differs between renderers. */
/** The world point under a screen position, ON THE SURFACE THAT IS THERE. */
function worldAtScreen(px: number, py: number, heightOffset?: number): Vec2;

// src/main.ts:974-985 function groundHeightUnder
/** The height of what is drawn under a screen point */
function groundHeightUnder(at: Vec2): number;

// src/main.ts:988-990 function sceneHeightAt
/** Whatever the player can see at a world point: a road deck, or the ground. */
function sceneHeightAt(p: Vec2): number;

// src/main.ts:1041-1053 function cancelGestures
/** Ends every gesture in progress without committing it */
function cancelGestures(): void;

// src/main.ts:1056-1073 function currentGesture
/** The gesture in progress, named by its dictionary key */
function currentGesture(): string | null;

// src/main.ts:1094-1102 function caused
/** Runs `fn` with the diary's cause set to `cause`, then puts the one before back. */
function caused<T>(cause: string, fn: () => T): T;

// src/main.ts:1105-1114 variable bulldozer
/** The bulldozer (`editor/bulldozer.ts`): a click removes what is under it, a box everything inside. */
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

// src/main.ts:1117-1119 function gestureInProgress
/** Whether anything is being drawn or dragged right now. */
function gestureInProgress(): boolean;

// src/main.ts:1136-1136 variable cloudStepOpen
/** A step the clouds' tool opened and its gesture has not closed yet. */
let cloudStepOpen = false;

// src/main.ts:1137-1150 variable cloudTool
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

// src/main.ts:1217-1396 function pointerDown
function pointerDown(e: PointerEvent, r: DOMRect): void;

// src/main.ts:1404-1475 function pointerMove
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void;

// src/main.ts:1480-1482 function poleReach
/** Pick radius for a pole, in world units at the current zoom (`PoleTool.reach`: one definition). */
function poleReach(): number;

// src/main.ts:1484-1513 function endPointer
function endPointer(e: PointerEvent): void;

// src/main.ts:1596-1602 function selectRoadType
function selectRoadType(i: number): void;

// src/main.ts:1604-1609 function setAlignment
function setAlignment(next: Alignment): void;

// src/main.ts:1614-1614 variable ROUNDABOUT_RADIUS_M
/** The roundabout's radius, in whole metres. */
const ROUNDABOUT_RADIUS_M = [32, 128] as const;

// src/main.ts:1634-1640 function setTerrainMode
function setTerrainMode(next: BrushMode): void;

// src/main.ts:1643-1643 variable TERRAIN_RADIUS_RANGE
/** One writer for each brush number */
const TERRAIN_RADIUS_RANGE = [30, 300] as const;

// src/main.ts:1644-1644 variable TERRAIN_STRENGTH_RANGE
const TERRAIN_STRENGTH_RANGE = [1, 160] as const;

// src/main.ts:1645-1648 function setTerrainRadius
function setTerrainRadius(value: number): void;

// src/main.ts:1649-1652 function setTerrainStrength
function setTerrainStrength(value: number): void;

// src/main.ts:1655-1668 function mapSlider
/** A slider of the map's own settings (the fog over the map, the weather, the gullies of the land) */
function mapSlider(min: number, max: number, step: number, get: () => number, set: (v: number) => void): ShellSlider;

// src/main.ts:1670-1671 function brushSlider
/** A slider of a brush, kept between sessions (`ui/toolChoices.ts`): not the map's, no undo. */
const brushSlider = (min: number, max: number, step: number, get: () => number, set: (v: number) => void): ShellSlider => …;

// src/main.ts:1675-1676 function weatherSlider
const weatherSlider = (key: keyof typeof doc.weather, min: number, max: number, step: number, scale: number): ShellSlider => …;

// src/main.ts:1680-1680 variable atmosphere
const atmosphere = { atmoClouds: 40, atmoCloudBase: 180, atmoCloudThickness: 150, atmoFog: 0, atmoFogHeight: 60 };

// src/main.ts:1690-1701 function applyAtmosphere
function applyAtmosphere(redraw = true): void;

// src/main.ts:1715-1771 variable SLIDERS
/** Every slider the interface shows, by name: its range and how the game reads and takes it. */
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

// src/main.ts:1789-1797 function wholeMapEdit
/** One edit of the whole map, asked first and undoable: what each "clear" and "spread" button does. */
function wholeMapEdit(confirmKey: string | null, edit: () => void): void;

// src/main.ts:1798-1817 variable MAP_ACTIONS
const MAP_ACTIONS: Readonly<Record<string, () => void>> = {
  clearTerrain: () => wholeMapEdit('confirm.clearTerrain', () => doc.clearTerrain()),
  clearGullies: () => { if (doc.gullyDabs.length > 0) wholeMapEdit('confirm.clearGullies', () => doc.clearGullies()); },
  clearFog: () => { if (doc.fogDabs.length > 0) wholeMapEdit('confirm.clearFog', () => doc.clearFog()); },
  clearTrees: () => {
    if (doc.trees.length > 0 || doc.treeClearings.length > 0) wholeMapEdit('confirm.clearTrees', () => doc.clearTrees());
  },
  clearElements: () => { if (doc.elements.length > 0) wholeMapEdit('confirm.clearElements', () => doc.clearElements()); },
  clearClouds: () => {
    if (doc.clouds.length > 0) wholeMapEdit('confirm.clearClouds', () => { for (const cloud of [...doc.clouds]) doc.removeCloud(cloud.id); });
  },
  // Spread clouds over the sky: as many as asked, about the tool's size, height and density, varied
  scatterClouds: () => wholeMapEdit(null, () => {
    const brush = cloudBrush();
    const laid = scatterClouds(cloudScatter.count, {
      size: brush.size * UNITS_PER_METER, height: brush.height * UNITS_PER_METER, density: brush.density / 100,
    }, cloudScatter.variation / 100, CLOUD_SPAN / 2, doc.clouds, editRandom(doc, 0, 0));
    if (doc.addClouds(laid) < cloudScatter.count) flashHint('hint.cloud.full');
  }),
};

// src/main.ts:1820-1831 function takeTool
/** Puts a tool in hand: what it does to the game (gestures dropped, the selection let go, the Builder switched on or off). */
function takeTool(next: Tool): void;

// src/main.ts:1842-1844 function setTool
/** Takes a tool in hand (a panel's own choice, a road class key). */
function setTool(next: Tool): void;

// src/main.ts:1847-1849 function freeSelection
/** Puts everything down: no tool in hand, nothing picked, no panel. */
function freeSelection(): void;

// src/main.ts:1852-1854 function pickTool
/** A tool button or key: picks the tool, or puts it down when it is already in hand. */
function pickTool(next: Tool): void;

// src/main.ts:1857-1857 variable shell
/** The interface's handle: the road inspector's eyedropper opens the road tool through it. */
let shell: ShellHandle | null = null;

// src/main.ts:1964-1971 function setSpeed
function setSpeed(speed: number): void;

// src/main.ts:2294-2296 function openMap
async function openMap(): Promise<void>;

// src/main.ts:2335-2343 function undo
function undo(): void;

// src/main.ts:2344-2351 function redo
function redo(): void;

// src/main.ts:2366-2369 function updateHistoryButtons
/** Whether there is something to undo and to redo, in the game's state. */
function updateHistoryButtons(): void;

// src/main.ts:2395-2399 function closeInspectorPanel
/** The road inspector closed (its × , Escape, Delete): nothing picked. */
function closeInspectorPanel(): void;

// src/main.ts:2444-2448 function flashHint
/** A transient answer of the game - saved, refused, undone, a road that cannot be laid */
function flashHint(key: string, params?: Readonly<Record<string, string | number>>): void;

// src/main.ts:2461-2466 function updateHint
function updateHint(): void;

// src/main.ts:2477-2477 variable TERRAIN_KEY_MODES
/** The terrain brush's operations in the number row's order (1 = raise). */
const TERRAIN_KEY_MODES: readonly BrushMode[] = ['raise', 'lower', 'flatten', 'river', 'paint', 'mesa', 'canyon', 'escarpment', 'sugarloaf', 'fog', 'cloud', 'elements', 'gully', 'trees', 'weather'];

// src/main.ts:2479-2555 variable gameKeys
/** Every key the game answers (`editor/gameKeys.ts`): one command a key. */
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

// src/main.ts:2574-2574 variable lab
/** The debugging lab (F7, `lab/lab.ts`): the switches, what each system costs, the code that runs, recordings and tests. */
const lab = createLab({ scene, sim, engine, frameTimer, effects, requestDraw, setPreset: (level) => { if (isQualityLevel(level)) graphics.setPreset(level); } });

// src/main.ts:2584-2584 variable actionsWarm
/** The Actions' physics asked for (`frame`). */
let actionsWarm = false;

// src/main.ts:2589-2591 function requestDraw
function requestDraw(): void;

// src/main.ts:2596-2596 variable zoneGrowthHold
/** Nothing grows before this simulation time (rubble of a collapse lying, `strikeAt`); `at` the time it was set. */
let zoneGrowthHold = { at: 0, until: 0 };

// src/main.ts:2597-2606 function growZones
function growZones(): void;

// src/main.ts:2641-2641 variable frameTimes
/** Every frame's whole time, the last 40 s (`sampleRing.ts`): the CPU the process spends a frame, sampled while it runs (`__roadcraft.usage`). */
const frameTimes = new SampleRing(2400);

// src/main.ts:2644-2745 function frame
/** @perFrame One frame: input, the topology catching up, the simulation, the drawing, the panels. */
function frame(now: number): void;

// src/main.ts:2783-2783 variable BARRIER_PICK_PIXELS
/** Pick radius for a barrier under a shift-click, screen pixels. */
const BARRIER_PICK_PIXELS = 10;

// src/main.ts:2808-2847 function drawStreetscapeHover
/** Where the landscaping tool would put its item: a ring on the footway, red where it cannot go. */
function drawStreetscapeHover(ctx: CanvasRenderingContext2D, at: (p: Vec2) => Vec2): void;

// src/main.ts:2849-2898 function drawPolePlan
function drawPolePlan(
  plan: PoleRunPlan | null,
  ctx: CanvasRenderingContext2D,
  at: (p: Vec2) => Vec2,
): void;

// src/main.ts:2901-2901 variable showChanges
/** F8: the diary's last changes drawn where they happened (`world/changes.ts`). */
let showChanges = false;

// src/main.ts:2942-3410 function drawOverlayScreen
function drawOverlayScreen(): void;

// src/main.ts:3421-3459 function drawBuildModes
function drawBuildModes(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], ground: readonly Vec2[], modes: readonly BuildMode[]): void;

// src/main.ts:3461-3475 function drawTunnelStretches
function drawTunnelStretches(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], modes: readonly BuildMode[], width: number): void;

// src/main.ts:3546-3571 function setNodeHeightMetres
function setNodeHeightMetres(id: NodeId, metres: number): void;

// src/main.ts:3573-3818 function showInspector
function showInspector(): void;

// src/main.ts:3821-3832 function duplicateSelectedSegment
/** Duplicates the inspected road and keeps the copy selected for immediate editing. */
function duplicateSelectedSegment(id = game.selectedSegment): void;

// src/main.ts:3960-3966 function gpuLines
/** The GPU time of the frames drawn since the last `__gpu(true)` (or the last 40 s) */
/** The F9 monitor's GPU lines (state tab, the whole width) */
function gpuLines(): string[];

// src/main.ts:3986-3986 function valueText
const valueText = (value: unknown): string => …;

// src/main.ts:3987-4002 variable healthPanel
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

// src/main.ts:4247-4250 function refreshShell
/** Whether the road tool is drawing a road right now (a drag, a chained stretch or a curve). */
/** Asks the interface to redraw its panels (a state it shows changed in the game). */
function refreshShell(): void;

// src/main.ts:4259-4274 variable actions
/** The Actions (`actionsWiring.ts`): the pistol, the bomb, the buildings it breaks and the fires it leaves. */
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
