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

// src/main.ts:301-301 variable measure
/** A measure running (`__roadcraft.measure`, `scripts/lib/measureCity.mjs`): `on` a frame drawn every refresh; `step` the simulation going on a fixed step of
 *  `MEASURE_STEP_S` a frame, whatever the wall clock did, so two runs of a scene step the same (report UI-15 a: the held clock left the agents, the growth and
 *  the wear out of every measure); `grown` the probes waiting for the next growth answer (`__roadcraft.growTimed`). One holder for the console's measures. */
const measure = { on: false, step: false, grown: [] as ((grown: readonly number[]) => void)[] };

// src/main.ts:303-303 variable PROBES
/** The console's probes (`__roadcraft`, `__frames`, `__gpu`, `__state`, `__changes`): in development, and in a build made to be measured (`npm run build:probe`, mode `probe`) - the product's code measured, without the development panels (report AST-15: every measure ran the development build). */
const PROBES = import.meta.env.DEV || import.meta.env.MODE === 'probe';

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

// src/main.ts:546-550 function select
/** What is selected - a road (and where along it) or a junction - written through the game's state with why (`gameState`). */
function select(segment: SegmentId | null, s: number | null, node: NodeId | null, cause: string): void;

// src/main.ts:563-567 variable cameraMotion
/** The keys held and the wheel's notches, spent as a glide frame by frame (`view/cameraMotion.ts`). */
const cameraMotion = new CameraMotion({
  view: () => view,
  size: () => ({ w: surface.cssW, h: surface.cssH }),
  heightUnder: (at) => groundHeightUnder(at),
});

// src/main.ts:608-608 variable view
let view: Viewport = flatViewport(camera);

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

// src/main.ts:812-821 function syncViewFromFlatCamera
function syncViewFromFlatCamera(): void;

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

// src/main.ts:915-928 function applySnapshot
/** `snapshot`: the model's own undo/redo state, restored exactly. */
/** `table`: an imported map's buildings' table, prepared by the city's worker (`editor/preparedCity.ts`). */
function applySnapshot(data: ReturnType<RoadDoc['toJSON']> | null, source: 'snapshot' | 'import' = 'snapshot', table: ArrayBufferLike | null = null): void;

// src/main.ts:942-953 function snapshotApplied
function snapshotApplied(): void;

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

// src/main.ts:1207-1212 function doubleClick
function doubleClick(e: MouseEvent, r: DOMRect): void;

// src/main.ts:1213-1216 function singleClick
function singleClick(e: MouseEvent, r: DOMRect): void;

// src/main.ts:1217-1396 function pointerDown
function pointerDown(e: PointerEvent, r: DOMRect): void;

// src/main.ts:1404-1475 function pointerMove
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void;

// src/main.ts:1480-1482 function poleReach
/** Pick radius for a pole, in world units at the current zoom (`PoleTool.reach`: one definition). */
function poleReach(): number;

// src/main.ts:1484-1513 function endPointer
function endPointer(e: PointerEvent): void;

// src/main.ts:1558-1566 function roadClasses
/** Every road class as the upgrade tool shows it: name, picture, total width in whole metres. */
function roadClasses(): ShellRoadClass[];

// src/main.ts:1577-1585 function laneChoices
/** The lane choices, the one the next road will be laid as lit: with no count chosen, the class's own. */
function laneChoices(): { id: string; label: string; on: boolean }[];

// src/main.ts:1586-1594 function setLanes
function setLanes(id: string): void;

// src/main.ts:1596-1602 function selectRoadType
function selectRoadType(i: number): void;

// src/main.ts:1604-1609 function setAlignment
function setAlignment(next: Alignment): void;

// src/main.ts:1614-1614 variable ROUNDABOUT_RADIUS_M
/** The roundabout's radius, in whole metres. */
const ROUNDABOUT_RADIUS_M = [32, 128] as const;

// src/main.ts:1617-1630 function setZone
/** A lot or zone setting changed (Zoning): the use, the density, the eraser, the lot verb, the cut. */
function setZone(patch: ShellZonePatch): void;

// src/main.ts:1633-1633 variable BRUSH_MODES
/** Every brush of the terrain tool, in the order of its number keys. */
const BRUSH_MODES: readonly BrushMode[] = ['raise', 'lower', 'flatten', 'river', 'paint', 'mesa', 'canyon', 'escarpment', 'sugarloaf', 'fog', 'cloud', 'elements', 'gully', 'trees', 'weather'];

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

