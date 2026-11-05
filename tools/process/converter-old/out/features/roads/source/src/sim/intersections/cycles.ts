// The code of src/sim/intersections/cycles.ts that is the feature's (the rest of the file is shared).

// src/sim/intersections/cycles.ts:11-16 slice of src/sim/intersections/cycles.ts#RoadCycle (shared): the feature's statements; "…" marks code left out
export interface RoadCycle {
  // … 3 lines
  readonly lanelets: ReadonlySet<LaneletId>;
}

// src/sim/intersections/cycles.ts:23-23 slice of src/sim/intersections/cycles.ts#indexes (shared): the feature's statements; "…" marks code left out
const indexes = new WeakMap<LaneletGraph, CycleIndex>();

// src/sim/intersections/cycles.ts:25-30 slice of src/sim/intersections/cycles.ts#indexOf (shared): the feature's statements; "…" marks code left out
function indexOf(w: SimWorld): CycleIndex {
  let index = indexes.get(w.graph);
  if (!index) indexes.set(w.graph, index = new CycleIndex());
  index.follow(w.graph, w.topologyLog);
  return index;
}

// src/sim/intersections/cycles.ts:33-35 slice of src/sim/intersections/cycles.ts#cyclesOf (shared): the feature's statements; "…" marks code left out
export function cyclesOf(w: SimWorld, lanelet: LaneletId): readonly RoadCycle[] {
  return indexOf(w).byLanelet.get(lanelet) ?? [];
}

// src/sim/intersections/cycles.ts:38-52 slice of src/sim/intersections/cycles.ts#cycleFull (shared): the feature's statements; "…" marks code left out
export function cycleFull(w: SimWorld, from: LaneletId, to: LaneletId, need: number): boolean {
  for (const cycle of cyclesOf(w, to)) {
    if (cycle.lanelets.has(from)) continue;
    let used = need;
    for (const lane of cycle.lanelets) {
      for (const id of w.rt(lane).order) {
        const v = w.veh(id);
        if (!v) continue;
        used += v.archetype.length + Math.max(JAM_GAP, v.driver.s0);
      }
    }
    if (used > cycle.storage * METER_SHARE) return true;
  }
// … 1 lines
}

// src/sim/intersections/cycles.ts:56-56 slice of src/sim/intersections/cycles.ts#CycleIndex.byLanelet (shared): the feature's statements; "…" marks code left out
  readonly byLanelet = new Map<LaneletId, RoadCycle[]>();

// src/sim/intersections/cycles.ts:58-58 slice of src/sim/intersections/cycles.ts#CycleIndex.ofStart (shared): the feature's statements; "…" marks code left out
  private readonly ofStart = new Map<LaneletId, { readonly key: string | null; readonly lanes: readonly LaneletId[]; readonly nodes: readonly NodeId[] }>();

// src/sim/intersections/cycles.ts:60-60 slice of src/sim/intersections/cycles.ts#CycleIndex.laneReaders (shared): the feature's statements; "…" marks code left out
  private readonly laneReaders = new Map<LaneletId, Set<LaneletId>>();

// src/sim/intersections/cycles.ts:61-61 slice of src/sim/intersections/cycles.ts#CycleIndex.nodeReaders (shared): the feature's statements; "…" marks code left out
  private readonly nodeReaders = new Map<NodeId, Set<LaneletId>>();

// src/sim/intersections/cycles.ts:68-84 slice of src/sim/intersections/cycles.ts#CycleIndex.follow (shared): the feature's statements; "…" marks code left out
  follow(graph: LaneletGraph, log: ChangeLog<GraphChange>): void {
    const changes = log.since(this.seen);
    if (changes !== null && !changes.length) return;
    this.seen = log.state;
    const starts = changes === null ? this.restart(graph) : new Set<LaneletId>();
    if (changes !== null) {
      for (const change of changes) {
        for (const lane of change.lanelets) {
          starts.add(lane);
          for (const start of this.laneReaders.get(lane) ?? []) starts.add(start);
        }
        for (const node of change.junctions) for (const start of this.nodeReaders.get(node) ?? []) starts.add(start);
      }
      for (const start of starts) this.forget(start);
    }
    for (const start of [...starts].sort()) if (graph.lanelets.get(start)?.kind === 'link') this.search(graph, start);
  }

// src/sim/intersections/cycles.ts:87-96 slice of src/sim/intersections/cycles.ts#CycleIndex.restart (shared): the feature's statements; "…" marks code left out
  private restart(graph: LaneletGraph): Set<LaneletId> {
    // … 5 lines
    const starts = new Set<LaneletId>();
    for (const lane of graph.lanelets.values()) if (lane.kind === 'link') starts.add(lane.id);
    return starts;
  }

// src/sim/intersections/cycles.ts:99-116 slice of src/sim/intersections/cycles.ts#CycleIndex.forget (shared): the feature's statements; "…" marks code left out
  private forget(start: LaneletId): void {
    const made = this.ofStart.get(start);
    if (!made) return;
    this.ofStart.delete(start);
    for (const lane of made.lanes) unfile(this.laneReaders, lane, start);
    for (const node of made.nodes) unfile(this.nodeReaders, node, start);
    if (made.key === null) return;
    const loop = this.loops.get(made.key)!;
    if (--loop.finders > 0) return;
    this.loops.delete(made.key);
    for (const id of loop.cycle.lanelets) {
      const list = this.byLanelet.get(id);
      if (!list) continue;
      const i = list.indexOf(loop.cycle);
      if (i >= 0) list.splice(i, 1);
      if (!list.length) this.byLanelet.delete(id);
    }
  }

