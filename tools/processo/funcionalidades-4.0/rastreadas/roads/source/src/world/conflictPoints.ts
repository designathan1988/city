// The code of src/world/conflictPoints.ts that is the feature's (the rest of the file is shared).

// src/world/conflictPoints.ts:27-40 slice of src/world/conflictPoints.ts#ConflictPoint (shared): the feature's statements; "…" marks code left out
export interface ConflictPoint {
  // … 2 lines
  readonly a: ConnectorId;
  readonly b: ConnectorId;
  // … 7 lines
  zone(on: ConnectorId, mine: BodyClass, theirs: BodyClass): Zone | null;
}

// src/world/conflictPoints.ts:46-46 slice of src/world/conflictPoints.ts#ConflictIndex.byConnector (shared): the feature's statements; "…" marks code left out
  readonly byConnector = new Map<ConnectorId, ConflictRef[]>();

// src/world/conflictPoints.ts:48-48 slice of src/world/conflictPoints.ts#ConflictIndex.diverges (shared): the feature's statements; "…" marks code left out
  readonly diverges = new Map<ConnectorId, Diverge[]>();

// src/world/conflictPoints.ts:50-50 slice of src/world/conflictPoints.ts#ConflictIndex.queueIntrusions (shared): the feature's statements; "…" marks code left out
  readonly queueIntrusions: { a: ConnectorId; b: ConnectorId; mine: BodyClass; theirs: BodyClass }[] = [];

// src/world/conflictPoints.ts:85-92 slice of src/world/conflictPoints.ts#ConflictIndex.setWindow (shared): the feature's statements; "…" marks code left out
  setWindow(graph: LaneletGraph, next: ReadonlySet<NodeId>): ReadonlySet<NodeId> {
    // … 2 lines
    for (const node of next) if (!this.ofJunction.has(node) && graph.junctions.has(node)) changed.add(node);
    // … 1 lines
    if (changed.size) this.update(graph, changed);
  // … 1 lines
  }

// src/world/conflictPoints.ts:100-104 slice of src/world/conflictPoints.ts#ConflictIndex.entering (shared): the feature's statements; "…" marks code left out
  entering(graph: LaneletGraph, next: ReadonlySet<NodeId>): Set<NodeId> {
    // … 1 lines
    for (const node of next) if (!this.ofJunction.has(node) && graph.junctions.has(node)) out.add(node);
  // … 1 lines
  }

// src/world/conflictPoints.ts:141-201 slice of src/world/conflictPoints.ts#ConflictIndex.prepare (shared): the feature's statements; "…" marks code left out
  *prepare(graph: LaneletGraph, measurer?: PairMeasurer, only?: ReadonlySet<NodeId> | null, around?: ReadonlySet<NodeId>): Generator<void | typeof WAITING, void> {
    // … 7 lines
    const junctions = (only ? [...only].map((node) => graph.junctions.get(node)).filter((j): j is JunctionTopology => j !== undefined) : [...graph.junctions.values()])
      .filter((j) => (around ? around.has(j.node) : this.inWindow(j.node)));
    // … 1 lines
    if (measurer?.sampleSweeps) yield* this.sampleOffPage(graph, junctions, measurer);
    // … 1 lines
    for (const junction of junctions) {
      rebuildWork.conflicts++;
      const ids = junction.connectors.slice().sort();
      const sweeps: Movement[] = [], pairKeys: string[] = [];
      for (const id of ids) {
        const c = graph.connectors.get(id);
        if (!c) continue;
        const keys: string[] = [];
        const sweep = sweepOf(graph, c, this.sweepCache, this.sweepCache, keys);
        if (sweep) sweeps.push({ connector: c, inbound: graph.lanelet(c.fromLane)!, outbound: graph.lanelet(c.toLane)!, sweep, shape: shapeOf(sweep), sweepKey: keys[0]! });
      }
      this.prepared.set(junction.node, { junction, ids, movements: sweeps, pairKeys });
      for (let i = 0; i < sweeps.length; i++) {
        for (let j = i + 1; j < sweeps.length; j++) {
          const a = sweeps[i]!, b = sweeps[j]!;
          const key = `${a.shape.hash}|${b.shape.hash}`;
          pairKeys.push(key);
          if (measurer?.finishPairs && !read.has(key)) read.set(key, { a: a.shape.centre, b: b.shape.centre });
          const known = this.pairCache.get(key);
          if (known && sameShape(known.a, a.shape) && sameShape(known.b, b.shape)) continue;
          const wasKept = this.store.kept.has(key);
          if (!wasKept && measurer) {
            if (!asked.has(key)) { asked.add(key); wanted.push({ key, a: a.sweep, shapeA: a.shape, b: b.sweep, shapeB: b.shape }); }
            continue;
          }
          this.pairCache.set(key, measurePair(key, a.sweep, a.shape, b.sweep, b.shape, this.store));
          if (wasKept) continue;
          this.measured.prepare++;
          yield;
        }
      }
    }
  // … 15 lines
  }

