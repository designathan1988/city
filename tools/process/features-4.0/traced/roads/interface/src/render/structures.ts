// What the feature uses of src/render/structures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/structures.ts:39-45 interface StructureDetails
/** The parts of a raised structure that are not its deck */

export interface StructureDetails {
  readonly group: Group;
  readonly triangles: number;
  /** Where the structures built stand */
  readonly spans: readonly (readonly [number, number, number, number])[];
  dispose(): void;
}

// src/render/structures.ts:48-48 variable SPAN_REACH
/** Room round a structure's road that its details read (abutments, casings met, portals). */
export const SPAN_REACH = m(24);

// src/render/structures.ts:51-85 function structureRibbons
/** DO NOT walk every ribbon of the map for a box. */
function structureRibbons(
  net: Network,
  elevation: RoadElevation,
  terrainAt: (x: number, y: number) => number,
  within?: readonly (readonly [number, number, number, number])[],
  candidates?: readonly SegmentRibbon[],
): { raised: SegmentRibbon[]; tunnels: SegmentRibbon[]; walled: SegmentRibbon[] };

// src/render/structures.ts:88-88 variable WALL_THICK
/** A retaining wall's thickness, its coping over the ground it holds, its footing under the road. */
const WALL_THICK = m(0.32);

// src/render/structures.ts:89-89 variable WALL_COPING
const WALL_COPING = m(0.18);

// src/render/structures.ts:90-90 variable WALL_FOOT
const WALL_FOOT = m(0.24);

// src/render/structures.ts:92-92 variable WALL_STEP
/** Spacing of a wall's sections along the road. */
const WALL_STEP = m(0.8);

// src/render/structures.ts:95-152 function retainingWalls
/** THE RETAINING WALLS of a cutting (`RoadSegment.cutWalls`) */
function retainingWalls(
  net: Network, walled: readonly SegmentRibbon[], elevation: RoadElevation, terrainAt: (x: number, y: number) => number,
  faces: Extrusion, lids: Extrusion, faceTile: number, lidTile: number,
): void;

// src/render/structures.ts:155-155 variable BEARING
/** Bearing inset: the deck rests ON the pier, so its top stops just under it. */
const BEARING = m(0.08);

// src/render/structures.ts:157-157 variable PIER_SLENDERNESS
/** How slender a column is allowed to be: height divided by width. */
const PIER_SLENDERNESS = 8;

// src/render/structures.ts:159-159 variable PIER_MAX_WIDTH_FACTOR
/** Nothing gets fatter than this, however tall the deck. */
const PIER_MAX_WIDTH_FACTOR = 1.8;

// src/render/structures.ts:161-161 variable MIN_SUPPORT
/** Shortest pier worth building. Below this the deck is on the ground. */
const MIN_SUPPORT = m(0.36);

// src/render/structures.ts:163-163 variable BENT_SPREAD
/** Where a bent's columns stand, as a fraction of the deck's half-width. */
const BENT_SPREAD = 0.55;

// src/render/structures.ts:165-165 variable CAP_DEPTH
/** Depth of the crossbeam along the road, as a multiple of a column's radius. */
const CAP_DEPTH = 2.2;

// src/render/structures.ts:167-167 variable CAP_HEIGHT
/** Depth of the crossbeam the columns carry. */
const CAP_HEIGHT = m(0.72);

// src/render/structures.ts:169-169 variable CAP_OVERHANG
/** How far the crossbeam reaches past the outermost column. */
const CAP_OVERHANG = m(0.64);

// src/render/structures.ts:181-181 variable ABUTMENT_LIFT
/** Height of the deck over the ground at which a ramp leaves its solid approach for the first span on piers */
const ABUTMENT_LIFT = m(0.48);

// src/render/structures.ts:183-183 variable ABUTMENT_DEPTH
/** Depth of an abutment along the road. */
const ABUTMENT_DEPTH = m(1.6);

// src/render/structures.ts:185-185 variable PIER_ABUTMENT_GAP
/** How far a pier must stand from an abutment: less than this and it is one wall. */
const PIER_ABUTMENT_GAP = m(10.4);

// src/render/structures.ts:187-187 variable PORTAL_WING
/** How far a tunnel portal's face reaches past the road, to close the cutting. */
const PORTAL_WING = m(5.6);

// src/render/structures.ts:189-189 variable PORTAL_THICKNESS
/** Depth of the portal face along the road. Thick enough to cover a grid cell. */
const PORTAL_THICKNESS = m(1.6);

// src/render/structures.ts:191-191 variable PORTAL_SLICE
/** Width of each slice of a portal's wing walls, which step down with the ground beside it. */
const PORTAL_SLICE = m(1.2);

