// The code of src/world/solids.ts that is the feature's (the rest of the file is shared).

// src/world/solids.ts:147-152 function solidsOf: the feature's own
export function solidsOf(doc: RoadDoc): Solids {
  let made = known.get(doc);
  if (!made) known.set(doc, (made = createSolids()));
  made.derived.refresh(doc);
  return made.solids;
}
