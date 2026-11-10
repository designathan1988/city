// The code of src/world/geometry.ts that is the feature's (the rest of the file is shared).

// src/world/geometry.ts:9-18 interface LegFrame: the feature's own
export interface LegFrame {
  /** Position on the centreline. */
  readonly p: Vec2;
  /** Unit direction pointing AWAY from the reference node. */
  readonly dir: Vec2;
  /** Unit left normal of `dir`. */
  readonly nrm: Vec2;
  /** Total arc length of the segment. */
  readonly length: number;
}

// src/world/geometry.ts:43-56 function frameFromNode: the feature's own
export function frameFromNode(
  pl: Polyline,
  segStartsAtNode: boolean,
  distance: number,
): LegFrame {
  const length = pl.length;
  if (segStartsAtNode) {
    const f = pl.sampleAt(distance);
    return { p: f.p, dir: f.t, nrm: perp(f.t), length };
  }
  const f = pl.sampleAt(length - distance);
  const dir = neg(f.t);
  return { p: f.p, dir, nrm: perp(dir), length };
}

// src/world/geometry.ts:59-60 function farNode: the feature's own
export const farNode = (seg: RoadSegment, from: NodeId): NodeId =>
  seg.a === from ? seg.b : seg.a;

// src/world/geometry.ts:62-63 function segmentStartsAt: the feature's own
export const segmentStartsAt = (seg: RoadSegment, node: NodeId): boolean =>
  seg.a === node;

// src/world/geometry.ts:66-100 class PolylineCache: the feature's own
export class PolylineCache {
  private map = new Map<SegmentId, Polyline>();

  /** The segment's centreline. */
  get(doc: RoadDoc, id: SegmentId): Polyline {
    let pl = this.map.get(id);
    if (!pl) {
      pl = segmentPolyline(doc, doc.requireSegment(id));
      this.map.set(id, pl);
    }
    return pl;
  }

  invalidate(id: SegmentId): void {
    this.map.delete(id);
  }

  clear(): void {
    this.map.clear();
  }

  /** A working copy over `other` (`Network.fork`): reads through it, keeps only the polylines it makes or drops. */
  forkFrom(other: PolylineCache): void {
    this.map = new ForkMap(other.map);
  }

  /** A working copy's changes (`forkFrom`) written back. */
  takeBack(fork: PolylineCache): void {
    (fork.map as ForkMap<SegmentId, Polyline>).applyTo(this.map);
  }

  get size(): number {
    return this.map.size;
  }
}