// src/render/structures.ts:193-193 variable PORTAL_COPING
/** How far a portal's wing wall stands over the ground at its own place. */
export const PORTAL_COPING = m(0.24);

// src/render/structures.ts:195-203 interface Placement
interface Placement {
  readonly x: number;
  readonly y: number;
  readonly yaw: number;
  readonly sx: number;
  readonly sy: number;
  readonly sz: number;
  readonly cy: number;
}

// src/render/structures.ts:206-206 variable PIER_FOOT_FLARE
/** A column's foot over its width: `CylinderGeometry(1, 1.08, ...)` scaled by the width (`buildStructureDetails`). */
const PIER_FOOT_FLARE = 1.08;

// src/render/structures.ts:212-221 function onAnotherCasing
/** DO NOT file a road by the cells round its vertices again */
export function onAnotherCasing(net: Network, self: SegmentId, x: number, y: number, clearance = 0): boolean;

// src/render/structures.ts:252-262 function boxAlong
/** A box placement whose LENGTH runs along the road and whose WIDTH runs across it, in world units. */
function boxAlong(frame: Frame, along: number, across: number, height: number, centreY: number, offset = 0): Placement;

// src/render/structures.ts:265-346 class Extrusion
/** Flat-shaded quads, indexed, built in world coordinates and mapped into three's frame once. */
class Extrusion { … }

// src/render/structures.ts:371-428 function parapetRun
/** The barrier along one edge of a raised deck, as a continuous extrusion. */
function parapetRun(
  out: Extrusion,
  centre: Polyline,
  side: 1 | -1,
  edge: number,
  deckAt: (x: number, y: number) => number,
  terrainAt: (x: number, y: number) => number,
  tile: number,
): void;

// src/render/structures.ts:431-436 interface StructureSheet
/** A sheet of a structure's quads (parapets, retaining walls, their backfill), as arrays. */
export interface StructureSheet {
  readonly positions: Float32Array<ArrayBuffer>;
  readonly normals: Float32Array<ArrayBuffer>;
  readonly uvs: Float32Array<ArrayBuffer>;
  readonly index: Uint16Array<ArrayBuffer> | Uint32Array<ArrayBuffer>;
}

// src/render/structures.ts:439-450 interface StructureParts
/** THE STRUCTURES OF SOME ROADS MADE, WITH NO MATERIAL (`buildStructureDetails`): each kind's instances' matrices, the sheets of quads, where they stand */
export interface StructureParts {
  readonly piers: Float32Array<ArrayBuffer>;
  readonly caps: Float32Array<ArrayBuffer>;
  readonly abutments: Float32Array<ArrayBuffer>;
  readonly jambs: Float32Array<ArrayBuffer>;
  readonly lintels: Float32Array<ArrayBuffer>;
  readonly headwalls: Float32Array<ArrayBuffer>;
  readonly parapets: StructureSheet | null;
  readonly walls: StructureSheet | null;
  readonly backfill: StructureSheet | null;
  readonly spans: readonly (readonly [number, number, number, number])[];
}

// src/render/structures.ts:452-465 function buildStructureDetails
export function buildStructureDetails(
  net: Network,
  elevation: RoadElevation,
  terrainAt: (x: number, y: number) => number,
  materials: SceneMaterials,
  /** The terrain's own material, for a retaining wall's backfill */
  groundMaterial?: Material,
  /** Only these roads' structures (a cell of the structure layer, `structureCells.ts`); all of the map without. */
  only?: readonly SegmentRibbon[],
  /** The shapes the instances are drawn with, kept by the caller across builds; made here (and let go with these) without. */
  shapes?: { readonly pier: BufferGeometry; readonly box: BufferGeometry },
): StructureDetails;

// src/render/structures.ts:468-672 function structureParts
/** The roads' structures made (`only`: these roads'; all of the map without); `deckTile`, the deck's texture scale (`SceneMaterials.scale.deck`). */
export function structureParts(net: Network, elevation: RoadElevation, terrainAt: (x: number, y: number) => number, deckTile: number, only?: readonly SegmentRibbon[]): StructureParts;

// src/render/structures.ts:675-715 function structureMeshes
/** THE STRUCTURES' MESHES from their parts (`structureParts`): the instances and sheets taken as they came; `groundMaterial`, the backfill's (none: no backfill drawn) */
export function structureMeshes(parts: StructureParts, materials: SceneMaterials, groundMaterial?: Material,
  shapes?: { readonly pier: BufferGeometry; readonly box: BufferGeometry }): StructureDetails;
