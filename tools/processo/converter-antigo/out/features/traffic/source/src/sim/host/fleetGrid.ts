// The code of src/sim/host/fleetGrid.ts that is the feature's (the rest of the file is shared).

// src/sim/host/fleetGrid.ts:64-77 method FleetGrid.cellsNear: the feature's own
  cellsNear(x0: number, y0: number, x1: number, y1: number, out: FleetCell<T>[]): FleetCell<T>[] {
    if (!(x0 <= x1 && y0 <= y1)) return out;
    const cx0 = Math.floor(x0 / FLEET_CELL), cy0 = Math.floor(y0 / FLEET_CELL);
    const cx1 = Math.floor(x1 / FLEET_CELL), cy1 = Math.floor(y1 / FLEET_CELL);
    if ((cx1 - cx0 + 1) * (cy1 - cy0 + 1) <= this.cells.size) {
      for (let cy = cy0; cy <= cy1; cy++) for (let cx = cx0; cx <= cx1; cx++) {
        const cell = this.cells.get(keyOf(cx, cy));
        if (cell) out.push(cell);
      }
      return out;
    }
    for (const cell of this.cells.values()) if (cell.cx >= cx0 && cell.cx <= cx1 && cell.cy >= cy0 && cell.cy <= cy1) out.push(cell);
    return out;
  }
