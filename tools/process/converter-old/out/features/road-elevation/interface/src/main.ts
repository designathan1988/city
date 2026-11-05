// What the feature uses of src/main.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/main.ts:156-156 type Alignment
type Alignment = 'straight' | 'curve' | 'free';

// src/main.ts:189-189 variable doc
const doc = new RoadDoc();

// src/main.ts:196-196 variable net
const net = new Network(doc);

// src/main.ts:197-197 variable camera
const camera = new Camera();

// src/main.ts:199-199 variable booted
/** Whether the whole module has run. */
let booted = false;

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

// src/main.ts:837-839 function mutate
function mutate(fn: () => boolean): void;

// src/main.ts:842-851 function mutateRoads
/** A road edit made in place a split, a class, lanes, parking, a section, a direction, a bend, a join, a copy, a node's */
function mutateRoads(fn: (doc: RoadDoc, net: Network) => boolean): boolean;

// src/main.ts:959-971 function worldAtScreen
/** The one conversion that differs between renderers. */
/** The world point under a screen position, ON THE SURFACE THAT IS THERE. */
function worldAtScreen(px: number, py: number, heightOffset?: number): Vec2;

// src/main.ts:2444-2448 function flashHint
/** A transient answer of the game - saved, refused, undone, a road that cannot be laid */
function flashHint(key: string, params?: Readonly<Record<string, string | number>>): void;

// src/main.ts:2589-2591 function requestDraw
function requestDraw(): void;

// src/main.ts:2641-2641 variable frameTimes
/** Every frame's whole time, the last 40 s (`sampleRing.ts`): the CPU the process spends a frame, sampled while it runs (`__roadcraft.usage`). */
const frameTimes = new SampleRing(2400);

// src/main.ts:2942-3410 function drawOverlayScreen
function drawOverlayScreen(): void;

// src/main.ts:3413-3419 variable BUILD_FILL
/** How each stretch of the road in hand will be built, drawn between its deck and the natural ground */
const BUILD_FILL: Readonly<Record<BuildMode, string | null>> = {
  ground: null,
  embankment: 'rgba(164, 120, 74, 0.62)',
  cutting: 'rgba(132, 128, 120, 0.62)',
  bridge: null,
  tunnel: 'rgba(40, 34, 30, 0.55)',
};

// src/main.ts:3420-3420 variable PIER_EVERY
const PIER_EVERY = 14;

// src/main.ts:3421-3459 function drawBuildModes
function drawBuildModes(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], ground: readonly Vec2[], modes: readonly BuildMode[]): void;

// src/main.ts:3461-3475 function drawTunnelStretches
function drawTunnelStretches(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], modes: readonly BuildMode[], width: number): void;

// src/main.ts:3478-3486 function buildSummary
/** The ways the road in hand is built, beside its length: "ponte 120 m · aterro 40 m". */
function buildSummary(stations: Parameters<typeof buildRuns>[0]): string;

// src/main.ts:3546-3571 function setNodeHeightMetres
function setNodeHeightMetres(id: NodeId, metres: number): void;

// src/main.ts:3837-3856 function currentStatus
/** The city's numbers now: the clock, the traffic, the money, the junctions that cannot be built. */
function currentStatus(): ShellStatus;
