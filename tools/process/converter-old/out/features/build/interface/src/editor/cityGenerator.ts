// What the feature uses of src/editor/cityGenerator.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/cityGenerator.ts:46-62 function greenCity
/** The generated city's green (V7): a square in each neighbourhood */
export function greenCity(doc: RoadDoc, net: Network, seed: number): { squares: number; pieces: number };

// src/editor/cityGenerator.ts:65-95 function zoneCity
/** Every lot zoned by its district */
export function zoneCity(doc: RoadDoc, net: Network, plan: CityPlan): number;

// src/editor/cityGenerator.ts:97-104 function nearAny
function nearAny(p: Vec2, segs: readonly [Vec2, Vec2][], d: number): boolean;
