// What the feature uses of src/world/roads/furnitureSets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/furnitureSets.ts:13-13 type FurnitureSet
/** THE FURNITURE A NEW ROAD IS BUILT WITH */
export type FurnitureSet = 'complete' | 'basic' | 'none';

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
