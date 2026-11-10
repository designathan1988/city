// The code of src/sim/routing/routeIndex.ts that is the feature's (the rest of the file is shared).

// src/sim/routing/routeIndex.ts:5-5 slice of src/sim/routing/routeIndex.ts#RouteIndex.byLane (shared): the feature's statements; "…" marks code left out
  private readonly byLane = new Map<LaneletId, Set<V>>();

// src/sim/routing/routeIndex.ts:7-7 slice of src/sim/routing/routeIndex.ts#RouteIndex.filed (shared): the feature's statements; "…" marks code left out
  private readonly filed = new Map<V, readonly LaneletId[]>();

// src/sim/routing/routeIndex.ts:8-8 slice of src/sim/routing/routeIndex.ts#RouteIndex.before (shared): the feature's statements; "…" marks code left out
  private readonly before = new Set<LaneletId>();

// src/sim/routing/routeIndex.ts:9-9 slice of src/sim/routing/routeIndex.ts#RouteIndex.after (shared): the feature's statements; "…" marks code left out
  private readonly after = new Set<LaneletId>();

// src/sim/routing/routeIndex.ts:34-37 slice of src/sim/routing/routeIndex.ts#RouteIndex.add (shared): the feature's statements; "…" marks code left out
  private add(id: LaneletId, v: V): void {
    const set = this.byLane.get(id);
    if (set) set.add(v); else this.byLane.set(id, new Set([v]));
  }

// src/sim/routing/routeIndex.ts:39-44 slice of src/sim/routing/routeIndex.ts#RouteIndex.drop (shared): the feature's statements; "…" marks code left out
  private drop(id: LaneletId, v: V): void {
    const set = this.byLane.get(id);
    if (!set) return;
    set.delete(v);
    if (!set.size) this.byLane.delete(id);
  }

// src/sim/routing/routeIndex.ts:59-61 slice of src/sim/routing/routeIndex.ts#RouteIndex.routing (shared): the feature's statements; "…" marks code left out
  routing(id: LaneletId): ReadonlySet<V> | undefined {
    return this.byLane.get(id);
  }

// src/sim/routing/routeIndex.ts:70-73 slice of src/sim/routing/routeIndex.ts#sameTail (shared): the feature's statements; "…" marks code left out
function sameTail(list: readonly LaneletId[], tail: readonly LaneletId[]): boolean {
  for (let i = 0; i < tail.length; i++) if (list[i + 1] !== tail[i]) return false;
// … 1 lines
}
