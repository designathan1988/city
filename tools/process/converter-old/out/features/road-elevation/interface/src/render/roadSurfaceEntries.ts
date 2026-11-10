// What the feature uses of src/render/roadSurfaceEntries.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadSurfaceEntries.ts:12-12 variable TILE
/** THE ROAD SURFACES' INPUTS AS THEY ARE FILED (`roadSurfaceInputs.ts`): the tiles, the entries, their kinds and the order a walk of the map
 *  meets them in, and what the inputs held round the view count an item by (`InputResidency`) - plain data and pure functions. */

/** World size of one tile. */
export const TILE = m(76.8);

// src/render/roadSurfaceEntries.ts:14-14 variable TILE_ORIGIN
/** Where the tile grid starts. */
export const TILE_ORIGIN = m(0.1484);

// src/render/roadSurfaceEntries.ts:26-32 interface Reach
/** Everything that reaches a tile of a pass, by kind, in input order. */
export interface Reach {
  readonly levels: Record<'casing' | 'sidewalk' | 'curb' | 'asphalt', readonly Input[]>;
  readonly medians: Record<'kerb' | 'planting' | 'paved', readonly Input[]>;
  readonly ramps: readonly Input[];
  readonly ribbons: readonly Input[];
  readonly quads: ReadonlyMap<string, readonly Input[]>;
}

// src/render/roadSurfaceEntries.ts:58-58 function paintSlot
export const paintSlot = (color: string): string => …;

// src/render/roadSurfaceEntries.ts:62-66 variable PHASE
/** The phases of the walk the inputs are ordered by, as `roadSurfaces.ts` once walked the map */
export const PHASE = {
  road: 0, junction: 1,
  taper: 1, mini: 2,
  lines: 0, parking: 1, taperPaint: 2, stops: 3, midStops: 4, zebras: 5, midZebras: 6, islands: 7, legends: 8,
} as const;

// src/render/roadSurfaceEntries.ts:100-105 interface SurfacePass
/** A pass of the surfaces as the build walks it: a structural level, or an alignment raised by hand. */
export interface SurfacePass {
  readonly id: string;
  readonly structure: RoadStructure;
  readonly segment?: SegmentId;
  readonly chain?: readonly SegmentId[];
}

// src/render/roadSurfaceEntries.ts:187-187 function alignmentPass
export const alignmentPass = (root: SegmentId): string => …;
