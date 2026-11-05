// The code of src/sim/routing/laneReach.ts that is the feature's (the rest of the file is shared).

// src/sim/routing/laneReach.ts:7-13 slice of src/sim/routing/laneReach.ts#ReachWorld (shared): the feature's statements; "…" marks code left out
export interface ReachWorld {
  // … 1 lines
  readonly graph: LaneletGraph;
  readonly topologyLog: ChangeLog<GraphChange>;
  lanelet(id: LaneletId): Lanelet | undefined;
  connector(id: string): Connector | undefined;
}

// src/sim/routing/laneReach.ts:17-17 slice of src/sim/routing/laneReach.ts#BodyReach.comp (shared): the feature's statements; "…" marks code left out
  private readonly comp = new Map<LaneletId, number>();

// src/sim/routing/laneReach.ts:18-18 slice of src/sim/routing/laneReach.ts#BodyReach.members (shared): the feature's statements; "…" marks code left out
  private readonly members = new Map<number, Set<LaneletId>>();

// src/sim/routing/laneReach.ts:20-20 slice of src/sim/routing/laneReach.ts#BodyReach.out (shared): the feature's statements; "…" marks code left out
  private readonly out = new Map<LaneletId, LaneletId[]>();

// src/sim/routing/laneReach.ts:21-21 slice of src/sim/routing/laneReach.ts#BodyReach.into (shared): the feature's statements; "…" marks code left out
  private readonly into = new Map<LaneletId, Map<LaneletId, number>>();

// src/sim/routing/laneReach.ts:26-26 slice of src/sim/routing/laneReach.ts#BodyReach.exitsMemo (shared): the feature's statements; "…" marks code left out
  private readonly exitsMemo = new Map<number, readonly LaneletId[]>();

// src/sim/routing/laneReach.ts:34-37 slice of src/sim/routing/laneReach.ts#BodyReach.exitsFrom (shared): the feature's statements; "…" marks code left out
  exitsFrom(w: ReachWorld, start: LaneletId): readonly LaneletId[] {
    const c = this.comp.get(start);
    return c === undefined ? [] : this.exitsOf(w, c);
  }

// src/sim/routing/laneReach.ts:43-50 slice of src/sim/routing/laneReach.ts#BodyReach.reachesFrom (shared): the feature's statements; "…" marks code left out
  reachesFrom(start: LaneletId, goal: LaneletId): boolean | null {
    const cu = this.comp.get(start), cv = this.comp.get(goal);
    if (cu === undefined || cv === undefined) return null;
    if (cu === cv) return true;
    let ahead = this.ahead.get(cu);
    if (!ahead) this.ahead.set(cu, ahead = this.walkCond(cu, this.condOut, null));
    return ahead.has(cv);
  }

// src/sim/routing/laneReach.ts:53-63 slice of src/sim/routing/laneReach.ts#BodyReach.buildAll (shared): the feature's statements; "…" marks code left out
  buildAll(w: ReachWorld): void {
    // … 3 lines
    const links: LaneletId[] = [];
    for (const lane of w.graph.lanelets.values()) if (lane.kind === 'link') links.push(lane.id);
    for (const id of links) { this.work++; this.out.set(id, this.next(w, id)); }
    for (const id of links) for (const v of this.out.get(id)!) this.countIn(v, id, 1);
    for (const list of this.tarjan(links, () => true)) this.label(list);
    for (const id of links) for (const v of this.out.get(id)!) this.cond(this.comp.get(id)!, this.comp.get(v)!, 1);
  }

