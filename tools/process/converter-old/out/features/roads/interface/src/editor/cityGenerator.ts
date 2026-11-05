// What the feature uses of src/editor/cityGenerator.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/cityGenerator.ts:19-28 function layCity
/** The city generator's hand on the map: the planned streets laid as roads */

/** A new document with the plan's streets: a node at every crossing and bend, a road between. */
export function layCity(plan: CityPlan): RoadDoc;

// src/editor/cityGenerator.ts:31-31 variable CITY_FOOTWAY
/** The generated city's footways, 3 m */
const CITY_FOOTWAY = m(3);

// src/editor/cityGenerator.ts:33-33 variable CITY_MEDIAN
/** The generated avenues' median, planted (a canteiro of 2 m: a tree pit with a kerb each side). */
const CITY_MEDIAN = m(2);

// src/editor/cityGenerator.ts:34-43 function widenCityFootways
export function widenCityFootways(doc: RoadDoc): void;

// src/editor/cityGenerator.ts:46-62 function greenCity
/** The generated city's green (V7): a square in each neighbourhood */
export function greenCity(doc: RoadDoc, net: Network, seed: number): { squares: number; pieces: number };

// src/editor/cityGenerator.ts:65-95 function zoneCity
/** Every lot zoned by its district */
export function zoneCity(doc: RoadDoc, net: Network, plan: CityPlan): number;

// src/editor/cityGenerator.ts:97-104 function nearAny
function nearAny(p: Vec2, segs: readonly [Vec2, Vec2][], d: number): boolean;
