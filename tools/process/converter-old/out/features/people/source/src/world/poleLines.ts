// The code of src/world/poleLines.ts that is the feature's (the rest of the file is shared).

// src/world/poleLines.ts:150-156 slice of src/world/poleLines.ts#polesInclude (shared): the feature's statements; "…" marks code left out
function polesInclude(net: Network): (id: SegmentId) => boolean {
  return (id) => {
    if (deckOf(net.doc, id) !== 'ground') return false;
    // … 1 lines
    return !!ribbon && carriesPedestrians(ribbon.road) && ribbon.road.sidewalk > 0;
  };
}
