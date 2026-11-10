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

// src/world/buildings/pads.ts:165-170 interface PadShapeCut
/** The town's platforms a shaping of the ground can read inside some boxes, as plain data */
export interface PadShapeCut {
  readonly apron: number;
  readonly pads: readonly Pad[];
  /** Each cell the boxes reach, by key: its platforms (indices into `pads`) in the town's order, empty for none. */
  readonly cells: ReadonlyMap<number, Int32Array>;
}
