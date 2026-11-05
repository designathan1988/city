// What the feature uses of src/sim/ambient/demand.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/ambient/demand.ts:6-6 interface Mix
/** What the land use asks of the streets at each hour */

/** What kind of place the view is on: shares of each, from the buildings round it. */
export interface Mix { readonly home: number; readonly shop: number; readonly work: number; readonly civic: number }

// src/sim/ambient/demand.ts:55-62 function kindOf
/** What kind of place a building is, from its function or its zone (a record, or its traffic record). */
export function kindOf(b: Pick<Building, 'use' | 'function'>): keyof Mix;
