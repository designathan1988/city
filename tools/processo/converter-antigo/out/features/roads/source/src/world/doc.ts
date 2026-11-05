// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:64-70 slice of src/world/doc.ts#fitRoadCurve (shared): the feature's statements; "…" marks code left out
export function fitRoadCurve(
  // … 1 lines
  lanes: number | null = null, direction: SegmentDirection = 'both', section?: RoadSection,
  parking?: SegmentParking,
): CurveShape | null {
  return fitShapeToRadius(a, b, shape, casingHalf(roadProfile(type, lanes, direction, section, parking)));
}

// src/world/doc.ts:89-113 slice of src/world/doc.ts#RoadNode (shared): the feature's statements; "…" marks code left out
export interface RoadNode {
  // … 14 lines
  laneLinks?: readonly LaneLink[];
  // … 1 lines
  approachRules?: readonly ApproachRuleEntry[];
  // … 1 lines
  signal?: SignalSettings;
// … 4 lines
}

// src/world/doc.ts:124-144 slice of src/world/doc.ts#RoadSegment (shared): the feature's statements; "…" marks code left out
export interface RoadSegment {
  // … 8 lines
  direction: SegmentDirection;
  // … 3 lines
  section?: RoadSection;
  // … 1 lines
  parking?: SegmentParking;
// … 4 lines
}

// src/world/doc.ts:183-183 getter RoadDoc.streetNamesInUse: the feature's own
  get streetNamesInUse(): ReadonlyMap<string, number> { return this.streetNameCount; }

// src/world/doc.ts:204-204 slice of src/world/doc.ts#RoadDoc.markingStyle (shared): the feature's statements; "…" marks code left out
  markingStyle: MarkingStyleId = 'classic';

// src/world/doc.ts:213-213 slice of src/world/doc.ts#RoadDoc.gullyDabs (shared): the feature's statements; "…" marks code left out
  readonly gullyDabs: GullyDab[] = [];

// src/world/doc.ts:214-214 slice of src/world/doc.ts#RoadDoc.gullyAuto (shared): the feature's statements; "…" marks code left out
  gullyAuto = DEFAULT_GULLY_AUTO;

// src/world/doc.ts:220-220 slice of src/world/doc.ts#RoadDoc.economy (shared): the feature's statements; "…" marks code left out
  economy: Economy = DEFAULT_ECONOMY;

// src/world/doc.ts:273-276 getter RoadDoc.forkOrigin: the feature's own
  get forkOrigin(): RoadDoc | null {
    const fork = this.forkedFrom;
    return fork && fork.doc.changes.version === fork.version ? fork.doc : null;
  }

// src/world/doc.ts:279-286 method RoadDoc.roadIdsChangedFrom: the feature's own
  roadIdsChangedFrom(before: RoadDoc): Set<number> | null {
    const mine = this.forkedFrom, theirs = before.forkedFrom;
    let since: number | null = null;
    if (mine && mine.doc === before && mine.version === before.changes.version) since = mine.version;
    else if (theirs && theirs.doc === this && theirs.version === before.changes.version) since = theirs.version;
    else if (mine && theirs && mine.doc === theirs.doc && mine.version === theirs.version && theirs.version === before.changes.version) since = mine.version;
    return since === null ? null : this.changes.idsSince(since, ROAD_KINDS);
  }

// src/world/doc.ts:373-373 getter RoadDoc.trafficRevision: the feature's own
  get trafficRevision(): number { return this.changes.serialOf('traffic'); }

// src/world/doc.ts:396-402 slice of src/world/doc.ts#RoadDoc.segmentRect (shared): the feature's statements; "…" marks code left out
  segmentRect(id: SegmentId): ChangeRect | null {
    // … 4 lines
    return rectAround([a, b], casingHalf(roadProfile(s.type, s.lanes, s.direction, s.section, s.parking)) + Math.abs(s.curve?.h ?? 0));
  }

// src/world/doc.ts:405-414 slice of src/world/doc.ts#RoadDoc.nodeRect (shared): the feature's statements; "…" marks code left out
  nodeRect(id: NodeId): ChangeRect | null {
    // … 3 lines
    for (const sid of n.incident) {
      // … 1 lines
      if (s) pad = Math.max(pad, casingHalf(roadProfile(s.type, s.lanes, s.direction, s.section, s.parking)) * 2);
    }
    return rectAround([n], pad);
  }

