// What the feature uses of src/sim/agents/laneGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/laneGrid.ts:30-150 class LaneGrid
/** DO NOT file every lane of the map again when the graph moves */
export class LaneGrid { … }

// src/sim/agents/laneGrid.ts:160-164 interface LaneGridWorld
/** DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle */
export interface LaneGridWorld {
  readonly graph: LaneletGraph;
  readonly topologyLog: ChangeLog<GraphChange>;
  readonly laneGrid: LaneGrid;
}

// src/sim/agents/laneGrid.ts:178-180 function lanesNearInto
/** `lanesNear` into `out`, in no set order (`LaneGrid.nearInto`). */
export function lanesNearInto(w: LaneGridWorld, x: number, y: number, reach: number, out: Lanelet[]): void;
