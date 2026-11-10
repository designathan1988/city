// The code of src/sim/peds/signalPostIndex.ts that is the feature's (the rest of the file is shared).

// src/sim/peds/signalPostIndex.ts:23-23 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.byNode (shared): the feature's statements; "…" marks code left out
  private readonly byNode = new Map<NodeId, SignalPost[]>();

// src/sim/peds/signalPostIndex.ts:24-24 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.cells (shared): the feature's statements; "…" marks code left out
  private readonly cells = new Map<number, SignalPost[]>();

// src/sim/peds/signalPostIndex.ts:26-26 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.found (shared): the feature's statements; "…" marks code left out
  private readonly found: SignalPost[] = [];

// src/sim/peds/signalPostIndex.ts:53-62 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction (shared): the feature's statements; "…" marks code left out
  private placeJunction(w: SignalPostWorld, node: NodeId): void {
    // … 3 lines
    for (const segment of record.incident) {
      // … 1 lines
      const post = signalPostPlace(w.net, node, segment);
      if (post) this.file(post);
    }
  }

// src/sim/peds/signalPostIndex.ts:65-67 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeAll (shared): the feature's statements; "…" marks code left out
  private placeAll(w: SignalPostWorld): void {
    for (const post of signalPosts(w.net, w.graph)) this.file(post);
  }

// src/sim/peds/signalPostIndex.ts:69-75 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.file (shared): the feature's statements; "…" marks code left out
  private file(post: SignalPost): void {
    const list = this.byNode.get(post.node);
    if (list) list.push(post); else this.byNode.set(post.node, [post]);
    const k = cellOf(Math.floor(post.x / POST_CELL), Math.floor(post.y / POST_CELL));
    const cell = this.cells.get(k);
    if (cell) cell.push(post); else this.cells.set(k, [post]);
  }

// src/sim/peds/signalPostIndex.ts:78-90 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift (shared): the feature's statements; "…" marks code left out
  private lift(node: NodeId): void {
    // … 3 lines
    for (const post of posts) {
      const k = cellOf(Math.floor(post.x / POST_CELL), Math.floor(post.y / POST_CELL));
      const cell = this.cells.get(k);
      if (!cell) continue;
      const at = cell.indexOf(post);
      if (at >= 0) cell.splice(at, 1);
      if (!cell.length) this.cells.delete(k);
    }
  }

// src/sim/peds/signalPostIndex.ts:93-104 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.near (shared): the feature's statements; "…" marks code left out
  near(x: number, y: number, reach: number): readonly SignalPost[] {
  // … 10 lines
  }
