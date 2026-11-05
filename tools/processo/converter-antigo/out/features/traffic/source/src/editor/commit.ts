// The code of src/editor/commit.ts that is the feature's (the rest of the file is shared).

// src/editor/commit.ts:63-159 slice of src/editor/commit.ts#commitRoadPath (shared): the feature's statements; "…" marks code left out
export function commitRoadPath(
  // … 9 lines
  parking?: SegmentParking,
// … 8 lines
): DraftResult {
  // … 10 lines
  const key = dryRunKey(start, end, type, pieces, lanes, parking, ground !== undefined);
  // … 2 lines
  if (!options.dryRun && kept && kept.doc === doc && kept.net === net && kept.version === doc.changes.version && kept.netRevision === net.revision
    && kept.groundSolve === (options.groundSolve ?? null) && kept.key === key) {
    dryRunsReused++;
    return layCopy(doc, net, kept.work, kept.workNet, kept.bore, kept.heightLimited, kept.finalHeightOffset, ground, timed);
  }
  // … 7 lines
  for (let i = 0; i < pieces.length; i++) {
    // … 17 lines
    const result = commitDraftInPlace(
      work, workNet, from, to, type, shape, 'ground',
      { start: currentHeight, end: nextHeight,
        smoothEnd: i < pieces.length - 1 },
      lanes,
      parking,
    );
    if (!result.committed && result.reason !== 'duplicate') return result;
    committed ||= result.committed;
  // … 2 lines
  }
  // … 1 lines
  if (!committed) return { committed: false, reason: 'duplicate' };
  // … 7 lines
  if (options.dryRun) {
    // … 6 lines
    lastDryRun = { doc, net, version: doc.changes.version, netRevision: net.revision, groundSolve: options.groundSolve ?? null, key,
      work, workNet, bore, heightLimited, finalHeightOffset: currentHeight };
  // … 1 lines
  }
// … 1 lines
}

// src/editor/commit.ts:188-191 slice of src/editor/commit.ts#dryRunKey (shared): the feature's statements; "…" marks code left out
function dryRunKey(start: Anchor, end: Anchor, type: number, pieces: readonly RoadPathPiece[], lanes: number | null,
  parking: SegmentParking | undefined, sampled: boolean): string {
  return JSON.stringify([start, end, type, pieces, lanes, parking ?? null, sampled]);
}

// src/editor/commit.ts:369-515 slice of src/editor/commit.ts#commitDraftInPlace (shared): the feature's statements; "…" marks code left out
function commitDraftInPlace(
  // … 9 lines
  parking?: SegmentParking,
): DraftResult {
  // … 113 lines
  for (let i = 1; i < ordered.length; i++) {
    // … 14 lines
    if (doc.addSegment(from.node, to.node, type, pieceCurve, from.s, direction, lanes, structure, undefined, parking)) made++;
  }
// … 4 lines
}

// src/editor/commit.ts:768-814 slice of src/editor/commit.ts#joinSegments (shared): the feature's statements; "…" marks code left out
export function joinSegments(doc: RoadDoc, nodeId: NodeId): boolean {
  // … 16 lines
  const parkingFirst = first.a === a ? first.parking : flipParking(first.parking);
  const parkingSecond = second.b === b ? second.parking : flipParking(second.parking);
  if (!sameParking(parkingFirst, parkingSecond)) return false;
  // … 18 lines
  const joined = doc.addSegment(a, b, first.type, null, dashOrigin, 'both', first.lanes, first.structure, section, parkingFirst);
  if (!joined) return false;
  if (first.cutWalls && second.cutWalls) doc.setSegmentCutWalls(joined.id, true);
  doc.carryMovements(a, bansAtA, first.id, joined.id);
  doc.carryMovements(b, bansAtB, second.id, joined.id);
  doc.carryCrossing(a, crossingAtA, first.id, joined.id);
  doc.carryCrossing(b, crossingAtB, second.id, joined.id);
// … 1 lines
}
