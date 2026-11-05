// The code of src/editor/poles.ts that is the feature's (the rest of the file is shared).

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
  // … 5 lines
  spacing = DEFAULT_POLE_SPACING,
// … 1 lines
): PoleRunPlan {
  // … 3 lines
  if (k && k.doc === doc && k.net === net && k.revision === net.revision && k.poles === poles &&
      k.fx === rawFrom.x && k.fy === rawFrom.y && k.tx === rawTo.x && k.ty === rawTo.y &&
      k.reach === reach && k.spacing === spacing && k.lamps === lamps) return k.plan;
  const plan = computePoleRun(doc, net, rawFrom, rawTo, reach, spacing, lamps, regional(net, ROUTE_MARGIN));
  lastPlan = { doc, net, revision: net.revision, poles, fx: rawFrom.x, fy: rawFrom.y, tx: rawTo.x, ty: rawTo.y, reach, spacing, lamps, plan };
  return plan;
}

// src/editor/poles.ts:275-365 slice of src/editor/poles.ts#computePoleRun (shared): the feature's statements; "…" marks code left out
function computePoleRun(
// … 8 lines
): PoleRunPlan {
  // … 37 lines
  const step = Math.max(MIN_POLE_SPACING, Math.min(MAX_POLE_SPACING, spacing));
  // … 1 lines
  for (let k = 1; k < keys.length; k++) {
    // … 1 lines
    const spans = Math.max(1, Math.round((s1 - s0) / step));
    for (let i = 1; i <= spans; i++) stations.push(s0 + ((s1 - s0) * i) / spans);
  }
// … 36 lines
}