// src/world/doc.ts:732-765 slice of src/world/doc.ts#RoadDoc.addSegment (shared): the feature's statements; "…" marks code left out
  addSegment(
    // … 5 lines
    direction: SegmentDirection = 'both',
    // … 2 lines
    section?: RoadSection,
    parking?: SegmentParking,
  ): RoadSegment | null {
    // … 2 lines
    const authoredSection = normalizeRoadSection(section);
    const authoredParking = normalizeParking(parking);
    // … 2 lines
    const s: RoadSegment = {
      id, a, b, curve, type, dashOrigin, direction,
      lanes: normaliseLaneCount(lanes, direction),
      // … 1 lines
      ...(authoredSection ? { section: authoredSection } : {}),
      ...(authoredParking ? { parking: authoredParking } : {}),
    };
  // … 8 lines
  }

// src/world/doc.ts:788-799 method RoadDoc.carryMovements: the feature's own
  carryMovements(node: NodeId, saved: readonly string[], from: SegmentId, to: SegmentId): void {
    const n = this.nodeToWrite(node);
    if (!n) return;
    for (const key of saved) {
      if (!movementMentions(key, from)) continue;
      const [a, b] = key.split('>') as [string, string];
      const moved = `${a === String(from) ? to : a}>${b === String(from) ? to : b}`;
      if (!n.blockedMovements.includes(moved)) n.blockedMovements.push(moved);
    }
    // The geometry is the same; what moves is the plan the traffic runs on.
    this.changes.record('traffic', this.rectsOrMap(this.nodeRect(node)), { ids: [node], detail: 'turn bans carried' });
  }

// src/world/doc.ts:802-808 method RoadDoc.carryCrossing: the feature's own
  carryCrossing(node: NodeId, saved: NodeCrossing | undefined, from: SegmentId, to: SegmentId): void {
    if (!this.nodes.has(node) || !saved || saved.segment !== from) return;
    const n = this.nodeToWrite(node)!;
    n.crossing = { kind: saved.kind, segment: to };
    dropStaleCrossing(n);
    this.markNode(node);
  }

// src/world/doc.ts:862-880 method RoadDoc.moveNode: the feature's own
  moveNode(id: NodeId, to: Vec2): boolean {
    const was = this.editableNode(id);
    if (!was) return false;
    // Dragging a node off the plate is the same offence as building one there, and is caught in the same place.
    const p = onWorld(to);
    if (was.x === p.x && was.y === p.y) return true;

    // DO NOT write only the rectangle after the move: whatever is derived where the node was
    const before = this.roadAreas(id);
    const n = this.nodeToWrite(id)!;
    n.x = p.x;
    n.y = p.y;
    for (const segId of n.incident) {
      const seg = this.segments.get(segId);
      if (seg) this.fitCurve(seg);
    }
    this.markNode(id, true, [...before, ...this.roadAreas(id)]);
    return true;
  }

// src/world/doc.ts:883-892 method RoadDoc.roadAreas: the feature's own
  private roadAreas(id: NodeId): ChangeRect[] {
    const out: ChangeRect[] = [];
    const rect = this.nodeRect(id);
    if (rect) out.push(rect);
    for (const sid of this.nodes.get(id)?.incident ?? []) {
      const s = this.segmentRect(sid);
      if (s) out.push(s);
    }
    return out;
  }

// src/world/doc.ts:928-937 slice of src/world/doc.ts#RoadDoc.setSegmentDirection (shared): the feature's statements; "…" marks code left out
  setSegmentDirection(id: SegmentId, direction: SegmentDirection): void {
    // … 1 lines
    if (!was || was.direction === direction) return;
    // … 2 lines
    s.direction = direction;
    s.lanes = normaliseLaneCount(s.lanes, direction);
  // … 2 lines
  }

