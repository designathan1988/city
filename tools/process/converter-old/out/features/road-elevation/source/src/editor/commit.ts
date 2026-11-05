// The code of src/editor/commit.ts that is the feature's (the rest of the file is shared).

// src/editor/commit.ts:40-40 slice of src/editor/commit.ts#CROSSING_CLEARANCE (shared): the feature's statements; "…" marks code left out
const CROSSING_CLEARANCE = ROAD_TUNING.clearance.elevated;

// src/editor/commit.ts:42-42 slice of src/editor/commit.ts#AUTO_TUNNEL_COVER (shared): the feature's statements; "…" marks code left out
const AUTO_TUNNEL_COVER = ROAD_TUNING.tunnel.autoCover;

// src/editor/commit.ts:44-57 slice of src/editor/commit.ts#DraftResult (shared): the feature's statements; "…" marks code left out
export interface DraftResult {
  // … 5 lines
  readonly elevation?: RoadElevation;
  // … 3 lines
  readonly stations?: readonly RoadStation[];
// … 2 lines
}

// src/editor/commit.ts:63-159 slice of src/editor/commit.ts#commitRoadPath (shared): the feature's statements; "…" marks code left out
export function commitRoadPath(
  // … 13 lines
  options: {
    readonly dryRun?: boolean;
    /** The roads' heights the game last solved on this same natural ground (`ground`) */
    readonly groundSolve?: RoadElevation | null;
  } = {},
): DraftResult {
  // … 11 lines
  const kept = lastDryRun;
  lastDryRun = null;
  if (!options.dryRun && kept && kept.doc === doc && kept.net === net && kept.version === doc.changes.version && kept.netRevision === net.revision
    && kept.groundSolve === (options.groundSolve ?? null) && kept.key === key) {
    dryRunsReused++;
    return layCopy(doc, net, kept.work, kept.workNet, kept.bore, kept.heightLimited, kept.finalHeightOffset, ground, timed);
  }
  // … 7 lines
  for (let i = 0; i < pieces.length; i++) {
    // … 9 lines
    const rise = length * MAX_AUTHORED_GRADE;
    const nextHeight = Math.max(currentHeight - rise,
      Math.min(currentHeight + rise, piece.end.heightOffset));
    const limited = Math.abs(nextHeight - piece.end.heightOffset) > 1e-6;
    heightLimited ||= limited;
    if (i === pieces.length - 1 && end.kind !== 'free' && limited) {
      return { committed: false, reason: 'tooShort' };
    }
    const result = commitDraftInPlace(
      // … 1 lines
      { start: currentHeight, end: nextHeight,
        smoothEnd: i < pieces.length - 1 },
    // … 2 lines
    );
    // … 2 lines
    currentHeight = nextHeight;
  // … 1 lines
  }
  // … 2 lines
  const bore = boreDeepCuts(doc, work, workNet, ground, options.groundSolve ?? null);
  if (bore.bored) workNet.rebuild();
  // … 5 lines
  if (options.dryRun) {
    // Paid for on the copy only at the click (`layCopy`): the balance read then.
    const charge = settleRoadEdit(roadsBefore(doc), work, false);
    if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
    const stations = draftStations(doc, work, workNet, pieces, type, start.at, ground, bore, options.groundSolve ?? null);
    timed('stations');
    // `draftStations` reads the copy and writes nothing to it: the click lays it as it is.
    lastDryRun = { doc, net, version: doc.changes.version, netRevision: net.revision, groundSolve: options.groundSolve ?? null, key,
      work, workNet, bore, heightLimited, finalHeightOffset: currentHeight };
    return { committed: true, heightLimited, finalHeightOffset: currentHeight, cost: charge.amount, stations };
  }
  return layCopy(doc, net, work, workNet, bore, heightLimited, currentHeight, ground, timed);
}

// src/editor/commit.ts:162-172 slice of src/editor/commit.ts#layCopy (shared): the feature's statements; "…" marks code left out
function layCopy(doc: RoadDoc, net: Network, work: RoadDoc, workNet: Network, bore: { bored: boolean; elevation: RoadElevation | null },
  heightLimited: boolean, finalHeightOffset: number, ground: ((x: number, y: number) => number) | undefined, timed: (what: string) => void): DraftResult {
  // … 6 lines
  return { committed: true, heightLimited, finalHeightOffset, cost: charge.amount, laid,
    ...(!bore.bored && ground && bore.elevation ? { elevation: bore.elevation } : {}) };
}

// src/editor/commit.ts:177-182 slice of src/editor/commit.ts#lastDryRun (shared): the feature's statements; "…" marks code left out
let lastDryRun: {
  readonly doc: RoadDoc; readonly net: Network; readonly version: number; readonly netRevision: number;
  readonly groundSolve: RoadElevation | null; readonly key: string;
  readonly work: RoadDoc; readonly workNet: Network; readonly bore: { bored: boolean; elevation: RoadElevation | null };
  readonly heightLimited: boolean; readonly finalHeightOffset: number;
} | null = null;

// src/editor/commit.ts:195-195 slice of src/editor/commit.ts#onTerrain (shared): the feature's statements; "…" marks code left out
let onTerrain: { readonly index: TerrainIndex; solve: RoadElevation | null } | null = null;

// src/editor/commit.ts:196-201 slice of src/editor/commit.ts#terrainIndexOf (shared): the feature's statements; "…" marks code left out
function terrainIndexOf(doc: RoadDoc): TerrainIndex {
  if (!onTerrain || onTerrain.index.relief !== doc.terrainRelief || !sameStamps(onTerrain.index.stamps, doc.terrainStamps)) {
    onTerrain = { index: new TerrainIndex(doc.terrainStamps, 0, doc.terrainRelief), solve: null };
  }
  return onTerrain.index;
}

