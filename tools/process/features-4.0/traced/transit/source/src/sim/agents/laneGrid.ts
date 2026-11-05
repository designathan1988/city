// The code of src/sim/agents/laneGrid.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/laneGrid.ts:188-192 function laneChangesSince: the feature's own
export function laneChangesSince(w: LaneGridWorld, seen: object | null): { readonly state: object; readonly boxes: readonly ChangeRect[] | null } {
  const grid = laneGrid(w);
  const since = grid.changes.since(seen);
  return { state: grid.changes.state, boxes: since === null ? null : since.flat() };
}
