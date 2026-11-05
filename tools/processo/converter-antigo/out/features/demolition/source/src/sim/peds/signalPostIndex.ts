// The code of src/sim/peds/signalPostIndex.ts that is the feature's (the rest of the file is shared).

// src/sim/peds/signalPostIndex.ts:9-14 interface SignalPostWorld: the feature's own
export interface SignalPostWorld {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly graph: LaneletGraph;
  signalChangesSince(serial: number): { readonly serial: number; readonly nodes: readonly NodeId[] | null };
}

// src/sim/peds/signalPostIndex.ts:18-18 function cellOf: the feature's own
const cellOf = (gx: number, gy: number): number => (gx + 32768) * 65536 + (gy + 32768);

// src/sim/peds/signalPostIndex.ts:26-26 field SignalPostIndex.found: the feature's own
  private readonly found: SignalPost[] = [];

// src/sim/peds/signalPostIndex.ts:29-33 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.reset (shared): the feature's statements; "…" marks code left out
  reset(): void {
    this.serial = -1;
  // … 2 lines
  }

// src/sim/peds/signalPostIndex.ts:36-50 method SignalPostIndex.follow: the feature's own
  follow(w: SignalPostWorld): void {
    const { serial, nodes } = w.signalChangesSince(this.serial);
    if (serial === this.serial && nodes !== null) return;
    this.serial = serial;
    if (nodes === null) {
      this.byNode.clear();
      this.cells.clear();
      this.placeAll(w);
      return;
    }
    for (const node of new Set(nodes)) {
      this.lift(node);
      this.placeJunction(w, node);
    }
  }

// src/sim/peds/signalPostIndex.ts:53-62 method SignalPostIndex.placeJunction: the feature's own
  private placeJunction(w: SignalPostWorld, node: NodeId): void {
    const junction = w.graph.junctions.get(node);
    const record = w.doc.node(node);
    if (!junction?.signalised || !record) return;
    for (const segment of record.incident) {
      if (!junction.groups.some((group) => group.segments.includes(segment))) continue;
      const post = signalPostPlace(w.net, node, segment);
      if (post) this.file(post);
    }
  }

// src/sim/peds/signalPostIndex.ts:65-67 method SignalPostIndex.placeAll: the feature's own
  private placeAll(w: SignalPostWorld): void {
    for (const post of signalPosts(w.net, w.graph)) this.file(post);
  }

// src/sim/peds/signalPostIndex.ts:69-75 method SignalPostIndex.file: the feature's own
  private file(post: SignalPost): void {
    const list = this.byNode.get(post.node);
    if (list) list.push(post); else this.byNode.set(post.node, [post]);
    const k = cellOf(Math.floor(post.x / POST_CELL), Math.floor(post.y / POST_CELL));
    const cell = this.cells.get(k);
    if (cell) cell.push(post); else this.cells.set(k, [post]);
  }

// src/sim/peds/signalPostIndex.ts:78-90 method SignalPostIndex.lift: the feature's own
  private lift(node: NodeId): void {
    const posts = this.byNode.get(node);
    if (!posts) return;
    this.byNode.delete(node);
    for (const post of posts) {
      const k = cellOf(Math.floor(post.x / POST_CELL), Math.floor(post.y / POST_CELL));
      const cell = this.cells.get(k);
      if (!cell) continue;
      const at = cell.indexOf(post);
      if (at >= 0) cell.splice(at, 1);
      if (!cell.length) this.cells.delete(k);
    }
  }

// src/sim/peds/signalPostIndex.ts:93-104 method SignalPostIndex.near: the feature's own
  near(x: number, y: number, reach: number): readonly SignalPost[] {
    const out = this.found;
    out.length = 0;
    if (!this.cells.size) return out;
    for (let gx = Math.floor((x - reach) / POST_CELL); gx <= Math.floor((x + reach) / POST_CELL); gx++) {
      for (let gy = Math.floor((y - reach) / POST_CELL); gy <= Math.floor((y + reach) / POST_CELL); gy++) {
        const cell = this.cells.get(cellOf(gx, gy));
        if (cell) for (const post of cell) out.push(post);
      }
    }
    return out;
  }
