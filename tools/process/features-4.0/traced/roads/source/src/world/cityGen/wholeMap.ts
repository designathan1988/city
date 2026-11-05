// The code of src/world/cityGen/wholeMap.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/wholeMap.ts:34-34 slice of src/world/cityGen/wholeMap.ts#WholeMapOptions (shared): the feature's statements; "…" marks code left out
export type WholeMapOptions = Omit<CityOptions, 'size' | 'radius'>;

// src/world/cityGen/wholeMap.ts:43-43 slice of src/world/cityGen/wholeMap.ts#EDGE_INSET (shared): the feature's statements; "…" marks code left out
export const EDGE_INSET = halfWidth(roadType(Math.max(0, ROAD_TYPES.findIndex((t) => t.id === 'avenue'))), Level.Sidewalk);
