// What the feature uses of src/render/roadTileMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadTileMesh.ts:14-25 interface SurfaceData
/** THE ROADS' SURFACE TILES MADE WHOLE OFF THE PAGE (`roadTile.worker.ts`): each tile's bands shaped */

/** Everything a share of a pass's tiles reads besides the ground: the roads near them, as typed arrays and small records. */
export interface SurfaceData {
  /** The solved profiles a query in the tiles can read (`RoadElevation.fieldFor`). */
  readonly field: FieldData;
  /** The looks of those roads and of the pass's chain. */
  readonly looks: readonly (readonly [SegmentId, RoadLook])[];
  /** The roads with a footway laid flush whose filed box meets the tiles (`Network.flushCells`). */
  readonly flush: FlushData;
  /** The dropped kerbs filed in the cells of the tiles, of the pass's structure. */
  readonly ramps: readonly CurbRamp[];
  /** The footways' outer edges in the cells of the tiles (`footwayEdgeCell`), four numbers an edge. */
  readonly edges: { readonly keys: Float64Array; readonly ends: Uint32Array; readonly xy: Float64Array };
}

// src/render/roadTileMesh.ts:28-32 interface MeshAsk
/** What a share of a pass's tiles is made from. */
export interface MeshAsk {
  readonly pass: PassDesc;
  readonly tiles: readonly { readonly rect: TileRect; readonly into: TilePolys }[];
  readonly data: SurfaceData;
}

// src/render/roadTileMesh.ts:35-35 type TileMeshes
/** A tile's surfaces, by name, in the pass's order, as handed back. */
export type TileMeshes = [string, Tile][];

// src/render/roadTileMesh.ts:38-74 function readsOf
/** The reads of `SurfaceData`, the ground from `heights` (a point missing pushed onto `missing`, read NaN). */
function readsOf(data: SurfaceData, heights: PointHeights, missing: number[]): SurfaceReads;

// src/render/roadTileMesh.ts:78-78 field TileMesher.heights
private readonly heights = new PointHeights();

// src/render/roadTileMesh.ts:96-112 method TileMesher.need
/** The points of the surfaces that read the ground at every vertex (the verge on the ground), each once: x, y. */
  need(): Float64Array;

// src/render/roadTileMesh.ts:115-121 method TileMesher.give
/** The ground's heights at `points` (x, y each), as the page reads it. */
  give(points: Float64Array, heights: Float64Array): void;

// src/render/roadTileMesh.ts:124-127 method TileMesher.renew
/** The reads made afresh: a field that read NaN for a point missing remembers it (`roadField.ts` `last`). */
  private renew(): void;

// src/render/roadTileMesh.ts:130-158 method TileMesher.run
/** Meshes every tile not made yet: the tiles, or the points still missing (`give` them, then `run` again). */
  run(beat?: () => void): { tiles: TileMeshes[] } | { need: Float64Array };
