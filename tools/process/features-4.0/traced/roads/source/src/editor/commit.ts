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
  // … 1 lines
  net: Network,
  start: Anchor,
  end: Anchor,
  // … 1 lines
  pieces: readonly RoadPathPiece[],
  // … 7 lines
  options: {
    readonly dryRun?: boolean;
    /** The roads' heights the game last solved on this same natural ground (`ground`) */
    readonly groundSolve?: RoadElevation | null;
  } = {},
): DraftResult {
  if (!pieces.length || !Number.isInteger(type) || type < 0 || type >= ROAD_TYPES.length) {
    return { committed: false, reason: 'degenerate' };
  }
  pieces = joinShortPieces(pieces);
  // … 4 lines
  if (net.revision !== doc.revision) net.rebuild();
  // … 1 lines
  const key = dryRunKey(start, end, type, pieces, lanes, parking, ground !== undefined);
  const kept = lastDryRun;
  lastDryRun = null;
  if (!options.dryRun && kept && kept.doc === doc && kept.net === net && kept.version === doc.changes.version && kept.netRevision === net.revision
    && kept.groundSolve === (options.groundSolve ?? null) && kept.key === key) {
    dryRunsReused++;
    return layCopy(doc, net, kept.work, kept.workNet, kept.bore, kept.heightLimited, kept.finalHeightOffset, ground, timed);
  }
  // … 2 lines
  const workNet = net.fork(work);
  // … 3 lines
  let currentHeight = pieces[0]?.start.heightOffset ?? 0;
  for (let i = 0; i < pieces.length; i++) {
    const piece = pieces[i] as RoadPathPiece;
    if (!validPoint(piece.start.at) || !validPoint(piece.end.at) ||
      !Number.isFinite(piece.start.heightOffset) || !Number.isFinite(piece.end.heightOffset)) {
      return { committed: false, reason: 'degenerate' };
    }
    const from: Anchor = i === 0 ? start : { kind: 'free', at: piece.start.at };
    const to: Anchor = i === pieces.length - 1 ? end : { kind: 'free', at: piece.end.at };
    const shape = fitRoadCurve(piece.start.at, piece.end.at, piece.curve, type);
    const length = Polyline.fromPoints(flattenSegment(piece.start.at, piece.end.at, shape)).length;
    const rise = length * MAX_AUTHORED_GRADE;
    const nextHeight = Math.max(currentHeight - rise,
      Math.min(currentHeight + rise, piece.end.heightOffset));
    const limited = Math.abs(nextHeight - piece.end.heightOffset) > 1e-6;
    heightLimited ||= limited;
    if (i === pieces.length - 1 && end.kind !== 'free' && limited) {
      return { committed: false, reason: 'tooShort' };
    }
    const result = commitDraftInPlace(
      work, workNet, from, to, type, shape, 'ground',
      { start: currentHeight, end: nextHeight,
        smoothEnd: i < pieces.length - 1 },
      lanes,
      parking,
    );
    if (!result.committed && result.reason !== 'duplicate') return result;
    committed ||= result.committed;
    currentHeight = nextHeight;
    workNet.rebuild();
  }
  // … 1 lines
  if (!committed) return { committed: false, reason: 'duplicate' };
  const bore = boreDeepCuts(doc, work, workNet, ground, options.groundSolve ?? null);
  if (bore.bored) workNet.rebuild();
  // … 2 lines
  const refused = refuseRoadEdit({ doc, net }, { doc: work, net: workNet });
  // … 2 lines
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
  const charge = settleRoadEdit(roadsBefore(doc), work);
  if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
  // … 2 lines
  net.takeBack(workNet);
  // … 1 lines
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

// src/editor/commit.ts:188-191 slice of src/editor/commit.ts#dryRunKey (shared): the feature's statements; "…" marks code left out
function dryRunKey(start: Anchor, end: Anchor, type: number, pieces: readonly RoadPathPiece[], lanes: number | null,
  parking: SegmentParking | undefined, sampled: boolean): string {
  return JSON.stringify([start, end, type, pieces, lanes, parking ?? null, sampled]);
}

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
  // … 5 lines
  for (const seg of fresh) {
    const box = workNet.ribbons.get(seg.id)?.full.bbox;
    if (!box) { minX = -Infinity; maxX = Infinity; minY = -Infinity; maxY = Infinity; break; }
    minX = Math.min(minX, box.minX); minY = Math.min(minY, box.minY); maxX = Math.max(maxX, box.maxX); maxY = Math.max(maxY, box.maxY);
  }
  // … 1 lines
  const reached = work.terrainStamps.some((stamp) =>
    stamp.x + stamp.radius >= minX - margin && stamp.x - stamp.radius <= maxX + margin &&
    stamp.y + stamp.radius >= minY - margin && stamp.y - stamp.radius <= maxY + margin);
  if (!reached) return { bored: false, elevation: null };
  // … 3 lines
  const elevation = solveOver(workNet, ground, sampled !== undefined, index, previous);
  // … 1 lines
  for (const seg of fresh) {
    const ribbon = workNet.ribbons.get(seg.id);
    if (!ribbon) continue;
    const length = ribbon.full.length;
    const steps = Math.max(2, Math.ceil(stepsIn(length, 3.2)));
    for (let i = 0; i <= steps; i++) {
      const p = ribbon.full.sampleAt((i / steps) * length).p;
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
  before: RoadDoc, work: RoadDoc, workNet: Network, pieces: readonly RoadPathPiece[], type: number, startAt: Vec2,
  // … 1 lines
  bore: { bored: boolean; elevation: RoadElevation | null }, previous: RoadElevation | null,
): RoadStation[] {
  const path = Polyline.fromPoints(pieces.flatMap((piece, i) => {
    const pts = flattenSegment(piece.start.at, piece.end.at, fitRoadCurve(piece.start.at, piece.end.at, piece.curve, type));
    return i === 0 ? pts : pts.slice(1);
  }));
  // … 1 lines
  for (const seg of work.addedSegments(before)) {
    const line = workNet.ribbons.get(seg.id)?.full;
    if (!line) continue;
    const mid = line.sampleAt(line.length / 2).p;
    if (path.closestPoint(mid).distance < 1) laid.add(seg.id);
  }
  // … 3 lines
  const elevation = bore.elevation && !bore.bored
    ? bore.elevation
    : solveOver(workNet, ground, sampled !== undefined, index, bore.elevation ?? previous);
  return stationsAlong(workNet, elevation, ground, orderAlong(work, laid, startAt), STATION_STEP);
}

// src/editor/commit.ts:280-298 slice of src/editor/commit.ts#joinShortPieces (shared): the feature's statements; "…" marks code left out
function joinShortPieces(pieces: readonly RoadPathPiece[]): readonly RoadPathPiece[] {
  const out: RoadPathPiece[] = [];
  let carry: RoadPathPiece['start'] | null = null;
  for (const piece of pieces) {
    const start: RoadPathPiece['start'] = carry ?? piece.start;
    if (dist(start.at, piece.end.at) < MIN_LINK_LENGTH * 0.25) {
      carry = start;
      continue;
    }
    out.push(carry ? { start, end: piece.end, curve: null } : piece);
    carry = null;
  }
  if (carry) {
    const last = out.pop();
    const tail = pieces[pieces.length - 1]!;
    out.push(last ? { start: last.start, end: tail.end, curve: null } : { start: carry, end: tail.end, curve: null });
  }
  return out;
}

// src/editor/commit.ts:369-515 slice of src/editor/commit.ts#commitDraftInPlace (shared): the feature's statements; "…" marks code left out
function commitDraftInPlace(
  // … 1 lines
  net: Network,
  start: Anchor,
  end: Anchor,
// … 6 lines
): DraftResult {
  const endpoints = materializeEndpoints(doc, net, start, end, heights);
  if (!endpoints || endpoints[0] === endpoints[1]) {
    return { committed: false, reason: 'degenerate' };
  }
  const [startNode, endNode] = endpoints;
  // … 2 lines
  if (start.kind === 'segment' || end.kind === 'segment') net.rebuild();
  // … 1 lines
  const a = start.at;
  const b = end.at;
  const draft = Polyline.fromPoints(flattenSegment(a, b, curve));
  // … 6 lines
  const box = draft.bbox;
  for (const { id } of ribbonsNear(net, [box.minX - MERGE_EPS, box.minY - MERGE_EPS, box.maxX + MERGE_EPS, box.maxY + MERGE_EPS])) {
    const seg = doc.segment(id);
    if (!seg) continue;
    if (!heights && seg.structure !== structure) continue;
    const existing = net.polylines.get(doc, id);
    // DO NOT test every piece of the draft against every piece of the road: a crossing lies in both pieces' boxes (`segSeg` keeps t and u in 0..1).
    const eb = existing.bbox;

    for (let di = 0; di + 1 < draft.n; di++) {
      const d0 = draft.point(di);
      const d1 = draft.point(di + 1);
      const dx0 = Math.min(d0.x, d1.x) - CROSS_PAD, dx1 = Math.max(d0.x, d1.x) + CROSS_PAD;
      const dy0 = Math.min(d0.y, d1.y) - CROSS_PAD, dy1 = Math.max(d0.y, d1.y) + CROSS_PAD;
      if (dx1 < eb.minX || dx0 > eb.maxX || dy1 < eb.minY || dy0 > eb.maxY) continue;
      const draftPiece = (draft.cum[di + 1] as number) - (draft.cum[di] as number);

      for (let ei = 0; ei + 1 < existing.n; ei++) {
        const e0 = existing.point(ei);
        const e1 = existing.point(ei + 1);
        if (Math.max(e0.x, e1.x) < dx0 || Math.min(e0.x, e1.x) > dx1 || Math.max(e0.y, e1.y) < dy0 || Math.min(e0.y, e1.y) > dy1) continue;
        const hit = segSeg(d0, d1, e0, e1);
        if (!hit) continue;

        const existingPiece =
          (existing.cum[ei + 1] as number) - (existing.cum[ei] as number);
        const draftS = (draft.cum[di] as number) + hit.t * draftPiece;
        const existingS = (existing.cum[ei] as number) + hit.u * existingPiece;

        if (heights) {
          const drafted = heights.start +
            (heights.end - heights.start) * (draftS / Math.max(1e-6, draft.length));
          const gap = Math.abs(drafted - segmentOffsetAt(doc, seg, existingS, existing.length));
          if (gap > CROSSING_CLEARANCE) continue;
          // Too far apart in height to meet, too close to pass over: refused, as moving a node already is.
          if (gap > HEIGHT_JOIN_EPS) return { committed: false, reason: 'clearance' };
        }

        // The start and end anchors already materialize these contacts.
        if (draftS <= MERGE_EPS || draftS >= draft.length - MERGE_EPS) continue;

        const draftQ = (di + hit.t) / Math.max(1, draft.n - 1);

        // A crossing NEAR an existing endpoint reuses that junction node.
        if (existingS <= MIN_LINK_LENGTH) {
          draftCuts.push({ node: seg.a, q: draftQ, s: draftS });
          slides.push({ node: seg.a, segment: id, at: hit.point });
          continue;
        }
        if (existingS >= existing.length - MIN_LINK_LENGTH) {
          draftCuts.push({ node: seg.b, q: draftQ, s: draftS });
          slides.push({ node: seg.b, segment: id, at: hit.point });
          continue;
        }

        const cuts = splitBySegment.get(id) ?? [];
        const duplicate = cuts.some(
          (cut) =>
            Math.abs(cut.s - existingS) <= MERGE_EPS &&
            Math.abs(cut.draftS - draftS) <= MERGE_EPS,
        );
        if (!duplicate) {
          cuts.push({ at: hit.point, s: existingS, draftQ, draftS });
          splitBySegment.set(id, cuts);
        }
      }
    }
  }
  // … 9 lines
  for (const [id, cuts] of splitBySegment) {
    // … 1 lines
    const nodes = splitSegmentAtCuts(doc, net, id, tagged);
    for (const cut of cuts) {
      const node = nodes.get(cut);
      if (node !== undefined) {
        draftCuts.push({ node, q: cut.draftQ, s: cut.draftS });
      }
    }
  }
  // … 1 lines
  const ordered = normalizeDraftStops([
    { node: startNode, q: 0, s: 0 },
    // … 1 lines
    { node: endNode, q: 1, s: draft.length },
  ]);
  // … 2 lines
  for (let i = 1; i < ordered.length; i++) {
    // … 11 lines
    const pieceCurve = curveShapeForRange(a, b, curve, fromPoint, toPoint, from.q, to.q);
    if (alreadyJoined(doc, from.node, to.node, pieceCurve, structure)) continue;
    const direction = ROAD_TYPES[type]?.lanes === 1 ? 'aToB' : 'both';
    if (doc.addSegment(from.node, to.node, type, pieceCurve, from.s, direction, lanes, structure, undefined, parking)) made++;
  }
// … 4 lines
}

// src/editor/commit.ts:584-634 slice of src/editor/commit.ts#materializeEndpoints (shared): the feature's statements; "…" marks code left out
function materializeEndpoints(
  // … 1 lines
  net: Network,
  start: Anchor,
  end: Anchor,
// … 1 lines
): [NodeId, NodeId] | null {
  // … 31 lines
  if (!queue('start', start) || !queue('end', end)) return null;
  for (const [id, cuts] of bySegment) {
    const nodes = splitSegmentAtCuts(doc, net, id, cuts);
    for (const cut of cuts) {
      const node = nodes.get(cut.tag);
      if (node !== undefined) resolved.set(cut.tag, node);
    }
  }
// … 4 lines
}

// src/editor/commit.ts:595-620 slice of src/editor/commit.ts#materializeEndpoints.queue (shared): the feature's statements; "…" marks code left out
  const queue = (tag: Tag, anchor: Anchor): boolean => {
    if (anchor.kind === 'node') {
      if (anchor.node === undefined || !doc.node(anchor.node)) return false;
      resolved.set(tag, anchor.node);
      return true;
    }

    if (anchor.kind === 'segment') {
      if (
        anchor.segment === undefined ||
        anchor.s === undefined ||
        !Number.isFinite(anchor.s) ||
        !doc.segment(anchor.segment)
      ) {
        return false;
      }
      const cuts = bySegment.get(anchor.segment) ?? [];
      cuts.push({ at: anchor.at, s: anchor.s, tag });
      bySegment.set(anchor.segment, cuts);
      return true;
    }

    resolved.set(tag, materializeFree(doc, net, anchor.at, heights?.[tag] ?? 0,
      tag === 'end' && heights?.smoothEnd === true, [...resolved.values()]));
    return true;
  };

// src/editor/commit.ts:637-652 slice of src/editor/commit.ts#materializeFree (shared): the feature's statements; "…" marks code left out
function materializeFree(doc: RoadDoc, net: Network, at: Vec2, heightOffset = 0, smooth = false,
  // … 1 lines
  fresh: readonly NodeId[] = []): NodeId {
  // … 2 lines
  for (const { id } of ribbonsNear(net, [at.x - MERGE_EPS, at.y - MERGE_EPS, at.x + MERGE_EPS, at.y + MERGE_EPS])) {
    const seg = doc.segment(id);
    if (seg) { candidates.add(seg.a); candidates.add(seg.b); }
  }
// … 6 lines
}

// src/editor/commit.ts:662-671 slice of src/editor/commit.ts#splitSegment (shared): the feature's statements; "…" marks code left out
export function splitSegment(
  // … 1 lines
  net: Network,
// … 3 lines
): NodeId | null {
  // … 1 lines
  return splitSegmentAtCuts(doc, net, id, [{ at, s, tag }]).get(tag) ?? null;
}

// src/editor/commit.ts:674-744 function reconcileMovedNode: the feature's own
export function reconcileMovedNode(doc: RoadDoc, net: Network, id: NodeId, depth = 0): DraftResult {
  if (depth > 16) return { committed: true };
  const node = doc.node(id);
  if (!node) return { committed: false, reason: 'degenerate' };

  // DO NOT walk `doc.nodes`.
  const near = new Set<NodeId>();
  for (const { id: sid } of ribbonsNear(net, [node.x - MERGE_EPS, node.y - MERGE_EPS, node.x + MERGE_EPS, node.y + MERGE_EPS])) {
    const seg = doc.segment(sid);
    if (seg) { near.add(seg.a); near.add(seg.b); }
  }
  for (const otherId of [...near].sort((p, q) => p - q)) {
    const other = doc.node(otherId);
    if (!other || other.id === id) continue;
    if (dist(node, other) > MERGE_EPS || Math.abs(other.heightOffset - node.heightOffset) > HEIGHT_JOIN_EPS) continue;
    // Merging along a road would fold that road to nothing.
    if (node.incident.some((sid) => { const seg = doc.segment(sid); return seg && (seg.a === other.id || seg.b === other.id); })) {
      return { committed: false, reason: 'tooShort' };
    }
    doc.mergeNodes(other.id, id);
    return { committed: true };
  }

  for (const sid of node.incident) {
    const seg = doc.segment(sid);
    const far = seg && doc.node(seg.a === id ? seg.b : seg.a);
    if (far && dist(node, far) < MIN_LINK_LENGTH * 0.25) return { committed: false, reason: 'tooShort' };
  }

  net.rebuild();
  for (const sid of [...node.incident]) {
    const moved = doc.segment(sid);
    if (!moved) continue;
    const path = net.polylines.get(doc, sid);
    // DO NOT walk `doc.segments`.
    const pb = path.bbox;
    for (const { id: otherId } of ribbonsNear(net, [pb.minX - MERGE_EPS, pb.minY - MERGE_EPS, pb.maxX + MERGE_EPS, pb.maxY + MERGE_EPS])) {
      const other = doc.segment(otherId);
      if (!other || otherId === sid || other.structure !== moved.structure) continue;
      if (other.a === moved.a || other.a === moved.b || other.b === moved.a || other.b === moved.b) continue;
      const line = net.polylines.get(doc, otherId);
      for (let i = 0; i + 1 < path.n; i++) {
        let crossed = false;
        for (let j = 0; j + 1 < line.n; j++) {
          const hit = segSeg(path.point(i), path.point(i + 1), line.point(j), line.point(j + 1));
          if (!hit) continue;
          const sMoved = (path.cum[i] as number) + hit.t * ((path.cum[i + 1] as number) - (path.cum[i] as number));
          const sOther = (line.cum[j] as number) + hit.u * ((line.cum[j + 1] as number) - (line.cum[j] as number));
          if (sMoved <= MERGE_EPS || sMoved >= path.length - MERGE_EPS) continue;
          const gap = Math.abs(segmentOffsetAt(doc, moved, sMoved, path.length) - segmentOffsetAt(doc, other, sOther, line.length));
          if (gap > CROSSING_CLEARANCE) continue;
          if (gap > HEIGHT_JOIN_EPS) return { committed: false, reason: 'clearance' };
          // Near the other road's end, its junction is reused, as a drawn road does.
          const onOther = sOther <= MIN_LINK_LENGTH ? other.a
            : sOther >= line.length - MIN_LINK_LENGTH ? other.b
              : splitSegmentAtCuts(doc, net, otherId, [{ at: hit.point, s: sOther, tag: 'x' }]).get('x');
          const onMoved = splitSegmentAtCuts(doc, net, sid, [{ at: hit.point, s: sMoved, tag: 'x' }]).get('x');
          if (onOther !== undefined && onMoved !== undefined) doc.mergeNodes(onOther, onMoved);
          net.rebuild();
          crossed = true;
          break;
        }
        if (crossed) {
          // The moved road is now two pieces; what is left of it is handled by a fresh pass rather than on stale arc lengths.
          return reconcileMovedNode(doc, net, id, depth + 1);
        }
      }
    }
  }
  return { committed: true };
}

// src/editor/commit.ts:747-765 function moveNodeChecked: the feature's own
export function moveNodeChecked(doc: RoadDoc, net: Network, id: NodeId, to: Vec2): DraftResult {
  // Made on a working copy (`RoadDoc.fork`) and judged against the map as it stands, as a drawn road is
  if (net.revision !== doc.revision) throw new Error('moveNodeChecked: the network is not built for the document as it stands');
  const work = doc.fork();
  const workNet = net.fork(work);
  if (!work.moveNode(id, to)) return { committed: false, reason: 'duplicate' };
  // `workNet` stands as before the move: the roads that did not move are filed where they are.
  const result = reconcileMovedNode(work, workNet, id);
  if (!result.committed) return result;
  workNet.rebuild();
  const refused = refuseRoadEdit({ doc, net }, { doc: work, net: workNet });
  if (refused) return { committed: false, reason: refused };
  // The roads made longer are paid for, shorter partly paid back (`editor/roads/economy.ts`).
  const charge = settleRoadEdit(roadsBefore(doc), work);
  if (!charge.affordable) return { committed: false, reason: 'funds', cost: charge.amount };
  doc.takeBack(work);
  net.takeBack(workNet);
  return { ...result, cost: charge.amount };
}

// src/editor/commit.ts:768-814 slice of src/editor/commit.ts#joinSegments (shared): the feature's statements; "…" marks code left out
export function joinSegments(doc: RoadDoc, nodeId: NodeId): boolean {
  // … 12 lines
  const sectionFirst = first.a === a ? first.section : flipSection(first.section);
  const sectionSecond = second.b === b ? second.section : flipSection(second.section);
  if (!sameRoadSectionIgnoringArrows(sectionFirst, sectionSecond)) return false;
  // … 14 lines
  const towardB = second.b === b ? second.section?.turnsForward : second.section?.turnsBackward;
  const towardA = first.a === a ? first.section?.turnsBackward : first.section?.turnsForward;
  const base = sectionForPiece(sectionFirst, false, false);
  const section = base && {
    ...base,
    ...(towardB ? { turnsForward: [...towardB] } : {}),
    ...(towardA ? { turnsBackward: [...towardA] } : {}),
  };
  const joined = doc.addSegment(a, b, first.type, null, dashOrigin, 'both', first.lanes, first.structure, section, parkingFirst);
  if (!joined) return false;
  if (first.cutWalls && second.cutWalls) doc.setSegmentCutWalls(joined.id, true);
  doc.carryMovements(a, bansAtA, first.id, joined.id);
  doc.carryMovements(b, bansAtB, second.id, joined.id);
  doc.carryCrossing(a, crossingAtA, first.id, joined.id);
  doc.carryCrossing(b, crossingAtB, second.id, joined.id);
// … 1 lines
}

// src/editor/commit.ts:817-853 slice of src/editor/commit.ts#duplicateSegment (shared): the feature's statements; "…" marks code left out
export function duplicateSegment(doc: RoadDoc, net: Network, id: SegmentId): SegmentId | null {
  // … 10 lines
  const sourceWidth = net.ribbons.get(id)?.road.width ?? m(4.8);
  const offset = Math.max(m(8.8), sourceWidth + m(4.8));
  const nx = (-dy / length) * offset;
  const ny = (dx / length) * offset;
  const copyA = doc.addNode({ x: a.x + nx, y: a.y + ny }, a.heightOffset);
  const copyB = doc.addNode({ x: b.x + nx, y: b.y + ny }, b.heightOffset);
// … 19 lines
}

// src/editor/commit.ts:856-966 slice of src/editor/commit.ts#splitSegmentAtCuts (shared): the feature's statements; "…" marks code left out
function splitSegmentAtCuts<Tag>(
  // … 1 lines
  net: Network,
// … 2 lines
): Map<Tag, NodeId> {
  // … 3 lines
  const pl = net.polylines.get(doc, id);
  // … 19 lines
  for (const request of ordered) {
    // … 4 lines
    if (request.s >= pl.length - MERGE_EPS) {
      result.set(request.tag, seg.b);
      continue;
    }
    // … 7 lines
    const q = parameterAtArc(pl, request.s);
    const at = control ? splitQuad(a, control, b, q).left[2] : request.at;
    interior.push({ at, s: request.s, q, tags: [request.tag] });
  }
  // … 3 lines
  const nodes = interior.map((cut) =>
    doc.addNode(cut.at, (originalA.heightOffset ?? 0) +
      ((originalB.heightOffset ?? 0) - (originalA.heightOffset ?? 0)) *
      (cut.s / Math.max(1e-6, pl.length))));
  for (let i = 0; i < interior.length; i++) {
    // … 1 lines
    const node = nodes[i] as { id: NodeId };
    for (const tag of cut.tags) result.set(tag, node.id);
  }
  // … 1 lines
  const nodeIds = [seg.a, ...nodes.map((node) => node.id), seg.b];
  // … 2 lines
  const arcs = [0, ...interior.map((cut) => cut.s), pl.length];
  // … 11 lines
  for (let i = 0; i + 1 < nodeIds.length; i++) {
    const pieceCurve = curveShapeForRange(
      a,
      b,
      seg.curve,
      points[i] as Vec2,
      points[i + 1] as Vec2,
      params[i] as number,
      params[i + 1] as number,
    );
    const piece = doc.addSegment(
      nodeIds[i] as NodeId,
      nodeIds[i + 1] as NodeId,
      type,
      pieceCurve,
      dashOrigin + (arcs[i] as number),
      seg.direction,
      seg.lanes,
      seg.structure,
      sectionForPiece(seg.section, i === 0, i + 2 === nodeIds.length),
      seg.parking,
    );
    if (piece && seg.cutWalls) doc.setSegmentCutWalls(piece.id, true);
    if (piece) pieces.push(piece.id);
  }
// … 8 lines
}
