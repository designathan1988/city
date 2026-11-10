// What the feature uses of src/world/buildings/floors.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/floors.ts:7-16 interface BuildingFloors
/** Nothing is evicted for room: DO NOT cap it. */
export interface BuildingFloors {
  /** The building's ground floor on the ground and the paving as they stand. */
  floorOf(b: Building): number;
  /** Forgets one building's floor (its site worked out again). */
  forget(b: Building): void;
  /** Forgets every floor (the roads or the land moved everywhere). */
  forgetAll(): void;
  /** Forgets the floors whose reach (`boxOf`) a change of the ground since the last `follow` touches; where the diary cannot say, those the regions the caller is shaping (`shaped`) reach, or every one without them. */
  follow(buildings: BuildingStore, ground: GroundSince, shaped?: readonly Rect[] | null): void;
}
