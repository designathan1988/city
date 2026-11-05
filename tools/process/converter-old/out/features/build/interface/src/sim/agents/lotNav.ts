// What the feature uses of src/sim/agents/lotNav.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lotNav.ts:12-15 variable SOLID
/** What a car cannot drive through on a lot. */
const SOLID: ReadonlySet<ElementKind> = new Set<ElementKind>([
  'stair', 'ramp', 'pillar', 'wall', 'fence', 'tree', 'bench', 'planter', 'railing', 'rocks', 'clock', 'hedge',
  'shrub', 'bin', 'lamp', 'bollard', 'parking',
]);

// src/sim/agents/lotNav.ts:17-20 variable PEOPLE_SOLID
/** What a person cannot walk through on a lot: a car's list less the stairs and ramps, and the flower beds. */
const PEOPLE_SOLID: ReadonlySet<ElementKind> = new Set<ElementKind>([
  'pillar', 'wall', 'fence', 'tree', 'bench', 'planter', 'railing', 'rocks', 'clock', 'hedge',
  'shrub', 'bin', 'lamp', 'bollard', 'parking', 'flowers',
]);

// src/sim/agents/lotNav.ts:25-29 interface LotGridOptions
/** The grid's make for one kind of agent: its cell, the clearance its centre keeps, what is solid to it. */
export interface LotGridOptions {
  readonly cell?: number;
  readonly clearance?: number;
  readonly solid?: ReadonlySet<ElementKind>;
}

// src/sim/agents/lotNav.ts:31-31 variable PEOPLE_GRID
/** A person's grid: 20 cm cells, so a gate's opening */
export const PEOPLE_GRID: LotGridOptions = { cell: m(0.2), clearance: m(0.25), solid: PEOPLE_SOLID };

// src/sim/agents/lotNav.ts:33-33 interface LotRect
export interface LotRect { readonly x: number; readonly y: number; readonly w: number; readonly d: number }

// src/sim/agents/lotNav.ts:36-36 type LotPart
/** What a lot's grid reads of a part of the lot (`buildLotGrid`): all a worker is sent of it (`lotBays.ts` `shareOf`). */
export type LotPart = Pick<BuildingElement, 'kind' | 'x' | 'y' | 'w' | 'd' | 'facing' | 'angle' | 'z'>;

// src/sim/agents/lotNav.ts:38-51 interface LotGrid
export interface LotGrid {
  readonly x0: number;
  readonly y0: number;
  /** Cell size. */
  readonly cell: number;
  readonly nx: number;
  readonly ny: number;
  /** 1 where a car's centre may not be. */
  readonly blocked: Uint8Array;
  /** Distance to the nearest exit along free cells; Infinity where none is reached. */
  readonly dist: Float32Array;
  /** Which exit each cell's distance leads to (index into the exits given), or -1. */
  readonly exitOf: Int32Array;
}

// src/sim/agents/lotNav.ts:54-60 function elementCorners
/** The corners of an element's box in the local frame, turned by its angle. */
function elementCorners(el: LotPart): { x: number; y: number }[];

// src/sim/agents/lotNav.ts:63-113 class RingCover
/** DO NOT build arrays per cell here */
class RingCover { … }

// src/sim/agents/lotNav.ts:178-248 function buildLotGrid
/** The lot's grid: `walls` are the buildings' footprints already in this building's local frame; `exits` the points */
export function buildLotGrid(b: { readonly elements?: readonly LotPart[] }, lot: LotRect, walls: readonly (readonly { x: number; y: number }[])[],
  exits: readonly { readonly inner: { x: number; y: number }; readonly edge: { x: number; y: number } }[],
  options: LotGridOptions = {}): LotGrid;

// src/sim/agents/lotNav.ts:273-282 function sees
/** Whether the straight line between two local points crosses only free cells. */
export function sees(g: Pick<LotGrid, "x0" | "y0" | "cell" | "nx" | "ny" | "blocked">, a: { x: number; y: number }, b: { x: number; y: number }): boolean;

// src/sim/agents/lotNav.ts:285-318 function wayOut
/** The way from a local point (in front of a stall) down the aisles to the nearest exit */
export function wayOut(g: LotGrid, x: number, y: number): { points: { x: number; y: number }[]; exit: number } | null;
