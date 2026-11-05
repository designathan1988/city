// The code of src/editor/growthFeed.ts that is the feature's (the rest of the file is shared).

// src/editor/growthFeed.ts:25-33 slice of src/editor/growthFeed.ts#GrowthHost (shared): the feature's statements; "…" marks code left out
export interface GrowthHost {
  // … 1 lines
  readonly net: Network | null;
  // … 2 lines
  readonly pavedAt?: PavingSource;
// … 2 lines
}

// src/editor/growthFeed.ts:89-89 slice of src/editor/growthFeed.ts#GrowthSession.roadsOf (shared): the feature's statements; "…" marks code left out
  private roadsOf: Network | null = null;

// src/editor/growthFeed.ts:99-101 slice of src/editor/growthFeed.ts#GrowthSession.busy (shared): the feature's statements; "…" marks code left out
  get busy(): boolean {
    return this.asked !== null;
  }

// src/editor/growthFeed.ts:104-115 slice of src/editor/growthFeed.ts#GrowthSession.ask (shared): the feature's statements; "…" marks code left out
  ask(refused: Set<number>, seed: number, most = 1): boolean {
    if (this.asked) return true;
  // … 9 lines
  }

// src/editor/growthFeed.ts:118-126 slice of src/editor/growthFeed.ts#GrowthSession.restart (shared): the feature's statements; "…" marks code left out
  restart(): void {
    // … 4 lines
    const asked = this.asked;
    this.asked = null;
    if (asked) this.send(asked.lot, asked.rng, asked.refused, asked.extra, asked.misses);
  }

// src/editor/growthFeed.ts:129-191 slice of src/editor/growthFeed.ts#GrowthSession.receive (shared): the feature's statements; "…" marks code left out
  receive(answer: GrowAnswer, onGrown: (id: number) => void = () => {}): GrowthOutcome {
    const asked = this.asked;
    if (!asked || answer.id !== asked.id) return { kind: 'stale' };
    this.asked = null;
    // … 5 lines
    const moved = changes === null || changes.length > 0 || (net?.state ?? null) !== asked.netState;
    // … 3 lines
    if ('error' in answer) {
      // … 1 lines
      console.error(`[growth] lot ${asked.lot.id} failed in the growth's worker: ${answer.error}`);
      open.count(asked.refused);
      if (this.lotOpen(doc, asked.lot, asked.refused)) { open.refuse(asked.lot, asked.refused); refused++; }
    // … 1 lines
    }
    for (const g of answer.grown) {
      open.count(asked.refused);
      const lot = asked.lots.get(g.lot)!;
      // … 1 lines
      if (!this.lotOpen(doc, lot, asked.refused)) return { kind: 'answered', grown, refused, again: true };
      if (g.record === null) {
        // … 1 lines
        if (moved) return { kind: 'answered', grown, refused, again: true };
        open.refuse(lot, asked.refused);
      // … 2 lines
      }
      if (moved && validateBuilding({ doc, net, groundAt, ...(pavedAt ? { pavedAt } : {}) }, { ...g.record, id: doc.buildings.nextId } as Building) !== null) {
        const times = (this.refusals.get(lot.id) ?? 0) + 1;
        this.refusals.set(lot.id, times);
        if (times >= MOST_REFUSALS) {
          this.refusals.delete(lot.id);
          open.refuse(lot, asked.refused);
          refused++;
        }
        // What came after was made beside it: let go with it.
        return { kind: 'answered', grown, refused, again: true };
      }
      this.refusals.delete(lot.id);
      const id = commitGrown(doc, lot, g.record);
      grown.push(id);
      onGrown(id);
    }
    if (answer.miss !== undefined && answer.grown.length === 0) {
      // … 1 lines
      open.count(asked.refused);
      const lot = asked.lot;
      if (!this.lotOpen(doc, lot, asked.refused)) return { kind: 'answered', grown, refused, again: true };
      if (asked.misses + 1 >= MOST_MISSES) {
        console.warn(`[growth] lot ${lot.id} set aside: ${answer.miss}`);
        open.refuse(lot, asked.refused);
        return { kind: 'answered', grown, refused: refused + 1, again: false };
      }
      // … 1 lines
      this.send(lot, asked.rng, asked.refused, asked.extra + MISS_MARGIN, asked.misses + 1);
    }
  // … 1 lines
  }

