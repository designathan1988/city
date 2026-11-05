// What the feature uses of src/world/buildings/lotTemplate.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/lotTemplate.ts:5-11 interface LotTemplate
/** A lot kept in the lot lab's library (`?lab=lots`, `editor/lotLab.ts`, `lot-library-plugin.ts`) */
export interface LotTemplate {
  readonly name: string;
  readonly use: ZoneUse;
  readonly density: ZoneDensity;
  readonly savedAt: string;
  readonly body: Omit<Building, 'id' | 'x' | 'y' | 'rotation' | 'builtAt' | 'decay'>;
}
