// The code of src/editor/commit.ts that is the feature's (the rest of the file is shared).

// src/editor/commit.ts:44-57 slice of src/editor/commit.ts#DraftResult (shared): the feature's statements; "…" marks code left out
export interface DraftResult {
  // … 9 lines
  readonly stations?: readonly RoadStation[];
// … 2 lines
}

// src/editor/commit.ts:63-159 slice of src/editor/commit.ts#commitRoadPath (shared): the feature's statements; "…" marks code left out
export function commitRoadPath(
// … 18 lines
): DraftResult {
  // … 64 lines
  if (options.dryRun) {
    // … 1 lines
    const charge = settleRoadEdit(roadsBefore(doc), work, false);
    if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
    // … 5 lines
    return { committed: true, heightLimited, finalHeightOffset: currentHeight, cost: charge.amount, stations };
  }
// … 1 lines
}

// src/editor/commit.ts:162-172 slice of src/editor/commit.ts#layCopy (shared): the feature's statements; "…" marks code left out
function layCopy(doc: RoadDoc, net: Network, work: RoadDoc, workNet: Network, bore: { bored: boolean; elevation: RoadElevation | null },
  heightLimited: boolean, finalHeightOffset: number, ground: ((x: number, y: number) => number) | undefined, timed: (what: string) => void): DraftResult {
  const charge = settleRoadEdit(roadsBefore(doc), work);
  if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
  // … 4 lines
  return { committed: true, heightLimited, finalHeightOffset, cost: charge.amount, laid,
    ...(!bore.bored && ground && bore.elevation ? { elevation: bore.elevation } : {}) };
}

// src/editor/commit.ts:254-277 slice of src/editor/commit.ts#draftStations (shared): the feature's statements; "…" marks code left out
function draftStations(
// … 3 lines
): RoadStation[] {
  // … 17 lines
  return stationsAlong(workNet, elevation, ground, orderAlong(work, laid, startAt), STATION_STEP);
}

// src/editor/commit.ts:747-765 slice of src/editor/commit.ts#moveNodeChecked (shared): the feature's statements; "…" marks code left out
export function moveNodeChecked(doc: RoadDoc, net: Network, id: NodeId, to: Vec2): DraftResult {
  // … 12 lines
  const charge = settleRoadEdit(roadsBefore(doc), work);
  if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
  // … 2 lines
  return { ...result, cost: charge.amount };
}
