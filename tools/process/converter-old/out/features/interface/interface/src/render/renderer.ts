// What the feature uses of src/render/renderer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/renderer.ts:152-152 function viewOf
const viewOf = (zoom: number): CityView => …;

// src/render/renderer.ts:183-183 type SkyMode
/** The scene renderer. ## The rebuild contract Everything derived from the road network or the terrain is rebuilt in */

/** How the sky is kept: `cycle` follows the residents' clock. */
export type SkyMode = 'day' | 'night' | 'cycle';

// src/render/renderer.ts:187-197 interface BlastHit
/** What an explosion broke, for the renderer to throw (`blast.ts`): world coordinates. */
export interface BlastHit {
  readonly ground: 'road' | 'earth' | 'building';
  readonly crater: boolean;
  readonly poles: readonly { x: number; y: number; lamp: boolean; mode: 'whole' | 'snap' | 'splinter'; dirX: number; dirY: number }[];
  /** Wires torn off a broken pole: from the pole still standing towards the one that went. */
  readonly wires: readonly { fromX: number; fromY: number; toX: number; toY: number }[];
  readonly posts: readonly { x: number; y: number; yaw: number }[];
  /** `id` and `archetype`: which vehicle, for a wreck of its own body (`AgentMeshes.carcass`). */
  readonly vehicles: readonly { x: number; y: number; angle: number; length: number; width: number; height: number; color: number; id?: number; archetype?: Archetype }[];
  readonly items: readonly { kind: string; x: number; y: number }[];
}

// src/render/renderer.ts:199-217 interface RenderStats
interface RenderStats {
  /** The triangles BUILT (every mesh the scene holds, shown or not). DO NOT read it as what was drawn: `drawnTriangles` is (report UI-15 c, the two had one name). */
  readonly builtTriangles: number;
  /** The triangles DRAWN by the last game frame's every pass, read right after its post chain (`frameDrawn`). */
  readonly drawnTriangles: number;
  /** The draw calls of the last game frame's every pass, read right after its post chain. DO NOT read `renderer.info.render` for it at any other time: the programs warmed and the uploads drained after the frame (`drainCompiles`, `drainWarm`) add theirs to it (report UI-15 b). */
  readonly drawCalls: number;
  readonly quality: QualityLevel;
  /** A smoothed 1/delta of the drawn frames: with the loop on demand it says how often the game drew, not how fast it could (report UI-15 d). DO NOT budget on it: `__roadcraft.budget` takes the frames' own times. */
  readonly fps: number;
  /** Wall time of the last world rebuild, in milliseconds. */
  readonly rebuildMs: number;
  /** The road surface tiles the last rebuild made afresh, and those it kept. */
  readonly roadTiles?: { readonly built: number; readonly reused: number };
  /** Increments once per completed world rebuild. */
  readonly rebuilds: number;
  /** Wall time of the last terrain-only update (a brush dab while roads are held), ms. */
  readonly terrainMs: number;
}

// src/render/renderer.ts:219-222 interface DrawOptions
interface DrawOptions {
  /** Keep the roads, and everything laid along them, as they are */
  readonly holdRoads?: boolean;
}

// src/render/renderer.ts:225-225 variable OCCLUSION_RETRY_FRAMES
/** How many frames the buildings' occlusion rests after a frame it hid nothing in (`occlusionIdle`). */
const OCCLUSION_RETRY_FRAMES = 15;

