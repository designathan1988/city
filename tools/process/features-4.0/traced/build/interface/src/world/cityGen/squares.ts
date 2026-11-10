// What the feature uses of src/world/cityGen/squares.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/squares.ts:13-13 variable NEIGHBOURHOOD
/** THE SQUARES OF A GENERATED CITY */

/** A neighbourhood's side: one square in each. */
export const NEIGHBOURHOOD = m(420);

// src/world/cityGen/squares.ts:15-15 variable AREA_RANGE
/** The block sizes a square is made of, square metres. */
const AREA_RANGE = [1500, 14000] as const;

// src/world/cityGen/squares.ts:17-22 interface SquarePlan
export interface SquarePlan {
  readonly lots: readonly number[];
  readonly centre: Vec2;
  readonly dabs: readonly PaintDab[];
  readonly trees: readonly PlantedTree[];
}

// src/world/cityGen/squares.ts:25-59 function blocks
/** The blocks: lots that touch (a corner within a metre of another's side) belong together. */
function blocks(lots: readonly Lot[]): Lot[][];

// src/world/cityGen/squares.ts:94-97 function rng
/** A small deterministic generator for the trees' looks. */
function rng(seed: number): () => number;

// src/world/cityGen/squares.ts:100-152 function planSquares
/** The squares of a city laid on `doc` with its lots cut. */
export function planSquares(doc: RoadDoc, seed: number): SquarePlan[];
