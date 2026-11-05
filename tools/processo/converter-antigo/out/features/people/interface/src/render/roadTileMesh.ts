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

// src/render/roadTileMesh.ts:38-74 function readsOf
/** The reads of `SurfaceData`, the ground from `heights` (a point missing pushed onto `missing`, read NaN). */
function readsOf(data: SurfaceData, heights: PointHeights, missing: number[]): SurfaceReads;
