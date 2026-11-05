// The code of src/sim/agents/kerbBays.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/kerbBays.ts:30-41 slice of src/sim/agents/kerbBays.ts#KerbReads (shared): the feature's statements; "…" marks code left out
export interface KerbReads {
  // … 1 lines
  readonly rank: (id: BuildingId) => number;
// … 8 lines
}

// src/sim/agents/kerbBays.ts:48-48 slice of src/sim/agents/kerbBays.ts#NO_LOT (shared): the feature's statements; "…" marks code left out
const NO_LOT = -1 as BuildingId;

// src/sim/agents/kerbBays.ts:114-124 slice of src/sim/agents/kerbBays.ts#EntranceBook.same (shared): the feature's statements; "…" marks code left out
  same(id: BuildingId, list: readonly Entrance[]): boolean {
    const slot = this.slots.get(id);
    const n = slot < 0 ? 0 : this.runs.length(slot) / 4;
    if (n !== list.length) return false;
    // … 1 lines
    for (let i = 0, at = slot < 0 ? 0 : this.runs.start(slot); i < n; i++, at += 4) {
      const [p, q] = list[i]!;
      if (ends[at] !== p.x || ends[at + 1] !== p.y || ends[at + 2] !== q.x || ends[at + 3] !== q.y) return false;
    }
  // … 1 lines
  }

// src/sim/agents/kerbBays.ts:127-137 slice of src/sim/agents/kerbBays.ts#EntranceBook.box (shared): the feature's statements; "…" marks code left out
  box(id: BuildingId): Box | null {
    const slot = this.slots.get(id);
    if (slot < 0) return null;
    // … 2 lines
    for (let at = this.runs.start(slot), end = at + this.runs.length(slot); at < end; at += 4) {
      box.x0 = Math.min(box.x0, ends[at]!, ends[at + 2]!); box.y0 = Math.min(box.y0, ends[at + 1]!, ends[at + 3]!);
      box.x1 = Math.max(box.x1, ends[at]!, ends[at + 2]!); box.y1 = Math.max(box.y1, ends[at + 1]!, ends[at + 3]!);
    }
  // … 1 lines
  }

// src/sim/agents/kerbBays.ts:140-151 slice of src/sim/agents/kerbBays.ts#EntranceBook.put (shared): the feature's statements; "…" marks code left out
  put(id: BuildingId, list: readonly Entrance[]): void {
    const was = this.box(id);
    if (was) this.file(id, was, false);
    const slot = this.slots.get(id);
    if (slot >= 0) { this.runs.drop(slot); this.slots.delete(id); }
    // … 1 lines
    const to = this.slots.take(id);
    let at = this.runs.put(to, list.length * 4);
    // … 1 lines
    for (const [p, q] of list) { ends[at++] = p.x; ends[at++] = p.y; ends[at++] = q.x; ends[at++] = q.y; }
    this.file(id, entranceBox(list), true);
  }

// src/sim/agents/kerbBays.ts:154-159 slice of src/sim/agents/kerbBays.ts#EntranceBook.each (shared): the feature's statements; "…" marks code left out
  each(id: BuildingId, visit: (px: number, py: number, qx: number, qy: number) => void): void {
    const slot = this.slots.get(id);
    if (slot < 0) return;
    // … 1 lines
    for (let at = this.runs.start(slot), end = at + this.runs.length(slot); at < end; at += 4) visit(ends[at]!, ends[at + 1]!, ends[at + 2]!, ends[at + 3]!);
  }

// src/sim/agents/kerbBays.ts:162-177 slice of src/sim/agents/kerbBays.ts#EntranceBook.file (shared): the feature's statements; "…" marks code left out
  private file(id: BuildingId, box: Box, add: boolean): void {
    for (let i = Math.floor(box.x0 / KERB_CELL); i <= Math.floor(box.x1 / KERB_CELL); i++) {
      for (let j = Math.floor(box.y0 / KERB_CELL); j <= Math.floor(box.y1 / KERB_CELL); j++) {
        // … 2 lines
        if (add) {
          // … 1 lines
          if (!cell.includes(id)) cell.push(id);
        } else if (cell) {
          const at = cell.indexOf(id);
          if (at >= 0) { cell[at] = cell[cell.length - 1]!; cell.pop(); }
        // … 1 lines
        }
      }
    }
  }

// src/sim/agents/kerbBays.ts:340-356 slice of src/sim/agents/kerbBays.ts#entrancesFacing (shared): the feature's statements; "…" marks code left out
function entrancesFacing(book: KerbBook, r: { x0: number; y0: number; x1: number; y1: number }, rank: (id: BuildingId) => number): Entrance[] {
  const ids = new Set<BuildingId>();
  for (let i = Math.floor(r.x0 / KERB_CELL); i <= Math.floor(r.x1 / KERB_CELL); i++) {
    for (let j = Math.floor(r.y0 / KERB_CELL); j <= Math.floor(r.y1 / KERB_CELL); j++) {
      for (const id of book.entrances.cells.get(kerbCell(i, j)) ?? []) ids.add(id as BuildingId);
    }
  }
  // … 1 lines
  for (const id of [...ids].sort((a, b) => rank(a) - rank(b))) {
    // Those `frontagesOn` passes over (the same test on the same box) left out: a lot of the next tile in the cell is no part of the street's key.
    book.entrances.each(id, (px, py, qx, qy) => {
      if (Math.max(px, qx) < r.x0 || Math.min(px, qx) > r.x1 || Math.max(py, qy) < r.y0 || Math.min(py, qy) > r.y1) return;
      out.push([{ x: px, y: py }, { x: qx, y: qy }]);
    });
  }
// … 1 lines
}
