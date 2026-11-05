// The code of src/world/lots.ts that is the feature's (the rest of the file is shared).

// src/world/lots.ts:209-210 slice of src/world/lots.ts#plannedStreet (shared): the feature's statements; "…" marks code left out
const plannedStreet = (doc: RoadDoc, r: SegmentRibbon): boolean =>
  doc.segment(r.id)?.structure === 'ground' && carriesPedestrians(r.road);
