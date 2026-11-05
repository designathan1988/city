// What the feature uses of src/world/cityGen/squares.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/squares.ts:17-22 interface SquarePlan
export interface SquarePlan {
  readonly lots: readonly number[];
  readonly centre: Vec2;
  readonly dabs: readonly PaintDab[];
  readonly trees: readonly PlantedTree[];
}

// src/world/cityGen/squares.ts:100-152 function planSquares
/** The squares of a city laid on `doc` with its lots cut. */
export function planSquares(doc: RoadDoc, seed: number): SquarePlan[];