// src/main.ts:1774-1776 function mapBiome
/** The map's biome (`world/ecology.ts`): one gives an old map its ecosystem too; "none" takes it away. Undoable. */
function mapBiome(): string;

// src/main.ts:1777-1786 function setMapBiome
function setMapBiome(key: string): void;

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

// src/main.ts:1834-1838 function showTool
/** What follows the tool in hand outside the interface: the pointer's cursor, the Builder's mode, the hint. */
function showTool(next: Tool): void;

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

// src/main.ts:1859-1946 function mountInterface
/** The interface (`ui/v2/shell.ts`): its HUD, dock, drawer and selection panel, handed the game's state and its commands. */
function mountInterface(): void;

// src/main.ts:1964-1971 function setSpeed
function setSpeed(speed: number): void;

// src/main.ts:1974-1978 function setDemand
/** How much traffic the city asks for: Low, Normal or Peak. */
function setDemand(value: number): void;

// src/main.ts:1980-1983 function toggleCongestion
function toggleCongestion(): void;

// src/main.ts:1993-1993 variable growthPool
/** NO WORLD BY TILES IS OPENED (the player, 2026-10-18): a city is the small city of Roadcraft 2.0 as one map (`openSmallCity`). */



/** THE GROWTH (`editor/growthFeed.ts`): the generated city's buildings and the zones' (`growZones`), one at a time, each designed in the growth's worker (`growthPool.ts`, `growth.worker.ts`) and written here when its answer comes - outside the frame, in the worker's message - checked again first when the document moved meanwhile. */
const growthPool = new GrowthPool((answer) => growthAnswered(answer), () => growth.restart());

// src/main.ts:1994-1996 variable growth
const growth = new GrowthSession(growthPool, () => ({
  doc, net, groundAt: (x, y) => scene.terrainHeightAt(x, y), pavedAt: scene.pavedHeightAt, cell: TERRAIN_CELL,
}));

// src/main.ts:1998-2012 function growthAnswered
/** The worker's answer: the building written, stamped as grown now. False: not now - nothing is written in the middle of a drag (an authoring preview); the pool offers it again at the next frame (`GrowthPool.pump`). */
function growthAnswered(answer: GrowAnswer): boolean;

// src/main.ts:2015-2027 function newMap
function newMap(): void;

// src/main.ts:2029-2033 function newWorld
/** A NEW CITY from the menu: the small city of Roadcraft 2.0 (`buildSmallCityMap`: straight blocks, medium size) laid out as ONE map, no tiles - the player, 2026-10-18: the tiles were for a city over the whole map, which never worked. */
function newWorld(): void;

// src/main.ts:2035-2039 function newLargeWorld
/** A LARGE CITY: the small city's generator over a larger radius (`largeCity.ts`), every block built, opened at a district while the rest of the world fills in behind the curtain. */
function newLargeWorld(): void;

// src/main.ts:2203-2205 function openSmallCity
/** The small city of a seed put in place of the map (the menu's, and the probes' with a seed of their own). */
function openSmallCity(seed: number): Promise<void>;

// src/main.ts:2208-2210 function openLargeCity
/** The large city of a seed put in place of the map (the menu's, and the probes'). */
function openLargeCity(seed: number, radius?: number): Promise<void>;

// src/main.ts:2244-2248 function setSky
function setSky(mode: string): void;

// src/main.ts:2257-2260 function saveMap
/** DO NOT say "saved" without asking `exportToFile` */
function saveMap(): void;

// src/main.ts:2263-2271 function openImported
/** Loads a picked map. */
function openImported(result: ImportResult): boolean | Promise<boolean>;

// src/main.ts:2294-2296 function openMap
async function openMap(): Promise<void>;

// src/main.ts:2319-2332 function cameraCommand
function cameraCommand(command: string): void;

// src/main.ts:2335-2343 function undo
function undo(): void;

// src/main.ts:2344-2351 function redo
function redo(): void;

// src/main.ts:2353-2363 function afterRestore
/** The game after an undo or a redo put records back (`editor/history.ts`): the network rebuilt only when the roads moved */
function afterRestore(): void;

// src/main.ts:2366-2369 function updateHistoryButtons
/** Whether there is something to undo and to redo, in the game's state. */
function updateHistoryButtons(): void;

