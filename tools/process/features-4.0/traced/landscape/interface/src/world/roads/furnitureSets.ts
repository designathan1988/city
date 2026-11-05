// What the feature uses of src/world/roads/furnitureSets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/furnitureSets.ts:13-13 type FurnitureSet
/** THE FURNITURE A NEW ROAD IS BUILT WITH */
export type FurnitureSet = 'complete' | 'basic' | 'none';

// src/world/roads/furnitureSets.ts:20-28 interface Pattern
interface Pattern {
  readonly kind: LandscapeKind;
  /** Spacing along the side, world units. */
  readonly every: number;
  /** First station past the corner, world units. */
  readonly first: number;
  /** Only on the right side of a -> b (one a street, as hydrants stand). */
  readonly rightOnly?: true;
}

// src/world/roads/furnitureSets.ts:32-35 variable SETS
/** Each set's patterns, from the catalogue (`furnitureCatalog.ts`), in its order. */
const SETS: Readonly<Record<Exclude<FurnitureSet, 'none'>, readonly Pattern[]>> = {
  complete: patternsOf('complete'),
  basic: patternsOf('basic'),
};

// src/world/roads/furnitureSets.ts:36-39 function patternsOf
function patternsOf(set: 'complete' | 'basic'): Pattern[];

// src/world/roads/furnitureSets.ts:42-42 variable CELL
/** The cell of the nearby-items grid, world units. */
const CELL = m(40);

// src/world/roads/furnitureSets.ts:44-44 variable NEAR_CELL
/** The cell a street's items are filed in for each piece's look round it (`SnapLocal.around`). */
const NEAR_CELL = m(8);

// src/world/roads/furnitureSets.ts:47-63 function nearGrid
/** A street's items filed by place: a piece looks at those round it, never the street's every item. */
function nearGrid(): { file(item: LandscapeItem): void; around(at: Vec2, radius: number): LandscapeItem[] };

// src/world/roads/furnitureSets.ts:65-68 interface FurniturePlacement
export interface FurniturePlacement {
  readonly kind: LandscapeKind;
  readonly at: Vec2;
}

// src/world/roads/furnitureSets.ts:71-164 function furnitureFor
/** The pieces of `set` along the given segments, each where the landscaping tool would accept it among `existing` */
export function furnitureFor(net: Network, segments: Iterable<SegmentId>, set: FurnitureSet,
  /** The items standing to place among; absent (the way to call it): the document's own, asked by place round each street */
  existing: Iterable<LandscapeItem> | null,
  /** The left side carries a power line whose poles hold the street lights (`editor/roads/powerLine.ts`): no lamp columns there. */
  powerLeft = false,
  /** Street trees, on the footways and down a planted median; the rest of the set without them when false. */
  trees = true): FurniturePlacement[];
