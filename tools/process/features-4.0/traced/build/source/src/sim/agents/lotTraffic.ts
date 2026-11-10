// The code of src/sim/agents/lotTraffic.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lotTraffic.ts:715-718 slice of src/sim/agents/lotTraffic.ts#kindClass (shared): the feature's statements; "…" marks code left out
function kindClass(w: SimWorld, building: BuildingId): number {
  const b = w.buildings.facts(building);
  return b ? KINDS.indexOf(b.kind) : GONE;
}
