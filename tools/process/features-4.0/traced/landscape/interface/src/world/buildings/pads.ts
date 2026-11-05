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

// src/world/buildings/pads.ts:116-154 function padOf
/** One building's platform (its blocks' and its open lots' rings, levels and bounds); null for one with none. */
export function padOf(b: Building, naturalGround: GroundAt, pavedAt: PavedAt | undefined, apron: number,
  floorOf?: (b: Building) => number): Pad | null;

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

// src/world/buildings/pads.ts:330-347 class Grading
/** What `shapeOverPads` works in, point after point, owned by the platforms that grade (`buildingPads`, `padShaper`): each ring's bound and its distance (NaN: not measured), grown, never made per point; the answer, one for every point (DO NOT make an object a corner graded). */
class Grading { … }
