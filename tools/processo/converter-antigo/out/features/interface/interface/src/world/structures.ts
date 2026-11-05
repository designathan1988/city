// What the feature uses of src/world/structures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/structures.ts:5-5 type RoadStructure
/** The structural levels a road can be built at. */
export type RoadStructure = 'ground' | 'elevated' | 'bridge' | 'tunnel';

// src/world/structures.ts:11-15 function migrateStructure
/** The structure a stored id stands for today, or null when it is not one. */
export function migrateStructure(value: unknown): RoadStructure | null;

// src/world/structures.ts:17-27 interface RoadStructureSpec
export interface RoadStructureSpec {
  readonly id: RoadStructure;
  /** Translation key for the structure's name, resolved by `ui/i18n`. */
  readonly key: string;
  /** Height the deck keeps above the highest ground under the span, in units. */
  readonly clearance: number;
  /** Thickness of the structural deck drawn under the asphalt. */
  readonly deck: number;
  /** Whether the structure stands on piers. */
  readonly supports: boolean;
}

// src/world/structures.ts:55-56 function roadStructure
export const roadStructure = (id: RoadStructure): RoadStructureSpec => …;
