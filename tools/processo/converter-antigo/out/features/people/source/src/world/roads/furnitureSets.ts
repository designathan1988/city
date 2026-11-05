// The code of src/world/roads/furnitureSets.ts that is the feature's (the rest of the file is shared).

// src/world/roads/furnitureSets.ts:71-164 slice of src/world/roads/furnitureSets.ts#furnitureFor (shared): the feature's statements; "…" marks code left out
export function furnitureFor(net: Network, segments: Iterable<SegmentId>, set: FurnitureSet,
  // … 5 lines
  trees = true): FurniturePlacement[] {
  // … 12 lines
  const streets = [...segments].sort((a, b) => a - b).filter((id) => {
    // … 1 lines
    return !!segment && !!ribbon && carriesPedestrians(ribbon.road) && ribbon.road.sidewalk > 0;
  });
// … 70 lines
}
