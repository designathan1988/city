// The code of src/world/grid.ts that is the feature's (the rest of the file is shared).

// src/world/grid.ts:11-13 function onGridLength: the feature's own
export function onGridLength(length: number, min = 0): number {
  return Math.max(min, Math.round(length / GRID_STEP)) * GRID_STEP;
}

// src/world/grid.ts:16-18 function snapToGrid: the feature's own
export function snapToGrid(p: { readonly x: number; readonly y: number }, step = GRID_STEP, offset = 0): { x: number; y: number } {
  return { x: Math.round((p.x - offset) / step) * step + offset, y: Math.round((p.y - offset) / step) * step + offset };
}

// src/world/grid.ts:21-24 function onGrid: the feature's own
export function onGrid(p: { readonly x: number; readonly y: number }, step = GRID_STEP, offset = 0): boolean {
  return Math.abs((p.x - offset) / step - Math.round((p.x - offset) / step)) < 1e-6 &&
    Math.abs((p.y - offset) / step - Math.round((p.y - offset) / step)) < 1e-6;
}
