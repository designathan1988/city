// What the feature uses of src/world/roads/templates.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/templates.ts:7-15 interface RoadTemplate
/** ROAD TEMPLATES: a profile with a name, to lay or apply again. */
export interface RoadTemplate {
  readonly id: string;
  /** A translation key for a built-in template; a player's template has a `name`. */
  readonly nameKey?: string;
  readonly name?: string;
  /** The class the profile is laid with (its colours, its rank, its turning rules). */
  readonly type: number;
  readonly profile: RoadProfileSpec;
}

// src/world/roads/templates.ts:21-28 function classTemplates
/** The classes as templates, in the order of the road tool. */
export function classTemplates(): RoadTemplate[];