// src/render/renderer.ts:227-396 interface SceneHandle
export interface SceneHandle {
  /** Loads what shots and blows leave behind */
  effects(): Promise<void>;
  /** The world of the last edit is still being built (`worldSteps`); the old one is drawn meanwhile. */
  readonly worldBusy: boolean;
  /** The opening's town (or a map being opened) is still being put together, nothing of it shown yet (`opened` in `draw`). */
  readonly opening: boolean;
  /** Settles once the programs of this opening are compiled (`shaderCatalogue.ts`): the curtain stays up until then. */
  readonly programsReady: Promise<void>;
  /** A map is being opened (a file, the autosave): nothing is shown until its whole world is in */
  beginLoad(): void;
  /** The next map opened is large: its first world covers the view alone, the rest filled after the curtain. */
  scopeOpening(large: boolean): void;
  /** The sun's cascades' passes since the last call (`SunCascades.takeStats`). */
  /** `unknown`: the still casters whose last change had no box to tell (the stores thrown away for them: what to give bounds to). */
  shadowStats(): (ReturnType<SunCascades['takeStats']> & { readonly unknown: readonly string[]; readonly widest: readonly string[]; readonly changers: readonly string[]; readonly near: readonly string[] }) | null;
  /** The town's cells by level and why below their detail (`BuildingLayer.levels`): asked by the probe, never each frame. */
  buildingLevels(): Readonly<Record<string, number>>;
  /** What draws each cell of buildings now, by key (`massCells.ts` census): the census of holes (`tests/perf/massBase.spec.ts`). */
  buildingCensus(): Map<string, string | null>;
  /** The geometry budgets kept (`quality.ts` GEOMETRY_BUDGETS): the tier's (null), or these (the probes' unbounded run: Infinity everywhere). */
  setGeometryBudget(budget: GeometryBudget | null): void;
  /** The scene's geometry read against the budget now (`governGeometry`, as every `GOVERN_FRAMES`): the bytes counted. */
  governGeometry(): number;
  /** The camera the frame is drawn from (the probes' census of what is in its view). */
  viewCamera(): Camera;
  /** What the opening still waits for (`draw`'s reveal): asked by the probes, never each frame. */
  openingState(): { readonly roadsIn: boolean; readonly coarsePending: boolean; readonly viewPending: boolean; readonly treesStarted: boolean; readonly treesPending: boolean; readonly programs: string; readonly worked: number; readonly worldBusy: boolean; readonly fillPending: boolean; readonly fillCellsLeft: number };
  /** Which renderer draws the frame (`gpu/backendChoice.ts`): WebGL, or three's WebGPU renderer with `?gpu=webgpu` (`MIGRACAO-WEBGPU.md`). */
  readonly backend: 'three-webgl' | 'three-webgpu';
  /** The same view drawn by the WebGL and the WebGPU renderer and the pictures compared (`gpu/webgpuView.ts`, `scripts/look.mjs` "compare"). */
  compare(options?: CompareOptions): Promise<CompareResult>;
  /** What the WebGPU renderer left out of its frames so far, drawables by reason (empty under WebGL). */
  webgpuLeftOut(): Record<string, number>;
  readonly viewport: Viewport;
  readonly scene: Scene;
  /** The underlying WebGL renderer. Exposed for diagnostics and tests. */
  readonly gl: WebGLRenderer;
  readonly stats: RenderStats;
  /** The GPU time of each frame drawn (`gpuTimer.ts`): the F9 monitor and the performance gate read it. */
  readonly gpu: GpuTimer;
  /** The buildings' occlusion culling on the page (`occlusion.ts`): on or off, and the last frame's numbers (walls laid, boxes tested and
   *  hidden, ms to lay them, the buildings' objects left out of the colour pass). */
  readonly occlusion: { enabled: boolean; readonly stats: Readonly<OcclusionStats & { objectsHidden: number; active: boolean }> };
  /** The solved road height field, for the editor's own previews. */
  elevationAt(x: number, y: number, structure?: RoadStructure): number;
  /** The editor's building ghost; null removes it. */
  setBuildingPreview(preview: BuildingPreviewInput | null): void;
  /** The pole run the pole tool would build, drawn as built; null removes it. */
  setPolePreview(net: Network, preview: PolePreviewInput | null): void;
  /** The metro seen as a cut through the ground (the transit tool on). */
  setTransitXray(on: boolean): void;
  /** The track being laid, built as it will be (`buildTrackPreview`); null removes it. */
  setTransitPreview(preview: { mode: 'train' | 'metro'; points: readonly { x: number; y: number }[] } | null): void;
  /** The lots of the Zoning tool coloured into the ground, all of them from the next frame (`lotOverlay.ts`) */
  setLotOverlay(input: LotOverlayInput | null, changes?: ChangeJournal): void;
  /** A blow on a building at world (x, y, z), `strength` 1..10: it breaks (`destruction.ts`). */
  strikeBuilding(b: Building, x: number, y: number, z: number, strength: number): boolean;
  /** A blow on the street at (x, y): a crater in the asphalt, dust. */
  strikeGround(x: number, y: number, strength: number): void;
  /** An explosion at world (x, y), height z, reaching `radius` (`blast.ts`), and the things it broke. */
  explode(x: number, y: number, z: number, radius: number, hit: BlastHit): void;
  /** People inside a struck building, thrown out of it (`ragdoll.fling`). */
  flingOccupants(list: readonly Occupant[]): void;
  /** Called when a struck building comes down after its blow (its pieces were made off the main thread). */
  onBuildingDown(listener: (id: number) => void): void;
  /** A broken hydrant spouting water (`blast.geyser`). */
  geyser(x: number, y: number, z: number, seconds?: number): void;
  /** A burst pipe: a small spout of water from a wall for a moment. */
  leak(x: number, y: number, z: number, seconds: number): void;
  /** Sparks of a short circuit at world (x, y), height z, for a while (`blast.arc`). */
  sparkAt(x: number, y: number, z: number, seconds: number): void;
  /** Soot laid on the ground (`blast.soot`). */
  soot(x: number, y: number, z: number, radius: number): void;
  /** Smoke in the air, 0 clear to 1 thick: the fog closes in, browner, the light dims. */
  setSmog(k: number): void;
  /** A lightning bolt onto the map point (x, y) now (`render/lightning.ts`). */
  strikeAt(x: number, y: number): void;
  /** Called at every strike with where it fell and how far from the view's centre, world units (for the thunder). */
  onStrike(listener: (x: number, y: number, distance: number) => void): void;
  /** How far the wind has carried the clouds, on the map (`world/clouds.ts` driftedCloud). */
  cloudDrift(): { readonly x: number; readonly y: number };
  /** A lasting fire at world (x, y), height z (a building burning). */
  burn(x: number, y: number, z: number, size: number, seconds: number): void;
  /** Whether anything is still moving on its own (an explosion, bodies, debris): keep drawing. */
  busy(): boolean;
  /** Clouds drifting over the map: worth a frame now and then even with nothing else moving. */
  drifting(): boolean;
  /** The bytes of the render targets and shadow maps alive (an estimate, `targetBytes.ts`): not in the scene, so not in `countMemory`. */
  targetBytes(): number;
  /** The bytes the buildings' detail holds on the GPU now (within the room the GPU ledger's cap leaves, `gpuLedger.ts`). */
  buildingDetailBytes(): number;
  /** Forgets a building's ruin (it was removed). */
  forgetRuin(id: number): void;
  /** The Builder's "Ocultar outros": undefined draws every building solid, null fades them all */
  setBuildingsDimmed(except: number | null | undefined): void;
  /** "See inside": the buildings near (x, y) drawn cut open at a floor; null draws them whole. */
  setBuildingCutaway(spec: CutawaySpec | null): void;
  /** The sky: always day, always night, or the residents' own clock. */
  setSkyMode(mode: SkyMode): void;
  /** Clouds and mist as the player set them (Landscape > Sky and weather). */
  setAtmosphere(atmosphere: Atmosphere): void;
  /** Perspective camera on, or the orthographic (isometric) view. */
  setPerspective(on: boolean): void;
  /** A shot: a tracer from the muzzle to where it struck, and the muzzle's flash (world x, y, height). */
  shot(from: readonly [number, number, number], to: readonly [number, number, number]): void;
  /** A person struck by a shot (world x, y, the height struck) */
  wound(x: number, y: number, z: number, dirX: number, dirY: number, severed: Severable | null): void;
  /** A shot along the line of sight between world points `a` and `b` (x, y, height) at the bodies on the ground */
  shootBody(a: readonly [number, number, number], b: readonly [number, number, number]): { alive: number; part: BodyPart } | 'hit' | null;
  /** Whether somebody is down on the ground as a body (not standing to be shot). */
  isDown(id: number): boolean;
  /** The bodies, guts and debris run at this share of real time (0: stopped; the weapons lab). */
  setEffectsSpeed(speed: number): void;
  /** Moves the bodies, guts and debris on by `seconds` once (a frame step while stopped). */
  stepEffects(seconds: number): void;
  /** Every body, piece, gut and blood stain gone (the weapons lab's clean slate). */
  clearCasualties(): void;
  /** Every body on the ground as the weapons lab measures it (`Ragdolls.probe`). */
  ragdollProbe(): RagdollProbe[];
  /** What poses a person now (`AgentMeshes.animProbe`). */
  animProbe(id: number): ReturnType<AgentMeshes['animProbe']>;
  /** Plays a clip on a person regardless (`AgentMeshes.forceClip`). */
  forceClip(id: number, clip: string | null): void;
  /** Every procedural person's skeleton measured against their body (`AgentMeshes.meshProbe`). */
  meshProbe(): ReturnType<AgentMeshes['meshProbe']>;
  /** A shot striking a vehicle at world (x, y, height z), coming along (dirX, dirY) */
  vehicleHit(x: number, y: number, z: number, dirX: number, dirY: number, glass: boolean, blood: boolean): void;
  /** The body a vehicle's driver or rider is drawn with (`AgentMeshes.driverBody`). */
  driverBody(vehicle: VehicleView, x: number, y: number): number | null;
  /** A two-wheeler its rider was shot off: the machine itself falling over and sliding to a stop. */
  dropVehicle(v: { id: number; archetype: Archetype; x: number; y: number; angle: number; color: number; dirX: number; dirY: number }): void;
  /** The height the terrain is drawn at — what anything laid on it must clear. */
  terrainHeightAt(x: number, y: number): number;
  /** The highest corner of the ground as drawn now (the mesh is flat between corners), read once per change of it. */
  landTop(): number;
  /** The universal grid (`world/grid.ts`) drawn over the whole map, on the ground, or not. */
  setGrid(on: boolean): void;
  /** The grid's cells a road was just laid over light up and fade back, a blink */
  flashGrid(cells: readonly { x: number; y: number; ring: number }[]): void;
  /** The roads just laid light up and fade back, a blink, over their whole width. */
  flashRoads(ids: readonly SegmentId[]): void;
  /** The ground what is built stands on (`builtGroundAt`) */
  builtGroundAt(x: number, y: number): number;
  /** The roads' heights an edit already solved for this network on the natural terrain (the tunnel test, `commitRoadPath`) */
  offerElevation(elevation: RoadElevation, networkRevision: number): void;
  /** The roads' heights last solved on the natural terrain, with the terrain revision they were solved on */
  roadsSolve(): { readonly elevation: RoadElevation; readonly terrainRevision: number } | null;
  /** The top of what is drawn at a point: a deck where a road's casing covers it, the terrain elsewhere. */
  surfaceHeightAt(x: number, y: number): number;
  /** The height of the paving at a point (NaN off the roads); with `pack`, what a worker reads it from over a rectangle (`world/paving.ts`). */
  readonly pavedHeightAt: PavingSource;
  /** The canvas's size read again, or `pixelRatio` taken (the dynamic resolution): the drawing buffer and the screen's targets made again only when either changed. */
  resize(pixelRatio?: number): void;
  draw(net: Network, sim: SimClient, alpha: number, delta: number, options?: DrawOptions): void;
  /** A tier, or the player's own settings on it (the options panel: shadows, occlusion, smoothing). */
  setQuality(level: QualityLevel | 'auto', settings?: QualitySettings): void;
  readonly quality: QualityLevel | 'auto';
  /** The effects panel's switches (`effectSwitches.ts`): each effect and layer on or off, and the buildings' settings, from the next frame. */
  setEffects(switches: EffectSwitches, buildings?: BuildingChoices): void;
  /** The lab's optimisations (`optimizations.ts`): each from the next frame; all at their defaults, the game as it draws. */
  setOptimizations(values: Optimizations): void;
  /** What the optimisations hold now (`optimizationsApply.ts` report). */
  optimizationReport(): Readonly<Record<string, number>>;
  /** The inspection camera (`inspector.ts`); null outside development builds. */
  readonly inspect: Inspector | null;
  /** Every figure drawn last frame and the body it was cast as: the runtime census. */
  census(): ReturnType<AgentMeshes['census']>;
  dispose(): void;
}

