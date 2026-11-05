// The code of src/world/roads/furnitureCatalog.ts that is the feature's (the rest of the file is shared).

// src/world/roads/furnitureCatalog.ts:6-15 slice of src/world/roads/furnitureCatalog.ts#FurnitureEntry (shared): the feature's statements; "…" marks code left out
export interface FurnitureEntry {
  readonly kind: LandscapeKind;
// … 7 lines
}

// src/world/roads/furnitureCatalog.ts:28-28 slice of src/world/roads/furnitureCatalog.ts#furnitureEntry (shared): the feature's statements; "…" marks code left out
export const furnitureEntry = (kind: LandscapeKind): FurnitureEntry | undefined => FURNITURE_CATALOG.find((e) => e.kind === kind);
