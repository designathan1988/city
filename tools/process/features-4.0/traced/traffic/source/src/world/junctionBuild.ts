// The code of src/world/junctionBuild.ts that is the feature's (the rest of the file is shared).

// src/world/junctionBuild.ts:26-28 slice of src/world/junctionBuild.ts#JunctionLane (shared): the feature's statements; "…" marks code left out
export interface JunctionLane extends Omit<LaneAsk, 'centre'> {
// … 1 lines
}

// src/world/junctionBuild.ts:31-38 slice of src/world/junctionBuild.ts#JunctionInput (shared): the feature's statements; "…" marks code left out
export interface JunctionInput {
  readonly node: JunctionNode;
  // … 1 lines
  readonly legs: readonly JunctionLeg[];
// … 3 lines
}

// src/world/junctionBuild.ts:41-53 slice of src/world/junctionBuild.ts#MadeConnector (shared): the feature's statements; "…" marks code left out
interface MadeConnector {
  // … 3 lines
  readonly turn: TurnKind;
  // … 4 lines
  readonly maxBodyClass: BodyClass | -1;
  // … 1 lines
  readonly again?: () => TurnPathResult;
}

// src/world/junctionBuild.ts:63-67 function isJunction: the feature's own
export function isJunction(node: Pick<RoadNode, 'incident' | 'end'>, inbound: number, outbound: number): boolean {
  // A road's end that is a turning circle turns its traffic round (`junction/bulb.ts`).
  const bulb = node.incident.length === 1 && node.end === 'bulb';
  return (node.incident.length >= 2 || bulb) && inbound > 0 && outbound > 0;
}

// src/world/junctionBuild.ts:70-78 slice of src/world/junctionBuild.ts#junctionKey (shared): the feature's statements; "…" marks code left out
export function junctionKey(j: JunctionInput, surfaceKey: string): string {
  // … 1 lines
  return `${node.id}|${new Digest().addText(String(surfaceKey)).addText(node.control)
    .addText(node.crossing ? `${node.crossing.kind},${node.crossing.segment}` : '-')
    .addText(node.incident.join(',')).addText(node.blockedMovements.join(',')).addText(linksDigest(node.laneLinks))
    .addText(`${node.uturn ?? '-'}${node.end ?? '-'}`)
    .addText(j.inbound.map((lane) => lane.key).join('|'))
    .addText(j.outbound.map((lane) => lane.key).join('|')).value()}`;
}

// src/world/junctionBuild.ts:81-84 slice of src/world/junctionBuild.ts#legLine (shared): the feature's statements; "…" marks code left out
function legLine(leg: JunctionLeg, from: NodeId): Polyline {
  const line = segmentLine(leg.from, leg.to, leg.segment.curve);
  return leg.segment.a === from ? line : line.reversed();
}

