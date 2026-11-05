// What the feature uses of src/world/elevation.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/elevation.ts:135-160 interface RoadElevation
/** The solved field: its reads (`roadField.ts`) and what the ground and the cache ask of it. */
export interface RoadElevation extends RoadReads {
  /** The solved height of a node, shared by every road that meets there. */
  nodeHeight(node: NodeId): number;
  /** Whether any road of these structures exists at all. */
  has(structures?: ReadonlySet<RoadStructure>): boolean;
  /** How far the ground should be pulled towards the road at a point, and to what height. */
  shapeAt(x: number, y: number, naturalGround: number): { height: number; weight: number };
  /** Boxes outside which `shapeAt` always answers weight 0 */
  shapeBounds(): readonly Aabb[];
  /** A digest of everything a height, frame or road query can read at any point of a rectangle, the terrain aside */
  digest(minX: number, minY: number, maxX: number, maxY: number, reach?: number): number;
  /** Where this solve and another can differ: the boxes */
  differences?(other: RoadElevation): { minX: number; minY: number; maxX: number; maxY: number }[] | null;
  /** Each road's whole solved profile as a digest, with its box (for `differences`). */
  summaries?(): ReadonlyMap<SegmentId, { digest: number; box: Aabb; oversized: boolean }>;
  /** The profiles a query at a point of `rects` can read, and the roads `also` */
  fieldFor?(rects: readonly (readonly [number, number, number, number])[], also: readonly SegmentId[]): { data: FieldData; transfer: ArrayBuffer[] };
  /** The roads `shapeAt` and the reads can meet at a point of `rects` */
  shapeCut?(rects: readonly (readonly [number, number, number, number])[]): RoadShapeCut;
  /** How many road solves this build ran (a road solved again in a later round counts again): what an edit cost. */
  readonly solved?: number;
  /** How many roads a scoped solve left unsolved (`SolveScope`): 0 or absent, every road is solved. */
  readonly pending?: number;
  /** The roads a scoped solve left unsolved, in id order: each answered as if absent until a later solve takes it. */
  pendingRoads?(): SegmentId[];
}

// src/world/elevation.ts:169-174 interface SolveScope
/**
 * THE ROADS A SOLVE TAKES NOW, THE OTHERS LEFT FOR LATER (`buildRoadElevation` `scope`): an opening solves the roads round its view and
 * shows the town, the rest is solved after, a share a frame, each share from the solve before (the incremental path an edit takes).
 * The roads not solved yet are absent from the solve: its answer is that of the network without them, and once every one is taken it
 * is the whole solve's, bit for bit (`tests/world/elevationScope.spec.ts`). A road's heights read only its two nodes', and a node's only
 * its roads' grade lines, so the roads well inside a region grown by a margin of a junction or two are already the whole solve's.
 */
export interface SolveScope {
  /** The roads whose bounds meet this rectangle (`Network.segmentCells`). */
  readonly region?: ChangeRect;
  /** And these roads. */
  readonly roads?: Iterable<SegmentId>;
}

// src/world/elevation.ts:212-212 variable SHAPE_INNER
/** Extra width past the casing over which the ground is held at road level. */
const SHAPE_INNER = m(1.2);

// src/world/elevation.ts:220-220 variable CUT_SHOULDER
/** The same batter for a tunnel's approach cutting, and much narrower. */
const CUT_SHOULDER = m(6.4);

// src/world/elevation.ts:222-222 variable CUT_WALL_REACH
/** How far past a road's casing a cutting held by retaining walls is dug (`RoadSegment.cutWalls`) */
export const CUT_WALL_REACH = SHAPE_INNER + CUT_SHOULDER;

// src/world/elevation.ts:232-237 interface StationHint
/** A road's ground read off the page (`roadStations`, the job system's `roadStations`), for the solve that reads it next: its line and casing as read, so the solve takes it only for the same road on the same ground. */
export interface StationHint {
  readonly xy: Float64Array;
  readonly half: number;
  readonly ceil: readonly number[];
  readonly base: readonly number[];
}

// src/world/elevation.ts:279-824 function buildRoadElevation
/** Solves the height of every road surface in the network: from nothing, or from a solve before it, where the network changed. */
export function buildRoadElevation(
  net: Network,
  terrainAt: (x: number, y: number) => number,
  /** A solve of this network on the SAME ground (`terrainAt` answering the same heights, through whatever function) */
  previous?: RoadElevation | null,
  /** The ground under some roads read off the page on this same ground (`roadStations`): taken for a road whose line and casing are the hint's. */
  hints?: ReadonlyMap<SegmentId, StationHint>,
  /** Where the ground moved since `previous` (a stroke of the terrain): the roads there solved again on the ground as it is, the others kept (DO NOT solve every road of the region again after a stroke). Null: `previous` is on the same ground. */
  groundMoved: readonly ChangeRect[] | null = null,
  /** The roads to solve now of those not solved yet (`SolveScope`), the others left for a later solve; null: every road. */
  scope: SolveScope | null = null,
): RoadElevation;

// src/world/elevation.ts:840-843 interface RoadShapeCut
/** The roads a shaping of the ground can read inside some rectangles */
export interface RoadShapeCut {
  readonly field: FieldData;
  readonly walls: Uint8Array;
}