// src/sim/routing/laneReach.ts:66-158 slice of src/sim/routing/laneReach.ts#BodyReach.follow (shared): the feature's statements; "…" marks code left out
  follow(w: ReachWorld, changes: readonly GraphChange[]): void {
    // … 3 lines
    const lanes = new Set<LaneletId>();
    for (const change of changes) {
      for (const id of change.lanelets) lanes.add(id);
      for (const node of change.junctions) for (const id of w.graph.inbound.get(node) ?? []) lanes.add(id);
    }
    for (const id of [...lanes]) if (!isLink(id)) for (const u of this.into.get(id)?.keys() ?? []) lanes.add(u);
    const pairs: [LaneletId, LaneletId][] = [];
    const crossing: [LaneletId, LaneletId][] = [];
    // … 2 lines
    for (const x of lanes) {
      const k = this.comp.get(x);
      if (k === undefined || isLink(x)) continue;
      this.work++;
      touched.add(k);
      const preds = [...this.into.get(x)?.keys() ?? []].filter((u) => u !== x && this.comp.get(u) === k);
      const succs = new Set(this.out.get(x)!.filter((v) => v !== x && this.comp.get(v) === k));
      for (const p of preds) for (const s of succs) if (p !== s) pairs.push([p, s]);
      for (const v of this.out.get(x)!) this.edge(x, v, -1);
      for (const [u, n] of [...this.into.get(x) ?? []]) {
        for (let i = 0; i < n; i++) this.edge(u, x, -1);
        const list = this.out.get(u);
        if (list) this.out.set(u, list.filter((v) => v !== x));
      }
      this.out.delete(x);
      this.into.delete(x);
      this.members.get(k)!.delete(x);
      if (!this.members.get(k)!.size) this.members.delete(k);
      this.comp.delete(x);
    }
    // … 1 lines
    for (const x of lanes) {
      if (!isLink(x) || this.comp.has(x)) continue;
      const k = this.nextComp++;
      this.comp.set(x, k);
      this.members.set(k, new Set([x]));
      this.out.set(x, []);
      touched.add(k);
    }
    // … 1 lines
    for (const x of lanes) {
      if (!isLink(x)) continue;
      this.work++;
      const was = this.out.get(x)!;
      const now = this.next(w, x).filter((v) => this.comp.has(v));
      const left = new Map<LaneletId, number>();
      for (const v of now) left.set(v, (left.get(v) ?? 0) + 1);
      for (const v of was) {
        const n = left.get(v) ?? 0;
        if (n > 0) { left.set(v, n - 1); continue; }
        if (this.comp.get(v) === this.comp.get(x)) pairs.push([x, v]);
        this.edge(x, v, -1);
      }
      for (const [v, n] of left) for (let i = 0; i < n; i++) {
        this.edge(x, v, 1);
        if (this.comp.get(v) !== this.comp.get(x)) crossing.push([x, v]);
      }
      this.out.set(x, now);
      touched.add(this.comp.get(x)!);
    }
    // … 2 lines
    for (const [u, v] of pairs) {
      const k = this.comp.get(u);
      if (k === undefined || this.comp.get(v) !== k || split.has(k)) continue;
      if (!this.reaches(u, v)) split.add(k);
    }
    // … 8 lines
    for (const [u, v] of crossing) {
      const cu = this.comp.get(u), cv = this.comp.get(v);
      if (cu === undefined || cv === undefined || cu === cv) continue;
      const ahead = this.walkCond(cv, this.condOut, null);
      if (!ahead.has(cu)) continue;
      const cycle = this.walkCond(cu, this.condIn, ahead);
      const lanesOf = [...cycle].flatMap((c) => [...this.members.get(c) ?? []]);
      this.retag(lanesOf, () => { for (const c of cycle) this.members.delete(c); touched.add(this.label(lanesOf)); });
    }
  // … 4 lines
  }

// src/sim/routing/laneReach.ts:68-68 slice of src/sim/routing/laneReach.ts#BodyReach.follow.isLink (shared): the feature's statements; "…" marks code left out
    const isLink = (id: LaneletId): boolean => w.graph.lanelets.get(id)?.kind === 'link';