// src/sim/intersections/cycles.ts:119-202 slice of src/sim/intersections/cycles.ts#CycleIndex.search (shared): the feature's statements; "…" marks code left out
  private search(graph: LaneletGraph, start: LaneletId): void {
    const lanes = new Set<LaneletId>([start]);
    // … 22 lines
    const best = new Map<LaneletId, number>();
    const parent = new Map<LaneletId, LaneletId>();
    // … 1 lines
    for (const n of next(start)) {
      const cost = length(start) + length(n);
      if (n === start) continue;
      if (cost <= MAX_CYCLE_STORAGE && cost < (best.get(n) ?? Infinity)) {
        best.set(n, cost);
        parent.set(n, start);
        open.push({ id: n, cost });
      }
    }
    let closing: LaneletId | null = null;
    // … 1 lines
    while (open.size) {
      // … 1 lines
      if (cost > (best.get(id) ?? Infinity) || cost >= closingCost) continue;
      for (const n of next(id)) {
        if (n === start) {
          if (cost < closingCost) {
            closingCost = cost;
            closing = id;
          }
          continue;
        }
        // … 1 lines
        if (c > MAX_CYCLE_STORAGE || c >= (best.get(n) ?? Infinity)) continue;
        best.set(n, c);
        parent.set(n, id);
      // … 1 lines
      }
    }
    // … 1 lines
    if (closing !== null) {
      const loop: LaneletId[] = [start];
      for (let at: LaneletId | undefined = closing; at !== undefined && at !== start; at = parent.get(at)) loop.push(at);
      key = [...loop].sort().join('|');
      const known = this.loops.get(key);
      if (known) known.finders++;
      else {
        // The connectors between the loop's links belong to it too: a car in the box between two of them is on the loop.
        const members = new Set<LaneletId>(loop);
        for (const link of loop) {
          for (const c of graph.exitsOf(link)) {
            const connector = graph.lanelets.get(c) as { toLane?: LaneletId } | undefined;
            if (connector?.toLane && members.has(connector.toLane)) members.add(c);
          }
        }
        const cycle: RoadCycle = { id: this.nextId++, storage: closingCost, lanelets: members };
        this.loops.set(key, { cycle, finders: 1 });
        for (const id of members) {
          const list = this.byLanelet.get(id);
          if (list) list.push(cycle); else this.byLanelet.set(id, [cycle]);
        }
      }
    }
    this.ofStart.set(start, { key, lanes: [...lanes], nodes: [...nodes] });
    for (const lane of lanes) file(this.laneReaders, lane, start);
    for (const node of nodes) file(this.nodeReaders, node, start);
  }

// src/sim/intersections/cycles.ts:122-126 slice of src/sim/intersections/cycles.ts#CycleIndex.search.length (shared): the feature's statements; "…" marks code left out
    const length = (id: LaneletId): number => {
      lanes.add(id);
      const lane = graph.lanelets.get(id);
      return lane?.kind === 'link' ? lane.length : 0;
    };

// src/sim/intersections/cycles.ts:128-140 slice of src/sim/intersections/cycles.ts#CycleIndex.search.next (shared): the feature's statements; "…" marks code left out
    const next = (id: LaneletId): LaneletId[] => {
      lanes.add(id);
      const to = graph.lanelets.get(id)?.to;
      if (to !== undefined) nodes.add(to);
      const out: LaneletId[] = [];
      for (const c of graph.exitsOf(id)) {
        lanes.add(c);
        const connector = graph.lanelets.get(c) as { kind: string; turn?: string; toLane?: LaneletId } | undefined;
        if (!connector || connector.turn === 'uturn' || !connector.toLane) continue;
        out.push(connector.toLane);
      }
      return out.sort();
    };

// src/sim/intersections/cycles.ts:205-208 slice of src/sim/intersections/cycles.ts#file (shared): the feature's statements; "…" marks code left out
function file<K>(lists: Map<K, Set<LaneletId>>, key: K, start: LaneletId): void {
  const set = lists.get(key);
  if (set) set.add(start); else lists.set(key, new Set([start]));
}

// src/sim/intersections/cycles.ts:210-215 slice of src/sim/intersections/cycles.ts#unfile (shared): the feature's statements; "…" marks code left out
function unfile<K>(lists: Map<K, Set<LaneletId>>, key: K, start: LaneletId): void {
  const set = lists.get(key);
  if (!set) return;
  set.delete(start);
  if (!set.size) lists.delete(key);
}

// src/sim/intersections/cycles.ts:219-219 slice of src/sim/intersections/cycles.ts#OpenSet.items (shared): the feature's statements; "…" marks code left out
  private readonly items: { id: LaneletId; cost: number }[] = [];

// src/sim/intersections/cycles.ts:225-235 slice of src/sim/intersections/cycles.ts#OpenSet.push (shared): the feature's statements; "…" marks code left out
  push(item: { id: LaneletId; cost: number }): void {
    // … 1 lines
    items.push(item);
  // … 7 lines
  }

// src/sim/intersections/cycles.ts:237-255 slice of src/sim/intersections/cycles.ts#OpenSet.pop (shared): the feature's statements; "…" marks code left out
  pop(): { id: LaneletId; cost: number } {
  // … 17 lines
  }

// src/sim/intersections/cycles.ts:259-260 slice of src/sim/intersections/cycles.ts#before (shared): the feature's statements; "…" marks code left out
const before = (a: { id: LaneletId; cost: number }, b: { id: LaneletId; cost: number }): boolean =>
  a.cost < b.cost || (a.cost === b.cost && a.id < b.id);
