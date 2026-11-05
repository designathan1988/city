// What the feature uses of src/world/buildings/blueprints.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/blueprints.ts:21-21 type BlueprintBody
/** Reusable building definitions: everything a building is except where it stands. */
export type BlueprintBody = Omit<Building, 'id' | 'x' | 'y' | 'rotation'>;

// src/world/buildings/blueprints.ts:23-29 interface Blueprint
export interface Blueprint {
  readonly key: string;
  /** Translation key for a built-in preset; user blueprints carry `name`. */
  readonly nameKey?: string;
  readonly name?: string;
  readonly body: BlueprintBody;
}