// src/world/junctionBuild.ts:87-225 slice of src/world/junctionBuild.ts#junctionMovements (shared): the feature's statements; "…" marks code left out
export function junctionMovements(j: JunctionInput, surfaceOf: () => JunctionSurface | null, turns: JunctionTurns): JunctionMade {
  // … 2 lines
  const legOf = new Map<SegmentId, JunctionLeg>();
  for (const l of j.legs) legOf.set(l.segment.id, l);
  // … 7 lines
  for (const lane of inbound) approaching.add(lane.segment);
  // … 38 lines
  for (let i = 0; i < inbound.length; i++) {
    // … 2 lines
    const incomingSegment = leg(inLane.segment).segment;
    const rules = inLane.from === incomingSegment.a ? incomingSegment.section?.turnsForward : incomingSegment.section?.turnsBackward;
    const rule = rules?.[inLane.laneIndex];
    const authored = rule !== undefined && rule !== 'all';
    // … 1 lines
    const manual = linksOf(node.laneLinks, inLane.segment, inLane.laneIndex);
    if (manual.length) {
      let count = 0;
      for (const link of manual) {
        const o = outbound.findIndex((lane) => lane.segment === link.to && lane.laneIndex === link.toLane);
        if (o < 0) continue;
        const outLane = outbound[o]!;
        if (node.blockedMovements.includes(movementKey(inLane.segment, link.to))) continue;
        const isReverse = outLane.segment === inLane.segment;
        const turn = isReverse ? 'uturn' : classifyTurn(inDir, tangentAtStart(outLane.centre));
        const carried = !isReverse && road !== null && road.includes(inLane.segment) && road.includes(link.to);
        addConnector(i, o, turn, carried);
        count++;
      }
      if (count > 0) continue;
    }
    const legal: { o: number; turn: TurnKind; carried: boolean }[] = [];
    // … 1 lines
    const dropped: typeof legal = [];
    // … 1 lines
    for (let o = 0; o < outbound.length; o++) {
      // … 2 lines
      const isReverse = outLane.segment === inLane.segment;
      // … 1 lines
      const turn = isReverse ? 'uturn' : classifyTurn(inDir, outDir);
      if (!laneTurnAllowed(rule, turn)) continue;
      // … 1 lines
      const carried = !isReverse && road !== null &&
        road.includes(inLane.segment) && road.includes(outLane.segment);
      const inSegment = leg(inLane.segment).segment;
      const inLanes = travelLanes(
        roadProfile(inSegment.type, inSegment.lanes, inSegment.direction, inSegment.section, inSegment.parking),
        inSegment.direction,
      );
      const outSegment = leg(outLane.segment).segment;
      const outLanes = travelLanes(
        roadProfile(outSegment.type, outSegment.lanes, outSegment.direction, outSegment.section, outSegment.parking),
        outSegment.direction,
      );
      if (node.blockedMovements.includes(movementKey(inLane.segment, outLane.segment))) continue;
      if (turn === 'uturn' && outbound.length > inLanes) {
        dropped.push({ o, turn, carried });
        continue;
      }
      legal.push({ o, turn, carried });
      const receivesAuthoredTurn = turn === 'left' ? outLane.laneIndex === 0 :
        turn === 'right' ? outLane.laneIndex === outLanes - 1 :
          outLane.laneIndex === inLane.laneIndex;
      if (authored ? receivesAuthoredTurn : laneIsPlausible(inLane, outLane, carried ? 'through' : turn, inLanes, outLanes)) {
        addConnector(i, o, turn, carried);
      }
    }
    // … 2 lines
    if (exits[i] === 0 && !legal.length) legal.push(...dropped);
    if (exits[i] === 0 && legal.length) {
      legal.sort((a, b) =>
        fallbackTurnRank(a.turn) - fallbackTurnRank(b.turn) ||
        outbound[a.o]!.laneIndex - outbound[b.o]!.laneIndex ||
        outbound[a.o]!.id.localeCompare(outbound[b.o]!.id),
      );
      const fallback = legal[0]!;
      // A lane that merges into the road it is on is still that road
      addConnector(i, fallback.o, fallback.turn, fallback.carried);
    }
  }
  // … 2 lines
  if (node.uturn !== undefined && node.incident.length === 2) {
    for (let i = 0; i < inbound.length; i++) {
      // … 1 lines
      if (inLane.segment !== node.uturn || inLane.laneIndex !== 0) continue;
      for (let o = 0; o < outbound.length; o++) if (outbound[o]!.segment === inLane.segment) addConnector(i, o, 'uturn');
    }
  }
// … 3 lines
}

// src/world/junctionBuild.ts:92-96 slice of src/world/junctionBuild.ts#junctionMovements.leg (shared): the feature's statements; "…" marks code left out
  const leg = (segment: SegmentId): JunctionLeg => {
    const found = legOf.get(segment);
    if (!found) throw new Error(`junction ${node.id} has no road ${segment}`);
    return found;
  };

// src/world/junctionBuild.ts:116-136 slice of src/world/junctionBuild.ts#junctionMovements.addConnector (shared): the feature's statements; "…" marks code left out
  const addConnector = (i: number, o: number, turn: TurnKind, carried = false): void => {
    const inLane = inbound[i]!, outLane = outbound[o]!;
    const cid = connectorId(inLane.id, outLane.id);
    if (ids.has(cid)) return;
    const waiting = inbound.filter((lane) => lane.id !== inLane.id);
    const inCentre = inLane.centre, outCentre = outLane.centre;
    const deferred = turns.deferred;
    const shape = (): TurnPathResult => (bulb
      ? bulbTurnPath(inCentre, outCentre, surfaceOf(), { x: node.x, y: node.y }, BULB_TURN_RADIUS)
      : node.uturn === inLane.segment && turn === 'uturn' && node.incident.length === 2
        ? medianUturnPath(inCentre, outCentre, surfaceOf())
        : turnPath(inCentre, outCentre, surfaceOf(), waiting, { fromRoadEnd: inLane.fromEdge, toRoadEnd: outLane.toEdge }, turns));
    const { path, maxBodyClass } = shape();
    ids.add(cid);
    exits[i]!++;
    made.push({
      from: i, to: o, turn, carried, group: groupBySegment.get(inLane.segment) ?? 0,
      speedLimit: Math.min(inLane.speedLimit, outLane.speedLimit) * turnSpeedFactor(turn), path, maxBodyClass,
      ...(turns.deferred !== deferred ? { again: shape } : {}),
    });
  };

