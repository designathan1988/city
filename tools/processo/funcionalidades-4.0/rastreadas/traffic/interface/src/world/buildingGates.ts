// What the feature uses of src/world/buildingGates.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildingGates.ts:13-13 variable entrancesOf
/** One building's entrances: its car gates and the edges of its parking, worked out once a record (records are replaced, never changed: `identityMemo`) - each street with parking read the buildings near it on both its sides, every one worked out again each time. */
export const entrancesOf: (b: Building) => readonly [Vec2, Vec2][] = identityMemo(workOutEntrances);
