// The code of src/world/turnPaths.ts that is the feature's (the rest of the file is shared).

// src/world/turnPaths.ts:73-73 function unflat: the feature's own
export const unflat = (xy: Float64Array): Vec2[] => Array.from({ length: xy.length / 2 }, (_, i) => ({ x: xy[i * 2]!, y: xy[i * 2 + 1]! }));

// src/world/turnPaths.ts:85-85 field JunctionSurface.key: the feature's own
  readonly key: string;

// src/world/turnPaths.ts:111-113 method JunctionSurface.flatRings: the feature's own
  flatRings(): Float64Array[] {
    return this.areas.map((a) => (a.flat ??= Float64Array.from({ length: a.poly.length * 2 }, (_, i) => (i % 2 ? a.poly[i >> 1]!.y : a.poly[i >> 1]!.x))));
  }

// src/world/turnPaths.ts:124-126 getter JunctionSurface.empty: the feature's own
  get empty(): boolean {
    return this.areas.length === 0;
  }

// src/world/turnPaths.ts:367-371 interface TurnPathResult: the feature's own
export interface TurnPathResult {
  readonly path: Polyline;
  /** Largest physical body whose swept corners fit; -1 if even a small body cannot. */
  readonly maxBodyClass: BodyClass | -1;
}

// src/world/turnPaths.ts:380-403 function turnPath: the feature's own
export function turnPath(inCentre: Polyline, outCentre: Polyline, surface: JunctionSurface | null,
  waiting: readonly WaitingLane[] = [], ends: RoadEnds = NO_ROAD_ENDS, turns: JunctionTurns | null = null): TurnPathResult {
  if (!surface || surface.empty) return {
    path: bezierTurn(inCentre, outCentre, FALLBACK_HANDLE), maxBodyClass: HEAVY,
  };
  // A junction rebuilt for a change that did not move this movement
  const key = turns ? `${laneEndKey(inCentre, true)}|${laneEndKey(outCentre, false)}|${surface.key}|` +
    `${ends.fromRoadEnd ? 1 : 0}${ends.toRoadEnd ? 1 : 0}|` +
    waiting.map((lane) => laneEndKey(lane.centre, true, QUEUE_REACH)).join(',') : '';
  const known = turns?.find(key);
  if (known !== undefined) return {
    path: makeChoice(inCentre, outCentre, known.choice), maxBodyClass: known.maxBodyClass,
  };
  // Off the page (`TurnPlan`): asked of the workers; the junction stands on the old shape until built again with the answer.
  if (turns?.deferring) {
    turns.defer(key, () => ({ inXY: inCentre.xy, outXY: outCentre.xy, waiting: waiting.map((lane) => lane.centre.xy), surfaceKey: surface.key, ...ends }),
      () => ({ centre: surface.centre, rings: surface.flatRings() }));
    return { path: bezierTurn(inCentre, outCentre, FALLBACK_HANDLE), maxBodyClass: HEAVY };
  }
  // DO NOT sweep the chosen path again, class by class, to find it
  const { choice, path, body: maxBodyClass } = chooseTurn(inCentre, outCentre, surface, waiting, ends);
  turns?.keep(key, { choice, maxBodyClass });
  return { path, maxBodyClass };
}

// src/world/turnPaths.ts:406-442 function bulbTurnPath: the feature's own
export function bulbTurnPath(inCentre: Polyline, outCentre: Polyline, surface: JunctionSurface | null,
  centre: Vec2, radius: number): TurnPathResult {
  const start = inCentre.sampleAt(inCentre.length), end = outCentre.sampleAt(0);
  const a = start.p, b = end.p;
  const angleAt = (p: Vec2): number => Math.atan2(p.y - centre.y, p.x - centre.x);
  const aIn = angleAt(a), aOut = angleAt(b);
  // Round the side the arriving lane looks into: the far side of the circle.
  const ahead = { x: a.x + start.t.x * radius, y: a.y + start.t.y * radius };
  const back = angleAt(ahead);
  const wrap = (x: number): number => ((x % (2 * Math.PI)) + 2 * Math.PI) % (2 * Math.PI);
  const ccwSweep = wrap(aOut - aIn);
  const sign = wrap(back - aIn) < ccwSweep ? 1 : -1;
  const sweep = sign > 0 ? ccwSweep : 2 * Math.PI - ccwSweep;
  // On the circle 45° in from each end, joined to the lanes by cubics tangent to both
  const lead = Math.min(Math.PI / 4, sweep / 3);
  const p1a = aIn + sign * lead, p2a = aIn + sign * (sweep - lead);
  const on = (ang: number): Vec2 => ({ x: centre.x + Math.cos(ang) * radius, y: centre.y + Math.sin(ang) * radius });
  const tangent = (ang: number): Vec2 => ({ x: -Math.sin(ang) * sign, y: Math.cos(ang) * sign });
  const cubic = (p0: Vec2, t0: Vec2, p3: Vec2, t3: Vec2, out: Vec2[]): void => {
    const h = 0.45 * Math.hypot(p3.x - p0.x, p3.y - p0.y);
    const c1 = addScaled(p0, t0, h), c2 = addScaled(p3, t3, -h);
    for (let i = out.length ? 1 : 0; i <= STEPS / 2; i++) {
      const t = i / (STEPS / 2), u = 1 - t;
      const w0 = u * u * u, w1 = 3 * u * u * t, w2 = 3 * u * t * t, w3 = t * t * t;
      out.push({ x: w0 * p0.x + w1 * c1.x + w2 * c2.x + w3 * p3.x, y: w0 * p0.y + w1 * c1.y + w2 * c2.y + w3 * p3.y });
    }
  };
  const pts: Vec2[] = [];
  cubic(a, start.t, on(p1a), tangent(p1a), pts);
  const arcSteps = Math.max(4, Math.ceil(stepsIn((sweep - 2 * lead) * radius, 1)));
  for (let i = 1; i <= arcSteps; i++) pts.push(on(p1a + sign * ((sweep - 2 * lead) * i) / arcSteps));
  cubic(on(p2a), tangent(p2a), b, end.t, pts);
  const path = Polyline.fromPoints(pts);
  if (!surface || surface.empty) return { path, maxBodyClass: HEAVY };
  const maxBodyClass = ([HEAVY, 1, 0] as const).find((body) => turnFits(inCentre, path, outCentre, surface, body)) ?? -1;
  return { path, maxBodyClass };
}