// src/world/doc.ts:951-961 slice of src/world/doc.ts#RoadDoc.setSegmentSection (shared): the feature's statements; "…" marks code left out
  setSegmentSection(id: SegmentId, section?: RoadSection): void {
    // … 1 lines
    const next = normalizeRoadSection(section);
    if (!was || (section !== undefined && !next) || sameRoadSection(was.section, next)) return;
    // … 2 lines
    if (next) segment.section = next;
    else delete segment.section;
  // … 2 lines
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

// src/world/doc.ts:977-984 method RoadDoc.setSegmentCutWalls: the feature's own
  setSegmentCutWalls(id: SegmentId, on: boolean): void {
    const was = this.segments.get(id);
    if (!was || (was.cutWalls === true) === on) return;
    const segment = this.segmentToWrite(id)!;
    if (on) segment.cutWalls = true;
    else delete segment.cutWalls;
    this.markSegment(id);
  }

// src/world/doc.ts:987-996 slice of src/world/doc.ts#RoadDoc.setNodeLaneLinks (shared): the feature's statements; "…" marks code left out
  setNodeLaneLinks(id: NodeId, links: readonly LaneLink[] | undefined): void {
    // … 2 lines
    const next = normalizeLaneLinks(links);
    if (linksDigest(was.laneLinks) === linksDigest(next)) return;
    // … 1 lines
    if (next) node.laneLinks = next;
    else delete node.laneLinks;
  // … 1 lines
  }

// src/world/doc.ts:999-1004 slice of src/world/doc.ts#RoadDoc.setMarkingStyle (shared): the feature's statements; "…" marks code left out
  setMarkingStyle(style: MarkingStyleId): void {
    if (this.markingStyle === style) return;
    this.markingStyle = style;
  // … 2 lines
  }

// src/world/doc.ts:1007-1014 method RoadDoc.setNodeUturn: the feature's own
  setNodeUturn(id: NodeId, from: SegmentId | undefined): void {
    const was = this.editableNode(id);
    if (!was || was.uturn === from) return;
    const node = this.nodeToWrite(id)!;
    if (from !== undefined) node.uturn = from;
    else delete node.uturn;
    this.markNode(id);
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

// src/world/doc.ts:1050-1055 method RoadDoc.setSegmentStructure: the feature's own
  setSegmentStructure(id: SegmentId, structure: RoadStructure): void {
    const was = this.segments.get(id);
    if (!was || was.structure === structure) return;
    this.segmentToWrite(id)!.structure = structure;
    this.markSegment(id);
  }

// src/world/doc.ts:1087-1098 slice of src/world/doc.ts#RoadDoc.withRoom (shared): the feature's statements; "…" marks code left out
  private withRoom<T extends { readonly x: number; readonly y: number }>(part: CappedPart, items: readonly T[]): { readonly taken: T[]; readonly counts: Map<number, number> } {
    // … 1 lines
    const cap = TILE_CAPS[part];
    // … 1 lines
    for (const item of items) {
      // … 1 lines
      if (n >= cap) continue;
    // … 2 lines
    }
  // … 1 lines
  }

// src/world/doc.ts:1129-1137 slice of src/world/doc.ts#RoadDoc.addGullyDab (shared): the feature's statements; "…" marks code left out
  addGullyDab(dab: GullyDab): boolean {
    const { taken, counts } = this.withRoom('gullyDabs', [dab]);
    if (!taken.length) return false;
    // … 1 lines
    this.gullyDabs.push({ ...dab });
    this.keepCounts('gullyDabs', counts);
    this.changes.record('gullies', [circleRect(dab)]);
  // … 1 lines
  }

// src/world/doc.ts:1148-1152 slice of src/world/doc.ts#RoadDoc.setGullyAuto (shared): the feature's statements; "…" marks code left out
  setGullyAuto(amount: number): void {
    const next = Math.min(1, Math.max(0, Number.isFinite(amount) ? amount : DEFAULT_GULLY_AUTO));
    if (next === this.gullyAuto) return;
    this.gullyAuto = next;    this.changes.record('gullies', null);
  }

// src/world/doc.ts:1155-1159 method RoadDoc.setBalance: the feature's own
  setBalance(balance: number, cause?: string): void {
    if (!Number.isFinite(balance) || balance === this.economy.balance) return;
    this.economy = { balance };
    this.changes.record('economy', null, cause ? { cause } : {});
  }

// src/world/doc.ts:1242-1251 method RoadDoc.removeElementItems: the feature's own
  removeElementItems(items: ReadonlySet<ElementItem>): number {
    if (items.size === 0) return 0;
    this.ownList('elements');
    const before = this.elements.length;
    keepWhere(this.elements, (e) => !items.has(e));
    for (const e of items) this.elementGrid?.remove(e);
    const gone = before - this.elements.length;
    if (gone > 0) this.changes.record('elements', [elementsBox([...items])!]);
    return gone;
  }

// src/world/doc.ts:1355-1363 method RoadDoc.setNodeCrossing: the feature's own
  setNodeCrossing(id: NodeId, kind: NodeCrossingKind, segment: SegmentId): boolean {
    const was = this.editableNode(id);
    if (!was || was.incident.length !== 2 || !was.incident.includes(segment)) return false;
    const n = this.nodeToWrite(id)!;
    n.crossing = { kind, segment };
    n.control = kind === 'signal' ? 'signal' : 'priority';
    this.markNode(id);
    return true;
  }

// src/world/doc.ts:1374-1383 slice of src/world/doc.ts#RoadDoc.setMovementBlocked (shared): the feature's statements; "…" marks code left out
  setMovementBlocked(id: NodeId, from: SegmentId, to: SegmentId, blocked: boolean): void {
    // … 2 lines
    const key = movementKey(from, to);
    const index = was.blockedMovements.indexOf(key);
    if (blocked && index < 0) this.nodeToWrite(id)!.blockedMovements.push(key);
    else if (!blocked && index >= 0) this.nodeToWrite(id)!.blockedMovements.splice(index, 1);
    else return;
  // … 1 lines
  }

// src/world/doc.ts:1827-1831 method RoadDoc.fork: the feature's own
  fork(): RoadDoc {
    const copy = new RoadDoc();
    this.layOver(copy);
    return copy;
  }

// src/world/doc.ts:1834-1844 method RoadDoc.addedSegments: the feature's own
  addedSegments(before: RoadDoc): RoadSegment[] {
    const map = this.segments;
    const out: RoadSegment[] = [];
    if (map instanceof ForkMap && this.below === before) {
      for (const [id, seg] of (map as ForkMap<SegmentId, RoadSegment>).changes().added) if (!before.segments.has(id)) out.push(seg);
      return out;
    }
    // @rebuild An independent copy (`clone`) cannot say what it wrote: every segment compared.
    for (const [id, seg] of map) if (!before.segments.has(id)) out.push(seg);
    return out;
  }

// src/world/doc.ts:1847-1850 method RoadDoc.takeBack: the feature's own
  takeBack(copy: RoadDoc): void {
    if (copy.below !== this) throw new Error('RoadDoc.takeBack: not a working copy of this document');
    this.takeFork(copy);
  }

// src/world/doc.ts:1853-1899 slice of src/world/doc.ts#RoadDoc.layOver (shared): the feature's statements; "…" marks code left out
  private layOver(copy: RoadDoc): void {
    // … 24 lines
    copy.gullyAuto = this.gullyAuto;
    // … 1 lines
    copy.economy = this.economy;
  // … 18 lines
  }

// src/world/doc.ts:1902-1950 slice of src/world/doc.ts#RoadDoc.takeFork (shared): the feature's statements; "…" marks code left out
  private takeFork(copy: RoadDoc): void {
    // … 32 lines
    this.gullyAuto = copy.gullyAuto;
    // … 1 lines
    this.economy = copy.economy;
  // … 12 lines
  }

// src/world/doc.ts:2358-2372 slice of src/world/doc.ts#RoadDoc.valueOf (shared): the feature's statements; "…" marks code left out
  valueOf(part: ValuePart): unknown {
    switch (part) {
      // … 4 lines
      case 'gullyAuto': return this.gullyAuto;
      // … 1 lines
      case 'economy': return this.economy;
    // … 4 lines
    }
  }

// src/world/doc.ts:2374-2400 slice of src/world/doc.ts#RoadDoc.restoreValue (shared): the feature's statements; "…" marks code left out
  private restoreValue(part: ValuePart, raw: unknown, kinds: Set<DocChangeKind>): void {
    // … 1 lines
    switch (part) {
      // … 1 lines
      case 'markingStyle': this.markingStyle = value as MarkingStyleId; kinds.add('markingStyle'); return;
      // … 2 lines
      case 'gullyAuto': this.gullyAuto = value as number; kinds.add('gullies'); return;
      // … 1 lines
      case 'economy': this.economy = value as Economy; kinds.add('economy'); return;
    // … 15 lines
    }
  }

// src/world/doc.ts:2404-2552 slice of src/world/doc.ts#RoadDoc.replaceContents (shared): the feature's statements; "…" marks code left out
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
  // … 1 lines
  } {
    // … 65 lines
    if (this.economy.balance !== source.economy.balance) {
      this.economy = { ...source.economy };
      others.push('economy');
    }
    // … 33 lines
    if (moved('gullies') && (whole || this.gullyAuto !== source.gullyAuto || !sameText(this.gullyDabs, source.gullyDabs))) {
      this.ownList('gullyDabs');
      this.gullyDabs.length = 0;
      pushAll(this.gullyDabs, source.gullyDabs.map((dab) => ({ ...dab })));
      this.gullyAuto = source.gullyAuto;
      others.push('gullies');
    }
  // … 36 lines
  }

// src/world/doc.ts:2599-2616 slice of src/world/doc.ts#RoadDoc.savedGlobals (shared): the feature's statements; "…" marks code left out
  savedGlobals(part: GlobalPart): Partial<SavedGlobals> {
    switch (part) {
      case 'values': return {
        // … 7 lines
        gullyAuto: this.gullyAuto,
        ...(this.economy.balance !== DEFAULT_ECONOMY.balance ? { economy: { balance: this.economy.balance } } : {}),
      };
    // … 3 lines
    }
  }

// src/world/doc.ts:2673-2698 slice of src/world/doc.ts#RoadDoc.worldPart (shared): the feature's statements; "…" marks code left out
  worldPart(part: WorldPart): Partial<SerializedDoc> {
    switch (part) {
      // … 6 lines
      case 'gullies': return this.gullyDabs.length > 0 || this.gullyAuto !== DEFAULT_GULLY_AUTO
        ? { gullies: { dabs: this.gullyDabs.map((dab) => ({ ...dab })), auto: this.gullyAuto } } : {};
      // … 12 lines
      case 'economy': return this.economy.balance !== DEFAULT_ECONOMY.balance ? { economy: { balance: this.economy.balance } } : {};
    // … 1 lines
    }
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 81 lines
    if (data.gullies) {
      for (const raw of data.gullies.dabs ?? []) {
        const dab = readGullyDab(raw);
        if (dab) doc.gullyDabs.push(dab);
      }
      // … 1 lines
      if (typeof auto === 'number' && Number.isFinite(auto)) doc.gullyAuto = Math.min(1, Math.max(0, auto));
    // … 1 lines
    }
    // … 11 lines
    doc.markingStyle = isMarkingStyle(data.markingStyle) ? data.markingStyle : 'classic';
    // … 27 lines
    if (economy) { doc.economy = economy; doc.changes.record('economy', null, { cause: 'map loaded' }); }
  // … 55 lines
  }

// src/world/doc.ts:2951-2954 slice of src/world/doc.ts#TILE_CAPS (shared): the feature's statements; "…" marks code left out
const TILE_CAPS: Readonly<Record<CappedPart, number>> = {
  terrainStamps: MAX_TERRAIN_STAMPS, terrainPaint: MAX_PAINT_DABS, fogDabs: MAX_FOG_DABS, gullyDabs: MAX_GULLY_DABS,
  trees: MAX_PLANTED_TREES, treeClearings: MAX_TREE_CLEARINGS, elements: MAX_ELEMENTS,
};

// src/world/doc.ts:3029-3047 slice of src/world/doc.ts#segmentRecord (shared): the feature's statements; "…" marks code left out
function segmentRecord(s: SerializedDoc['segments'][number], a: NodeId, b: NodeId): RoadSegment {
  const section = normalizeRoadSection(s.section);
  const parking = normalizeParking(s.parking);
  return {
    // … 8 lines
    ...(section ? { section } : {}),
    ...(parking ? { parking } : {}),
  // … 3 lines
  };
}

// src/world/doc.ts:3050-3064 slice of src/world/doc.ts#finishNode (shared): the feature's statements; "…" marks code left out
function finishNode(node: RoadNode, n: SerializedDoc['nodes'][number]): void {
  // … 5 lines
  const links = normalizeLaneLinks(n.laneLinks);
  if (links) node.laneLinks = links;
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

// src/world/doc.ts:3263-3268 slice of src/world/doc.ts#normaliseLaneCount (shared): the feature's statements; "…" marks code left out
function normaliseLaneCount(lanes: number | null, direction: SegmentDirection): number | null {
  // … 3 lines
  return direction === 'both' ? Math.max(2, Math.ceil(value / 2) * 2) : value;
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

// src/world/doc.ts:3304-3311 slice of src/world/doc.ts#copySegment (shared): the feature's statements; "…" marks code left out
function copySegment(segment: RoadSegment): RoadSegment {
  return {
    // … 2 lines
    ...(segment.section ? { section: cloneRoadSection(segment.section) } : {}),
  // … 1 lines
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

// src/world/doc.ts:3672-3687 slice of src/world/doc.ts#savedSegment (shared): the feature's statements; "…" marks code left out
export function savedSegment(s: RoadSegment): SerializedDoc['segments'][number] {
  return {
    // … 9 lines
    ...(s.section ? { section: cloneRoadSection(s.section) } : {}),
  // … 2 lines
  };
}
