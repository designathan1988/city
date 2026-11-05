// The code of src/world/buildings/serialize.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/serialize.ts:398-398 function isSerializedBuildings: the feature's own
export const isSerializedBuildings = (value: unknown): boolean => value === undefined || Array.isArray(value);
