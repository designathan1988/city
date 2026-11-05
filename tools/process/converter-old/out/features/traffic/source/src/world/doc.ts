// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:64-70 slice of src/world/doc.ts#fitRoadCurve (shared): the feature's statements; "…" marks code left out
export function fitRoadCurve(
  // … 2 lines
  parking?: SegmentParking,
): CurveShape | null {
  return fitShapeToRadius(a, b, shape, casingHalf(roadProfile(type, lanes, direction, section, parking)));
}

// src/world/doc.ts:89-113 slice of src/world/doc.ts#RoadNode (shared): the feature's statements; "…" marks code left out
export interface RoadNode {
  // … 16 lines
  approachRules?: readonly ApproachRuleEntry[];
  // … 1 lines
  signal?: SignalSettings;
// … 4 lines
}

// src/world/doc.ts:124-144 slice of src/world/doc.ts#RoadSegment (shared): the feature's statements; "…" marks code left out
export interface RoadSegment {
  // … 14 lines
  parking?: SegmentParking;
// … 4 lines
}

// src/world/doc.ts:732-765 slice of src/world/doc.ts#RoadDoc.addSegment (shared): the feature's statements; "…" marks code left out
  addSegment(
    // … 9 lines
    parking?: SegmentParking,
  ): RoadSegment | null {
    // … 3 lines
    const authoredParking = normalizeParking(parking);
    // … 2 lines
    const s: RoadSegment = {
      // … 4 lines
      ...(authoredParking ? { parking: authoredParking } : {}),
    };
  // … 8 lines
  }

// src/world/doc.ts:964-974 slice of src/world/doc.ts#RoadDoc.setSegmentParking (shared): the feature's statements; "…" marks code left out
  setSegmentParking(id: SegmentId, parking?: SegmentParking): void {
    // … 1 lines
    const next = normalizeParking(parking);
    if (!was || sameParking(was.parking, next)) return;
    // … 2 lines
    if (next) segment.parking = next;
    else delete segment.parking;
  // … 2 lines
  }

// src/world/doc.ts:1027-1036 slice of src/world/doc.ts#RoadDoc.setNodeApproachRules (shared): the feature's statements; "…" marks code left out
  setNodeApproachRules(id: NodeId, rules: readonly ApproachRuleEntry[] | undefined): void {
    // … 2 lines
    const next = normalizeApproachRules(rules);
    if (rulesDigest(was.approachRules) === rulesDigest(next)) return;
    // … 1 lines
    if (next) node.approachRules = next;
    else delete node.approachRules;
  // … 1 lines
  }

// src/world/doc.ts:1039-1048 slice of src/world/doc.ts#RoadDoc.setNodeSignal (shared): the feature's statements; "…" marks code left out
  setNodeSignal(id: NodeId, signal: SignalSettings | undefined): void {
    // … 2 lines
    const next = normalizeSignalSettings(signal);
    if (signalDigest(was.signal) === signalDigest(next)) return;
    // … 1 lines
    if (next) node.signal = next;
    else delete node.signal;
  // … 1 lines
  }

// src/world/doc.ts:3029-3047 slice of src/world/doc.ts#segmentRecord (shared): the feature's statements; "…" marks code left out
function segmentRecord(s: SerializedDoc['segments'][number], a: NodeId, b: NodeId): RoadSegment {
  // … 1 lines
  const parking = normalizeParking(s.parking);
  return {
    // … 9 lines
    ...(parking ? { parking } : {}),
  // … 3 lines
  };
}

// src/world/doc.ts:3050-3064 slice of src/world/doc.ts#finishNode (shared): the feature's statements; "…" marks code left out
function finishNode(node: RoadNode, n: SerializedDoc['nodes'][number]): void {
  // … 7 lines
  const rules = normalizeApproachRules(n.approachRules);
  if (rules) node.approachRules = rules;
  const signal = normalizeSignalSettings(n.signal);
  if (signal) node.signal = signal;
// … 2 lines
}

// src/world/doc.ts:3181-3255 slice of src/world/doc.ts#SerializedDoc (shared): the feature's statements; "…" marks code left out
export interface SerializedDoc {
  // … 18 lines
  readonly segments: readonly {
    id: number;
    a: number;
    b: number;
    type: number;
    curve: CurveShape | null;
    dashOrigin?: number;
    direction?: SegmentDirection;
    lanes?: number | null;
    section?: RoadSection;
    parking?: SegmentParking;
    /** A current structure id, or a legacy one `migrateStructure` maps. */
    structure?: RoadStructure | 'viaduct';
    /** Retaining walls in its cutting; absent on every older map. */
    cutWalls?: boolean;
  }[];
// … 39 lines
}

// src/world/doc.ts:3271-3277 slice of src/world/doc.ts#sameNode (shared): the feature's statements; "…" marks code left out
function sameNode(p: RoadNode, q: RoadNode): boolean {
  return p.x === q.x && p.y === q.y && p.heightOffset === q.heightOffset && p.smooth === q.smooth &&
    p.control === q.control && sameList(p.incident, q.incident) && sameList(p.blockedMovements, q.blockedMovements) &&
    p.crossing?.kind === q.crossing?.kind && p.crossing?.segment === q.crossing?.segment &&
    linksDigest(p.laneLinks) === linksDigest(q.laneLinks) && rulesDigest(p.approachRules) === rulesDigest(q.approachRules) &&
    signalDigest(p.signal) === signalDigest(q.signal) && p.end === q.end && p.uturn === q.uturn;
}

// src/world/doc.ts:3280-3285 slice of src/world/doc.ts#sameSegment (shared): the feature's statements; "…" marks code left out
function sameSegment(p: RoadSegment, q: RoadSegment): boolean {
  return p.a === q.a && p.b === q.b && p.type === q.type && p.dashOrigin === q.dashOrigin &&
    p.direction === q.direction && p.lanes === q.lanes && p.structure === q.structure && p.cutWalls === q.cutWalls &&
    sameRoadSection(p.section, q.section) && sameParking(p.parking, q.parking) &&
    (p.curve?.t ?? null) === (q.curve?.t ?? null) && (p.curve?.h ?? null) === (q.curve?.h ?? null);
}

// src/world/doc.ts:3289-3301 slice of src/world/doc.ts#copyNode (shared): the feature's statements; "…" marks code left out
function copyNode(node: RoadNode): RoadNode {
  return {
    // … 6 lines
    ...(node.signal ? { signal: { ...node.signal, ...(node.signal.greens ? { greens: [...node.signal.greens] } : {}) } } : {}),
  // … 2 lines
  };
}

// src/world/doc.ts:3650-3661 slice of src/world/doc.ts#savedNode (shared): the feature's statements; "…" marks code left out
export function savedNode(n: RoadNode): SerializedDoc['nodes'][number] {
  return {
    // … 5 lines
    ...(n.signal ? { signal: { ...n.signal, ...(n.signal.greens ? { greens: [...n.signal.greens] } : {}) } } : {}),
  // … 2 lines
  };
}
