// The code of src/sim/agents/lotNav.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lotNav.ts:12-15 slice of src/sim/agents/lotNav.ts#SOLID (shared): the feature's statements; "…" marks code left out
const SOLID: ReadonlySet<ElementKind> = new Set<ElementKind>([
  'stair', 'ramp', 'pillar', 'wall', 'fence', 'tree', 'bench', 'planter', 'railing', 'rocks', 'clock', 'hedge',
  'shrub', 'bin', 'lamp', 'bollard', 'parking',
]);

// src/sim/agents/lotNav.ts:17-20 slice of src/sim/agents/lotNav.ts#PEOPLE_SOLID (shared): the feature's statements; "…" marks code left out
const PEOPLE_SOLID: ReadonlySet<ElementKind> = new Set<ElementKind>([
  'pillar', 'wall', 'fence', 'tree', 'bench', 'planter', 'railing', 'rocks', 'clock', 'hedge',
  'shrub', 'bin', 'lamp', 'bollard', 'parking', 'flowers',
]);

// src/sim/agents/lotNav.ts:25-29 slice of src/sim/agents/lotNav.ts#LotGridOptions (shared): the feature's statements; "…" marks code left out
export interface LotGridOptions {
  // … 2 lines
  readonly solid?: ReadonlySet<ElementKind>;
}

// src/sim/agents/lotNav.ts:36-36 slice of src/sim/agents/lotNav.ts#LotPart (shared): the feature's statements; "…" marks code left out
export type LotPart = Pick<BuildingElement, 'kind' | 'x' | 'y' | 'w' | 'd' | 'facing' | 'angle' | 'z'>;

// src/sim/agents/lotNav.ts:54-60 slice of src/sim/agents/lotNav.ts#elementCorners (shared): the feature's statements; "…" marks code left out
function elementCorners(el: LotPart): { x: number; y: number }[] {
  const alongY = el.facing === 0 || el.facing === 2;
  const sx = (alongY ? el.w : el.d) / 2;
  const sy = (alongY ? el.d : el.w) / 2;
  const c = Math.cos(el.angle ?? 0), s = Math.sin(el.angle ?? 0);
  return [[-sx, -sy], [sx, -sy], [sx, sy], [-sx, sy]].map(([a, b]) => ({ x: el.x + a! * c - b! * s, y: el.y + a! * s + b! * c }));
}

// src/sim/agents/lotNav.ts:178-248 slice of src/sim/agents/lotNav.ts#buildLotGrid (shared): the feature's statements; "…" marks code left out
export function buildLotGrid(b: { readonly elements?: readonly LotPart[] }, lot: LotRect, walls: readonly (readonly { x: number; y: number }[])[],
  // … 1 lines
  options: LotGridOptions = {}): LotGrid {
  // … 8 lines
  const solids = (b.elements ?? []).filter((el) => kinds.has(el.kind) && el.z < overhead).map(elementCorners);
  // … 4 lines
  for (const ring of solids) {
    let rx0 = Infinity, ry0 = Infinity, rx1 = -Infinity, ry1 = -Infinity;
    for (const p of ring) { rx0 = Math.min(rx0, p.x); ry0 = Math.min(ry0, p.y); rx1 = Math.max(rx1, p.x); ry1 = Math.max(ry1, p.y); }
    const i0 = Math.max(0, Math.floor((rx0 - x0) / CELL)), i1 = Math.min(nx - 1, Math.floor((rx1 - x0) / CELL));
    const j0 = Math.max(0, Math.floor((ry0 - y0) / CELL)), j1 = Math.min(ny - 1, Math.floor((ry1 - y0) / CELL));
    if (!(i0 <= i1 && j0 <= j1)) continue;
    const cover = new RingCover(ring);
    for (let j = j0; j <= j1; j++) {
      for (let i = i0; i <= i1; i++) {
        if (!solid[j * nx + i] && cover.touches(x0 + i * CELL, y0 + j * CELL, CELL)) solid[j * nx + i] = 1;
      }
    }
  }
// … 41 lines
}
