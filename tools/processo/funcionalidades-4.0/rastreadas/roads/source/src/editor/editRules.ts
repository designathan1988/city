// The code of src/editor/editRules.ts that is the feature's (the rest of the file is shared).

// src/editor/editRules.ts:19-23 slice of src/editor/editRules.ts#RoadState (shared): the feature's statements; "…" marks code left out
export interface RoadState {
  // … 2 lines
  readonly net: Network;
}

// src/editor/editRules.ts:30-30 slice of src/editor/editRules.ts#MAX_BUILT_GRADE (shared): the feature's statements; "…" marks code left out
const MAX_BUILT_GRADE = ROAD_TUNING.grade.refuse;

// src/editor/editRules.ts:32-32 slice of src/editor/editRules.ts#PASS_CLEARANCE (shared): the feature's statements; "…" marks code left out
const PASS_CLEARANCE = ROAD_TUNING.clearance.elevated;

// src/editor/editRules.ts:43-60 slice of src/editor/editRules.ts#refuseRoadEdit (shared): the feature's statements; "…" marks code left out
export function refuseRoadEdit(before: RoadState, after: RoadState): RoadEditRefusal | null {
  for (const [node, gap] of written(before.net, after.net, 'impossible')) {
    const was = before.doc.node(node) ? before.net.impossible.get(node) : undefined;
    if (was === undefined || gap < was - GAP_NOISE) return 'sharp';
  }
  for (const [node, short] of written(before.net, after.net, 'squeezed')) {
    const was = before.doc.node(node) ? before.net.squeezed.get(node) : undefined;
    if (was === undefined || short > was + SQUEEZE_SLACK) return 'squeezed';
  }
// … 8 lines
}

// src/editor/editRules.ts:57-57 slice of src/editor/editRules.ts#refuseRoadEdit.deck (shared): the feature's statements; "…" marks code left out
  const deck = (id: SegmentId, p: Vec2): number => solves.after().onSegment(id, p.x, p.y);

// src/editor/editRules.ts:63-69 slice of src/editor/editRules.ts#flatSolves (shared): the feature's statements; "…" marks code left out
function flatSolves(before: RoadState, after: RoadState): { before(): RoadElevation; after(): RoadElevation } {
  let solvedBefore: RoadElevation | null = null;
  let solvedAfter: RoadElevation | null = null;
// … 3 lines
}

// src/editor/editRules.ts:66-66 slice of src/editor/editRules.ts#flatSolves.beforeSolve (shared): the feature's statements; "…" marks code left out
  const beforeSolve = (): RoadElevation => (solvedBefore ??= buildRoadElevation(before.net, FLAT_GROUND));

// src/editor/editRules.ts:68-68 slice of src/editor/editRules.ts#flatSolves.after (shared): the feature's statements; "…" marks code left out
  return { before: beforeSolve, after: () => (solvedAfter ??= buildRoadElevation(after.net, FLAT_GROUND, beforeSolve())) };

// src/editor/editRules.ts:72-78 slice of src/editor/editRules.ts#written (shared): the feature's statements; "…" marks code left out
function written(before: Network, after: Network, name: 'impossible' | 'squeezed'): Iterable<[NodeId, number]> {
  const map = after[name];
  // … 1 lines
  if (after.forkedFrom !== before || !(map instanceof ForkMap)) throw new Error('refuseRoadEdit: the edit is judged on a working copy of the network (`Network.fork`)');
  const { replaced, added } = (map as ForkMap<NodeId, number>).changes();
  return [...replaced, ...added];
}

// src/editor/editRules.ts:81-95 slice of src/editor/editRules.ts#guardRoadEdit (shared): the feature's statements; "…" marks code left out
export function guardRoadEdit(doc: RoadDoc, net: Network, edit: (doc: RoadDoc, net: Network) => boolean):
  { readonly changed: boolean; readonly refused: RoadEditRefusal | null } {
  if (net.revision !== doc.revision) net.rebuild();
  const money = roadsBefore(doc);
  // … 1 lines
  const workNet = net.fork(work);
  if (!edit(work, workNet)) return { changed: false, refused: null };
  if (workNet.revision !== work.revision) workNet.rebuild();
  // … 1 lines
  const refused = refuseRoadEdit({ doc, net }, { doc: work, net: workNet }) ?? (settleRoadEdit(money, work).affordable ? null : 'funds');
  if (refused) return { changed: false, refused };
  // … 1 lines
  net.takeBack(workNet);
// … 1 lines
}

// src/editor/editRules.ts:142-164 slice of src/editor/editRules.ts#grade (shared): the feature's statements; "…" marks code left out
function grade(state: RoadState, id: SegmentId, solved: () => RoadElevation): number {
  // … 3 lines
  const length = net.polylines.get(doc, id).length;
  // … 1 lines
  if (Math.abs(deckAt(doc, seg, length, length) - deckAt(doc, seg, 0, length)) < SAME_LEVEL) return 0;
  const line = net.ribbons.get(id)?.full;
  if (!line) return 0;
  const elevation = solved();
  // … 1 lines
  for (let s = 0; s <= line.length; s += GRADE_STEP) {
    const p = line.sampleAt(Math.min(s, line.length)).p;
    heights.push(elevation.onSegment(id, p.x, p.y));
  }
// … 7 lines
}