// src/render/renderer.ts:399-399 variable SURFACE_SETS
/** Roads at grade, and the decks above them, as `surfaceHeightAt` asks them. */
const SURFACE_SETS: readonly ReadonlySet<RoadStructure>[] = [GROUND_ONLY, new Set<RoadStructure>(['elevated', 'bridge'])];

// src/render/renderer.ts:404-404 variable FAR_ROAD_CELL
/** The far roads' cells made from the network at a scoped opening (`networkFarRoads.ts`): a road block's side (`roadSurfaces.ts` CHUNK tiles). */
const FAR_ROAD_CELL = 8 * TILE;

// src/render/renderer.ts:406-406 variable GOVERN_FRAMES
/** Frames between two reads of the scene's geometry against the tier's budget (`governGeometry`): a walk of the scene's objects, ~1-3 ms. */
const GOVERN_FRAMES = 60;

// src/render/renderer.ts:415-415 function isLeafCards
/** A plant's leaf cards (`scenery.ts` `plantMeshes`, `gardens.ts`): `...-trees-<species>-cards`, `...-bushes-<kind>-cards`. */
const isLeafCards = (mesh: Object3D): boolean => …;

// src/render/renderer.ts:432-4205 function createSceneRenderer
export function createSceneRenderer(
  canvas: HTMLCanvasElement,
  initialCentre: Vec2,
  initialZoom: number,
  initialQuality: QualityLevel | 'auto' = 'auto',
  askDraw: () => void = () => {},
  /** The engine the scene's products are registered with (`core/engine.ts`): the game's one, from `main.ts`. */
  engine: Engine = new Engine(),
  /** The player's own settings on the tier (the options panel); the tier's own when absent. */
  initialSettings?: QualitySettings,
  /** Where the WebGL renderer comes from: three's own, always, in the game; a renderer on a GPU that draws nothing in the specs that open a city in Node (`tests/support/nullGpu.ts`). */
  makeRenderer: (parameters: WebGLRendererParameters) => WebGLRenderer = (parameters) => new WebGLRenderer(parameters),
): SceneHandle;

// src/render/renderer.ts:1464-1470 function createSceneRenderer.occluderDrawn
const occluderDrawn = (x: number, y: number): boolean =>;
