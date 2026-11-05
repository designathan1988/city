// What the feature uses of src/editor/blocks.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/blocks.ts:10-17 interface BlockGrid
/** Several blocks at once: a grid of streets laid in one gesture */
export interface BlockGrid {
  readonly cols: number;
  readonly rows: number;
  /** Distance between street centrelines, metres (whole metres: the zoning subgrid). */
  readonly blockMetres: number;
  /** Turn of the grid, radians. */
  readonly angle: number;
}

// src/editor/blocks.ts:20-29 function blockGridLines
/** The street lines of a grid, as pairs of world points. */
export function blockGridLines(centre: Vec2, grid: BlockGrid): [Vec2, Vec2][];

// src/editor/blocks.ts:32-49 function commitBlockGrid
/** DO NOT make a network per line: each was built from nothing over the whole map, as many times as the grid has lines. */
export function commitBlockGrid(
  doc: RoadDoc, net: Network, centre: Vec2, grid: BlockGrid, type: number, lanes: number | null, parking?: SegmentParking,
): number;
