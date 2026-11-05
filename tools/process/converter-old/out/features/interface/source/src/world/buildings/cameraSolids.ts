// The code of src/world/buildings/cameraSolids.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/cameraSolids.ts:32-32 field SolidPages.epoch: the feature's own
  private epoch = 0;

// src/world/buildings/cameraSolids.ts:47-50 method SolidPages.walk: the feature's own
  walk(): void {
    this.epoch = (this.epoch + 1) >>> 0;
    if (this.epoch === 0) { for (const s of this.stamps) s.fill(0); this.epoch = 1; }
  }

// src/world/buildings/cameraSolids.ts:53-58 method SolidPages.visitOnce: the feature's own
  visitOnce(slot: number): boolean {
    const stamps = this.stamps[slot >>> PAGE_BITS]!, k = slot & (PAGE - 1);
    if (stamps[k] === this.epoch) return false;
    stamps[k] = this.epoch;
    return true;
  }

// src/world/buildings/cameraSolids.ts:273-276 method SolidFiling.forEachIn: the feature's own
  forEachIn(x0: number, y0: number, x1: number, y1: number, visit: SolidVisit): void {
    this.pages.walk();
    this.visitCells(x0, y0, x1, y1, visit);
  }

// src/world/buildings/cameraSolids.ts:278-281 method SolidFiling.forEachInAny: the feature's own
  forEachInAny(rects: readonly Rect[], visit: SolidVisit): void {
    this.pages.walk();
    for (let i = 0; i < rects.length; i++) { const box = rects[i]!; this.visitCells(box[0], box[1], box[2], box[3], visit); }
  }

// src/world/buildings/cameraSolids.ts:284-296 method SolidFiling.visitCells: the feature's own
  private visitCells(x0: number, y0: number, x1: number, y1: number, visit: SolidVisit): void {
    for (let cx = Math.floor(x0 / CELL); cx <= Math.floor(x1 / CELL); cx++) {
      for (let cy = Math.floor(y0 / CELL); cy <= Math.floor(y1 / CELL); cy++) {
        const list = this.cells.get(cellKey(cx, cy));
        if (!list) continue;
        for (const slot of list) {
          if (!this.pages.visitOnce(slot)) continue;
          const p = this.pages.page(slot), o = at(slot);
          visit(p[o + S_BX]!, p[o + S_BY]!, p[o + S_COS]!, p[o + S_SIN]!, p[o + S_X0]!, p[o + S_Y0]!, p[o + S_X1]!, p[o + S_Y1]!, p[o + S_TOP]! - CLEARANCE);
        }
      }
    }
  }

// src/world/buildings/cameraSolids.ts:298-313 method SolidFiling.forEachOccluder: the feature's own
  forEachOccluder(x0: number, y0: number, x1: number, y1: number, visit: OccluderVisit): void {
    this.pages.walk();
    for (let cx = Math.floor(x0 / CELL); cx <= Math.floor(x1 / CELL); cx++) {
      for (let cy = Math.floor(y0 / CELL); cy <= Math.floor(y1 / CELL); cy++) {
        const list = this.cells.get(cellKey(cx, cy));
        if (!list) continue;
        for (const slot of list) {
          const p = this.pages.page(slot), o = at(slot);
          const bottom = p[o + S_WALL0]!;
          if (Number.isNaN(bottom) || !this.pages.visitOnce(slot)) continue;
          const inset = p[o + S_INSET]!;
          visit(p[o + S_BX]!, p[o + S_BY]!, p[o + S_COS]!, p[o + S_SIN]!, p[o + S_X0]! + inset, p[o + S_Y0]! + inset, p[o + S_X1]! - inset, p[o + S_Y1]! - inset, bottom, p[o + S_WALL1]!);
        }
      }
    }
  }