// src/editor/growthFeed.ts:200-216 slice of src/editor/growthFeed.ts#GrowthSession.readChanges (shared): the feature's statements; "…" marks code left out
  private readChanges(doc: RoadDoc, net: Network | null): void {
    // … 6 lines
    if (!net) return;
    const delta = this.roadsOf === net && this.roadsState !== null ? net.changesSince(this.roadsState) : null;
    if (delta === null) this.forgetRoads();
    else {
      for (const id of delta.segments) if (this.sentRoads.has(id)) this.staleRoads.add(id);
      for (const node of delta.nodes) if (this.sentKerbs.has(node)) this.staleKerbs.add(node);
    }
    this.roadsOf = net;
    this.roadsState = net.state;
  }

// src/editor/growthFeed.ts:227-274 slice of src/editor/growthFeed.ts#GrowthSession.send (shared): the feature's statements; "…" marks code left out
  private send(lot: Lot, rng: RngState, refused: Set<number>, extra: number, misses: number): void {
    // … 25 lines
    let paving: PavingData | null = null;
    // … 2 lines
    if (pavedAt) {
      const packed = pavedAt.pack?.(reach.lot) ?? null;
      if (packed) { paving = packed.data; transfer.push(...packed.transfer); }
      if (!packed || misses > 0) {
        const points = streetPointsOf(lot);
        paved = new Float64Array(points.length * 3);
        points.forEach((p, k) => { paved![k * 3] = p.x; paved![k * 3 + 1] = p.y; paved![k * 3 + 2] = pavedAt(p.x, p.y); });
      }
    }
    // … 3 lines
    this.asked = { id, lot, lots, rng, refused, extra, misses, netState: net?.state ?? null };
    this.lane.post({
      // … 2 lines
      paving, paved, pavedRead: pavedAt !== undefined, roads: net !== null,
    }, transfer);
  }

// src/editor/growthFeed.ts:300-332 slice of src/editor/growthFeed.ts#GrowthSession.syncRoads (shared): the feature's statements; "…" marks code left out
  private syncRoads(net: Network, rect: ChangeRect): NonNullable<GrowthSync['roads']> {
    // … 3 lines
    for (const id of this.staleRoads) {
      const piece = roadPiece(net, id as SegmentId);
      if (piece) segments.push(piece);
      else { dropSegments.push(id); this.sentRoads.delete(id); }
    }
    for (const node of this.staleKerbs) {
      const piece = junctionPiece(net, node as NodeId);
      if (piece) junctions.push(piece);
      else { dropJunctions.push(node); this.sentKerbs.delete(node); }
    }
    // … 3 lines
    for (const id of net.segmentCells.idsMeeting(rect)) {
      if (this.sentRoads.has(id)) continue;
      const piece = roadPiece(net, id);
      if (!piece) continue;
      this.sentRoads.add(id);
      segments.push(piece);
    }
    for (const node of net.junctionCells.idsMeeting(rect)) {
      if (this.sentKerbs.has(node)) continue;
      const piece = junctionPiece(net, node);
      if (!piece) continue;
      this.sentKerbs.add(node);
      junctions.push(piece);
    }
  // … 1 lines
  }

// src/editor/growthFeed.ts:336-341 slice of src/editor/growthFeed.ts#roadPiece (shared): the feature's statements; "…" marks code left out
function roadPiece(net: Network, id: SegmentId): RoadPiece | null {
  const ribbon = net.ribbons.get(id);
  const rect = net.segmentCells.rectOf(id);
  if (!ribbon || !rect) return null;
  return { id, rect, width: ribbon.road.width, sidewalk: ribbon.road.sidewalk, structure: net.doc.segment(id)?.structure ?? null, xy: ribbon.full.xy };
}

// src/editor/growthFeed.ts:344-353 slice of src/editor/growthFeed.ts#junctionPiece (shared): the feature's statements; "…" marks code left out
function junctionPiece(net: Network, node: NodeId): JunctionPiece | null {
  const junction = net.junctions.get(node)?.get(Level.Curb);
  const rect = net.junctionCells.rectOf(node);
  if (!junction || !rect) return null;
  const flat = junction.ring.flatten();
  const ring = new Float64Array(flat.length * 2);
  flat.forEach((p, k) => { ring[k * 2] = p.x; ring[k * 2 + 1] = p.y; });
  const box = junction.ring.bbox;
  return { node, rect, ring, bbox: { minX: box.minX, minY: box.minY, maxX: box.maxX, maxY: box.maxY }, empty: junction.ring.isEmpty };
}
