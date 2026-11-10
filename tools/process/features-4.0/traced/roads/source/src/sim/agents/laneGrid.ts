// The code of src/sim/agents/laneGrid.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/laneGrid.ts:18-18 slice of src/sim/agents/laneGrid.ts#byId (shared): the feature's statements; "…" marks code left out
const byId = (a: Lanelet, b: Lanelet): number => (a.id < b.id ? -1 : a.id > b.id ? 1 : 0);

// src/sim/agents/laneGrid.ts:20-27 slice of src/sim/agents/laneGrid.ts#Filed (shared): the feature's statements; "…" marks code left out
interface Filed {
  readonly lane: Lanelet;
// … 5 lines
}

// src/sim/agents/laneGrid.ts:31-31 slice of src/sim/agents/laneGrid.ts#LaneGrid.filed (shared): the feature's statements; "…" marks code left out
  private readonly filed = new Map<LaneletId, Filed>();

// src/sim/agents/laneGrid.ts:32-32 slice of src/sim/agents/laneGrid.ts#LaneGrid.cells (shared): the feature's statements; "…" marks code left out
  private readonly cells = new Map<number, LaneletId[]>();

// src/sim/agents/laneGrid.ts:42-58 slice of src/sim/agents/laneGrid.ts#LaneGrid.follow (shared): the feature's statements; "…" marks code left out
  follow(graph: LaneletGraph, log: ChangeLog<GraphChange>): void {
    const since = log.since(this.seen);
    if (since !== null && !since.length) return;
    this.seen = log.state;
    if (since === null) { this.fileAll(graph); return; }
    // … 1 lines
    for (const change of since) {
      for (const id of change.lanelets) {
        const was = this.filed.get(id);
        const now = graph.lanelets.get(id);
        if (was && was.lane === now) continue;
        if (was) { this.unfile(was); boxes.push(boxOf(was.lane)); }
        if (now) { this.file(now); boxes.push(boxOf(now)); }
      }
    }
  // … 1 lines
  }

// src/sim/agents/laneGrid.ts:61-66 slice of src/sim/agents/laneGrid.ts#LaneGrid.fileAll (shared): the feature's statements; "…" marks code left out
  private fileAll(graph: LaneletGraph): void {
    // … 2 lines
    for (const lane of graph.lanelets.values()) this.file(lane);
  // … 1 lines
  }

// src/sim/agents/laneGrid.ts:68-81 slice of src/sim/agents/laneGrid.ts#LaneGrid.file (shared): the feature's statements; "…" marks code left out
  private file(lane: Lanelet): void {
    // … 1 lines
    const box = lane.centre.bbox;
    // … 1 lines
    for (let gx = Math.floor(box.minX / LANE_CELL); gx <= Math.floor(box.maxX / LANE_CELL); gx++) {
      for (let gy = Math.floor(box.minY / LANE_CELL); gy <= Math.floor(box.maxY / LANE_CELL); gy++) {
        const k = cellKey(gx, gy);
        cells.push(k);
        const list = this.cells.get(k);
        if (list) list.push(lane.id); else this.cells.set(k, [lane.id]);
      }
    }
    this.filed.set(lane.id, { lane, cells, digest: NaN, visit: 0 });
  }

// src/sim/agents/laneGrid.ts:83-93 slice of src/sim/agents/laneGrid.ts#LaneGrid.unfile (shared): the feature's statements; "…" marks code left out
  private unfile(was: Filed): void {
    // … 1 lines
    for (const k of was.cells) {
      // … 2 lines
      const i = list.indexOf(was.lane.id);
      if (i >= 0) list.splice(i, 1);
    // … 1 lines
    }
    this.filed.delete(was.lane.id);
  }

// src/sim/agents/laneGrid.ts:96-100 slice of src/sim/agents/laneGrid.ts#LaneGrid.lanesIn (shared): the feature's statements; "…" marks code left out
  private lanesIn(x0: number, y0: number, x1: number, y1: number): Lanelet[] {
    const out: Lanelet[] = [];
    this.gather(x0, y0, x1, y1, out);
    return out.sort(byId);
  }

