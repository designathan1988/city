// What the feature uses of src/world/buildings/serialize.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/serialize.ts:59-59 type SerializedBuilding
/** The stored shape of a building is the model itself, as plain JSON, with `schema` naming its version. */
export type SerializedBuilding = Building;

// src/world/buildings/serialize.ts:305-395 function migrateBuilding
/** Any stored building, of any schema this build knows, as a current one: DO NOT migrate one of its own again (a new object each time, every derived product kept by record - bounds, footprints, blocks - worked out again for it: a tile's bakes migrated each building twice). */
export function migrateBuilding(raw: unknown): Building | null;