// src/world/junctionBuild.ts:123-127 slice of src/world/junctionBuild.ts#junctionMovements.addConnector.shape (shared): the feature's statements; "…" marks code left out
    const shape = (): TurnPathResult => (bulb
      ? bulbTurnPath(inCentre, outCentre, surfaceOf(), { x: node.x, y: node.y }, BULB_TURN_RADIUS)
      : node.uturn === inLane.segment && turn === 'uturn' && node.incident.length === 2
        ? medianUturnPath(inCentre, outCentre, surfaceOf())
        : turnPath(inCentre, outCentre, surfaceOf(), waiting, { fromRoadEnd: inLane.fromEdge, toRoadEnd: outLane.toEdge }, turns));

// src/world/junctionBuild.ts:230-272 function junctionAsk: the feature's own
export function junctionAsk(j: JunctionInput, surface: SurfaceSource, before: JunctionTurns | null,
  sent: { readonly lanes: Map<string, LaneAsk>; readonly legs: Map<SegmentId, JunctionLeg> }): JunctionAsk {
  const n = j.node;
  const lane = (l: JunctionLane): LaneAsk => {
    let made = sent.lanes.get(l.id);
    if (!made) {
      sent.lanes.set(l.id, made = {
        id: l.id, key: l.key, segment: l.segment, from: l.from, to: l.to, laneIndex: l.laneIndex, speedLimit: l.speedLimit,
        centre: l.centre.xy, fromEdge: l.fromEdge, toEdge: l.toEdge,
      });
    }
    return made;
  };
  const leg = ({ segment: s, from, to }: JunctionLeg): JunctionLeg => {
    let made = sent.legs.get(s.id);
    if (!made) {
      sent.legs.set(s.id, made = {
        segment: {
          id: s.id, a: s.a, b: s.b, curve: s.curve, type: s.type, lanes: s.lanes, direction: s.direction,
          ...(s.section ? { section: s.section } : {}), ...(s.parking ? { parking: s.parking } : {}),
        },
        from, to,
      });
    }
    return made;
  };
  return {
    node: {
      id: n.id, x: n.x, y: n.y, incident: n.incident, control: n.control, blockedMovements: n.blockedMovements,
      ...(n.crossing ? { crossing: n.crossing } : {}), ...(n.laneLinks ? { laneLinks: n.laneLinks } : {}),
      ...(n.uturn !== undefined ? { uturn: n.uturn } : {}), ...(n.end !== undefined ? { end: n.end } : {}),
    },
    legs: j.legs.map(leg),
    inbound: j.inbound.map(lane),
    outbound: j.outbound.map(lane),
    surface: {
      centre: surface.centre,
      parts: surface.parts.map((p) => ('full' in p ? { full: p.full.xy, half: p.half, mouth: p.mouth } : 'edges' in p ? p.flatXY()
        : Float64Array.from(p.flatMap((q) => [q.x, q.y])))),
    },
    before: before ? before.entries() : null,
  };
}

// src/world/junctionBuild.ts:275-306 slice of src/world/junctionBuild.ts#buildJunctionOf (shared): the feature's statements; "…" marks code left out
export function buildJunctionOf(ask: JunctionAsk, lines: Map<Float64Array, Polyline>, rings: Map<Float64Array, Vec2[]>): JunctionAnswer {
  // … 11 lines
  const input: JunctionInput = { node: ask.node, legs: ask.legs, inbound: ask.inbound.map(lane), outbound: ask.outbound.map(lane) };
  const source: SurfaceSource = {
    centre: ask.surface.centre,
    parts: ask.surface.parts.map((p) => (p instanceof Float64Array ? ring(p) : { full: line(p.full), half: p.half, mouth: p.mouth })),
  };
  let surface: JunctionSurface | undefined;
  // … 1 lines
  const key = junctionKey(input, surfaceOf().key);
  const turns = new JunctionTurns(ask.before ? JunctionTurns.of(ask.before) : null);
  const made = junctionMovements(input, surfaceOf, turns);
  return {
    key,
    groups: made.groups,
    signalised: made.signalised,
    connectors: made.connectors.map((c) => ({
      from: c.from, to: c.to, turn: c.turn, carried: c.carried, group: c.group, speedLimit: c.speedLimit, maxBodyClass: c.maxBodyClass, path: c.path.xy,
    })),
    turns: turns.settle().entries(),
  };
}

