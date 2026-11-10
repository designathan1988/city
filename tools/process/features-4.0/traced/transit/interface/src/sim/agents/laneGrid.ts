// What the feature uses of src/sim/agents/laneGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/laneGrid.ts:30-150 class LaneGrid
/** DO NOT file every lane of the map again when the graph moves */
export class LaneGrid { … }

// src/sim/agents/laneGrid.ts:35-35 field LaneGrid.changes
/** The boxes of the lanes each follow changed, before and after (null: every lane). */
  readonly changes = new ChangeLog<readonly ChangeRect[]>();

// src/sim/agents/laneGrid.ts:160-164 interface LaneGridWorld
/** DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle */
export interface LaneGridWorld {
  readonly graph: LaneletGraph;
  readonly topologyLog: ChangeLog<GraphChange>;
  readonly laneGrid: LaneGrid;
}

// src/sim/agents/laneGrid.ts:167-170 function laneGrid
/** The world's lanes' grid (`SimWorld.laneGrid`), brought up to the lane graph's changes. */
export function laneGrid(w: LaneGridWorld): LaneGrid;

// src/sim/agents/laneGrid.ts:183-185 function lanesInBox
/** The lanes whose bounds meet a box, in id order (`LaneGrid.inBox`). */
export function lanesInBox(w: LaneGridWorld, x0: number, y0: number, x1: number, y1: number): Lanelet[];
