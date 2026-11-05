// What the feature uses of src/editor/barriers.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/barriers.ts:12-12 variable FOOTWAY_REACH
/** Drawing walls, fences and hedges along a path (`world/barriers.ts`). */

/** How far from the back of a footway a point is still put onto it, world units. */
const FOOTWAY_REACH = m(3);

// src/editor/barriers.ts:16-37 function snapBarrierPoint
function snapBarrierPoint(net: Network | null, kind: BarrierKind, at: Vec2): Vec2;

// src/editor/barriers.ts:40-61 function barrierProblem
/** Why a run cannot be built, or null: it reaches a road or its footway, as a building's own parts may not */
function barrierProblem(net: Network | null, kind: BarrierKind, points: readonly Vec2[]): 'road' | 'short' | null;

// src/editor/barriers.ts:76-162 class BarrierTool
/** THE WALLS TOOL: a wall, fence or hedge traced point by point */
export class BarrierTool { … }

// src/editor/barriers.ts:165-174 function onCarriageway
/** Whether any corner of a footprint is on a carriageway or its kerb (a kerb barrier may stand on the footway). */
function onCarriageway(net: Network, rect: readonly Vec2[]): boolean;