// src/main.ts:2395-2399 function closeInspectorPanel
/** The road inspector closed (its × , Escape, Delete): nothing picked. */
function closeInspectorPanel(): void;

// src/main.ts:2444-2448 function flashHint
/** A transient answer of the game - saved, refused, undone, a road that cannot be laid */
function flashHint(key: string, params?: Readonly<Record<string, string | number>>): void;

// src/main.ts:2451-2458 function hintKey
/** The tool's hint: what a click does now. */
function hintKey(prefix: string): string;

// src/main.ts:2461-2466 function updateHint
function updateHint(): void;

// src/main.ts:2468-2473 function listenHint
/** A listener of the hint: told the hint now, and any answer said before it came. */
function listenHint(listen: (text: string, flash: boolean) => void): void;

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

// src/main.ts:2570-2570 variable frameClock
/** The frame asked for, and the time between frames (`frameLoop.ts`). */
const frameClock = new FrameClock(frame, () => booted && scene.opening);

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

// src/main.ts:2641-2641 variable frameTimes
/** Every frame's whole time, the last 40 s (`sampleRing.ts`): the CPU the process spends a frame, sampled while it runs (`__roadcraft.usage`). */
const frameTimes = new SampleRing(2400);

// src/main.ts:2644-2745 function frame
/** @perFrame One frame: input, the topology catching up, the simulation, the drawing, the panels. */
function frame(now: number): void;

// src/main.ts:2783-2783 variable BARRIER_PICK_PIXELS
/** Pick radius for a barrier under a shift-click, screen pixels. */
const BARRIER_PICK_PIXELS = 10;

// src/main.ts:2901-2901 variable showChanges
/** F8: the diary's last changes drawn where they happened (`world/changes.ts`). */
let showChanges = false;

// src/main.ts:2942-3410 function drawOverlayScreen
function drawOverlayScreen(): void;

// src/main.ts:3546-3571 function setNodeHeightMetres
function setNodeHeightMetres(id: NodeId, metres: number): void;

// src/main.ts:3573-3818 function showInspector
function showInspector(): void;

// src/main.ts:3821-3832 function duplicateSelectedSegment
/** Duplicates the inspected road and keeps the copy selected for immediate editing. */
function duplicateSelectedSegment(id = game.selectedSegment): void;

// src/main.ts:3835-3835 variable statusListeners
/** Who shows the city's numbers (the interface, `onStatus`). */
const statusListeners: ((status: ShellStatus) => void)[] = [];

// src/main.ts:3837-3856 function currentStatus
/** The city's numbers now: the clock, the traffic, the money, the junctions that cannot be built. */
function currentStatus(): ShellStatus;

// src/main.ts:3933-3941 function setMarkingStyle
/** The map's road paint (`world/roads/markingStyle.ts`) */
function setMarkingStyle(value: string): void;

// src/main.ts:3944-3946 function setQuality
function setQuality(value: string): void;

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

// src/main.ts:4022-4024 function compiledUniforms
/** The uniforms the renderer compiled a material with: `onBeforeCompile` puts textures there that are in no field of the material (report TST-09). */
function compiledUniforms(material: object): Record<string, { value: unknown }> | undefined;

// src/main.ts:4027-4033 function pacedFps
/** Frames a second of the last 2 s, from the frames' own start times; null when the game did not draw every refresh in them
 *  (the loop on demand: a smoothed 1/delta then told how often it drew, not how fast it could - report UI-15 d). */
function pacedFps(): number | null;