// src/editor/commit.ts:204-212 slice of src/editor/commit.ts#solveOver (shared): the feature's statements; "…" marks code left out
function solveOver(net: Network, ground: (x: number, y: number) => number, sampled: boolean, index: TerrainIndex | null,
  previous: RoadElevation | null): RoadElevation {
  if (sampled) return buildRoadElevation(net, ground, previous);
  // … 1 lines
  const same = onTerrain !== null && onTerrain.index === index ? onTerrain : null;
  const elevation = buildRoadElevation(net, ground, same?.solve ?? null);
  if (same) same.solve = elevation;
  return elevation;
}

// src/editor/commit.ts:214-251 slice of src/editor/commit.ts#boreDeepCuts (shared): the feature's statements; "…" marks code left out
function boreDeepCuts(before: RoadDoc, work: RoadDoc, workNet: Network, sampled?: (x: number, y: number) => number,
  previous: RoadElevation | null = null): { bored: boolean; elevation: RoadElevation | null } {
  // … 18 lines
  const elevation = solveOver(workNet, ground, sampled !== undefined, index, previous);
  // … 1 lines
  for (const seg of fresh) {
    // … 4 lines
    for (let i = 0; i <= steps; i++) {
      // … 1 lines
      if (ground(p.x, p.y) - elevation.onSegment(seg.id, p.x, p.y) > AUTO_TUNNEL_COVER) {
        work.setSegmentStructure(seg.id, 'tunnel');
        changed = true;
        break;
      }
    }
  }
  return { bored: changed, elevation };
}

// src/editor/commit.ts:254-277 slice of src/editor/commit.ts#draftStations (shared): the feature's statements; "…" marks code left out
function draftStations(
  // … 2 lines
  bore: { bored: boolean; elevation: RoadElevation | null }, previous: RoadElevation | null,
): RoadStation[] {
  // … 14 lines
  const elevation = bore.elevation && !bore.bored
    ? bore.elevation
    : solveOver(workNet, ground, sampled !== undefined, index, bore.elevation ?? previous);
  return stationsAlong(workNet, elevation, ground, orderAlong(work, laid, startAt), STATION_STEP);
}

// src/editor/commit.ts:369-515 slice of src/editor/commit.ts#commitDraftInPlace (shared): the feature's statements; "…" marks code left out
function commitDraftInPlace(
  // … 6 lines
  structure: RoadStructure,
// … 3 lines
): DraftResult {
  // … 19 lines
  for (const { id } of ribbonsNear(net, [box.minX - MERGE_EPS, box.minY - MERGE_EPS, box.maxX + MERGE_EPS, box.maxY + MERGE_EPS])) {
    // … 2 lines
    if (!heights && seg.structure !== structure) continue;
    // … 4 lines
    for (let di = 0; di + 1 < draft.n; di++) {
      // … 7 lines
      for (let ei = 0; ei + 1 < existing.n; ei++) {
        // … 11 lines
        if (heights) {
          // … 3 lines
          if (gap > CROSSING_CLEARANCE) continue;
        // … 2 lines
        }
      // … 28 lines
      }
    }
  }
  // … 27 lines
  for (let i = 1; i < ordered.length; i++) {
    // … 12 lines
    if (alreadyJoined(doc, from.node, to.node, pieceCurve, structure)) continue;
    // … 1 lines
    if (doc.addSegment(from.node, to.node, type, pieceCurve, from.s, direction, lanes, structure, undefined, parking)) made++;
  }
// … 4 lines
}

// src/editor/commit.ts:556-581 slice of src/editor/commit.ts#alreadyJoined (shared): the feature's statements; "…" marks code left out
function alreadyJoined(
  // … 4 lines
  structure: RoadStructure = 'ground',
): boolean {
  // … 2 lines
  return node.incident.some((id) => {
    // … 1 lines
    if (!seg || seg.structure !== structure || (seg.a !== b && seg.b !== b)) return false;
  // … 12 lines
  });
}

// src/editor/commit.ts:654-659 slice of src/editor/commit.ts#segmentOffsetAt (shared): the feature's statements; "…" marks code left out
function segmentOffsetAt(doc: RoadDoc, segment: NonNullable<ReturnType<RoadDoc['segment']>>, s: number, length: number): number {
  // … 3 lines
  return a + (b - a) * t + roadStructure(segment.structure).clearance;
}

// src/editor/commit.ts:674-744 slice of src/editor/commit.ts#reconcileMovedNode (shared): the feature's statements; "…" marks code left out
export function reconcileMovedNode(doc: RoadDoc, net: Network, id: NodeId, depth = 0): DraftResult {
  // … 29 lines
  for (const sid of [...node.incident]) {
    // … 5 lines
    for (const { id: otherId } of ribbonsNear(net, [pb.minX - MERGE_EPS, pb.minY - MERGE_EPS, pb.maxX + MERGE_EPS, pb.maxY + MERGE_EPS])) {
      // … 4 lines
      for (let i = 0; i + 1 < path.n; i++) {
        // … 1 lines
        for (let j = 0; j + 1 < line.n; j++) {
          // … 6 lines
          if (gap > CROSSING_CLEARANCE) continue;
        // … 10 lines
        }
      // … 4 lines
      }
    }
  }
// … 1 lines
}
