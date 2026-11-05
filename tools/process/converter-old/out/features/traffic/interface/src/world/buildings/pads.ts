// What the feature uses of src/world/buildings/pads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/pads.ts:24-27 interface PadLevel
/** The platform's height under a ring, as plain numbers (`padLevelAt`) */
export interface PadLevel {
  readonly slope: LotSlope;
  readonly under: number;
}

// src/world/buildings/pads.ts:32-45 interface Pad
export interface Pad {
  readonly rings: readonly (readonly Vec2[])[];
  /** The platform's height under each ring (`padLevelAt`). */
  readonly levels: readonly PadLevel[];
  readonly water: readonly boolean[];
  /** Whether each ring is a paved lot (anything but grass and water). */
  readonly paved: readonly boolean[];
  readonly solidCount: number;
  /** Whether its blocks stand at floors of their own (a split level). */
  readonly stepped: boolean;
  readonly box: Aabb;
  /** Each ring's own bounds, for the nearest-ring search's lower bound (`shapeAt`). */
  readonly boxes: readonly Aabb[];
}

// src/world/buildings/pads.ts:47-52 interface BuildingPads
export interface BuildingPads {
  /** The graded height at a point, and how fully it replaces the land (0 or 1): one answer reused, read before the next call. */
  shapeAt(x: number, y: number, naturalGround: number): { height: number; weight: number };
  shapeBounds(): readonly Aabb[];
  readonly count: number;
}

// src/world/buildings/pads.ts:157-162 interface TownPads
/** The town's platforms, one put in, changed or taken out by its building's id (`put`). */
export interface TownPads extends BuildingPads {
  /** A building's platform now (null: it has none, or is gone). */
  put(id: number, pad: Pad | null): void;
  /** The platforms a point inside `boxes` can read, as plain data for the ground's worker (`padShaper`). */
  cut(boxes: readonly (readonly [number, number, number, number])[]): PadShapeCut;
}

// src/world/buildings/pads.ts:165-170 interface PadShapeCut
/** The town's platforms a shaping of the ground can read inside some boxes, as plain data */
export interface PadShapeCut {
  readonly apron: number;
  readonly pads: readonly Pad[];
  /** Each cell the boxes reach, by key: its platforms (indices into `pads`) in the town's order, empty for none. */
  readonly cells: ReadonlyMap<number, Int32Array>;
}

// src/world/buildings/pads.ts:184-311 function townPads
/** The platforms filed under every grid cell their box reaches: a corner of the ground asks only the ones over it. */
/** DO NOT hold every building's platform (its rings, levels and bounds: ~6.8 KB of the page's heap a building of the city, 2026-10-21): with `resolve`, each one's box is kept in columns by its id (`core/idSlots.ts`) and its id in the squares of `FILE_CELLS` cells it reaches (a platform reaches ~25 cells, ~4 squares); the platform itself is worked out again by `resolve` when a shaping reads it, the last `PADS_KEPT` kept. A cell's platforms are its square's whose box reaches it, in the square's order. */
function townPads(apron: number, resolve: ((id: number) => Pad | null) | null): TownPads;
