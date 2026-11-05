// The code of src/editor/poles.ts that is the feature's (the rest of the file is shared).

// src/editor/poles.ts:50-52 slice of src/editor/poles.ts#snapPole (shared): the feature's statements; "…" marks code left out
export function snapPole(doc: RoadDoc, net: Network, at: Vec2, reach: number): PoleSnap {
  return snapOn(doc, net, poleLinesAt(net, at), at, reach);
}

// src/editor/poles.ts:55-63 slice of src/editor/poles.ts#snapOn (shared): the feature's statements; "…" marks code left out
function snapOn(doc: RoadDoc, net: Network, lines: PoleLines, at: Vec2, reach: number): PoleSnap {
  // … 3 lines
  const footway = onFootway(net, at);
  const on = nearestOnContour(lines, at, footway ? m(12) : LINE_CATCH);
  if (on && onFootway(net, on.at)) return { at: on.at, kind: 'footway' };
// … 1 lines
}

// src/editor/poles.ts:95-101 slice of src/editor/poles.ts#routeGraph (shared): the feature's statements; "…" marks code left out
function routeGraph(net: Network, lines: PoleLines, region: ChangeRect): RouteGraph {
  // … 2 lines
  const graph = buildRouteGraph(net, lines, region);
  graphs.set(lines, graph);
  return graph;
}

// src/editor/poles.ts:113-137 slice of src/editor/poles.ts#routeRun (shared): the feature's statements; "…" marks code left out
function routeRun(net: Network, from: PoleSnap, to: PoleSnap, first = ROUTE_MARGIN):
  { readonly path: Vec2[]; readonly lines: PoleLines } | 'noPlace' | 'noPath' {
  // … 2 lines
  for (let margin = Math.max(first, dist(from.at, to.at)); ; margin *= 2) {
    // … 5 lines
    const lines = poleLinesIn(net, region);
    const a = placeOf(lines, from);
    const b = placeOf(lines, to);
    if (!a || !b) return 'noPlace';
    // … 1 lines
    const found = route(routeGraph(net, lines, region), a, b, endOff(from, a), endOff(to, b), reached);
    // … 1 lines
    if (!found && reached.x0 > region[0] + CLOSED_PAD && reached.y0 > region[1] + CLOSED_PAD && reached.x1 < region[2] - CLOSED_PAD && reached.y1 < region[3] - CLOSED_PAD) return 'noPath';
    // … 3 lines
    if (found && (found.cost < 2 * reach || last)) return { path: found.path, lines };
  // … 1 lines
  }
}

// src/editor/poles.ts:146-149 slice of src/editor/poles.ts#regional (shared): the feature's statements; "…" marks code left out
const regional = (net: Network, first: number): Routing => ({
  linesAt: (at) => poleLinesAt(net, at),
  route: (from, to) => routeRun(net, from, to, first),
});

// src/editor/poles.ts:160-199 slice of src/editor/poles.ts#poleRunPlanner (shared): the feature's statements; "…" marks code left out
export function poleRunPlanner(net: Network): PoleRunPlanner {
// … 38 lines
}

// src/editor/poles.ts:177-197 slice of src/editor/poles.ts#poleRunPlanner.plan (shared): the feature's statements; "…" marks code left out
    plan(doc, rawFrom, rawTo, reach, spacing = DEFAULT_POLE_SPACING, lamps = 'alternate') {
      // … 1 lines
      if (!inner(tile.region, rawFrom) || !inner(tile.region, rawTo)) {
        return computePoleRun(doc, net, rawFrom, rawTo, reach, spacing, lamps, regional(net, ROUTE_MARGIN));
      }
      return computePoleRun(doc, net, rawFrom, rawTo, reach, spacing, lamps, {
        linesAt: () => tile.lines,
        route: (from, to) => {
          const a = placeOf(tile.lines, from);
          const b = placeOf(tile.lines, to);
          if (!a || !b) return 'noPlace';
          const found = route(tile.graph, a, b, endOff(from, a), endOff(to, b));
          const r = tile.region;
          const x0 = Math.min(from.at.x, to.at.x), y0 = Math.min(from.at.y, to.at.y);
          const x1 = Math.max(from.at.x, to.at.x), y1 = Math.max(from.at.y, to.at.y);
          // As in `routeRun`: a way out of the lines and back is longer than twice the distance to their border.
          if (found && found.cost < 2 * Math.min(x0 - r[0], y0 - r[1], r[2] - x1, r[3] - y1)) return { path: found.path, lines: tile.lines };
          return routeRun(net, from, to, ROUTE_MARGIN);
        },
      });
    },