// src/main.ts:4036-4231 statement <IfStatement>
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  /** The surgical profiler's numbers (`core/profiler.ts`), as its HUD shows them. */
  profiler: () => profiler.view(),
  /** The engine's ledger: each product's computes, last refresh and in all (`core/engine.ts`). */
  ledger: () => Object.fromEntries(engine.ledger),
  /** The job pool as it stands (`jobs/jobPool.ts` jobPoolStats): its workers, in the page or a job process, and what each runs. */
  jobPool: () => jobPoolStats(),
  /** Which renderer draws the frame (`?gpu=webgpu`, `MIGRACAO-WEBGPU.md`). */
  backend: scene.backend,
  /** The view drawn by the WebGL and the WebGPU renderer in the same instant, the pictures compared (`scripts/look.mjs` "compare"). */
  compare: (options?: Parameters<SceneHandle['compare']>[0]) => scene.compare(options),
  /** What the WebGPU renderer left out of its frames so far, by reason. */
  webgpuLeftOut: () => scene.webgpuLeftOut(),
  /** The buildings' occlusion culling (`render/occlusion.ts`): `enabled` switches it, `stats` the last frame's walls laid, boxes hidden and ms. */
  occlusion: scene.occlusion,
  doc,
  net,
  sim,
  camera,
  surface,
  DT,
  /** The map to a file as Ctrl+S makes it; the hint it would show. */
  exportMap: () => exportToFile(doc, sessionSettings()),
  /** The autosave written now, resolved once it is stored (the route's reopening step, `scripts/look/route.json`). */
  saved: () => { persistence.saveSessionSoon(doc, sessionSettings); persistence.flush(); return persistence.written(); },
  importMap: async () => openImported(await importFromFile()),
  /** The public transport tool. */
  transit: transitEditor,
  /** A measure's mode (`scripts/lib/measureCity.mjs`), a frame drawn every refresh: `true` the simulation's clock held (the sun, the population and the vehicles still); `'running'` the simulation going on a fixed step of 1/60 s a frame; `false` the speed as it was. */
  measure: (on: boolean | 'running') => {
    measure.on = on !== false;
    measure.step = on === 'running';
    // The clock alone, not the player's pause: DO NOT go through `setPaused`, which keeps the pause in the player's settings.
    sim.clock.paused = on === true || game.paused;
    frameClock.restart();
    requestDraw();
  },
  setTraffic: (enabled: boolean) => {
    if (!game.paused !== enabled) setSpeed(game.paused ? game.speed : 0);
  },
  setRoadHeight: (metres: number) => {
    roadTool.heightChosen(metres * UNITS_PER_METER);
    requestDraw();
  },
  /** A generated city: the small city of Roadcraft 2.0 as one map (`smallCity`). DO NOT open the world by tiles: the player, 2026-10-18, wants no city by tiles. */
  /** `radiusMetres`: the large city over another radius, its density kept (the scale steps). */
  generateCity: (options: { seed?: number; size?: 'small' | 'large'; radiusMetres?: number } = {}) => (options.size === 'large' ? openLargeCity(options.seed ?? 7, options.radiusMetres !== undefined ? m(options.radiusMetres) : undefined) : openSmallCity(options.seed ?? 7)),
  /** Probe: one building grown on a zoned lot, and what it cost (ms). */
  /** Probe: one building grown as the game grows them - asked of the growth's worker, written when its answer comes - and the ms from the ask to the building written. DO NOT time the page's own grower: the game never grows there (report UI-15 g). */
  growTimed: (): Promise<{ grown: readonly number[]; ms: number } | { grown: null; why: string }> => {
    if (growth.busy) return Promise.resolve({ grown: null, why: 'a building is being made already' });
    const t = performance.now();
    if (!growth.ask(lotTool.refusedNow(), 0x5eed)) return Promise.resolve({ grown: null, why: 'no open lot' });
    return new Promise((done) => measure.grown.push((grown) => done({ grown, ms: performance.now() - t })));
  },
  /** Replaces the map as loading a file does: document, network and simulation topology. */
  loadDoc: (data: ReturnType<RoadDoc['toJSON']>) => {
    history.begin(doc);
    applySnapshot(data, 'import');
  },
  /** Centres the play camera on a world point, so the next frame builds what is there */
  /** The transport tool, for the browser checks. */
  transitTool: () => transitEditor,
  lookAt: (x: number, y: number, zoom = camera.zoom) => {
    camera.x = x;
    camera.y = y;
    camera.zoom = zoom;
    syncViewFromFlatCamera();
    requestDraw();
  },
  /** The camera put at a centre, zoom, bearing and tilt (radians) exactly, as `view()` reads them (`scripts/look.mjs`) */
  /** The small city of Roadcraft 2.0 of a seed, as the menu's "New city" opens it, with no question asked (`scripts/probe.mjs`, `scripts/lib/measureCity.mjs`). */
  smallCity: (seed = 7) => openSmallCity(seed),
  /** Probe: the effects, the buildings' choices and the optimisations as the game holds them, each patch over its default (none: the default itself, the very object the game compares by). */
  variant: (fx?: Partial<EffectSwitches> | null, b?: Partial<BuildingChoices> | null, opt?: Partial<Optimizations> | null) => {
    scene.setEffects(fx ? { ...ALL_EFFECTS_ON, ...fx } : ALL_EFFECTS_ON, b ? { ...DEFAULT_BUILDING_CHOICES, ...b } : DEFAULT_BUILDING_CHOICES);
    scene.setOptimizations(opt ? { ...DEFAULT_OPTIMIZATIONS, ...opt } as Optimizations : DEFAULT_OPTIMIZATIONS);
    requestDraw();
  },
  setView: (at: { x: number; y: number; zoom: number; azimuth: number; elevation: number }) => {
    camera.x = at.x;
    camera.y = at.y;
    camera.zoom = at.zoom;
    syncViewFromFlatCamera();
    view.setOrbit(at.azimuth, at.elevation);
    view.moveTo({ x: at.x, y: at.y });
    requestDraw();
  },
  /** Forces one frame. Used by the browser verification harness. */
  redraw: () => requestDraw(),
  /** What the scene holds in memory, in MB (`render/memoryCounters.ts`) */
  /** Everything the scene holds, by layer and kind of mesh (`render/inventory.ts`) */
  inventory: (options: { text?: boolean; top?: number } = {}) => {
    const inv = sceneInventory(scene.scene);
    const heap = (performance as { memory?: { usedJSHeapSize: number } }).memory?.usedJSHeapSize;
    const counts = {
      doc: {
        nodes: doc.nodes.size, segments: doc.segments.size, buildings: doc.buildings.size, lots: doc.lots.length, poles: doc.poles.size,
        landscape: doc.landscape.size, barriers: doc.barriers.size, trees: doc.trees.length, zones: doc.zones.length, people: doc.people.length,
      },
      sim: { vehicles: sim.vehicles.size, people: sim.pedViews.length },
      gl: { geometries: scene.gl.info.memory.geometries, textures: scene.gl.info.memory.textures, programs: scene.gl.info.programs?.length ?? 0, drawCalls: scene.stats.drawCalls, builtTriangles: scene.stats.builtTriangles, drawnTriangles: scene.stats.drawnTriangles },
      heapMB: heap === undefined ? null : Math.round(heap / 1e6),
    };
    if (options.text) return `${formatInventory(inv, options.top ?? 40)}

${JSON.stringify(counts)}`;
    return { ...inv, counts };
  },
  /** Every resource the process reaches, cheap enough to sample while it runs (`scripts/look.mjs` usage): the frame's CPU (its whole time), the GPU's own timer, the JS heap, the GPU's geometries, textures and programs, what is drawn, and whether the opening still holds the town. */
  usage: () => {
    const heap = (performance as { memory?: { usedJSHeapSize: number; totalJSHeapSize: number; jsHeapSizeLimit: number } }).memory;
    const frames = frameTimes.stats(STUTTER_MS);
    const gpu = scene.gpu.samples.stats();
    const stats = scene.stats;
    const mb = (bytes: number): number => Math.round(bytes / 1e5) / 10;
    return {
      at: Math.round(performance.now()), opening: scene.opening, worldBusy: scene.worldBusy,
      cpuP50: frames.p50, cpuP95: frames.p95, cpuP99: frames.p99, cpuMax: frames.max, cpuN: frames.n, hitches: frames.over,
      // The last `recent` frames' own CPU time (a measure's window, not the ring's 40 s): its median.
      cpuRecentP50: ((recent: number) => { const a: number[] = []; for (let i = 0; i < recent; i++) { const v = frameTimes.newest(i); if (Number.isFinite(v)) a.push(v); } a.sort((x, y) => x - y); return a.length ? a[a.length >> 1]! : NaN; })(120),
      gpuP50: gpu.p50, gpuP95: gpu.p95, gpuP99: gpu.p99, gpuMax: gpu.max, gpuN: gpu.n, gpuSupported: scene.gpu.supported,
      heapMB: heap ? mb(heap.usedJSHeapSize) : null, heapTotalMB: heap ? mb(heap.totalJSHeapSize) : null, heapLimitMB: heap ? mb(heap.jsHeapSizeLimit) : null,
      geometries: scene.gl.info.memory.geometries, textures: scene.gl.info.memory.textures, programs: scene.gl.info.programs?.length ?? 0,
      drawCalls: stats.drawCalls, triangles: stats.drawnTriangles, buildingDetailMB: mb(scene.buildingDetailBytes()), fps: stats.fps,
    };
  },
  memory: () => {
    const mb = (bytes: number): number => Math.round(bytes / 1e5) / 10;
    const c = countMemory(scene.scene, { uniformsOf: compiledUniforms });
    const heap = (performance as { memory?: { usedJSHeapSize: number } }).memory?.usedJSHeapSize;
    return {
      geometryMB: mb(c.geometryBytes), geometryCpuMB: mb(c.geometryCpuBytes), textureMB: mb(c.textureBytes), targetMB: mb(scene.targetBytes()),
      buildingDetailMB: mb(scene.buildingDetailBytes()),
      heapMB: heap === undefined ? null : mb(heap),
      cpuByName: c.cpuByName.map(([name, bytes]) => `${name} ${mb(bytes)}`),
      // What holds the geometry on the GPU, by holder (digits taken out: one row a kind of block or cell), most first.
      gpuByName: [...[...c.bytesByName].reduce((rows, [name, bytes]) => rows.set(name.replace(/\d+/g, '#'), (rows.get(name.replace(/\d+/g, '#')) ?? 0) + bytes), new Map<string, number>())]
        .sort((a, b) => b[1] - a[1]).slice(0, 12).map(([name, bytes]) => `${name} ${mb(bytes)}`),
      // What holds the textures, by holder and texture (digits taken out, the count of each kind kept), most first.
      textureByName: [...[...c.textureBytesByName].reduce((rows, [name, bytes]) => {
        const key = name.replace(/\d+/g, '#');
        const row = rows.get(key) ?? [0, 0];
        row[0] += bytes;
        row[1]++;
        return rows.set(key, row);
      }, new Map<string, [number, number]>())].sort((a, b) => b[1][0] - a[1][0]).slice(0, 16).map(([name, [bytes, n]]) => `${name} x${n} ${mb(bytes)}`),
      buffers: c.buffers, releasedBuffers: c.releasedBuffers, textures: c.textures,
      glGeometries: scene.gl.info.memory.geometries, glTextures: scene.gl.info.memory.textures, programs: scene.gl.info.programs?.length ?? 0,
      drawCalls: scene.stats.drawCalls, builtTriangles: scene.stats.builtTriangles, drawnTriangles: scene.stats.drawnTriangles,
    };
  },
  /** The running game against the city plan's budgets */
  budget: (view: CityView) => {
    if (!(view in VIEW_BUDGETS)) throw new Error(`view: ${Object.keys(VIEW_BUDGETS).join(' | ')}`);
    const c = countMemory(scene.scene, { uniformsOf: compiledUniforms });
    const gpu = scene.gpu.supported ? scene.gpu.samples.toArray().slice(-120).sort((a, b) => a - b) : [];
    const rows = cityBudgetTable({
      view,
      triangles: scene.stats.drawnTriangles,
      drawCalls: scene.stats.drawCalls,
      gpuMs: gpu.length ? Math.round(gpu[Math.ceil(gpu.length * 0.95) - 1]! * 10) / 10 : null,
      fps: pacedFps(),
      bytesByName: c.bytesByName,
      textureBytes: c.textureBytes,
      targetBytes: scene.targetBytes(),
      geometryCpuBytes: c.geometryCpuBytes,
      heapBytes: (performance as { memory?: { usedJSHeapSize: number } }).memory?.usedJSHeapSize ?? null,
    });
    console.table(rows);
    return rows;
  },
  pickAtScreen: (x: number, y: number) =>
    findAnchor(
      doc,
      net,
      worldAtScreen(x, y),
      view.zoom,
    ),
  // The pole tool, as the tool itself runs it: plan from two raw points, then commit that plan.
  utilities: {
    plan: (from: Vec2, to: Vec2, reach = POLE_PICK_PIXELS / view.zoom) =>
      planPoleRun(doc, net, from, to, reach),
    run: (from: Vec2, to: Vec2, reach = POLE_PICK_PIXELS / view.zoom) => {
      const plan = planPoleRun(doc, net, from, to, reach);
      return mutateBuilt(() => commitPoleRun(doc, plan));
    },
  },
  audit: () => [...sim.issues],
  /** The live three.js scene handle, for browser-driven checks. */
  scene: () => scene,
  /** The orbit camera (`setOrbit(azimuth, elevation)`), for photographing from a chosen angle. */
  view: () => view,
  /** The building tool, for browser-driven checks. */
  buildings: buildings.tool,
};

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