// src/world/conflictPoints.ts:242-279 slice of src/world/conflictPoints.ts#ConflictIndex.sampleOffPage (shared): the feature's statements; "…" marks code left out
  private *sampleOffPage(graph: LaneletGraph, junctions: readonly JunctionTopology[], measurer: PairMeasurer): Generator<void | typeof WAITING, void> {
    // … 2 lines
    const crossings: Lanelet[] = [];
    // … 1 lines
    for (const junction of junctions) {
      for (const id of junction.connectors) {
        const c = graph.connectors.get(id);
        if (!c) continue;
        const inbound = graph.lanelet(c.fromLane), crossing = graph.lanelet(c.lanelet), outbound = graph.lanelet(c.toLane);
        if (!inbound || !crossing || !outbound) continue;
        const { c0, count } = sweepSpan(crossing.length);
        // Looked up when its three lines are digested; asked otherwise, the worker digesting them.
        const d0 = LINE_DIGESTS.get(inbound.centre.xy), d1 = LINE_DIGESTS.get(crossing.centre.xy), d2 = LINE_DIGESTS.get(outbound.centre.xy);
        if (d0 !== undefined && d1 !== undefined && d2 !== undefined) {
          const key = sweepKey(d0, d1, d2, count);
          if (this.sweepCache.has(key) || asked.has(key)) continue;
          asked.add(key);
        }
        asks.push({ inXY: inbound.centre.xy, crossXY: crossing.centre.xy, outXY: outbound.centre.xy, c0, count });
        crossings.push(crossing);
      }
    }
    // … 1 lines
    for (let k = 0; k < (sweeps?.length ?? 0); k++) {
      // … 10 lines
      SHAPES.set(sampled.frame, shapeFrom(sampled.shape, ask.c0, ask.count, crossings[k]!, sampled.frame));
    }
  }

// src/world/conflictPoints.ts:286-297 slice of src/world/conflictPoints.ts#ConflictIndex.takePrepared (shared): the feature's statements; "…" marks code left out
  private takePrepared(graph: LaneletGraph, junction: JunctionTopology, previousSweeps: Map<string, Float64Array>, nextSweeps: Map<string, Float64Array>): PreparedJunction | null {
    const ready = this.prepared.get(junction.node);
    if (!ready) return null;
    this.prepared.delete(junction.node);
    if (ready.junction !== junction) return null;
    for (const m of ready.movements) {
      const c = m.connector;
      if (graph.connectors.get(c.id) !== c || graph.lanelet(c.fromLane) !== m.inbound || graph.lanelet(c.lanelet) !== m.sweep.crossing
        || graph.lanelet(c.toLane) !== m.outbound || (previousSweeps.get(m.sweepKey) ?? nextSweeps.get(m.sweepKey)) !== m.sweep.frame) return null;
    }
    return ready;
  }

// src/world/conflictPoints.ts:299-299 slice of src/world/conflictPoints.ts#ConflictIndex.ofJunction (shared): the feature's statements; "…" marks code left out
  private readonly ofJunction = new Map<NodeId, { readonly points: number[]; readonly connectors: ConnectorId[]; readonly pairs: string[]; readonly sweeps: string[] }>();

// src/world/conflictPoints.ts:304-326 slice of src/world/conflictPoints.ts#ConflictIndex.build (shared): the feature's statements; "…" marks code left out
  build(graph: LaneletGraph): void {
    // … 13 lines
    for (const junction of graph.junctions.values()) if (this.inWindow(junction.node)) this.buildJunction(graph, junction, previous, next, previousSweeps, nextSweeps);
  // … 7 lines
  }

// src/world/conflictPoints.ts:329-362 slice of src/world/conflictPoints.ts#ConflictIndex.update (shared): the feature's statements; "…" marks code left out
  update(graph: LaneletGraph, junctions: ReadonlySet<NodeId>): void {
    // … 3 lines
    for (const node of junctions) {
      // … 3 lines
      const gone = new Set<ConnectorId>(made.connectors);
      // … 2 lines
      for (let i = this.queueIntrusions.length - 1; i >= 0; i--) if (gone.has(this.queueIntrusions[i]!.a)) this.queueIntrusions.splice(i, 1);
    // … 2 lines
    }
    for (const node of [...junctions].sort((x, y) => x - y)) {
      const junction = graph.junctions.get(node);
      // … 1 lines
      if (!junction || !this.inWindow(node)) continue;
      this.buildJunction(graph, junction, this.pairCache, this.pairCache, this.sweepCache, this.sweepCache);
    // … 1 lines
    }
  // … 11 lines
  }

