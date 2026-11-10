// The code of src/world/buildings/cameraSolids.ts that is the feature's (the rest of the file is shared).

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
