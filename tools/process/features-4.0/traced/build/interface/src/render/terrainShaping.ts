// What the feature uses of src/render/terrainShaping.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/terrainShaping.ts:7-7 type TerrainRegion
/** WHICH CORNERS OF A PLATE'S GROUND A SHAPING PASS ASKS */

/** A box of grid corners, inclusive: x0..x1 across, y0..y1 down. */
export type TerrainRegion = readonly [number, number, number, number];

// src/render/terrainShaping.ts:10-14 interface TerrainShaper
/** What `shapeToRoads` needs to know about the road network. */
export interface TerrainShaper {
  shapeAt(x: number, y: number, naturalGround: number): { height: number; weight: number };
  /** Boxes outside which `shapeAt` always answers weight 0. */
  shapeBounds(): readonly Aabb[];
}

// src/render/terrainShaping.ts:17-23 interface CornerGrid
/** A plate's grid of corners: `n` a side, corner `i` at `ix + iy * n`, and its world point. */
export interface CornerGrid {
  readonly n: number;
  readonly cell: number;
  readonly half: number;
  x(i: number): number;
  y(i: number): number;
}

// src/render/terrainShaping.ts:26-54 function shapeCorners
/** DO NOT walk `shapeBounds` against the regions: every box of the town */
export function shapeCorners(
  grid: CornerGrid,
  natural: ArrayLike<number>,
  shape: TerrainShaper,
  regions: readonly TerrainRegion[] | null,
  visited: Int32Array,
  pass: number,
  take: (i: number, height: number) => void,
): void;

// src/render/terrainShaping.ts:64-64 type ShapeAt
type ShapeAt = TerrainShaper['shapeAt'];

// src/render/terrainShaping.ts:67-75 function shapeBoth
/** The roads' and the buildings' platforms' shaping as one (`renderer.ts` `shapeGround`, and the ground's worker) */
export function shapeBoth(roads: { shapeAt: ShapeAt } | null, pads: { shapeAt: ShapeAt }): ShapeAt;
