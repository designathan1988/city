// What the feature uses of src/world/buildings/validate.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/validate.ts:49-61 type BuildingProblem
/** Why a building cannot stand where it is. */
export type BuildingProblem =
  | 'cut'
  | 'setback'
  | 'roofSpace'
  | 'outline'
  | 'size'
  | 'overlap'
  | 'support'
  | 'footprint'
  | 'bounds'
  | 'road'
  | 'building'
  | 'slope';

// src/world/buildings/validate.ts:63-71 interface SiteContext
export interface SiteContext {
  readonly doc: RoadDoc;
  /** Null skips the road test (a headless check with no network built). */
  readonly net: Network | null;
  /** Null skips the slope test. */
  readonly groundAt: GroundAt | null;
  /** The paving's height (footways, carriageways), NaN off it: the street floor a grown building takes. */
  readonly pavedAt?: (x: number, y: number) => number;
}