// src/editor/editRules.ts:167-185 slice of src/editor/editRules.ts#tooSteep (shared): the feature's statements; "…" marks code left out
function tooSteep(before: RoadState, after: RoadState, changed: readonly SegmentId[],
  solves: { before(): RoadElevation; after(): RoadElevation }): boolean {
  // … 8 lines
  const afterElevation = solves.after, beforeElevation = solves.before;
  for (const id of touched) {
    const now = grade(after, id, afterElevation);
    if (now <= MAX_BUILT_GRADE) continue;
    if (!changed.includes(id) && before.doc.segment(id) && grade(before, id, beforeElevation) >= now - 1e-6) continue;
  // … 1 lines
  }
// … 1 lines
}

// src/editor/editRules.ts:188-236 slice of src/editor/editRules.ts#overlapsAnother (shared): the feature's statements; "…" marks code left out
function overlapsAnother(before: RoadState, after: RoadState, id: SegmentId,
  deck: (id: SegmentId, p: Vec2) => number): boolean {
  // … 2 lines
  const ribbon = net.ribbons.get(id);
  if (!seg || !ribbon) return false;
  const line = ribbon.full;
  const length = line.length;
  const trims = net.trims.get(id);
  const from = (trims?.a[Level.Casing] ?? 0) + SAMPLE_STEP / 2;
  const to = length - (trims?.b[Level.Casing] ?? 0) - SAMPLE_STEP / 2;
  if (to <= from) return false;
  // … 2 lines
  const box = line.bbox;
  const grow = halfWidth(ribbon.road, Level.Sidewalk);
  // … 1 lines
  for (const near of ribbonsNear(net, [box.minX - grow, box.minY - grow, box.maxX + grow, box.maxY + grow])) {
    const otherId = near.id;
    const other = doc.segment(otherId);
    if (otherId === id || !other) continue;
    const ob = net.ribbons.get(otherId)?.full.bbox;
    const limit = overlapReach(net, id, otherId);
    if (!ob || ob.minX > box.maxX + limit || ob.maxX < box.minX - limit ||
      ob.minY > box.maxY + limit || ob.maxY < box.minY - limit) continue;
    others.push({ id: otherId, seg: other, limit });
  }
  // … 2 lines
  const steps = Math.max(1, Math.ceil((to - from) / SAMPLE_STEP));
  for (let k = 0; k <= steps; k++) {
    const s = from + ((to - from) * k) / steps;
    const p = line.sampleAt(s).p;
    const height = deckAt(doc, seg, s, length);
    for (const { id: otherId, seg: other, limit } of others) {
      const otherLine = net.ribbons.get(otherId)?.full;
      if (!otherLine) continue;
      const ob = otherLine.bbox;
      if (p.x < ob.minX - limit || p.x > ob.maxX + limit || p.y < ob.minY - limit || p.y > ob.maxY + limit) continue;
      const hit = otherLine.closestPoint(p);
      if (hit.distance >= limit) continue;
      if (withinSharedPlate(after, seg, other, otherId, hit.s, otherLine.length)) continue;
      if (Math.abs(deck(id, p) - deck(otherId, hit.point)) >= PASS_CLEARANCE) continue;
      if (overlappedBefore(before, p, height, hit.point, deckAt(doc, other, hit.s, otherLine.length))) continue;
      return true;
    }
  }
// … 1 lines
}

// src/editor/editRules.ts:239-245 slice of src/editor/editRules.ts#withinSharedPlate (shared): the feature's statements; "…" marks code left out
function withinSharedPlate(after: RoadState, seg: RoadSegment, other: RoadSegment, otherId: SegmentId, s: number, length: number): boolean {
  const trims = after.net.trims.get(otherId);
  // … 1 lines
  if (shares(other.a) && s <= (trims?.a[Level.Casing] ?? 0) + SAMPLE_STEP) return true;
  if (shares(other.b) && s >= length - (trims?.b[Level.Casing] ?? 0) - SAMPLE_STEP) return true;
// … 1 lines
}

// src/editor/editRules.ts:261-267 slice of src/editor/editRules.ts#overlapReach (shared): the feature's statements; "…" marks code left out
function overlapReach(net: Network, a: SegmentId, b: SegmentId): number {
  const ra = net.ribbons.get(a)?.road;
  const rb = net.ribbons.get(b)?.road;
  if (!ra || !rb) return 0;
  return Math.max(halfWidth(ra, Level.Asphalt) + halfWidth(rb, Level.Sidewalk),
    halfWidth(ra, Level.Sidewalk) + halfWidth(rb, Level.Asphalt)) - OVERLAP_SLACK;
}

// src/editor/editRules.ts:270-284 slice of src/editor/editRules.ts#segmentsThrough (shared): the feature's statements; "…" marks code left out
function segmentsThrough(state: RoadState, p: Vec2, height: number): SegmentId[] {
  // … 1 lines
  for (const { id } of ribbonsNear(state.net, [p.x - SAME_PLACE, p.y - SAME_PLACE, p.x + SAME_PLACE, p.y + SAME_PLACE])) {
    const seg = state.doc.segment(id);
    if (!seg) continue;
    const line = state.net.polylines.get(state.doc, id);
    const box = line.bbox;
    if (p.x < box.minX - SAME_PLACE || p.x > box.maxX + SAME_PLACE || p.y < box.minY - SAME_PLACE || p.y > box.maxY + SAME_PLACE) continue;
    const hit = line.closestPoint(p);
    if (hit.distance > SAME_PLACE) continue;
    if (Math.abs(deckAt(state.doc, seg, hit.s, line.length) - height) > SAME_LEVEL) continue;
    out.push(id);
  }
// … 1 lines
}