// src/world/conflictPoints.ts:365-428 slice of src/world/conflictPoints.ts#ConflictIndex.buildJunction (shared): the feature's statements; "…" marks code left out
  private buildJunction(graph: LaneletGraph, junction: JunctionTopology,
    // … 1 lines
    previousSweeps: Map<string, Float64Array>, nextSweeps: Map<string, Float64Array>): void {
    // … 1 lines
    const made = { points: [] as number[], connectors: [] as ConnectorId[], pairs: [] as string[], sweeps: [] as string[] };
    this.ofJunction.set(junction.node, made);
    const ready = this.takePrepared(graph, junction, previousSweeps, nextSweeps);
    const ids = ready?.ids ?? junction.connectors.slice().sort();
    made.connectors.push(...ids);
    // … 1 lines
    if (ready) {
      for (const m of ready.movements) { nextSweeps.set(m.sweepKey, m.sweep.frame); made.sweeps.push(m.sweepKey); movements.push(m); }
    } else {
      for (const id of ids) {
        const c = graph.connectors.get(id);
        if (!c) continue;
        const sweep = sweepOf(graph, c, previousSweeps, nextSweeps, made.sweeps);
        if (sweep) movements.push({ connector: c, inbound: undefined, outbound: undefined, sweep, shape: shapeOf(sweep), sweepKey: '' });
      }
    }
    // … 3 lines
    for (let i = 0; i < movements.length; i++) {
      for (let j = i + 1; j < movements.length; j++) {
        // … 1 lines
        const pairKey = ready ? ready.pairKeys[k++]! : `${shapeA.hash}|${shapeB.hash}`;
        const pair = cachedPair(previous, next, pairKey, sa, shapeA, sb, shapeB, this.store);
        made.pairs.push(pairKey);
        this.pairUses.set(pairKey, (this.pairUses.get(pairKey) ?? 0) + 1);
        const zones = pair.zones;
        if (!zones) continue;
        // … 1 lines
        if (a.fromLane === b.fromLane) {
          pushDiverge(this.diverges, a.id, b.id, zones, false);
          pushDiverge(this.diverges, b.id, a.id, zones, true);
          continue;
        }
        // … 1 lines
        const facts = pair.facts ??= pairFacts(zones);
        this.recordIntrusions(a.id, b.id, facts);
        // … 3 lines
        if (a.toLane === b.toLane) {
          kind = 'merge';
          at = sa.path.sampleAt(sa.crossing.length).p;
        } else {
          // Both depend only on the two sweeps, so they are kept with the pair.
          if (pair.crossing === undefined) {
            pair.crossing = firstCrossing(sa.crossing.centre.toPoints(), sb.crossing.centre.toPoints());
          }
          if (pair.crossing) {
            kind = 'cross';
            at = pair.crossing.at;
          } else {
            pair.centre ??= zoneCentre(sa, zones);
            at = pair.centre;
          }
        }
        made.points.push(this.add(junction.node, a.id, b.id, kind, at, zones, facts));
      }
    }
  }

// src/world/conflictPoints.ts:430-432 slice of src/world/conflictPoints.ts#ConflictIndex.recordIntrusions (shared): the feature's statements; "…" marks code left out
  private recordIntrusions(a: ConnectorId, b: ConnectorId, facts: PairFacts): void {
    for (const x of facts.intrusions) this.queueIntrusions.push(x.onB ? { a: b, b: a, mine: x.mine, theirs: x.theirs } : { a, b, mine: x.mine, theirs: x.theirs });
  }

// src/world/conflictPoints.ts:434-473 slice of src/world/conflictPoints.ts#ConflictIndex.add (shared): the feature's statements; "…" marks code left out
  private add(
    // … 1 lines
    a: ConnectorId,
    b: ConnectorId,
  // … 4 lines
  ): number {
    const [lo, hi] = a < b ? [a, b] : [b, a];
    // … 1 lines
    const key = `${node}|${lo}|${hi}`;
    let id = this.idByKey.get(key);
    if (id === undefined) {
      id = this.nextId++;
      this.idByKey.set(key, id);
      let keys = this.keysOf.get(node);
      if (!keys) this.keysOf.set(node, keys = new Set());
      keys.add(key);
    }
    // … 2 lines
    const point: ConflictPoint = {
      id,
      // … 1 lines
      a,
      b,
      // … 4 lines
      zone: (on, mine, theirs) =>
        on === a ? zones.get(mine, theirs, false) : on === b ? zones.get(mine, theirs, true) : null,
    };
    // … 1 lines
    this.points[id] = point;
    pushRef(this.byConnector, a, { point: id, s: sA, other: b, kind, exit: facts.exitA });
    pushRef(this.byConnector, b, { point: id, s: sB, other: a, kind, exit: facts.exitB });
    return id;
  }