// src/world/turnPaths.ts:448-475 function medianUturnPath: the feature's own
export function medianUturnPath(inCentre: Polyline, outCentre: Polyline, surface: JunctionSurface | null): TurnPathResult {
  const start = inCentre.sampleAt(inCentre.length), end = outCentre.sampleAt(0);
  const a = start.p, b = end.p, t = start.t;
  // Across, towards the departing lane; and how far along the road it starts from here.
  const across = { x: b.x - a.x, y: b.y - a.y };
  const ahead = across.x * t.x + across.y * t.y;
  const side = { x: across.x - t.x * ahead, y: across.y - t.y * ahead };
  const width = Math.hypot(side.x, side.y);
  if (width < GEO_EPS) return { path: Polyline.fromPoints([a, b]), maxBodyClass: -1 };
  const n = { x: side.x / width, y: side.y / width };
  const r = width / 2;
  // Straight on first where the departing lane begins further along (`ahead` > 0).
  const from = ahead > 0 ? addScaled(a, t, ahead) : a;
  const centre = addScaled(from, n, r);
  const pts: Vec2[] = ahead > GEO_EPS ? [a] : [];
  const steps = Math.max(12, Math.ceil(stepsIn(Math.PI * r, 0.5)));
  for (let i = 0; i <= steps; i++) {
    const phi = (Math.PI * i) / steps;
    // From `from` (angle -n), forward round the far side (+t), to the other lane (+n).
    const c = Math.cos(phi), s = Math.sin(phi);
    pts.push({ x: centre.x - n.x * r * c + t.x * r * s, y: centre.y - n.y * r * c + t.y * r * s });
  }
  if (ahead < -GEO_EPS) pts.push(b);
  const path = Polyline.fromPoints(pts);
  const maxBodyClass = ([HEAVY, 1, 0] as const).find((body) => r >= (CENTRE_TURN_RADIUS[body] ?? Infinity) - m(0.05) &&
    (!surface || surface.empty || turnFits(inCentre, path, outCentre, surface, body))) ?? -1;
  return { path, maxBodyClass };
}

// src/world/turnPaths.ts:478-481 function turnFits: the feature's own
export function turnFits(inCentre: Polyline, path: Polyline, outCentre: Polyline,
  surface: JunctionSurface, body: BodyClass, ends: RoadEnds = NO_ROAD_ENDS): boolean {
  return sweep(surface, inCentre, path, outCentre, [], body, ends) !== null;
}

// src/world/turnPaths.ts:483-486 function makeChoice: the feature's own
const makeChoice
 = (a: Polyline, b: Polyline, choice: TurnChoice): Polyline =>
  typeof choice === 'number' ? bezierTurn(a, b, choice) :
    twoPieceTurn(a, b, { x: choice.x, y: choice.y }, choice.share);

// src/world/turnPaths.ts:518-535 function laneEndKey: the feature's own
function laneEndKey(lane: Polyline, atEnd: boolean, reach = LANE_REACH): string {
  let byReach = laneKeys.get(lane);
  const tag = `${atEnd ? 'e' : 's'}${reach}`;
  const cached = byReach?.get(tag);
  if (cached) return cached;
  const digest = new Digest();
  digest.add(lane.length);
  for (let d = 0; d <= reach; d += KEY_STEP) {
    const f = lane.sampleAt(atEnd ? Math.max(0, lane.length - d) : Math.min(lane.length, d));
    digest.point(f.p);
  }
  const end = lane.sampleAt(atEnd ? lane.length : 0).t;
  digest.point(end);
  const value = digest.value();
  if (!byReach) laneKeys.set(lane, (byReach = new Map()));
  byReach.set(tag, value);
  return value;
}

// src/world/turnPaths.ts:583-632 slice of src/world/turnPaths.ts#chooseTurnFor (shared): the feature's statements; "…" marks code left out
function chooseTurnFor(candidates: Candidates, surface: JunctionSurface,
  queues: readonly Queue[], body: BodyClass, ends: RoadEnds): { choice: TurnChoice; path: Polyline } | null {
  // … 6 lines
  if (base === null) {
    // … 20 lines
    for (let w = 0; w < WAYPOINTS.length; w++) {
      // … 1 lines
      const point = { x: surface.centre.x + offset.x, y: surface.centre.y + offset.y };
    // … 6 lines
    }
  // … 2 lines
  }
// … 7 lines
}