// src/sim/routing/laneReach.ts:161-169 slice of src/sim/routing/laneReach.ts#BodyReach.next (shared): the feature's statements; "…" marks code left out
  private next(w: ReachWorld, lane: LaneletId): LaneletId[] {
    const out: LaneletId[] = [];
    for (const id of w.graph.exitsOf(lane)) {
      const connector = w.connector(id);
      if (connector && connector.maxBodyClass >= this.body && connector.toLane && w.graph.lanelets.get(connector.toLane)?.kind === 'link') out.push(connector.toLane);
    }
    for (const sibling of w.graph.siblingLanes(lane)) if (w.graph.lanelets.get(sibling)?.kind === 'link') out.push(sibling);
    return out;
  }

// src/sim/routing/laneReach.ts:172-194 slice of src/sim/routing/laneReach.ts#BodyReach.reaches (shared): the feature's statements; "…" marks code left out
  private reaches(u: LaneletId, v: LaneletId): boolean {
    if (u === v) return true;
    const fwd = new Set([u]), back = new Set([v]);
    let fq = [u], bq = [v];
    while (fq.length && bq.length) {
      if (fq.length <= bq.length) {
        const next: LaneletId[] = [];
        for (const x of fq) for (const y of this.out.get(x) ?? []) {
          if (back.has(y)) return true;
          if (!fwd.has(y)) { fwd.add(y); next.push(y); this.work++; }
        }
        fq = next;
      } else {
        const next: LaneletId[] = [];
        for (const x of bq) for (const y of this.into.get(x)?.keys() ?? []) {
          if (fwd.has(y)) return true;
          if (!back.has(y)) { back.add(y); next.push(y); this.work++; }
        }
        bq = next;
      }
    }
  // … 1 lines
  }

// src/sim/routing/laneReach.ts:197-235 slice of src/sim/routing/laneReach.ts#BodyReach.tarjan (shared): the feature's statements; "…" marks code left out
  private tarjan(lanes: readonly LaneletId[], inSet: (id: LaneletId) => boolean): LaneletId[][] {
    const order = new Map<LaneletId, number>(), low = new Map<LaneletId, number>();
    const stack: LaneletId[] = [], onStack = new Set<LaneletId>();
    const out: LaneletId[][] = [];
    // … 1 lines
    for (const root of lanes) {
      if (order.has(root)) continue;
      const frames: { lane: LaneletId; next: LaneletId[]; at: number }[] = [];
      const open = (lane: LaneletId): void => {
        this.work++;
        order.set(lane, counter); low.set(lane, counter); counter++;
        stack.push(lane); onStack.add(lane);
        frames.push({ lane, next: (this.out.get(lane) ?? []).filter(inSet), at: 0 });
      };
      open(root);
      while (frames.length) {
        const f = frames[frames.length - 1]!;
        if (f.at < f.next.length) {
          const to = f.next[f.at++]!;
          if (!order.has(to)) open(to);
          else if (onStack.has(to)) low.set(f.lane, Math.min(low.get(f.lane)!, order.get(to)!));
          continue;
        }
        frames.pop();
        const parent = frames[frames.length - 1];
        if (parent) low.set(parent.lane, Math.min(low.get(parent.lane)!, low.get(f.lane)!));
        if (low.get(f.lane) !== order.get(f.lane)) continue;
        const members: LaneletId[] = [];
        while (stack.length) {
          const lane = stack.pop()!;
          onStack.delete(lane);
          members.push(lane);
          if (lane === f.lane) break;
        }
        out.push(members);
      }
    }
    return out;
  }

// src/sim/routing/laneReach.ts:238-243 slice of src/sim/routing/laneReach.ts#BodyReach.label (shared): the feature's statements; "…" marks code left out
  private label(lanes: readonly LaneletId[]): number {
    // … 1 lines
    this.members.set(k, new Set(lanes));
    for (const id of lanes) this.comp.set(id, k);
  // … 1 lines
  }