// src/world/conflictPoints.ts:475-477 slice of src/world/conflictPoints.ts#ConflictIndex.refs (shared): the feature's statements; "…" marks code left out
  refs(connector: ConnectorId): readonly ConflictRef[] {
    return this.byConnector.get(connector) ?? [];
  }

// src/world/conflictPoints.ts:479-481 slice of src/world/conflictPoints.ts#ConflictIndex.divergesOf (shared): the feature's statements; "…" marks code left out
  divergesOf(connector: ConnectorId): readonly Diverge[] {
    return this.diverges.get(connector) ?? [];
  }

// src/world/conflictPoints.ts:501-505 slice of src/world/conflictPoints.ts#Sweep (shared): the feature's statements; "…" marks code left out
interface Sweep extends SweepSamples {
  readonly crossing: Lanelet;
// … 2 lines
}

// src/world/conflictPoints.ts:507-529 slice of src/world/conflictPoints.ts#sweepOf (shared): the feature's statements; "…" marks code left out
function sweepOf(graph: LaneletGraph, c: Connector,
  previous: Map<string, Float64Array>, next: Map<string, Float64Array>, used?: string[]): Sweep | null {
  const inbound = graph.lanelet(c.fromLane);
  const crossing = graph.lanelet(c.lanelet);
  const outbound = graph.lanelet(c.toLane);
  if (!inbound || !crossing || !outbound) return null;
  // … 1 lines
  const path = joinedPath(inbound.centre, crossing.centre, outbound.centre);
  const { c0, count } = sweepSpan(crossing.length);
  // … 2 lines
  const key = sweepKey(lineDigest(inbound.centre.xy), lineDigest(crossing.centre.xy), lineDigest(outbound.centre.xy), count);
  // … 1 lines
  const frame = previous.get(key) ?? next.get(key) ?? sweepFrame(path, c0, count);
  next.set(key, frame);
  used?.push(key);
  let frameLengthSq = FRAME_LENGTHS.get(frame);
  if (!frameLengthSq) {
    frameLengthSq = frameLengthsOf(frame);
    FRAME_LENGTHS.set(frame, frameLengthSq);
  }
  return { crossing, path, c0, count, length: crossing.length, frame, frameLengthSq, bounds: null, grid: null, range: null };
}

// src/world/conflictPoints.ts:565-573 slice of src/world/conflictPoints.ts#Movement (shared): the feature's statements; "…" marks code left out
interface Movement {
  readonly connector: Connector;
  // … 1 lines
  readonly inbound: Lanelet | undefined;
  readonly outbound: Lanelet | undefined;
// … 3 lines
}

// src/world/conflictPoints.ts:576-581 slice of src/world/conflictPoints.ts#PreparedJunction (shared): the feature's statements; "…" marks code left out
interface PreparedJunction {
  readonly junction: JunctionTopology;
  readonly ids: readonly ConnectorId[];
// … 2 lines
}

// src/world/conflictPoints.ts:596-604 slice of src/world/conflictPoints.ts#shapeOf (shared): the feature's statements; "…" marks code left out
function shapeOf(s: Sweep): SweepShape {
  // … 1 lines
  if (kept && kept.centre === s.crossing.centre.xy && kept.count === s.count && kept.c0 === s.c0 && kept.length === s.crossing.length) {
    return kept;
  }
// … 3 lines
}

// src/world/conflictPoints.ts:606-609 slice of src/world/conflictPoints.ts#measureShape (shared): the feature's statements; "…" marks code left out
function measureShape(s: Sweep): SweepShape {
  // … 1 lines
  return shapeFrom(shapeDigest(s.frame, s.crossing.centre.xy), s.c0, s.count, s.crossing, s.frame);
}

// src/world/conflictPoints.ts:612-621 slice of src/world/conflictPoints.ts#shapeFrom (shared): the feature's statements; "…" marks code left out
function shapeFrom(digest: number, c0: number, count: number, crossing: Lanelet, frame: Float64Array): SweepShape {
  return {
    hash: `${digest.toString(36)}:${count}:${c0}:${crossing.length}`,
    // … 2 lines
    length: crossing.length,
    // … 1 lines
    centre: crossing.centre.xy,
  };
}
