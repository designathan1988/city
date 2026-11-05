// The code of src/world/markings.ts that is the feature's (the rest of the file is shared).

// src/world/markings.ts:346-367 slice of src/world/markings.ts#junctionDetailAt (shared): the feature's statements; "…" marks code left out
export function junctionDetailAt(net: Network, node: NodeId, stops: Bar[], zebras: Bar[]): void {
  const junction = net.junctions.get(node)?.get(Level.Asphalt);
  if (!junction) return;
  if (junction.legs.some((leg) => leg.road.id === 'highway' || leg.road.id === 'ramp')) return;
  // … 2 lines
  const movementsConflict = junction.legs.length >= 3;
  // … 1 lines
  junction.legs.forEach((leg, i) => {
    const rt = leg.road;
    const trim = junction.trims[i] as number;
    // A stop line only means something where traffic arrives.
    const crossing = net.crosswalkDistanceAt(leg.seg, node);
    if (movementsConflict && leg.approaching) {
      const bar = stopBar(leg.origin, leg.dir, leg.nrm, rt, trim, leg.shift);
      if (bar) stops.push(bar);
    }
    if (crossing > 0) {
      zebras.push(...crosswalkBars(leg.origin, leg.dir, leg.nrm, rt, trim));
    }
  });
}

// src/world/markings.ts:401-451 slice of src/world/markings.ts#transitionMarkings (shared): the feature's statements; "…" marks code left out
export function transitionMarkings(net: Network, node: NodeId): StrokeSpec[] {
  const junction = net.junctions.get(node)?.get(Level.Asphalt);
  if (!junction || !junction.transition || junction.legs.length !== 2) return [];
  const a = junction.legs[0] as Leg;
  const b = junction.legs[1] as Leg;
  const ra = a.road;
  const rb = b.road;
  const axis = new TransitionAxis(a, junction.trims[0] as number, b, junction.trims[1] as number);
  // … 8 lines
  if (a.direction !== 'both' || b.direction !== 'both') return out;
  if (ra.markings === 'none' && rb.markings === 'none') return out;
  // … 1 lines
  const median = mix(ra.median / 2, rb.median / 2);
  if (ra.median > 0 || rb.median > 0) {
    // The two directions part to either side of the reservation's nose.
    const colour = markingColor(rb.median > 0 ? rb : ra);
    solid((u) => median(u), m(0.28), colour);
    solid((u) => -median(u), m(0.28), colour);
  } else {
    // One solid line, in each road's own colour on its own half of the taper.
    const line = axis.offset(() => 0);
    const mid = Math.floor(line.length / 2);
    const first = ra.markings === 'none' ? rb : ra;
    const second = rb.markings === 'none' ? ra : rb;
    out.push({ points: line.slice(0, mid + 1), width: m(0.16), color: markingColor(first), dash: null, dashOffset: 0 });
    out.push({ points: line.slice(mid), width: m(0.16), color: markingColor(second), dash: null, dashOffset: 0 });
  }
  // … 2 lines
  const shared = Math.min(lanesPerDirection(ra), lanesPerDirection(rb));
  const lane = mix(laneWidth(ra), laneWidth(rb));
  for (let k = 1; k < shared; k++) {
    for (const side of [-1, 1]) {
      out.push({
        points: axis.offset((u) => side * (median(u) + lane(u) * k)),
        width: m(0.128),
        color: LANE_LINE,
        dash: DASH,
        dashOffset: 0,
      });
    }
  }
// … 1 lines
}