// src/sim/routing/laneReach.ts:246-256 slice of src/sim/routing/laneReach.ts#BodyReach.retag (shared): the feature's statements; "…" marks code left out
  private retag(lanes: readonly LaneletId[], relabel: () => void): void {
    const set = new Set(lanes);
    const ways: [LaneletId, LaneletId][] = [];
    for (const x of lanes) {
      for (const v of this.out.get(x) ?? []) ways.push([x, v]);
      for (const [u, n] of this.into.get(x) ?? []) if (!set.has(u)) for (let i = 0; i < n; i++) ways.push([u, x]);
    }
    for (const [a, b] of ways) this.cond(this.comp.get(a)!, this.comp.get(b)!, -1);
    // … 1 lines
    for (const [a, b] of ways) this.cond(this.comp.get(a)!, this.comp.get(b)!, 1);
  }

// src/sim/routing/laneReach.ts:259-263 slice of src/sim/routing/laneReach.ts#BodyReach.edge (shared): the feature's statements; "…" marks code left out
  private edge(u: LaneletId, v: LaneletId, n: 1 | -1): void {
    this.countIn(v, u, n);
    const cu = this.comp.get(u), cv = this.comp.get(v);
    if (cu !== undefined && cv !== undefined) this.cond(cu, cv, n);
  }

// src/sim/routing/laneReach.ts:265-270 slice of src/sim/routing/laneReach.ts#BodyReach.countIn (shared): the feature's statements; "…" marks code left out
  private countIn(v: LaneletId, u: LaneletId, n: number): void {
    let m = this.into.get(v);
    if (!m) this.into.set(v, m = new Map());
    const left = (m.get(u) ?? 0) + n;
    if (left > 0) m.set(u, left); else m.delete(u);
  }

// src/sim/routing/laneReach.ts:300-321 slice of src/sim/routing/laneReach.ts#BodyReach.exitsOf (shared): the feature's statements; "…" marks code left out
  private exitsOf(w: ReachWorld, root: number): readonly LaneletId[] {
    // … 3 lines
    while (stack.length) {
      // … 7 lines
      const own: LaneletId[] = [];
      for (const lane of this.members.get(f.c) ?? []) if (boundaryExit(w, lane)) own.push(lane);
      // … 1 lines
      if (!own.length && f.ahead.length === 1) { this.exitsMemo.set(f.c, this.exitsMemo.get(f.ahead[0]!)!); continue; }
      const all = new Set<LaneletId>(own);
      for (const d of f.ahead) for (const id of this.exitsMemo.get(d)!) all.add(id);
      this.exitsMemo.set(f.c, [...all].sort());
    }
  // … 1 lines
  }

// src/sim/routing/laneReach.ts:333-337 slice of src/sim/routing/laneReach.ts#boundaryExit (shared): the feature's statements; "…" marks code left out
export function boundaryExit(w: ReachWorld, id: LaneletId): boolean {
  const lane = w.lanelet(id);
  return !!lane && lane.kind === 'link' && lane.to !== undefined &&
    w.doc.mapEdge(lane.to) && w.graph.exitsOf(id).length === 0;
}

// src/sim/routing/laneReach.ts:345-350 slice of src/sim/routing/laneReach.ts#LaneReaches.exitsFrom (shared): the feature's statements; "…" marks code left out
  exitsFrom(w: ReachWorld, start: LaneletId, body: BodyClass): readonly LaneletId[] {
    // … 3 lines
    return reach.exitsFrom(w, start);
  }

// src/sim/routing/laneReach.ts:353-358 slice of src/sim/routing/laneReach.ts#LaneReaches.mayReach (shared): the feature's statements; "…" marks code left out
  mayReach(w: ReachWorld, start: LaneletId, goal: LaneletId, body: BodyClass): boolean {
    // … 3 lines
    return reach.reachesFrom(start, goal) !== false;
  }
