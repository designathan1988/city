// What the feature uses of src/world/roads/catalog.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/catalog.ts:9-9 type CatalogCategory
/** THE ROAD CATALOGUE (the player's order of 2026-10-09) */
export type CatalogCategory = 'streets' | 'avenues' | 'highways' | 'special' | 'mine';

// src/world/roads/catalog.ts:10-10 variable CATALOG_CATEGORIES
export const CATALOG_CATEGORIES: readonly CatalogCategory[] = ['streets', 'avenues', 'highways', 'special', 'mine'];

// src/world/roads/catalog.ts:12-14 interface CatalogRoad
export interface CatalogRoad extends RoadTemplate {
  readonly category: CatalogCategory;
}