// src/world/junctionBuild.ts:281-285 slice of src/world/junctionBuild.ts#buildJunctionOf.ring (shared): the feature's statements; "…" marks code left out
  const ring = (xy: Float64Array): Vec2[] => {
    // … 1 lines
    if (!made) rings.set(xy, (made = unflat(xy)));
    return made;
  };

// src/world/junctionBuild.ts:286-286 slice of src/world/junctionBuild.ts#buildJunctionOf.lane (shared): the feature's statements; "…" marks code left out
  const lane = (l: LaneAsk): JunctionLane => ({ ...l, centre: line(l.centre) });

// src/world/junctionBuild.ts:293-293 slice of src/world/junctionBuild.ts#buildJunctionOf.surfaceOf (shared): the feature's statements; "…" marks code left out
  const surfaceOf = (): JunctionSurface => (surface ??= new JunctionSurface(source));

// src/world/junctionBuild.ts:313-320 slice of src/world/junctionBuild.ts#classifyTurn (shared): the feature's statements; "…" marks code left out
export function classifyTurn(inDir: Vec2, outDir: Vec2): TurnKind {
// … 6 lines
}

// src/world/junctionBuild.ts:322-334 slice of src/world/junctionBuild.ts#shouldSignalise (shared): the feature's statements; "…" marks code left out
function shouldSignalise(
  // … 2 lines
  leg: (segment: SegmentId) => JunctionLeg,
): boolean {
  // … 2 lines
  return incident.length >= 3 &&
    (incident.length >= 4 || incident.some((s) => {
      const { segment } = leg(s);
      return roadProfile(segment.type, segment.lanes, segment.direction, segment.section, segment.parking).lanes >= 4;
    }));
}

// src/world/junctionBuild.ts:336-337 slice of src/world/junctionBuild.ts#turnSpeedFactor (shared): the feature's statements; "…" marks code left out
const turnSpeedFactor = (turn: TurnKind): number =>
  turn === 'through' ? 1 : turn === 'right' ? 0.6 : turn === 'left' ? 0.65 : 0.45;

// src/world/junctionBuild.ts:358-384 slice of src/world/junctionBuild.ts#carriedPair (shared): the feature's statements; "…" marks code left out
function carriedPair(node: JunctionNode, leg: (segment: SegmentId) => JunctionLeg): readonly SegmentId[] | null {
  const incident = [...new Set(node.incident)];
  if (incident.length === 2) return incident;
  if (incident.length < 3) return null;
  const legs = incident.map((segment) => {
    const seg = leg(segment).segment;
    return { segment, type: seg.section?.priority ?? seg.type, dir: smoothedDirectionFromNode(legLine(leg(segment), node.id)) };
  });
  // … 1 lines
  for (let i = 0; i < legs.length; i++) {
    for (let j = i + 1; j < legs.length; j++) {
      const a = legs[i]!;
      const b = legs[j]!;
      // Both directions point away from the node: a straight road has them opposed, so the bend is what is left of a half turn.
      const between = Math.abs(normalizeAngle(angleOf(b.dir) - angleOf(a.dir)));
      const bend = Math.PI - between;
      if (bend < STRAIGHT_BEND) return null;
      pairs.push({ members: [a.segment, b.segment], rank: Math.min(a.type, b.type), bend });
    }
  }
// … 6 lines
}

// src/world/junctionBuild.ts:394-406 slice of src/world/junctionBuild.ts#laneIsPlausible (shared): the feature's statements; "…" marks code left out
function laneIsPlausible(
  // … 2 lines
  turn: TurnKind,
// … 2 lines
): boolean {
  const i = inLane.laneIndex;
  const o = outLane.laneIndex;
  if (turn === 'left' || turn === 'uturn') return i === 0 && o === 0;
  if (turn === 'right') return i === inLanes - 1 && o === outLanes - 1;
  return o === i;
}

// src/world/junctionBuild.ts:409-414 slice of src/world/junctionBuild.ts#fallbackTurnRank (shared): the feature's statements; "…" marks code left out
function fallbackTurnRank(turn: TurnKind): number {
  if (turn === 'through') return 0;
  if (turn === 'right') return 1;
  if (turn === 'left') return 2;
// … 1 lines
}
