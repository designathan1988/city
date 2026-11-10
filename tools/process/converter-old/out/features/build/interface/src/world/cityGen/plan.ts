// What the feature uses of src/world/cityGen/plan.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/plan.ts:11-11 type CityStyle
/** A GENERATED CITY'S PLAN: what the player asks for (`CityOptions`) made into the city's grain */

export type CityStyle = 'grid' | 'organic' | 'radial' | 'mixed' | 'blocks';

// src/world/cityGen/plan.ts:39-39 type CitySize
export type CitySize = 'small' | 'medium' | 'large';

// src/world/cityGen/plan.ts:41-54 interface CityOptions
export interface CityOptions {
  readonly seed: number;
  readonly size: CitySize;
  readonly style: CityStyle;
  /** Downtowns, 1 to 4. */
  readonly centres: number;
  /** 0 a town of houses, 1 a city of towers. */
  readonly density: number;
  /** Share of blocks made parks, 0 to 0.2. */
  readonly parks: number;
  readonly industry: boolean;
  /** The city's radius in world units, over the size's (probes). */
  readonly radius?: number;
}

// src/world/cityGen/plan.ts:61-69 interface CityPlan
export interface CityPlan {
  readonly options: CityOptions;
  readonly field: TensorField;
  readonly radius: number;
  readonly centres: readonly Vec2[];
  readonly graph: StreetGraph;
  /** What the land at a point is for. */
  zoneAt(p: Vec2, onMain: boolean): { use: ZoneUse; density: ZoneDensity } | 'park';
}
