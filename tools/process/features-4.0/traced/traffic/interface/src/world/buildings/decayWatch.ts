// What the feature uses of src/world/buildings/decayWatch.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/decayWatch.ts:8-58 function createDecayWatch
/** DO NOT read `change.ids ?? []`: after a load the new town never ran down and the old one stayed due. */
export function createDecayWatch(doc: RoadDoc, wear: (id: BuildingId, decay: number) => void): (now: number) => boolean;