// src/editor/poles.ts:240-258 slice of src/editor/poles.ts#planPoleRun (shared): the feature's statements; "…" marks code left out
export function planPoleRun(
  // … 1 lines
  net: Network,
  // … 3 lines
  spacing = DEFAULT_POLE_SPACING,
// … 1 lines
): PoleRunPlan {
  // … 2 lines
  const k = lastPlan;
  if (k && k.doc === doc && k.net === net && k.revision === net.revision && k.poles === poles &&
      k.fx === rawFrom.x && k.fy === rawFrom.y && k.tx === rawTo.x && k.ty === rawTo.y &&
      k.reach === reach && k.spacing === spacing && k.lamps === lamps) return k.plan;
  const plan = computePoleRun(doc, net, rawFrom, rawTo, reach, spacing, lamps, regional(net, ROUTE_MARGIN));
  lastPlan = { doc, net, revision: net.revision, poles, fx: rawFrom.x, fy: rawFrom.y, tx: rawTo.x, ty: rawTo.y, reach, spacing, lamps, plan };
  return plan;
}

// src/editor/poles.ts:261-265 slice of src/editor/poles.ts#lastPlan (shared): the feature's statements; "…" marks code left out
let lastPlan: {
  readonly doc: RoadDoc; readonly net: Network; readonly revision: number; readonly poles: number;
  readonly fx: number; readonly fy: number; readonly tx: number; readonly ty: number;
  readonly reach: number; readonly spacing: number; readonly lamps: PoleLampMode; readonly plan: PoleRunPlan;
} | null = null;

// src/editor/poles.ts:275-365 slice of src/editor/poles.ts#computePoleRun (shared): the feature's statements; "…" marks code left out
function computePoleRun(
  // … 1 lines
  net: Network,
// … 6 lines
): PoleRunPlan {
  const from = snapOn(doc, net, routing.linesAt(rawFrom), rawFrom, reach);
  const to = snapOn(doc, net, routing.linesAt(rawTo), rawTo, reach);
  // … 1 lines
  if (from.kind === 'free' || to.kind === 'free') return refuse('offFootway');
  if (from.pole !== undefined && from.pole === to.pole) return { from, to, poles: [] };
  if (dist(from.at, to.at) < 1e-3) return { from, to, poles: [] };
  const routed = routing.route(from, to);
  if (routed === 'noPlace' || routed === 'noPath') return refuse('noPath');
  const { path, lines } = routed;
  if (path.length < 2) return refuse('noPath');
  // … 3 lines
  for (let i = 1; i < path.length; i++) cum.push(cum[i - 1]! + dist(path[i - 1]!, path[i]!));
  // … 12 lines
  for (const index of corners(path, CORNER_TOLERANCE).slice(1, -1)) {
    const s = cum[index]!;
    if (s < MIN_POLE_GAP || total - s < MIN_POLE_GAP) continue;
    const last = keys[keys.length - 1]!;
    if (keys.length > 1 && s - last < MIN_POLE_GAP) keys[keys.length - 1] = (last + s) / 2;
    else keys.push(s);
  }
  // … 3 lines
  const { accesses } = lines;
  const step = Math.max(MIN_POLE_SPACING, Math.min(MAX_POLE_SPACING, spacing));
  // … 1 lines
  for (let k = 1; k < keys.length; k++) {
    // … 1 lines
    const spans = Math.max(1, Math.round((s1 - s0) / step));
    for (let i = 1; i <= spans; i++) stations.push(s0 + ((s1 - s0) * i) / spans);
  }
  // … 17 lines
  const points = kept.map((s, i) => (i === 0 ? from.at : i === kept.length - 1 ? to.at : at(s)));
  // … 1 lines
  points.forEach((p, index) => {
    const existing = index === 0 && from.pole !== undefined ? doc.pole(from.pole)
      : index === points.length - 1 && to.pole !== undefined ? doc.pole(to.pole)
        : doc.poleNear(p, REUSE_RADIUS);
    const planned: PlannedPole = {
      at: existing ? { x: existing.x, y: existing.y } : p,
      existing: existing?.id ?? null,
      // A pole that already exists keeps the lamp it has.
      lamp: existing ? existing.lamp : lampFor(index, lamps),
    };
    const prev = poles[poles.length - 1];
    // The same standing pole twice in a row is one pole.
    if (prev && planned.existing !== null && prev.existing === planned.existing) return;
    poles.push(planned);
  });
  if (poles.length < 2) return { from, to, poles: [] };
  return { from, to, poles };
}

// src/editor/poles.ts:391-404 slice of src/editor/poles.ts#PoleToolHost (shared): the feature's statements; "…" marks code left out
export interface PoleToolHost {
  // … 1 lines
  readonly net: Network;
// … 10 lines
}
