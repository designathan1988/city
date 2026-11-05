// What the feature uses of src/render/flatAtlas.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/flatAtlas.ts:46-46 variable REGION_STRIDE
/** THE WORLD'S GROUND, plate by plate of the unbounded grid (`world/bounds.ts`): the plates near the view made in full, a ring of far patches round it, nothing beyond */

const REGION_STRIDE = 1000;

// src/render/flatAtlas.ts:48-48 variable KEEP_MARGIN
/** DO NOT cap the plates by count: the six newest were kept wherever the view stood */
const KEEP_MARGIN = 1.25;

// src/render/flatAtlas.ts:54-54 variable NEAR_VIEW
/** Beyond this much ground in view, no plate is drawn in full: the far mesh alone. */
const NEAR_VIEW = m(1400);

// src/render/flatAtlas.ts:56-56 variable SIDE
/** Corners along a plate's side (the terrain's grid). */
const SIDE = Math.round((2 * TERRAIN_HALF) / TERRAIN_CELL) + 1;

// src/render/flatAtlas.ts:60-60 variable TILE_UNLOADED
/** A load or an unload of a tile (CLAUDE.md, "Streaming world"): what a plate holds is read again from the document. */
const TILE_UNLOADED = 'tile unloaded';

// src/render/flatAtlas.ts:63-63 variable FAR_RING
/** THE MAP'S GROUND (`wholeMap.ts` MAP_HALF, the player's order: never a world without an edge): the plates of the map round the origin's plate (5 x 5 plates, 10 km; the map's edge lies on the plates' edges), drawn far as the quadtree's nodes the view asks (`farGround.ts`), its cut sides on the map's edge, nothing past it */
const FAR_RING = Math.round(MAP_HALF / PLATE_SIZE - 0.5);

// src/render/flatAtlas.ts:69-69 variable WOODS_MOST
/** Plates' woods at most (the level-0 nodes the view can ask, `farGround.ts`, for a map of any size; the shader's fades are sized by it). */
export const WOODS_MOST = 192;

// src/render/flatAtlas.ts:71-85 class PlateSource
class PlateSource implements TerrainSource { … }

// src/render/flatAtlas.ts:72-72 field PlateSource.changes
changes!: RoadDoc['changes'];

// src/render/flatAtlas.ts:73-73 field PlateSource.terrainRevision
terrainRevision = 0;

// src/render/flatAtlas.ts:74-74 field PlateSource.terrainStamps
terrainStamps: TerrainStamp[] = [];

// src/render/flatAtlas.ts:75-75 field PlateSource.terrainRelief
terrainRelief!: RoadDoc['terrainRelief'];

// src/render/flatAtlas.ts:76-76 field PlateSource.natureRevision
natureRevision = 0;

// src/render/flatAtlas.ts:77-77 field PlateSource.nature
nature: RoadDoc['nature'] = null;

// src/render/flatAtlas.ts:78-78 field PlateSource.paintRevision
paintRevision = 0;

// src/render/flatAtlas.ts:79-79 field PlateSource.terrainPaint
terrainPaint: PaintDab[] = [];

// src/render/flatAtlas.ts:80-80 field PlateSource.stampSources
stampSources: readonly TerrainStamp[] = [];

// src/render/flatAtlas.ts:81-81 field PlateSource.paintSources
paintSources: readonly PaintDab[] = [];

// src/render/flatAtlas.ts:87-109 interface Plate
interface Plate {
  /** Its key (`world/bounds.ts` `plateOf`) and centre. */
  readonly key: number;
  readonly cx: number;
  readonly cy: number;
  readonly source: PlateSource;
  /** The document's stamps and dabs that reach it, moved to its map (about its centre). */
  /** The copies, moved to the plate's map, of the stamps and dabs that reach it (`copyOn`): none for the others. */
  readonly stampCopies: WeakMap<TerrainStamp, TerrainStamp>;
  readonly dabCopies: WeakMap<PaintDab, PaintDab>;
  surface: PlateSurface | null;
  shown: boolean;
  /** Its surface's programs built (`uploads.ts` programsReady): not shown before, the far patch standing in. */
  ready: boolean;
  /** The land at the plate's grid corners, the base level added, read off the page (`flatGroundPool.ts`), rows from the north edge; null until then. */
  heights: Float64Array | null;
  /** Its first build made off the page, until the plate is made from it (`plateMesh.ts`). */
  mesh: PlateMesh | null;
  /** Whether its corners were asked of the workers (and not yet come back). */
  asked: boolean;
  /** The highest corner of its drawn ground, kept as it changes. */
  top: number;
}

// src/render/flatAtlas.ts:112-116 interface FarResident
/** A node of the far ground resident: what it is, its slot in the far mesh, and its woods' slot (level 0 only). */
interface FarResident {
  readonly node: FarNode;
  readonly slot: number;
  readonly woods: number;
}

// src/render/flatAtlas.ts:119-120 function reaches
/** Whether a disc reaches a plate (its square grown by the disc). */
const reaches = (p: { readonly cx: number; readonly cy: number }, x: number, y: number, radius: number): boolean => …;

// src/render/flatAtlas.ts:123-123 variable FAR_QUADS
/** The far ground: a coarse patch of `FAR_QUADS` x `FAR_QUADS` quads per far plate, in world coordinates, each in its slot */
const FAR_QUADS = 32;

// src/render/flatAtlas.ts:125-125 variable SKIRT
const SKIRT = m(30);

// src/render/flatAtlas.ts:129-129 interface FlatFar
/** The far mesh's room: `slots` patches, written one slot at a time (`writeFarPatch`), drawn by the index (`farIndexInto`). */
export interface FlatFar { readonly geometry: BufferGeometry; readonly slots: number }

// src/render/flatAtlas.ts:141-143 function writeFarPatch
/** Writes plate (cx, cy)'s patch into `slot` from the land `ground` (the base added here), and marks only that slot to be sent. */
export function writeFarPatch(far: FlatFar, slot: number, cx: number, cy: number, ground: (x: number, y: number) => number): void;

// src/render/flatAtlas.ts:150-187 function writeFarNode
/** Writes the patch of `FAR_QUADS` quads of `step` from (x0, y0) into `slot`, its skirts `skirt` deep (a coarser node's sampling misses more: deeper), from the land `ground` (the base added here); marks only that slot to be sent. */
export function writeFarNode(far: FlatFar, slot: number, x0: number, y0: number, step: number, skirt: number, ground: (x: number, y: number) => number): void;

// src/render/flatAtlas.ts:231-960 function createFlatAtlas
export function createFlatAtlas(anisotropy: number): TerrainSurface;

// src/render/flatAtlas.ts:431-473 function createFlatAtlas.ensure
const ensure = (p: Plate): TerrainSurface =>;

// src/render/flatAtlas.ts:701-704 function createFlatAtlas.shaperFor
/** A road field asked in a plate's own coordinates. */
  const shaperFor = (p: Plate, shape: TerrainShaper): TerrainShaper => …;