// src/sim/agents/laneGrid.ts:103-117 slice of src/sim/agents/laneGrid.ts#LaneGrid.gather (shared): the feature's statements; "…" marks code left out
  private gather(x0: number, y0: number, x1: number, y1: number, out: Lanelet[]): void {
    // … 1 lines
    for (let gx = Math.floor(x0 / LANE_CELL); gx <= Math.floor(x1 / LANE_CELL); gx++) {
      for (let gy = Math.floor(y0 / LANE_CELL); gy <= Math.floor(y1 / LANE_CELL); gy++) {
        // … 2 lines
        for (const id of list) {
          // … 3 lines
          out.push(filed.lane);
        }
      }
    }
  }

// src/sim/agents/laneGrid.ts:120-124 slice of src/sim/agents/laneGrid.ts#LaneGrid.nearInto (shared): the feature's statements; "…" marks code left out
  nearInto(x: number, y: number, reach: number, out: Lanelet[]): void {
    out.length = 0;
    if (!Number.isFinite(x) || !Number.isFinite(y)) { for (const f of this.filed.values()) out.push(f.lane); return; }
    this.gather(x - reach, y - reach, x + reach, y + reach, out);
  }

// src/sim/agents/laneGrid.ts:127-130 slice of src/sim/agents/laneGrid.ts#LaneGrid.near (shared): the feature's statements; "…" marks code left out
  near(x: number, y: number, reach: number): readonly Lanelet[] {
  // … 2 lines
  }

// src/sim/agents/laneGrid.ts:133-141 slice of src/sim/agents/laneGrid.ts#LaneGrid.inBox (shared): the feature's statements; "…" marks code left out
  inBox(x0: number, y0: number, x1: number, y1: number): Lanelet[] {
    const out: Lanelet[] = [];
    for (const lane of this.lanesIn(x0, y0, x1, y1)) {
      const box = lane.centre.bbox;
      if (box.maxX < x0 || box.minX > x1 || box.maxY < y0 || box.minY > y1) continue;
      out.push(lane);
    }
    return out;
  }

// src/sim/agents/laneGrid.ts:144-149 slice of src/sim/agents/laneGrid.ts#LaneGrid.digest (shared): the feature's statements; "…" marks code left out
  digest(lane: Lanelet): number {
    const filed = this.filed.get(lane.id);
    if (!filed || filed.lane !== lane) return digestOf(lane);
    if (Number.isNaN(filed.digest)) filed.digest = digestOf(lane);
    return filed.digest;
  }

// src/sim/agents/laneGrid.ts:152-152 slice of src/sim/agents/laneGrid.ts#digestOf (shared): the feature's statements; "…" marks code left out
const digestOf = (lane: Lanelet): number => new Digest().addText(lane.id).add(lane.length).addAll(lane.centre.xy).value();

// src/sim/agents/laneGrid.ts:154-157 slice of src/sim/agents/laneGrid.ts#boxOf (shared): the feature's statements; "…" marks code left out
function boxOf(lane: Lanelet): ChangeRect {
  const b = lane.centre.bbox;
  return [b.minX, b.minY, b.maxX, b.maxY];
}

// src/sim/agents/laneGrid.ts:160-164 slice of src/sim/agents/laneGrid.ts#LaneGridWorld (shared): the feature's statements; "…" marks code left out
export interface LaneGridWorld {
  readonly graph: LaneletGraph;
  readonly topologyLog: ChangeLog<GraphChange>;
// … 1 lines
}

// src/sim/agents/laneGrid.ts:173-175 slice of src/sim/agents/laneGrid.ts#lanesNear (shared): the feature's statements; "…" marks code left out
export function lanesNear(w: LaneGridWorld, x: number, y: number, reach: number): readonly Lanelet[] {
// … 1 lines
}

// src/sim/agents/laneGrid.ts:178-180 slice of src/sim/agents/laneGrid.ts#lanesNearInto (shared): the feature's statements; "…" marks code left out
export function lanesNearInto(w: LaneGridWorld, x: number, y: number, reach: number, out: Lanelet[]): void {
  laneGrid(w).nearInto(x, y, reach, out);
}

// src/sim/agents/laneGrid.ts:183-185 slice of src/sim/agents/laneGrid.ts#lanesInBox (shared): the feature's statements; "…" marks code left out
export function lanesInBox(w: LaneGridWorld, x0: number, y0: number, x1: number, y1: number): Lanelet[] {
// … 1 lines
}
