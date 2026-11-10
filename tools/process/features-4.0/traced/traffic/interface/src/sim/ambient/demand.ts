// What the feature uses of src/sim/ambient/demand.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/ambient/demand.ts:6-6 interface Mix
/** What the land use asks of the streets at each hour */

/** What kind of place the view is on: shares of each, from the buildings round it. */
export interface Mix { readonly home: number; readonly shop: number; readonly work: number; readonly civic: number }

// src/sim/ambient/demand.ts:9-22 function busy
/** How busy each kind of place is at an hour (0..1): bumps at the hours people go out (GTA's day of the popcycle). */
export function busy(kind: keyof Mix, hour: number): number;

// src/sim/ambient/demand.ts:35-44 function flows
/** People going in at and coming out of a kind of place at an hour (shares of everybody, an hour): from how many are there */
export function flows(kind: keyof Mix, hour: number): { in: number; out: number };

// src/sim/ambient/demand.ts:52-52 variable CARS
export const CARS: Mix = { home: 2.6, shop: 6, work: 4.8, civic: 3.6 };
