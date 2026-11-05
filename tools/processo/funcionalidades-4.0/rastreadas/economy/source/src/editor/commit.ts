// The code of src/editor/commit.ts that is the feature's (the rest of the file is shared).

// src/editor/commit.ts:63-159 slice of src/editor/commit.ts#commitRoadPath (shared): the feature's statements; "…" marks code left out
export function commitRoadPath(
// … 18 lines
): DraftResult {
  // … 64 lines
  if (options.dryRun) {
    // … 2 lines
    if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
    // … 5 lines
    return { committed: true, heightLimited, finalHeightOffset: currentHeight, cost: charge.amount, stations };
  }
// … 1 lines
}

// src/editor/commit.ts:162-172 slice of src/editor/commit.ts#layCopy (shared): the feature's statements; "…" marks code left out
function layCopy(doc: RoadDoc, net: Network, work: RoadDoc, workNet: Network, bore: { bored: boolean; elevation: RoadElevation | null },
  heightLimited: boolean, finalHeightOffset: number, ground: ((x: number, y: number) => number) | undefined, timed: (what: string) => void): DraftResult {
  // … 1 lines
  if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
  // … 4 lines
  return { committed: true, heightLimited, finalHeightOffset, cost: charge.amount, laid,
    ...(!bore.bored && ground && bore.elevation ? { elevation: bore.elevation } : {}) };
}

// src/editor/commit.ts:747-765 slice of src/editor/commit.ts#moveNodeChecked (shared): the feature's statements; "…" marks code left out
export function moveNodeChecked(doc: RoadDoc, net: Network, id: NodeId, to: Vec2): DraftResult {
  // … 13 lines
  if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
  // … 2 lines
  return { ...result, cost: charge.amount };
}
