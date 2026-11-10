// What the feature uses of src/render/networkFarRoadsBuild.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/networkFarRoadsBuild.ts:18-28 interface FarRoadInput
/**
 * THE WHOLE NETWORK'S FAR ROADS, MADE FROM THE NETWORK ITSELF (`networkFarRoads.ts`): every road's and junction's real outline (the
 * network's own rings: the carriageway, and the casing its footways and kerbs fill) laid at its height and filled, a cell at a time.
 * Pure, run by a job worker (`networkFarRoads.worker.ts`) or, where none can be made, on the page.
 *
 * DO NOT fill the outlines as they come, one over the other: the casing's outline spans the carriageway too, and two coplanar layers
 * told apart by a polygon offset alone fight triangle by triangle in a floating-point depth buffer (its minimum resolvable difference
 * is per primitive, 2^(e - n) of the largest depth the triangle spans: OpenGL ES 3.0 spec, 3.6.5 "Depth Offset"), the saw-tooth of
 * step-25k 89-ne-district (2026-10-21). Each cell's layers are made disjoint instead: the carriageways merged (each class under the
 * one before it cut away), the casing merged and the carriageways cut out of it (the band between the kerb and the footway's edge),
 * each clipped to its cell exactly (a cell is a road block, `roadSurfaces.ts` CHUNK: its detailed blocks replace it whole).
 */

/** What the page hands over: each outline's vertices (x, y, height), how many each has, its layer (0 the casing, 1 the carriageway) and its colour (linear rgb). */
export interface FarRoadInput {
  /** The cells' side and the grid's origin, world units (the road blocks', `roadSurfaces.ts` CHUNK tiles). */
  readonly cell: number;
  readonly origin: number;
  /** The area whose roads are drawn by their detailed surfaces already (the opening's tiles): cut out of every cell. Null: none. */
  readonly exclude: readonly [number, number, number, number] | null;
  readonly points: Float64Array;
  readonly counts: Uint32Array;
  readonly layers: Uint8Array;
  readonly colors: Float32Array;
}
