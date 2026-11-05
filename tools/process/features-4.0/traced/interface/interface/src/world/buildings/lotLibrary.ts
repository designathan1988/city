// What the feature uses of src/world/buildings/lotLibrary.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/lotLibrary.ts:4-4 variable loaders
/** The lots made in the lot lab (`lots/*.json`, 2026-10-05): each a building of the tower kit with its whole lot */
const loaders = import.meta.glob<LotTemplate>('/lots/*.json', { import: 'default' });

// src/world/buildings/lotLibrary.ts:6-6 function nameOf
const nameOf = (path: string): string => …;

// src/world/buildings/lotLibrary.ts:9-9 variable LOT_NAMES
/** The names of the lots, in a stable order. */
export const LOT_NAMES: readonly string[] = Object.keys(loaders).map(nameOf).sort();
