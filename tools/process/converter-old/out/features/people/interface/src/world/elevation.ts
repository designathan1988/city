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

// src/world/elevation.ts:208-208 variable GROUND_ONLY
export const GROUND_ONLY: ReadonlySet<RoadStructure> = new Set<RoadStructure>(['ground']);

// src/world/elevation.ts:840-843 interface RoadShapeCut
/** The roads a shaping of the ground can read inside some rectangles */
export interface RoadShapeCut {
  readonly field: FieldData;
  readonly walls: Uint8Array;
}
