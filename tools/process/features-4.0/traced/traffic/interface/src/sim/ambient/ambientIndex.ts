// What the feature uses of src/sim/ambient/ambientIndex.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/ambient/ambientIndex.ts:17-27 function buildingsChangedSince
/** The buildings the diary names as changed after `serial`; null when they must all be read again */
export function buildingsChangedSince(journal: ChangeJournal, serial: number): Set<number> | null;

// src/sim/ambient/ambientIndex.ts:30-30 function cellKey
/** A cell's key from its column and row (both within ±32 767), as `world/cells.ts`. */
export const cellKey = (cx: number, cy: number): number => …;

// src/sim/ambient/ambientIndex.ts:35-35 function spotKey
/** DO NOT key a bay by text: its place as one number, in steps of 0.1 m, one to a place within 3,355 km of the origin (6,710 km across) */
export const spotKey = (x: number, y: number): number => …;

// src/sim/ambient/ambientIndex.ts:66-68 constructor SightWalls.constructor
constructor();

// src/sim/ambient/ambientIndex.ts:230-235 constructor BuildingMix.constructor
constructor();
